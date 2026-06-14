#include "hooks.h"

#include "RE/A/ActorMagicCaster.h"
#include "RE/B/BGSAttackData.h"
#include "RE/C/Character.h"
#include "RE/P/PlayerCharacter.h"
#include "REL/Offset.h"
#include "SKSE/Impl/PCH.h"
#include "Utility/util.h"
#include "st-hooks.h"
#include "stamina-manager.h"


namespace EXCO::HOOKS
{

// needed cause for NPCs, the update function appears to have a delta of 0.
// always
static float GetEffectiveDelta(float a_deltaTime)
{
    if (a_deltaTime > 0.0f)
        return a_deltaTime;

    if (const auto* timer = RE::BSTimer::GetSingleton())
        return timer->realTimeDelta;
    return 0.016833;
}

void PlayerUpdateHook::Call(RE::PlayerCharacter* a_this, float a_deltaTime)
{
    StaminaCost::ManageSneakStamina(a_this, a_deltaTime);
    StaminaCost::ManageSprintStamina(a_this, a_deltaTime);
    StaminaCost::ManageBowDraw(a_this, a_deltaTime);
    StaminaCost::ManageSwimStamina(a_this, a_deltaTime);
    func(a_this, a_deltaTime);
}

void NPCUpdateHook::Call(RE::Actor* a_this, float a_deltaTime)
{
    const float r_delta = GetEffectiveDelta(a_deltaTime);
    StaminaCost::ManageSneakStamina(a_this, r_delta);
    StaminaCost::ManageSprintStamina(a_this, r_delta);
    StaminaCost::ManageBowDraw(a_this, r_delta);
    StaminaCost::ManageSwimStamina(a_this, r_delta);
    func(a_this, a_deltaTime);
}

// does not work for a horse jumping.
// i assume npcs use either a different branch in this function or a different
// function fine for now since NPCs don't really jump and i call this a benefit
// for having a horse
float JumpGetScaleHook::Call(RE::TESObjectREFR* a_this)
{
    const auto scale = func(a_this);

    auto* actor = a_this ? a_this->As<RE::Actor>() : nullptr;

    if (!actor)
    {
        return scale;
    }

    StaminaCost::ManageJumpStamina(actor);
    return scale;
}

void StartCastingHook::Call(RE::ActorMagicCaster* a_this)
{
    int break_loop = 0;

    if (!StaminaCost::ManageCastStamina(a_this))
        break_loop = 1;

    if (break_loop == 1)
    {
        a_this->InterruptCast(true);
    }
    else
    {
        func(a_this);
    }
}

void CasterUpdateHook::Call(RE::ActorMagicCaster* a_this, float a_deltaTime)
{
    if (!a_this)
    {
        return func(a_this, a_deltaTime);
    }

    const auto source = a_this->GetCastingSource();
    int break_loop    = 0;

    if (source != RE::MagicSystem::CastingSource::kLeftHand && source != RE::MagicSystem::CastingSource::kRightHand)
    {
        return func(a_this, a_deltaTime);
    }

    if (a_this->state.any(RE::MagicCaster::State::kCasting) &&
        a_this->currentSpell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration)
    {
        if (!StaminaCost::ManageConcentrationStamina(a_this, a_deltaTime))
            break_loop = 1;
    }

    if (break_loop == 1)
    {
        a_this->InterruptCast(true);
    }

    func(a_this, a_deltaTime);
}

// event gets sent twice when moving diagonally backwards in either direction.
// timer may prevent double attack cost. 10ms is too little. 60 should work fine
// may go up to 150 even, but needs testing
static inline std::chrono::steady_clock::time_point time_allowed;
static constexpr auto ATTACK_COST_COOLDOWN = std::chrono::milliseconds(60);
float AttackStamCostHook::Call(RE::ActorValueOwner* a_this, RE::BGSAttackData* a_attack)
{
    const float result = func(a_this, a_attack);
    const auto actor   = skyrim_cast<RE::Actor*>(a_this);

    if (!actor)
        return result;

    if (actor->IsPlayerRef())
    {
        const auto now = std::chrono::steady_clock::now();

        if (now < time_allowed)
        {
            return result;
        }

        time_allowed = now + ATTACK_COST_COOLDOWN;
    }

    if (a_attack->data.flags.any(RE::AttackData::AttackFlag::kBashAttack, RE::AttackData::AttackFlag::kPowerAttack))
    {

        return result;
    }

    // applies the attack cost to player and npc
    if (const auto res = StaminaCost::ManageAttackStamina(actor, a_attack); res > 0)
    {
        if (Util::stam_cost_pl_glob && actor->IsPlayerRef())
            Util::stam_cost_pl_glob->value = std::clamp(res, 3.f, 15.f);
        return res;
    }
    return result;
}

float AttackChance::Call(RE::Actor* a_attacker, RE::Actor* a_victim, RE::BGSAttackData* a_attack)
{
    if (a_attacker->GetActorValue(RE::ActorValue::kStamina) < StaminaCost::GetAttackStaminaCost(a_attacker))
        return 0;

    return func(a_attacker, a_victim, a_attack);
}

void FireArrowHook::Call(RE::TESObjectWEAP* a_weapon, RE::TESObjectREFR* a_source, RE::TESAmmo* a_overwriteAmmo,
                         RE::EnchantmentItem* a_ammoEnch, RE::AlchemyItem* a_poison)
{

    const auto actor = a_source->As<RE::Actor>();
    if (!actor)
    {
        return;
    }

    if (StaminaCost::ManageBowStamina(actor, a_weapon))
    {
        func(a_weapon, a_source, a_overwriteAmmo, a_ammoEnch, a_poison);
    }
}
void InstallHooks()
{
    HookUtils::WriteVFunc<RE::PlayerCharacter, 0, 0xad, PlayerUpdateHook>();
    HookUtils::WriteVFunc<RE::Character, 0, 0xad, NPCUpdateHook>();
    HookUtils::WriteVFunc<RE::ActorMagicCaster, 0, 0x06, StartCastingHook>();
    HookUtils::WriteVFunc<RE::ActorMagicCaster, 0, 0x1d, CasterUpdateHook>();
    HookUtils::WriteCall5<JumpGetScaleHook>(RELOCATION_ID(36271, 37257), OFFSET(0x190, 0x17f));
    HookUtils::WriteCall5<AttackStamCostHook>(RELOCATION_ID(37650, 38603), OFFSET(0x16e, 0x171));
    HookUtils::WriteCall5<AttackChance>(RELOCATION_ID(48139, 49170), OFFSET(0x2ae, 0x28d));
    HookUtils::WriteCall5<FireArrowHook>(RELOCATION_ID(41778, 42859), OFFSET(0x133, 0x138));
}
} // namespace EXCO::HOOKS
