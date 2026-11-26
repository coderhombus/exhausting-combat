#include "stamina/costs.h"

#include <algorithm>
#include <chrono>
#include <format>
#include <string>

#include <styyx-utils.h>

#include "config/settings.h"
#include "integrations/perk-entry-points.h"
#include "logging.h"
#include "stamina/actor-context.h"
#include "stamina/formulas.h"
#include "stamina/rules.h"

// Log format (one line per event; per-frame drains at trace, everything else at debug):
//
//   [action] actor | payer: payer | inputs | cost <before perks> -> <after perks> | outcome
//   [action] actor | skipped: reason
//
// `actor` is whose settings, skills and perks apply; `payer` is whose stamina changes (the
// horse when riding). Names only, no FormIDs.

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

// One jump can reach the jump hook for both rider and horse; merge reports this close together.
constexpr auto kJumpMergeWindow = std::chrono::milliseconds(300);

// A mounted attack can be seen by the attack-cost hook and by swing animation events (and a
// power attack can swing more than once). Charges for the same rider within this window are
// merged. Mounted swings take well over a second, so separate attacks are not merged. Unverified
// in game: shorten if quick consecutive mounted attacks are under-charged.
constexpr auto kMountedAttackMergeWindow = std::chrono::milliseconds(600);

RE::TESGlobal*    g_playerAttackCostGlobal = nullptr;
AttackCooldown    g_playerAttackCooldown;
EventDeduplicator g_jumpDedup{ kJumpMergeWindow };
EventDeduplicator g_mountedAttackDedup{ kMountedAttackMergeWindow };

// ---------------------------------------------------------------------------------------------
// Logging
// ---------------------------------------------------------------------------------------------

bool ShouldLog(spdlog::level::level_enum a_level)
{
    auto* logger = spdlog::default_logger_raw();
    return logger && logger->should_log(a_level);
}

void LogCharge(spdlog::level::level_enum a_level, std::string_view a_action, const RE::Actor* a_actor, const RE::Actor* a_payer,
               std::string_view a_inputs, float a_cost, float a_finalCost, std::string_view a_outcome)
{
    spdlog::log(a_level, "[{}] {} | payer: {} | {} | cost {:.3f} -> {:.3f} | {}", a_action, Describe(a_actor), Describe(a_payer), a_inputs,
                a_cost, a_finalCost, a_outcome);
}

void LogSkipped(spdlog::level::level_enum a_level, std::string_view a_action, const RE::Actor* a_actor, std::string_view a_reason)
{
    spdlog::log(a_level, "[{}] {} | skipped: {}", a_action, Describe(a_actor), a_reason);
}

// ---------------------------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------------------------

void Pay(RE::Actor& a_payer, float a_cost)
{
    if (a_cost > 0.0f) {
        a_payer.DamageActorValue(RE::ActorValue::kStamina, a_cost);
    }
}

std::uint32_t KeyOf(const RE::Actor& a_actor)
{
    return a_actor.GetFormID();
}

bool IsExemptForLowMaxStamina(RE::Actor& a_actor, float a_cost)
{
    return Stamina::IsExemptForLowMaxStamina(a_actor.IsPlayerRef(), Config::Get().exemptNpcsWithLowMaxStamina.GetValue(),
                                             a_actor.GetActorValueMax(RE::ActorValue::kStamina), a_cost);
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

// ---------------------------------------------------------------------------------------------
// Movement and bow draw (per frame)
// ---------------------------------------------------------------------------------------------

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
    RE::Actor&   payer   = a_actor;
    RE::ActorPtr rider   = role == MountRole::Mount ? GetRider(a_actor) : RE::ActorPtr{};
    RE::Actor&   subject = rider ? *rider : a_actor;

    if (RequiresPositiveStamina(cost) && GetStamina(payer) <= 0.0f) {
        return;
    }
    if (!ShouldChargeStamina(&subject)) {
        return;  // not logged: this would repeat every frame
    }

    const float perSecond = MovementCostPerSecond(cost, subject);
    const float amount    = perSecond * a_deltaSeconds;
    const float finalCost = PEPE::Apply(*entry, &subject, amount, GetRelevantWeapon(subject));
    Pay(payer, finalCost);

    if (ShouldLog(spdlog::level::trace)) {
        LogCharge(spdlog::level::trace, ToString(cost), &subject, &payer,
                  std::format("state={} perSecond={:.3f} dt={:.4f}", ToString(state), perSecond, a_deltaSeconds), amount, finalCost, "paid");
    }
}

