#include "stamina/costs.h"

#include <algorithm>

#include <styyx-utils.h>

#include "config/settings.h"
#include "integrations/perk-entry-points.h"
#include "logging.h"
#include "stamina/actor-context.h"
#include "stamina/formulas.h"
#include "stamina/rules.h"

namespace ExhaustingCombat::Stamina
{
namespace PEPE = Integrations::PerkEntryPoints;

namespace
{
// Global published for ESP-side conditions/scripts: the player's last attack cost, clamped to a
// range the consumer expects. Editor ID and range are kept from the original implementation.
constexpr const char* kPlayerAttackCostGlobalEditorID = "SST_StaminaCostPlayer";
constexpr float       kPlayerAttackCostGlobalMin      = 3.0f;
constexpr float       kPlayerAttackCostGlobalMax      = 15.0f;

RE::TESGlobal* g_playerAttackCostGlobal = nullptr;

void Pay(RE::Actor& a_payer, float a_cost)
{
    if (a_cost > 0.0f) {
        a_payer.DamageActorValue(RE::ActorValue::kStamina, a_cost);
    }
}

const PEPE::Entry* PerkEntryFor(MovementCost a_cost)
{
    switch (a_cost) {
        case MovementCost::Run:
            return &PEPE::kRunStamina;
        case MovementCost::Sneak:
            return &PEPE::kSneakStamina;
        case MovementCost::Sprint:
            return &PEPE::kSprintStamina;
        case MovementCost::MountSprint:
            return &PEPE::kMountSprintStamina;
        case MovementCost::Swim:
            return &PEPE::kSwimStamina;
        case MovementCost::None:
            return nullptr;
    }
    return nullptr;
}

/// Cost per second before perks. `a_subject` supplies profile, skills and carried weight.
float MovementCostPerSecond(MovementCost a_cost, RE::Actor& a_subject)
{
    const auto& profile = ProfileFor(a_subject);
    switch (a_cost) {
        case MovementCost::Run:
            return EncumbranceScaledCostFor(a_subject, profile.runCost.GetValue());
        case MovementCost::Sneak:
            return profile.sneakCost.GetValue() * SkillFactorFor(a_subject, RE::ActorValue::kSneak);
        case MovementCost::Sprint:
            return EncumbranceScaledCostFor(a_subject, profile.sprintCost.GetValue());
        case MovementCost::MountSprint:
            return profile.mountSprintCost.GetValue();  // flat: horse load is not modelled
        case MovementCost::Swim:
            return EncumbranceScaledCostFor(a_subject, profile.swimCost.GetValue());
        case MovementCost::None:
            return 0.0f;
    }
    return 0.0f;
}

void ChargeMovement(RE::Actor& a_actor, float a_deltaSeconds)
{
    const MountRole     role  = GetMountRole(a_actor);
    const MovementState state = ClassifyMovement(ReadMovementFlags(a_actor));
    const MovementCost  cost  = MovementCostFor(state, role);
    const auto*         entry = PerkEntryFor(cost);
    if (!entry) {
        return;
    }

    // The moving body pays. For a ridden mount, the rider's settings, perks and gate apply.
    RE::Actor&   payer = a_actor;
    RE::ActorPtr rider = role == MountRole::Mount ? GetRider(a_actor) : RE::ActorPtr{};
    RE::Actor&   subject = rider ? *rider : a_actor;

    if (RequiresPositiveStamina(cost) && GetStamina(payer) <= 0.0f) {
        return;
    }
    if (!ShouldChargeStamina(&subject)) {
        return;
    }

    const float perSecond = MovementCostPerSecond(cost, subject);
    const float amount    = PEPE::Apply(*entry, &subject, perSecond * a_deltaSeconds, GetRelevantWeapon(subject));
    Pay(payer, amount);

    if (IsTraceEnabled()) {
        logs::trace("[{}] {} state={} perSecond={:.3f} dt={:.4f} -> {:.4f}{}", ToString(cost), Describe(&payer), ToString(state), perSecond,
                    a_deltaSeconds, amount, rider ? std::format(" (rider {})", Describe(rider.get())) : std::string{});
    }
}

/// Crossbows drain for the whole reload animation (bow "drawn"/"attached" states).
void ChargeBowDraw(RE::Actor& a_actor, float a_deltaSeconds)
{
    if (!ShouldChargeStamina(&a_actor) || !IsDrawingBowWithoutZoom(a_actor)) {
        return;
    }
    auto*       object    = a_actor.GetEquippedObject(false);
    auto*       weapon    = object ? object->As<RE::TESObjectWEAP>() : nullptr;
    const float perSecond = ProfileFor(a_actor).bowDrawCost.GetValue() * SkillFactorFor(a_actor, RE::ActorValue::kArchery);
    const float amount    = PEPE::Apply(PEPE::kBowDrawStamina, &a_actor, perSecond * a_deltaSeconds, weapon);
    RE::Actor&  payer     = PayerFor(a_actor);
    Pay(payer, amount);

    if (IsTraceEnabled()) {
        logs::trace("[bow-draw] {} perSecond={:.3f} dt={:.4f} -> {:.4f} paid by {}", Describe(&a_actor), perSecond, a_deltaSeconds, amount,
                    Describe(&payer));
    }
}

bool IsExemptForLowMaxStamina(RE::Actor& a_actor, float a_cost)
{
    return Stamina::IsExemptForLowMaxStamina(a_actor.IsPlayerRef(), Config::Get().exemptNpcsWithLowMaxStamina.GetValue(),
                                             a_actor.GetActorValueMax(RE::ActorValue::kStamina), a_cost);
}

/// Shared by fire-and-forget casts (a_deltaSeconds = 1) and concentration (per frame).
bool ChargeSpell(RE::ActorMagicCaster* a_caster, float a_scale, bool a_isConcentrationFrame)
{
    if (!a_caster || !a_caster->currentSpell) {
        return true;
    }
    RE::MagicItem* spell = a_caster->currentSpell;
    if (spell->GetCastingType() == RE::MagicSystem::CastingType::kScroll) {
        return true;  // scrolls are free
    }
    const bool isConcentration = spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
    if (isConcentration != a_isConcentrationFrame) {
        return true;  // concentration is charged per frame from the caster update hook instead
    }

    RE::Actor* actor = a_caster->GetCasterAsActor();
    if (!actor || !StyyxUtil::MagicUtil::IsSpellPlayable(spell) || !ShouldChargeStamina(actor)) {
        return true;
    }

    const float baseCost    = ProfileFor(*actor).castCost.GetValue();
    const float magicka     = spell->CalculateMagickaCost(actor);
    const float skillFactor = SkillFactorFor(*actor, spell->GetAssociatedSkill());
    const float cost        = CastCost(baseCost, magicka, skillFactor) * a_scale;
    const float finalCost   = PEPE::Apply(PEPE::kCastStamina, actor, cost, spell);

    const auto log = [&](std::string_view a_outcome) {
        if (a_isConcentrationFrame ? IsTraceEnabled() : IsDebugEnabled()) {
            spdlog::log(a_isConcentrationFrame ? spdlog::level::trace : spdlog::level::debug,
                      "[cast] {} spell={} base={} magicka={:.1f} skill={:.3f} scale={:.4f} -> {:.3f} (after perks {:.3f}) {}",
                      Describe(actor), Describe(spell), baseCost, magicka, skillFactor, a_scale, cost, finalCost, a_outcome);
        }
    };

    if (IsExemptForLowMaxStamina(*actor, finalCost)) {
        log("exempt: max stamina below cost");
        return true;
    }
    if (finalCost <= 0.0f) {
        log("free");
        return true;
    }

    RE::Actor& payer = PayerFor(*actor);
    Pay(payer, finalCost);
    const bool canContinue = GetStamina(payer) > 0.0f;
    log(canContinue ? "paid" : "paid; out of stamina, interrupting");
    if (!canContinue) {
        logs::info("{} ran out of stamina casting {}; cast interrupted", Describe(actor), Describe(spell));
    }
    return canContinue;
}
}  // namespace

void ChargeContinuousCosts(RE::Actor* a_actor, float a_deltaSeconds)
{
    if (!a_actor) {
        return;
    }
    ChargeMovement(*a_actor, a_deltaSeconds);
    ChargeBowDraw(*a_actor, a_deltaSeconds);
}

void ChargeJump(RE::Actor* a_actor)
{
    if (!ShouldChargeStamina(a_actor)) {
        return;
    }
    const float base   = EncumbranceScaledCostFor(*a_actor, ProfileFor(*a_actor).jumpCost.GetValue());
    const float amount = PEPE::Apply(PEPE::kJumpStamina, a_actor, base, GetRelevantWeapon(*a_actor));
    RE::Actor&  payer  = PayerFor(*a_actor);
    Pay(payer, amount);
    logs::debug("[jump] {} base={:.3f} -> {:.3f} paid by {}", Describe(a_actor), base, amount, Describe(&payer));
}

bool ChargeSpellCast(RE::ActorMagicCaster* a_caster)
{
    return ChargeSpell(a_caster, 1.0f, false);
}

bool ChargeConcentration(RE::ActorMagicCaster* a_caster, float a_deltaSeconds)
{
    return ChargeSpell(a_caster, a_deltaSeconds, true);
}

bool ChargeBowShot(RE::Actor* a_actor, RE::TESObjectWEAP* a_weapon)
{
    if (!ShouldChargeStamina(a_actor)) {
        return true;
    }
    const float skillFactor = SkillFactorFor(*a_actor, RE::ActorValue::kArchery);
    const float cost        = ProfileFor(*a_actor).bowShotCost.GetValue() * skillFactor;
    const float finalCost   = PEPE::Apply(PEPE::kBowShotStamina, a_actor, cost, a_weapon);

    if (finalCost <= 0.0f) {
        return true;
    }
    // Same exemption as attacks and casts: NPCs with too little max stamina shoot for free
    // instead of never being able to shoot.
    if (IsExemptForLowMaxStamina(*a_actor, finalCost)) {
        logs::debug("[bow-shot] {} cost {:.3f} exempt: max stamina below cost", Describe(a_actor), finalCost);
        return true;
    }

    RE::Actor& payer = PayerFor(*a_actor);
    Pay(payer, finalCost);
    const bool canShoot = GetStamina(payer) > 0.0f;
    logs::debug("[bow-shot] {} weapon={} skill={:.3f} -> {:.3f} (after perks {:.3f}) paid by {}{}", Describe(a_actor), Describe(a_weapon),
                skillFactor, cost, finalCost, Describe(&payer), canShoot ? "" : "; out of stamina, shot blocked");
    if (!canShoot) {
        logs::info("{} is out of stamina; shot blocked", Describe(a_actor));
    }
    return canShoot;
}

float ComputeAttackCost(RE::Actor* a_actor, const RE::BGSAttackData* a_attack, AttackCostUse a_use)
{
    if (!a_attack || !ShouldChargeStamina(a_actor)) {
        return 0.0f;
    }
    const bool isBash        = a_attack->data.flags.any(RE::AttackData::AttackFlag::kBashAttack);
    const bool isPowerAttack = a_attack->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);
    const auto level         = a_use == AttackCostUse::Charge ? spdlog::level::debug : spdlog::level::trace;
    const bool logEnabled    = a_use == AttackCostUse::Charge ? IsDebugEnabled() : IsTraceEnabled();

