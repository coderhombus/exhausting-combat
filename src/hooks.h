#pragma once

#include "REL/Relocation.h"
namespace EXCO
{
namespace HOOKS
{

void InstallHooks();

struct PlayerUpdateHook
{
    static void Call(RE::PlayerCharacter* a_this, float a_deltaTime);
    static inline REL::Relocation<decltype(Call)> func;
};

struct NPCUpdateHook
{
    static void Call(RE::Actor* a_this, float a_deltaTime);
    static inline REL::Relocation<decltype(Call)> func;
};

struct JumpGetScaleHook
{
    static float Call(RE::TESObjectREFR* a_this);
    static inline REL::Relocation<decltype(Call)> func;
};

struct StartCastingHook
{
    static void Call(RE::ActorMagicCaster* a_this);
    static inline REL::Relocation<decltype(Call)> func;
};

struct CasterUpdateHook
{
    static void Call(RE::ActorMagicCaster* a_this, float a_deltaTime);
    static inline REL::Relocation<decltype(Call)> func;
};

struct AttackStamCostHook
{
    static float Call(RE::ActorValueOwner* a_this, RE::BGSAttackData* a_attack);
    static inline REL::Relocation<decltype(Call)> func;
};

struct AttackChance
{
    static float Call(RE::Actor* a_attacker, RE::Actor* a_victim, RE::BGSAttackData* a_attack);
    static inline REL::Relocation<decltype(Call)> func;
};

struct FireArrowHook
{
    static void Call(RE::TESObjectWEAP* a_weapon, RE::TESObjectREFR* a_source, RE::TESAmmo* a_overwriteAmmo,
                     RE::EnchantmentItem* a_ammoEnch, RE::AlchemyItem* a_poison);
    static inline REL::Relocation<decltype(Call)> func;
};
} // namespace HOOKS
} // namespace EXCO