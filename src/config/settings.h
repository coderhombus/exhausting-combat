#pragma once

// The single source of truth for every configurable value: TOML section/key, default, range,
// menu label, tooltip/TOML comment and menu visibility all live in settings.cpp.
// Engine-independent; gameplay code reaches it through Config::Get().

#include <cstdint>
#include <vector>

#include "config/parameter.h"

namespace ExhaustingCombat::Config
{
/// Per-actor-type stamina costs. The player and NPCs each have one; their defaults are
/// identical (player values are canonical, NPC values start as copies).
struct ActorProfile
{
    BoolParameter equippedWeightOnly;
    BoolParameter combatOnly;

    FloatParameter attackCost;
    FloatParameter attackMult;
    FloatParameter powerAttackMult;
    FloatParameter sprintCost;
    FloatParameter swimCost;
    FloatParameter sneakCost;
    FloatParameter runCost;
    FloatParameter castCost;
    FloatParameter bowDrawCost;
    FloatParameter bowShotCost;
    FloatParameter jumpCost;
    FloatParameter mountSprintCost;
};

/// Skills whose level can reduce stamina costs. Engine code maps RE::ActorValue onto these.
enum class Skill : std::uint8_t
{
    OneHanded,
    TwoHanded,
    Archery,
    Block,
    HeavyArmor,
    LightArmor,
    Sneak,
    Alteration,
    Illusion,
    Destruction,
    Conjuration,
    Restoration,
    SpeedMult,        // not implemented yet (parameter hidden in the menu)
    InventoryWeight,  // not implemented yet (parameter hidden in the menu)
};

struct SkillScaling
{
    FloatParameter oneHanded;
    FloatParameter twoHanded;
    FloatParameter archery;
    FloatParameter block;
    FloatParameter heavyArmor;
    FloatParameter lightArmor;
    FloatParameter sneak;
    FloatParameter alteration;
    FloatParameter illusion;
    FloatParameter destruction;
    FloatParameter conjuration;
    FloatParameter restoration;
    FloatParameter speedMult;
    FloatParameter inventoryWeight;

    [[nodiscard]] const FloatParameter& For(Skill a_skill) const noexcept;
};

/// spdlog level numbers: 0 trace, 1 debug, 2 info, 3 warn, 4 error, 5 critical, 6 off.
inline constexpr std::int32_t kLogLevelTrace = 0;
inline constexpr std::int32_t kLogLevelInfo  = 2;
inline constexpr std::int32_t kLogLevelOff   = 6;

struct GeneralSettings
{
    IntParameter logLevel;
};

struct Settings
{
    Settings();

    GeneralSettings general;
    ActorProfile    player;
    ActorProfile    npc;
    BoolParameter   exemptNpcsWithLowMaxStamina;  // NPC-only option
    SkillScaling    skills;

    [[nodiscard]] const ActorProfile& Profile(bool a_isPlayer) const noexcept { return a_isPlayer ? player : npc; }

    /// Every parameter, in file and menu order.
    [[nodiscard]] std::vector<ParameterRef> All();

    void ResetToDefaults();
};

/// Process-wide settings instance (constructed on first use, so no static-init-order issues).
[[nodiscard]] Settings& Get();
}  // namespace ExhaustingCombat::Config
