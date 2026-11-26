#pragma once

// Stamina charges for every action the mod handles. Hooks call these and nothing else.
// Each function: gate -> base cost (formulas.h) -> PEPE perks -> clamp -> exemptions -> pay,
// and writes exactly one log line per event (see "Log format" in costs.cpp).

#include <string_view>

#include <RE/Skyrim.h>

namespace ExhaustingCombat::Stamina
{
/// Per-frame drains: movement (exactly one of run/sneak/sprint/swim/mount sprint) and bow draw.
void ChargeContinuousCosts(RE::Actor* a_actor, float a_deltaSeconds);

/// One jump. The hook may report a mounted jump for the rider, the horse or both; the horse
/// pays once, using the rider's settings.
void ChargeJump(RE::Actor* a_actor);

/// Fire-and-forget / charged cast starting. Returns false if the cast must be interrupted.
[[nodiscard]] bool ChargeSpellCast(RE::ActorMagicCaster* a_caster);

/// One frame of a concentration spell. Returns false if the cast must be interrupted.
[[nodiscard]] bool ChargeConcentration(RE::ActorMagicCaster* a_caster, float a_deltaSeconds);

/// One arrow or bolt. Returns false if the shot must not be fired.
[[nodiscard]] bool ChargeBowShot(RE::Actor* a_actor, RE::TESObjectWEAP* a_weapon);

/// The engine's attack-stamina-cost query. Returns the cost the engine should apply to the
/// attacker. When the player attacks on horseback, the horse is charged here instead and the
/// engine gets 0.
[[nodiscard]] float ResolveAttackStaminaCost(RE::Actor* a_actor, const RE::BGSAttackData* a_attack, float a_vanillaCost);

/// AI attack selection: false if the attacker cannot afford this attack.
[[nodiscard]] bool CanAffordAttack(RE::Actor* a_attacker, const RE::BGSAttackData* a_attack);

/// A melee swing animation event on a mounted rider (see stamina/mounted-combat.h). Charges
/// the mount unless the same attack was already charged through ResolveAttackStaminaCost.
void ChargeMountedSwing(RE::Actor& a_rider, std::string_view a_eventName);

/// Looks up forms. Call at kDataLoaded.
void InitializeForms();
}  // namespace ExhaustingCombat::Stamina
