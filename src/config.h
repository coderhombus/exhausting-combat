#pragma once
#include "REX/REX/TOML.h"
#include "mod-data.h"

namespace EXCO::CONFIG
{
inline REX::TOML::Bool equipped_weight_only_npc{STAM_NPC, "bNPCEquippedWeightOnly", true};
inline REX::TOML::Bool equipped_weight_only_player{STAM_PL, "bPlayerEquippedWeightOnly", true};

inline REX::TOML::F32 stamina_cost_attack_npc{STAM_NPC, "fNPCStaminaCostAttack", 9.F};
inline REX::TOML::F32 stamina_cost_attack_player{STAM_PL, "fPlayerStaminaCostAttack", 9.F};
inline REX::TOML::F32 stamina_mult_attack_npc{STAM_NPC, "fNPCStaminaMultAttack", 1.414F};
inline REX::TOML::F32 stamina_mult_attack_player{STAM_PL, "fPlayerStaminaMultAttack", 1.414F};
inline REX::TOML::F32 stamina_mult_pattack_npc{STAM_NPC, "fNPCStaminaMultPAtack", 2.0F};
inline REX::TOML::F32 stamina_mult_pattack_player{STAM_PL, "fPlayerStaminaMultPAttack", 2.0F};
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


// Used for both players and npcs
inline REX::TOML::F32 stamina_av_mult_onehanded{STAM_AV, "fAVOneHandedMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_twohanded{STAM_AV, "fAVTwoHandedMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_archery{STAM_AV, "fAVArcheryMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_block{STAM_AV, "fAVBlockMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_heavyarmor{STAM_AV, "fAVHeavyArmorMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_lightarmor{STAM_AV, "fAVLightArmorMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_sneak{STAM_AV, "fAVSneakMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_alteration{STAM_AV, "fAVAlterationMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_illusion{STAM_AV, "fAVIllusionMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_destruction{STAM_AV, "fAVDestructionMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_conjuration{STAM_AV, "fAVConjurationMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_restoration{STAM_AV, "fAVRestorationMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_speedmult{STAM_AV, "fAVSpeedMultMult", 1.F};
inline REX::TOML::F32 stamina_av_mult_inventoryweight{STAM_AV, "fAVInventoryWeightMult", 1.F};

inline REX::TOML::F32 stamina_cost_running_npc{STAM_NPC, "fNPCStaminaCostRunning", 1.F};
inline REX::TOML::F32 stamina_cost_running_player{STAM_PL, "fPlayerStaminaCostRunning", 1.F};

void UpdateSettings(bool a_save = false);
} // namespace EXCO::CONFIG
