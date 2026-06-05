#pragma once

namespace EXCO
{
struct Util
{
    static RE::Actor* GetDamagedActorIfMount(RE::Actor* a_actor);
    static bool IsPlayerAndZoom(const RE::Actor* a_actor);
    static bool IsBowDrawNoZoom(const RE::Actor* a_actor);
    static void InterruptAttack(RE::Actor* a_actor);
    static inline RE::TESGlobal* stam_cost_pl_glob;
    static void LookupStaminaCostGlobal();
};
} // namespace EXCO