/// Crossbows drain for the whole reload animation (bow "drawn"/"attached" states).
void ChargeBowDraw(RE::Actor& a_eventActor, float a_deltaSeconds)
{
    if (!IsDrawingBowWithoutZoom(a_eventActor)) {
        return;
    }
    const ChargeParties parties = ResolveParties(a_eventActor);
    RE::Actor&          actor   = *parties.actor;
    if (!ShouldChargeStamina(&actor)) {
        return;  // not logged: this would repeat every frame
    }

    auto*       object      = a_eventActor.GetEquippedObject(false);
    auto*       weapon      = object ? object->As<RE::TESObjectWEAP>() : nullptr;
    const float skillFactor = SkillFactorFor(actor, RE::ActorValue::kArchery);
    const float perSecond   = ProfileFor(actor).bowDrawCost.GetValue() * skillFactor;
    const float amount      = perSecond * a_deltaSeconds;
    const float finalCost   = PEPE::Apply(PEPE::kBowDrawStamina, &actor, amount, weapon);
    Pay(*parties.payer, finalCost);

    if (ShouldLog(spdlog::level::trace)) {
        LogCharge(spdlog::level::trace, "bow-draw", &actor, parties.payer,
                  std::format("weapon={} skill={:.3f} perSecond={:.3f} dt={:.4f}", Describe(weapon), skillFactor, perSecond, a_deltaSeconds),
                  amount, finalCost, "paid");
    }
}

// ---------------------------------------------------------------------------------------------
// Spells
// ---------------------------------------------------------------------------------------------

/// Shared by fire-and-forget casts (a_deltaSeconds = 1) and concentration (per frame).
bool ChargeSpell(RE::ActorMagicCaster* a_caster, float a_deltaSeconds, bool a_isConcentrationFrame)
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

    RE::Actor* caster = a_caster->GetCasterAsActor();
    if (!caster || !StyyxUtil::MagicUtil::IsSpellPlayable(spell)) {
        return true;
    }

    const auto       level   = a_isConcentrationFrame ? spdlog::level::trace : spdlog::level::debug;
    const char*      action  = a_isConcentrationFrame ? "concentration" : "cast";
    const auto       parties = ResolveParties(*caster);
    RE::Actor&       actor   = *parties.actor;
    const GateResult gate    = EvaluateGate(actor);
    if (gate != GateResult::Charge) {
        if (ShouldLog(level)) {
            LogSkipped(level, action, &actor, ToString(gate));
        }
        return true;
    }

    const auto& profile     = ProfileFor(actor);
    const float baseCost    = profile.castCost.GetValue();
    const float magicka     = spell->CalculateMagickaCost(&actor);
    const float skillFactor = SkillFactorFor(actor, spell->GetAssociatedSkill());
    const float castCost    = CastCost(baseCost, magicka, skillFactor);
    const float concMult    = a_isConcentrationFrame ? profile.concentrationMult.GetValue() : 1.0f;
    const float cost        = a_isConcentrationFrame ? ConcentrationCostPerSecond(castCost, concMult) * a_deltaSeconds : castCost;
    const float finalCost   = PEPE::Apply(PEPE::kCastStamina, &actor, cost, spell);

    const auto log = [&](std::string_view a_outcome) {
        if (ShouldLog(level)) {
            const std::string inputs =
                a_isConcentrationFrame ?
                    std::format("spell={} base={:.2f} magicka={:.1f} skill={:.3f} concentrationMult={:.3f} dt={:.4f}", Describe(spell),
                                baseCost, magicka, skillFactor, concMult, a_deltaSeconds) :
                    std::format("spell={} base={:.2f} magicka={:.1f} skill={:.3f}", Describe(spell), baseCost, magicka, skillFactor);
            LogCharge(level, action, &actor, parties.payer, inputs, cost, finalCost, a_outcome);
        }
    };

    if (IsExemptForLowMaxStamina(actor, finalCost)) {
        log("free: exempt, max stamina below cost");
        return true;
    }
    if (finalCost <= 0.0f) {
        log("free");
        return true;
    }

    Pay(*parties.payer, finalCost);
    const bool canContinue = GetStamina(*parties.payer) > 0.0f;
    log(canContinue ? "paid" : "paid; out of stamina, cast interrupted");
    if (!canContinue) {
        logs::info("[{}] {} | payer: {} | out of stamina, {} interrupted", action, Describe(&actor), Describe(parties.payer),
                   Describe(spell));
    }
    return canContinue;
}

