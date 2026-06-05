#pragma once

#include "API/PerkEntryPointExtenderAPI.h"

namespace EXCO
{

namespace DATA
{
inline constexpr auto MOD_VERSION = 1;
inline constexpr auto MOD_NAME    = "Exhaustion Combat";
} // namespace DATA

inline constexpr auto TOML_DEFAULT       = "Data/SKSE/Plugins/exhausting-combat.toml";
inline constexpr auto TOML_CUSTOM        = "Data/SKSE/Plugins/exhausting-combat_custom.toml";
inline constexpr auto STAM_NPC           = "Stamina.NPC";
inline constexpr auto STAM_PL            = "Stamina.Player";
inline constexpr auto SETTING_EXHAUSTION = "Exhaustion";
inline constexpr auto SETTING_NPCS       = "NPCS";

namespace ENTRIES
{
inline constexpr auto attackStam                 = "NormalAttackStamina";
inline constexpr RE::PerkEntryPoint attackStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto jumpStam                 = "JumpStamina";
inline constexpr RE::PerkEntryPoint jumpStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto sneakStam                 = "SneakStamina";
inline constexpr RE::PerkEntryPoint sneakStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto sprintStam                 = "SprintStamina";
inline constexpr RE::PerkEntryPoint sprintStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto mountSprintStam                 = "MountSprintStamina";
inline constexpr RE::PerkEntryPoint mountSprintStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto bowStam                 = "BowStamina";
inline constexpr RE::PerkEntryPoint bowStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto bowDrawStam                 = "BowDrawStamina";
inline constexpr RE::PerkEntryPoint bowDrawStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto castStam                 = "CastStamina";
inline constexpr RE::PerkEntryPoint castStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;

inline constexpr auto setBowDrain                 = "SetBowDrain";
inline constexpr RE::PerkEntryPoint setBowDrainEP = RE::PerkEntryPoint::kCanDualCastSpell;

inline constexpr auto swimStam                 = "SwimStamina";
inline constexpr RE::PerkEntryPoint swimStamEP = RE::PerkEntryPoint::kModPowerAttackStamina;
} // namespace ENTRIES

} // namespace EXCO