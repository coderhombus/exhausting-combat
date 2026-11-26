#pragma once

// Stamina charges for every action the mod handles. Hooks call these and nothing else.
// Each function: gate -> base cost (formulas.h) -> PEPE perks -> clamp -> exemptions -> pay.

#include <RE/Skyrim.h>

namespace ExhaustingCombat::Stamina
{
/// Per-frame drains: movement (exactly one of run/sneak/sprint/swim/mount sprint) and bow draw.
void ChargeContinuousCosts(RE::Actor* a_actor, float a_deltaSeconds);

/// One jump.
void ChargeJump(RE::Actor* a_actor);

/// Fire-and-forget / charged cast starting. Returns false if the cast must be interrupted.
[[nodiscard]] bool ChargeSpellCast(RE::ActorMagicCaster* a_caster);

/// One frame of a concentration spell. Returns false if the cast must be interrupted.
[[nodiscard]] bool ChargeConcentration(RE::ActorMagicCaster* a_caster, float a_deltaSeconds);

/// One arrow or bolt. Returns false if the shot must not be fired.
[[nodiscard]] bool ChargeBowShot(RE::Actor* a_actor, RE::TESObjectWEAP* a_weapon);

/// Whether an attack cost is for real or only being evaluated (AI attack choice); affects logging.
enum class AttackCostUse
{
    Charge,
    Preview,
};

/// Stamina cost of a melee attack or bash after perks and exemptions. 0 means "no cost from this
/// mod" (gated out, unarmed, exempt). The engine applies the returned value itself.
[[nodiscard]] float ComputeAttackCost(RE::Actor* a_actor, const RE::BGSAttackData* a_attack, AttackCostUse a_use);

/// Publishes the player's last attack cost to the SST_StaminaCostPlayer global (if present).
void PublishPlayerAttackCost(float a_cost);

/// Looks up forms. Call at kDataLoaded.
void InitializeForms();
}  // namespace ExhaustingCombat::Stamina
