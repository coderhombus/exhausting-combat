#pragma once

namespace EXCO
{
namespace MENU
{
namespace SHARED
{
void LoadFromConfig();
void RestoreDefaults();

} // namespace SHARED

namespace TITLE
{
inline std::string mod_name         = "Exhausting Combat";
inline std::string npc_section      = "NPC Stamina";
inline std::string player_section   = "Player Stamina";
inline std::string settings_section = "Settings";
inline std::string restore_button   = "Reset Defaults";
inline std::string system_section   = "System";
} // namespace TITLE

namespace LAB
{
// NPCs
inline std::string npc_attack_cost = "Attack Cost##npc";
inline std::string npc_sprint_cost = "Sprint Cost##npc";
inline std::string npc_swim_cost   = "Swim Cost##npc";
inline std::string npc_sneak_cost  = "Sneak Cost##npc";
inline std::string npc_cast_cost   = "Casting Cost##npc";
// not functional atm, don't expose in menu
inline std::string npc_jump_cost = "Jumping Cost##npc";
// may not be functional and no need to expose it in menu before testing it
inline std::string npc_horseride_sprint_cost = "Horse Riding Cost##npc";
inline std::string npc_bow_draw_cost         = "Bow Drawing Cost##npc";
inline std::string npc_bow_shoot_cost        = "Bow Shooting Cost##npc";
// Player
inline std::string player_attack_cost           = "Attack Cost##player";
inline std::string player_sprint_cost           = "Sprint Cost##player";
inline std::string player_swim_cost             = "Swim Cost##player";
inline std::string player_sneak_cost            = "Sneak Cost##player";
inline std::string player_cast_cost             = "Casting Cost##player";
inline std::string player_jump_cost             = "Jumping Cost##player";
inline std::string player_horseride_sprint_cost = "Horse Riding Cost##player";
inline std::string player_bow_draw_cost         = "Bow Drawing Cost##player";
inline std::string player_bow_shoot_cost        = "Bow Shooting Cost##player";
} // namespace LAB
namespace TOOL
{

// NPCs
inline std::string npc_attack_cost = "Stamina cost for normal attacks";
inline std::string npc_sprint_cost = "Stamina cost for Sprinting, replaces vanilla "
                                     "sprint cost calculation. vanilla default 7";
inline std::string npc_swim_cost   = "Stamina cost for swimming";
inline std::string npc_sneak_cost  = "Stamina cost for sneaking";
inline std::string npc_cast_cost   = "Stamina cost for casting";
// not functional atm, don't expose in menu
inline std::string npc_jump_cost = "Stamina cost for jumping";
// may not be functional and no need to expose it in menu before testing it
inline std::string npc_horseride_sprint_cost = "Stamina cost for sprinting on horseback";
inline std::string npc_bow_draw_cost         = "Stamina cost for drawing a bow without zoom. Affects the entire reload "
                                               "animation of crossbows!";
inline std::string npc_bow_shoot_cost        = "Stamina cost for firing an arrow/bolt";
// Player
inline std::string player_attack_cost           = "Stamina Cost for normal attacks";
inline std::string player_sprint_cost           = "Stamina Cost for Sprinting, replaces vanilla "
                                                  "sprint cost calculation. vanilla default 7";
inline std::string player_swim_cost             = "Stamina cost for swimming";
inline std::string player_sneak_cost            = "Stamina cost for sneaking";
inline std::string player_cast_cost             = "Stamina cost for casting";
inline std::string player_jump_cost             = "Stamina cost for jumping";
inline std::string player_horseride_sprint_cost = "Stamina cost for sprinting on horseback";
inline std::string player_bow_draw_cost = "Stamina cost for drawing a bow without zoom. Affects the entire reload "
                                          "animation of crossbows!";

inline std::string player_bow_shoot_cost = "Stamina cost for firing an arrow/bolt";
} // namespace TOOL

namespace VAR
{

inline float npc_attack_cost{};
inline float npc_sprint_cost{};
inline float npc_swim_cost{};
inline float npc_sneak_cost{};
inline float npc_cast_cost{};
inline float npc_jump_cost{};
inline float npc_horseride_sprint_cost{};
inline float npc_bow_draw_cost{};
inline float npc_bow_shoot_cost{};

inline float player_attack_cost{};
inline float player_sprint_cost{};
inline float player_swim_cost{};
inline float player_sneak_cost{};
inline float player_cast_cost{};
inline float player_jump_cost{};
inline float player_horseride_sprint_cost{};
inline float player_bow_draw_cost{};
inline float player_bow_shoot_cost{};
} // namespace VAR

} // namespace MENU
} // namespace EXCO