#include "hooks.h"

#include <chrono>

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <styyx-utils.h>

#include "logging.h"
#include "stamina/costs.h"
#include "stamina/rules.h"

// Hook targets are Skyrim AE 1.6.1170 Address Library IDs and offsets (see plugin-info.h for the
// runtime check). Each hook only translates engine arguments and calls into stamina/costs.

namespace ExhaustingCombat::Hooks
{
namespace
{
// NPC update hooks receive a delta of 0 (always, as far as tested), so NPCs use the timer's
// real-time delta instead. This ignores slow-time effects, unlike the player's delta.
// Possible alternative after in-game testing: RE::BSTimer::delta (game time, respects slow-mo).
constexpr float kFallbackFrameSeconds = 0.016833f;  // ~59.4 FPS, used only if BSTimer is missing

float GetEffectiveDelta(float a_deltaTime)
{
    if (a_deltaTime > 0.0f) {
        return a_deltaTime;
    }
    if (const auto* timer = RE::BSTimer::GetSingleton()) {
        return timer->realTimeDelta;
    }
    return kFallbackFrameSeconds;
}

Stamina::AttackCooldown g_playerAttackCooldown;

struct PlayerUpdateHook
{
    static void Call(RE::PlayerCharacter* a_this, float a_deltaTime)
    {
        Stamina::ChargeContinuousCosts(a_this, a_deltaTime);
        func(a_this, a_deltaTime);
    }
    static inline REL::Relocation<decltype(Call)> func;
};

struct NPCUpdateHook
{
    static void Call(RE::Actor* a_this, float a_deltaTime)
    {
        Stamina::ChargeContinuousCosts(a_this, GetEffectiveDelta(a_deltaTime));
        func(a_this, a_deltaTime);
    }
    static inline REL::Relocation<decltype(Call)> func;
};

// Charges on the jump's "get scale" call. Does not fire for horse jumps; NPCs probably use a
// different path (untested). Fine for now: free horse jumps are a perk of having a horse.
struct JumpGetScaleHook
{
    static float Call(RE::TESObjectREFR* a_this)
    {
        const float scale = func(a_this);
        if (auto* actor = a_this ? a_this->As<RE::Actor>() : nullptr) {
            Stamina::ChargeJump(actor);
        }
        return scale;
    }
    static inline REL::Relocation<decltype(Call)> func;
};

struct StartCastingHook
{
    static void Call(RE::ActorMagicCaster* a_this)
    {
        if (a_this && !Stamina::ChargeSpellCast(a_this)) {
            a_this->InterruptCast(true);
            return;
        }
        func(a_this);
    }
    static inline REL::Relocation<decltype(Call)> func;
};

// Concentration spells pay per frame here; StartCasting only fires once per cast.
struct CasterUpdateHook
{
    static void Call(RE::ActorMagicCaster* a_this, float a_deltaTime)
    {
        if (a_this && a_this->currentSpell && a_this->state.any(RE::MagicCaster::State::kCasting) &&
            a_this->currentSpell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration) {
            const auto source = a_this->GetCastingSource();
            if ((source == RE::MagicSystem::CastingSource::kLeftHand || source == RE::MagicSystem::CastingSource::kRightHand) &&
                !Stamina::ChargeConcentration(a_this, a_deltaTime)) {
                a_this->InterruptCast(true);
            }
        }
        func(a_this, a_deltaTime);
    }
    static inline REL::Relocation<decltype(Call)> func;
};

struct AttackStaminaCostHook
{
    static float Call(RE::ActorValueOwner* a_this, RE::BGSAttackData* a_attack)
    {
        const float vanillaCost = func(a_this, a_attack);
        auto*       actor       = skyrim_cast<RE::Actor*>(a_this);
        if (!actor || !a_attack) {
            return vanillaCost;
        }

        const bool isPlayer   = actor->IsPlayerRef();
        const bool suppressed = isPlayer && !g_playerAttackCooldown.TryStart(Stamina::AttackCooldown::Clock::now());
        const bool continuous = a_attack->data.flags.any(RE::AttackData::AttackFlag::kContinuousAttack);
        const float modCost   = (suppressed || continuous) ? 0.0f : Stamina::ComputeAttackCost(actor, a_attack, Stamina::AttackCostUse::Charge);
        const float result    = Stamina::ResolveAttackHookCost(vanillaCost, modCost, continuous, suppressed);

        if (isPlayer && modCost > 0.0f) {
            Stamina::PublishPlayerAttackCost(modCost);
        }
        if (IsDebugEnabled() && (suppressed || continuous || modCost <= 0.0f)) {
            logs::debug("[attack] {} vanilla={:.3f} mod={:.3f} -> {:.3f}{}{}", Describe(actor), vanillaCost, modCost, result,
                        suppressed ? " (duplicate event within cooldown)" : "", continuous ? " (continuous attack: vanilla)" : "");
        }
        return result;
    }
    static inline REL::Relocation<decltype(Call)> func;
};

// AI attack selection: an attack the attacker cannot afford gets a 0% chance.
struct AttackChanceHook
{
    static float Call(RE::Actor* a_attacker, RE::Actor* a_victim, RE::BGSAttackData* a_attack)
    {
        if (a_attacker && a_attack) {
            const float cost = Stamina::ComputeAttackCost(a_attacker, a_attack, Stamina::AttackCostUse::Preview);
            if (a_attacker->GetActorValue(RE::ActorValue::kStamina) < cost) {
                return 0.0f;
            }
        }
        return func(a_attacker, a_victim, a_attack);
    }
    static inline REL::Relocation<decltype(Call)> func;
};

struct FireArrowHook
{
    static void Call(RE::TESObjectWEAP* a_weapon, RE::TESObjectREFR* a_source, RE::TESAmmo* a_overwriteAmmo, RE::EnchantmentItem* a_ammoEnchantment,
                     RE::AlchemyItem* a_poison)
    {
        auto* actor = a_source ? a_source->As<RE::Actor>() : nullptr;
        if (actor && !Stamina::ChargeBowShot(actor, a_weapon)) {
            return;  // out of stamina: the arrow is not fired
        }
        func(a_weapon, a_source, a_overwriteAmmo, a_ammoEnchantment, a_poison);
    }
    static inline REL::Relocation<decltype(Call)> func;
};
}  // namespace

void Install()
{
    using StyyxUtil::HookUtils::WriteCall5;
    using StyyxUtil::HookUtils::WriteVFunc;

    // Virtual function hooks: Actor::Update (0xAD), MagicCaster StartCast (0x06) / Update (0x1D).
    WriteVFunc<RE::PlayerCharacter, 0, 0xAD, PlayerUpdateHook>();
    WriteVFunc<RE::Character, 0, 0xAD, NPCUpdateHook>();
    WriteVFunc<RE::ActorMagicCaster, 0, 0x06, StartCastingHook>();
    WriteVFunc<RE::ActorMagicCaster, 0, 0x1D, CasterUpdateHook>();

    // Call-site hooks (AE IDs; SE IDs were 36271, 37650, 48139, 41778).
    WriteCall5<JumpGetScaleHook>(REL::ID(37257), 0x17F);
    WriteCall5<AttackStaminaCostHook>(REL::ID(38603), 0x171);
    WriteCall5<AttackChanceHook>(REL::ID(49170), 0x28D);
    WriteCall5<FireArrowHook>(REL::ID(42859), 0x138);
    static_assert(kCallHookCount == 4, "update kCallHookCount when adding or removing WriteCall5 hooks");

    logs::info("installed 4 virtual and {} call hooks", kCallHookCount);
}
}  // namespace ExhaustingCombat::Hooks
