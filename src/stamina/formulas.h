#pragma once

// Pure stamina-cost math. Engine-independent: no RE/SKSE includes, so this header is compiled
// into the unit-test executable on any host. Every function is a direct transcription of the
// formulas the plugin used before the refactor (pinned by tests/formulas-tests.cpp).

#include <algorithm>
#include <cmath>

namespace ExhaustingCombat::Stamina
{
// Vanilla weapon weights span roughly 2 (dagger) to 33 (heaviest two-hander). Item weight is
// normalised into this band to produce a 1x..2x attack multiplier.
inline constexpr float kLightestVanillaWeaponWeight = 2.0f;
inline constexpr float kHeaviestVanillaWeaponWeight = 33.0f;

// Items lighter than this (including weightless ones) are treated as weighing this much.
inline constexpr float kMinimumItemWeight = 1.0f;

// Guards the encumbrance ratio against a zero/negative carry weight.
inline constexpr float kMinimumCarryWeight = 1.0f;

// Carrying exactly your carry weight adds this share of the base cost (0.5 = +50%).
inline constexpr float kFullEncumbranceCostShare = 0.5f;

// Spell cost: base + min(sqrt(magicka) * scale, cap). A 0-magicka spell still counts as 1.
inline constexpr float kMinimumMagickaCost      = 1.0f;
inline constexpr float kCastCostMagickaScale    = 0.70f;
inline constexpr float kMaxCastCostFromMagicka  = 25.0f;

// Skill level at which a skill multiplier reaches its full effect.
inline constexpr float kSkillLevelForFullEffect = 100.0f;

/// Clamps a final cost to >= 0. NaN becomes 0 so a broken perk or config can never heal stamina.
[[nodiscard]] inline float ClampCost(float a_cost) noexcept
{
    if (std::isnan(a_cost)) {
        return 0.0f;
    }
    return std::max(0.0f, a_cost);
}

/// 1.0 for the lightest vanilla weapon (or lighter), 2.0 for the heaviest (or heavier).
[[nodiscard]] inline float WeightMultiplier(float a_itemWeight) noexcept
{
    const float weight     = std::max(a_itemWeight, kMinimumItemWeight);
    const float normalised = (weight - kLightestVanillaWeaponWeight) / (kHeaviestVanillaWeaponWeight - kLightestVanillaWeaponWeight);
    return 1.0f + std::clamp(normalised, 0.0f, 1.0f);
}

/// Base cost raised by how loaded the actor is: base * (1 + 0.5 * carried / carryWeight).
/// The ratio is not capped, so over-encumbered actors pay more than +50%.
[[nodiscard]] inline float EncumbranceScaledCost(float a_baseCost, float a_carriedWeight, float a_carryWeight) noexcept
{
    const float ratio = a_carriedWeight / std::max(a_carryWeight, kMinimumCarryWeight);
    return a_baseCost + ratio * (a_baseCost * kFullEncumbranceCostShare);
}

/// Cost factor from a skill: 1 - (skill / 100) * multiplier, never below 0.
/// The multiplier is clamped to [0, 1] here as a last line of defence; config validation
/// already clamps it on load.
[[nodiscard]] inline float SkillCostFactor(float a_skillLevel, float a_multiplier) noexcept
{
    const float multiplier = std::clamp(a_multiplier, 0.0f, 1.0f);
    return std::max(0.0f, 1.0f - (a_skillLevel / kSkillLevelForFullEffect) * multiplier);
}

/// Stamina cost of casting a spell (per cast for fire-and-forget, per second for concentration).
/// A base cost <= 0 disables casting costs entirely and is returned unchanged (no magicka part).
[[nodiscard]] inline float CastCost(float a_baseCost, float a_magickaCost, float a_skillFactor) noexcept
{
    if (a_baseCost <= 0.0f) {
        return a_baseCost;
    }
    const float magicka  = std::max(a_magickaCost, kMinimumMagickaCost);
    const float addition = std::min(std::sqrt(magicka) * kCastCostMagickaScale, kMaxCastCostFromMagicka);
    return (a_baseCost + addition) * a_skillFactor;
}

struct AttackCostInputs
{
    float baseCost        = 0.0f;
    float itemWeight      = 0.0f;
    float attackMult      = 1.0f;
    float powerAttackMult = 1.0f;
    bool  isPowerAttack   = false;
    float skillFactor     = 1.0f;  // product of every SkillCostFactor that applies
};

/// The power-attack multiplier actually applied: the setting for power attacks, 1 otherwise.
[[nodiscard]] inline float AppliedPowerAttackMult(const AttackCostInputs& a_in) noexcept
{
    return a_in.isPowerAttack ? a_in.powerAttackMult : 1.0f;
}

/// Melee attack / bash cost before perks: base * weightMult * attackMult * (powerAttackMult) * skill.
[[nodiscard]] inline float AttackCost(const AttackCostInputs& a_in) noexcept
{
    return a_in.baseCost * WeightMultiplier(a_in.itemWeight) * a_in.attackMult * AppliedPowerAttackMult(a_in) * a_in.skillFactor;
}

/// Concentration spells pay a fire-and-forget cast's cost per second, times their own multiplier.
[[nodiscard]] inline float ConcentrationCostPerSecond(float a_castCost, float a_concentrationMult) noexcept
{
    return a_castCost * a_concentrationMult;
}
}  // namespace ExhaustingCombat::Stamina