// ---------------------------------------------------------------------------------------------
// Attacks
// ---------------------------------------------------------------------------------------------

AttackFlags ReadAttackFlags(const RE::BGSAttackData& a_attack)
{
    return AttackFlags{
        .bash       = a_attack.data.flags.any(RE::AttackData::AttackFlag::kBashAttack),
        .power      = a_attack.data.flags.any(RE::AttackData::AttackFlag::kPowerAttack),
        .continuous = a_attack.data.flags.any(RE::AttackData::AttackFlag::kContinuousAttack),
    };
}

/// An attack's cost and everything needed to log it.
struct AttackEvaluation
{
    GateResult gate      = GateResult::Charge;
    bool       unarmed   = false;
    bool       exempt    = false;
    float      cost      = 0.0f;  // before perks
    float      finalCost = 0.0f;  // after perks and clamp; 0 when gated, unarmed or exempt
    std::string inputs;

    [[nodiscard]] float Charged() const { return (gate != GateResult::Charge || unarmed || exempt) ? 0.0f : finalCost; }

    [[nodiscard]] std::string_view Reason() const
    {
        if (gate != GateResult::Charge) {
            return ToString(gate);
        }
        if (unarmed) {
            return "unarmed: no weapon, shield or torch (intentionally no extra cost)";
        }
        if (exempt) {
            return "exempt: max stamina below cost";
        }
        return finalCost > 0.0f ? "" : "free";
    }
};

/// Pure calculation for `a_actor`'s settings and equipment; applies perks, pays nothing.
AttackEvaluation EvaluateAttackCost(RE::Actor& a_actor, const AttackFlags& a_flags)
{
    AttackEvaluation result;
    result.gate = EvaluateGate(a_actor);
    if (result.gate != GateResult::Charge) {
        return result;
    }

    const AttackItem item = ResolveAttackItem(a_actor, a_flags.bash);
    if (item.kind == AttackItemKind::None) {
        // Intentional: unarmed attacks have no item weight, so this mod adds no cost and the
        // engine's own cost applies. Revisit when supporting unarmed perk mods / fist weapons.
        result.unarmed = true;
        result.inputs  = std::format("bash={} power={}", a_flags.bash, a_flags.power);
        return result;
    }

    const auto&            profile = ProfileFor(a_actor);
    const AttackCostInputs inputs{
        .baseCost        = profile.attackCost.GetValue(),
        .itemWeight      = item.weight,
        .attackMult      = profile.attackMult.GetValue(),
        .powerAttackMult = profile.powerAttackMult.GetValue(),
        .isPowerAttack   = a_flags.power,
        .skillFactor     = item.skillFactor,
    };
    result.cost      = Stamina::AttackCost(inputs);
    result.finalCost = PEPE::Apply(PEPE::kAttackStamina, &a_actor, result.cost, item.form);
    result.exempt    = IsExemptForLowMaxStamina(a_actor, result.finalCost);
    result.inputs    = std::format("item={} bash={} power={} base={:.2f} weight={:.1f} (x{:.3f}) attackMult={:.3f} powerMult={:.3f} skill={:.3f}",
                                   Describe(item.form), a_flags.bash, a_flags.power, inputs.baseCost, item.weight,
                                   WeightMultiplier(item.weight), inputs.attackMult, AppliedPowerAttackMult(inputs), item.skillFactor);
    return result;
}

