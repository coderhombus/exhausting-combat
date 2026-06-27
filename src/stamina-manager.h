#pragma once

#include "RE/A/Actor.h"
namespace EXCO
{
struct StaminaCost
{
    static void ManageSneakStamina(RE::Actor* a_actor, float a_deltaTime);
    static void ManageJumpStamina(RE::Actor* a_actor);
    static void ManageSprintStamina(RE::Actor* a_actor, float a_deltaTime);
    static bool ManageCastStamina(RE::ActorMagicCaster* a_caster);
    static bool ManageConcentrationStamina(RE::ActorMagicCaster* a_caster, float a_deltaTime);
    static float ManageAttackStamina(RE::Actor* a_actor, const RE::BGSAttackData* a_attack);
    static bool ManageBowStamina(RE::Actor* a_actor, RE::TESObjectWEAP* a_weapon);
    static void ManageBowDraw(RE::Actor* a_actor, float a_deltaTime);
    static float GetAttackStaminaCost(RE::Actor* a_actor, bool isBash, bool isPowerAttack);
    static float GetAttackStaminaCost_Safe(RE::ActorValueOwner* a_actor, RE::BGSAttackData* a_attack);
    static void ManageSwimStamina(RE::Actor* a_actor, float a_deltaTime);
    static void ManageRunningStamina(RE::Actor* a_actor, float a_deltaTime);
    static void GetAPIModifier();

  private:
    // static inline std::mutex mut;
    static float CalculateAttackCost(RE::Actor* a_actor, bool isBash, bool isPowerAttack);
    static float CalculateCastCost(const RE::ActorMagicCaster* a_caster, const RE::MagicItem* a_spell);
    static float CalculateJumpCost(RE::Actor* a_actor);
    static float CalculateSprintCost(RE::Actor* a_actor);
    static float CalculateWeightModi(float a_baseCost, const RE::Actor* a_actor);
    static float CalculateHorseSprintCost(RE::Actor* a_actor);
    static float CalculateSwimCost(RE::Actor* a_actor);
    static float CalculateRunningCost(RE::Actor* a_actor);
    static bool ActorValidAndNotGod(RE::Actor* a_actor);
    static float AVScaling(RE::Actor* a_actor, RE::ActorValue a_akValue);
    static RE::ActorValue AssociatedSkillWeapon(RE::TESObjectWEAP* weap);
    static RE::TESObjectWEAP* GetRelevantWeapon(RE::Actor* a_actor);
    static bool DumpSpecificVersion();
};
} // namespace EXCO