    const AttackItem item = ResolveAttackItem(*a_actor, isBash);
    if (item.kind == AttackItemKind::None) {
        // Intentional: unarmed attacks have no item weight, so this mod adds no cost and the
        // engine's own cost applies. Revisit when supporting unarmed perk mods / fist weapons.
        if (logEnabled) {
            spdlog::log(level, "[attack] {} bash={} power={}: no weapon, shield or torch identified; no cost added", Describe(a_actor), isBash,
                      isPowerAttack);
        }
        return 0.0f;
    }

    const auto&            profile = ProfileFor(*a_actor);
    const AttackCostInputs inputs{
        .baseCost        = profile.attackCost.GetValue(),
        .itemWeight      = item.weight,
        .attackMult      = profile.attackMult.GetValue(),
        .powerAttackMult = profile.powerAttackMult.GetValue(),
        .isPowerAttack   = isPowerAttack,
        .skillFactor     = item.skillFactor,
    };
    const float cost      = AttackCost(inputs);
    const float finalCost = PEPE::Apply(PEPE::kAttackStamina, a_actor, cost, item.form);
    const bool  exempt    = IsExemptForLowMaxStamina(*a_actor, finalCost);

    if (logEnabled) {
        spdlog::log(level,
                  "[attack{}] {} item={} bash={} power={} base={} weight={:.1f} (x{:.3f}) mult={} powerMult={} skill={:.3f} -> {:.3f} (after "
                  "perks {:.3f}){}",
                  a_use == AttackCostUse::Preview ? " preview" : "", Describe(a_actor), Describe(item.form), isBash, isPowerAttack,
                  inputs.baseCost, item.weight, WeightMultiplier(item.weight), inputs.attackMult, inputs.powerAttackMult, item.skillFactor, cost,
                  finalCost, exempt ? " exempt: max stamina below cost" : "");
    }
    return exempt ? 0.0f : finalCost;
}

void PublishPlayerAttackCost(float a_cost)
{
    if (g_playerAttackCostGlobal) {
        g_playerAttackCostGlobal->value = std::clamp(a_cost, kPlayerAttackCostGlobalMin, kPlayerAttackCostGlobalMax);
    }
}

void InitializeForms()
{
    g_playerAttackCostGlobal = RE::TESForm::LookupByEditorID<RE::TESGlobal>(kPlayerAttackCostGlobalEditorID);
    if (g_playerAttackCostGlobal) {
        logs::info("found global {}", kPlayerAttackCostGlobalEditorID);
    } else {
        logs::info("global {} not found; the player's attack cost will not be published", kPlayerAttackCostGlobalEditorID);
    }
}
}  // namespace ExhaustingCombat::Stamina