void LogAttack(spdlog::level::level_enum a_level, std::string_view a_action, const RE::Actor* a_actor, const RE::Actor* a_payer,
               const AttackEvaluation& a_cost, std::string_view a_outcome)
{
    if (!ShouldLog(a_level)) {
        return;
    }
    if (a_cost.inputs.empty()) {
        LogSkipped(a_level, a_action, a_actor, a_outcome);
    } else {
        LogCharge(a_level, a_action, a_actor, a_payer, a_cost.inputs, a_cost.cost, a_cost.finalCost, a_outcome);
    }
}

/// Charges the mount for the player's mounted attack, merging duplicate reports.
void ChargeMountedAttack(const ChargeParties& a_parties, const AttackFlags& a_flags, std::string_view a_source)
{
    RE::Actor&       rider = *a_parties.actor;
    const AttackEvaluation cost  = EvaluateAttackCost(rider, a_flags);
    const float      due   = cost.Charged();

    if (due <= 0.0f) {
        const auto reason = cost.Reason();
        LogAttack(spdlog::level::debug, "attack-mounted", &rider, a_parties.payer, cost,
                  std::format("{} ({})", reason.empty() ? "free" : reason, a_source));
        return;
    }
    if (!g_mountedAttackDedup.TryAccept(KeyOf(rider), EventDeduplicator::Clock::now())) {
        LogAttack(spdlog::level::debug, "attack-mounted", &rider, a_parties.payer, cost,
                  std::format("already charged for this attack ({})", a_source));
        return;
    }
    Pay(*a_parties.payer, due);
    LogAttack(spdlog::level::debug, "attack-mounted", &rider, a_parties.payer, cost, std::format("paid ({})", a_source));
}
}  // namespace

// -------------------------------------------------------------------------------------------------
// Public entry points
// -------------------------------------------------------------------------------------------------

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
    if (!a_actor) {
        return;
    }
    const ChargeParties parties = ResolveParties(*a_actor);
    RE::Actor&          actor   = *parties.actor;
    const GateResult    gate    = EvaluateGate(actor);
    if (gate != GateResult::Charge) {
        LogSkipped(spdlog::level::debug, "jump", &actor, ToString(gate));
        return;
    }
    if (!g_jumpDedup.TryAccept(KeyOf(*parties.payer), EventDeduplicator::Clock::now())) {
        LogSkipped(spdlog::level::debug, "jump", &actor, std::format("already charged for this jump (reported for {})", Describe(a_actor)));
        return;
    }

    const float baseCost  = ProfileFor(actor).jumpCost.GetValue();
    const float cost      = EncumbranceScaledCostFor(actor, baseCost);
    const float finalCost = PEPE::Apply(PEPE::kJumpStamina, &actor, cost, GetRelevantWeapon(actor));
    Pay(*parties.payer, finalCost);
    if (ShouldLog(spdlog::level::debug)) {
        LogCharge(spdlog::level::debug, "jump", &actor, parties.payer, std::format("base={:.2f} reportedFor={}", baseCost, Describe(a_actor)),
                  cost, finalCost, "paid");
    }
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
    if (!a_actor) {
        return true;
    }
    const ChargeParties parties = ResolveParties(*a_actor);
    RE::Actor&          actor   = *parties.actor;
    const GateResult    gate    = EvaluateGate(actor);
    if (gate != GateResult::Charge) {
        LogSkipped(spdlog::level::debug, "bow-shot", &actor, ToString(gate));
        return true;
    }

    const float skillFactor = SkillFactorFor(actor, RE::ActorValue::kArchery);
    const float cost        = ProfileFor(actor).bowShotCost.GetValue() * skillFactor;
    const float finalCost   = PEPE::Apply(PEPE::kBowShotStamina, &actor, cost, a_weapon);
    const auto  log         = [&](std::string_view a_outcome) {
        if (ShouldLog(spdlog::level::debug)) {
            LogCharge(spdlog::level::debug, "bow-shot", &actor, parties.payer,
                      std::format("weapon={} skill={:.3f}", Describe(a_weapon), skillFactor), cost, finalCost, a_outcome);
        }
    };

    if (finalCost <= 0.0f) {
        log("free");
        return true;
    }
    // Same exemption as attacks and casts: NPCs with too little max stamina shoot for free
    // instead of never being able to shoot.
    if (IsExemptForLowMaxStamina(actor, finalCost)) {
        log("free: exempt, max stamina below cost");
        return true;
    }

    Pay(*parties.payer, finalCost);
    const bool canShoot = GetStamina(*parties.payer) > 0.0f;
    log(canShoot ? "paid" : "paid; out of stamina, shot blocked");
    if (!canShoot) {
        logs::info("[bow-shot] {} | payer: {} | out of stamina, shot blocked", Describe(&actor), Describe(parties.payer));
    }
    return canShoot;
}

