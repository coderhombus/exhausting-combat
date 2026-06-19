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
    static inline bool GetMount(RE::Actor* a_actor, RE::ActorPtr* a_mountOut)
    {
        using func_t = decltype(&GetMount);
        REL::Relocation<func_t> func{RELOCATION_ID(37757, 38702)};
        return func(a_actor, a_mountOut);
    }
};
} // namespace EXCO