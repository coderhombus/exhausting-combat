#pragma once
#include "REX/REX/TOML.h"
#include "mod-data.h"

namespace EXCO::CONFIG
{
inline REX::TOML::F32 stamina_cost_attack_npc{STAM_NPC, "fNPCStaminaCostAttack", 10.F};
inline REX::TOML::F32 stamina_cost_attack_player{STAM_PL, "fPlayerStaminaCostAttack", 10.F};
inline REX::TOML::F32 stamina_cost_jump_npc{STAM_NPC, "fNPCStaminaCostJump", 10.F};
inline REX::TOML::F32 stamina_cost_jump_player{STAM_PL, "fPlayerStaminaCostJump", 15.F};
inline REX::TOML::F32 stamina_cost_sneak_npc{STAM_NPC, "fNPCStaminaCostSneak", 1.F};
inline REX::TOML::F32 stamina_cost_sneak_player{STAM_PL, "fPlayerStaminaCostSneak", 1.F};
inline REX::TOML::F32 stamina_cost_casting_npc{STAM_NPC, "fNPCStaminaCostCast", 10.F};
inline REX::TOML::F32 stamina_cost_casting_player{STAM_PL, "fPlayerStaminaCostCast", 10.F};
inline REX::TOML::F32 stamina_cost_sprint_npc{STAM_NPC, "fNPCStaminaCostSprint", 5.F};
inline REX::TOML::F32 stamina_cost_sprint_player{STAM_PL, "fPlayerStaminaCostSprint", 5.F};

inline REX::TOML::F32 stamina_cost_sprint_horse_npc{STAM_NPC, "fNPCStaminaCostSprintHorse", 10.F};
inline REX::TOML::F32 stamina_cost_sprint_horse_player{STAM_PL, "fPlayerStaminaCostSprintHorse", 10.F};

inline REX::TOML::F32 stamina_cost_bow_npc{STAM_NPC, "fNPCStaminaCostBow", 10.F};
inline REX::TOML::F32 stamina_cost_bow_player{STAM_PL, "fPlayerStaminaCostBow", 10.F};

inline REX::TOML::F32 stamina_cost_bow_drain_npc{STAM_NPC, "fNPCStaminaCostBowDrain", 10.F};
inline REX::TOML::F32 stamina_cost_bow_drain_player{STAM_PL, "fPlayerStaminaCostBowDrain", 10.F};
inline REX::TOML::F32 stamina_cost_swim_npc{STAM_NPC, "fNPCStaminaCostSwim", 5.F};
inline REX::TOML::F32 stamina_cost_swim_player{STAM_PL, "fPlayerStaminaCostSwim", 5.F};

void UpdateSettings(bool a_save = false);
} // namespace EXCO::CONFIG