float ResolveAttackStaminaCost(RE::Actor* a_actor, const RE::BGSAttackData* a_attack, float a_vanillaCost)
{
    if (!a_actor || !a_attack) {
        return a_vanillaCost;
    }
    const AttackFlags flags    = ReadAttackFlags(*a_attack);
    const bool        isPlayer = a_actor->IsPlayerRef();

    // The player on horseback: the horse pays (the rider's stamina is not shown while riding).
    const ChargeParties parties = ResolveParties(*a_actor);
    if (parties.payer != a_actor && !flags.continuous) {
        ChargeMountedAttack(parties, flags, "attack-cost event");
        return 0.0f;
    }

    // The attack-cost event fires twice when attacking while moving diagonally backwards.
    const bool suppressed = isPlayer && !g_playerAttackCooldown.TryStart(AttackCooldown::Clock::now());
    if (suppressed || flags.continuous) {
        const float result = ResolveAttackHookCost(a_vanillaCost, 0.0f, flags.continuous, suppressed);
        if (ShouldLog(spdlog::level::debug)) {
            LogSkipped(spdlog::level::debug, "attack", a_actor,
                       std::format("{}; engine cost {:.3f}", suppressed ? "duplicate event within cooldown" : "continuous attack", result));
        }
        return result;
    }

    const AttackEvaluation cost   = EvaluateAttackCost(*a_actor, flags);
    const float      due    = cost.Charged();
    const float      result = ResolveAttackHookCost(a_vanillaCost, due, false, false);
    if (isPlayer && due > 0.0f && g_playerAttackCostGlobal) {
        g_playerAttackCostGlobal->value = std::clamp(due, kPlayerAttackCostGlobalMin, kPlayerAttackCostGlobalMax);
    }

    const auto reason = cost.Reason();
    LogAttack(spdlog::level::debug, "attack", a_actor, a_actor, cost,
              due > 0.0f ? std::format("charged by engine: {:.3f}", result) :
                           std::format("{}; engine cost {:.3f}", reason.empty() ? "free" : reason, result));
    return result;
}

bool CanAffordAttack(RE::Actor* a_attacker, const RE::BGSAttackData* a_attack)
{
    if (!a_attacker || !a_attack) {
        return true;
    }
    const AttackEvaluation cost      = EvaluateAttackCost(*a_attacker, ReadAttackFlags(*a_attack));
    const float      stamina   = a_attacker->GetActorValue(RE::ActorValue::kStamina);
    const bool       canAfford = !(stamina < cost.Charged());
    if (ShouldLog(spdlog::level::trace)) {
        LogAttack(spdlog::level::trace, "attack-ai-check", a_attacker, a_attacker, cost,
                  std::format("stamina {:.2f}: {}", stamina, canAfford ? "allowed" : "attack chance set to 0"));
    }
    return canAfford;
}

void ChargeMountedSwing(RE::Actor& a_rider, std::string_view a_eventName)
{
    const ChargeParties parties = ResolveParties(a_rider);
    if (parties.payer == &a_rider) {
        return;  // not (or no longer) mounted
    }
    AttackFlags flags;
    if (auto* process = a_rider.currentProcess; process && process->high && process->high->attackData) {
        flags = ReadAttackFlags(*process->high->attackData);
    }
    if (flags.continuous) {
        return;
    }
    ChargeMountedAttack(parties, flags, std::format("swing event {}", a_eventName));
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
