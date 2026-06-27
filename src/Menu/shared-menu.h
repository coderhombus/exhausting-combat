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
inline std::string av_section       = "AV Scaling";
inline std::string settings_section = "Settings";
inline std::string restore_button   = "Reset Defaults";
inline std::string system_section   = "System";
} // namespace TITLE

namespace LAB
{
// NPCs
inline std::string npc_weight_mode  = "Equipped Weight Only##npc";
inline std::string npc_attack_cost  = "Attack Cost##npc";
inline std::string npc_attack_mult  = "Attack Mult##npc";
inline std::string npc_pattack_mult = "Power Attack Mult##npc";
inline std::string npc_sprint_cost  = "Sprint Cost##npc";
inline std::string npc_swim_cost    = "Swim Cost##npc";
inline std::string npc_sneak_cost   = "Sneak Cost##npc";
inline std::string npc_cast_cost    = "Casting Cost##npc";
// not functional atm, don't expose in menu
inline std::string npc_jump_cost = "Jumping Cost##npc";
// may not be functional and no need to expose it in menu before testing it
inline std::string npc_horseride_sprint_cost = "Horse Riding Cost##npc";
inline std::string npc_bow_draw_cost         = "Bow Drawing Cost##npc";
inline std::string npc_bow_shoot_cost        = "Bow Shooting Cost##npc";

inline std::string npc_running_cost = "Running Cost##npc";

// Player
inline std::string player_weight_mode           = "Equipped Weight Only##player";
inline std::string player_attack_cost           = "Attack Cost##player";
inline std::string player_attack_mult           = "Attack Mult##player";
inline std::string player_pattack_mult          = "Power Attack Mult##player";
inline std::string player_sprint_cost           = "Sprint Cost##player";
inline std::string player_swim_cost             = "Swim Cost##player";
inline std::string player_sneak_cost            = "Sneak Cost##player";
inline std::string player_cast_cost             = "Casting Cost##player";
inline std::string player_jump_cost             = "Jumping Cost##player";
inline std::string player_horseride_sprint_cost = "Horse Riding Cost##player";
inline std::string player_bow_draw_cost         = "Bow Drawing Cost##player";
inline std::string player_bow_shoot_cost        = "Bow Shooting Cost##player";
inline std::string player_running_cost          = "Running Cost##player";
// AV Scaling
inline std::string stamina_av_mult_onehanded       = "OneHanded Scaling##avscaling";
inline std::string stamina_av_mult_twohanded       = "TwoHanded Scaling##avscaling";
inline std::string stamina_av_mult_archery         = "Archery Scaling##avscaling";
inline std::string stamina_av_mult_block           = "Block Scaling##avscaling";
inline std::string stamina_av_mult_heavyarmor      = "HeavyArmor Scaling##avscaling";
inline std::string stamina_av_mult_lightarmor      = "LightArmor Scaling##avscaling";
inline std::string stamina_av_mult_sneak           = "Sneak Scaling##avscaling";
inline std::string stamina_av_mult_alteration      = "Alteration Scaling##avscaling";
inline std::string stamina_av_mult_illusion        = "Illusion Scaling##avscaling";
inline std::string stamina_av_mult_destruction     = "Destruction Scaling##avscaling";
inline std::string stamina_av_mult_conjuration     = "Conjuration Scaling##avscaling";
inline std::string stamina_av_mult_restoration     = "Restoration Scaling##avscaling";
inline std::string stamina_av_mult_speedmult       = "SpeedMult Scaling##avscaling";
inline std::string stamina_av_mult_inventoryweight = "InventoryWeight Scaling##avscaling";
} // namespace LAB
namespace TOOL
{

// NPCs
inline std::string npc_weight_mode  = "Only equipped items contribute to weight calculations";
inline std::string npc_attack_cost  = "Stamina cost for normal attacks";
inline std::string npc_attack_mult  = "Stamina mult for normal attacks";
inline std::string npc_pattack_mult = "Stamina mult for power attacks";
inline std::string npc_sprint_cost  = "Stamina cost for Sprinting, replaces vanilla "
                                      "sprint cost calculation. vanilla default 7";
inline std::string npc_swim_cost    = "Stamina cost for swimming";
inline std::string npc_sneak_cost   = "Stamina cost for sneaking";
inline std::string npc_cast_cost    = "Stamina cost for casting";
// not functional atm, don't expose in menu
inline std::string npc_jump_cost = "Stamina cost for jumping";
// may not be functional and no need to expose it in menu before testing it
inline std::string npc_horseride_sprint_cost = "Stamina cost for sprinting on horseback";
inline std::string npc_bow_draw_cost         = "Stamina cost for drawing a bow without zoom. Affects the entire reload "
                                               "animation of crossbows!";
inline std::string npc_bow_shoot_cost        = "Stamina cost for firing an arrow/bolt";

inline std::string npc_running_cost = "Stamina cost for running";

// Player
inline std::string player_weight_mode           = "Only equipped items contribute to weight calculations";
inline std::string player_attack_cost           = "Stamina Cost for normal attacks";
inline std::string player_attack_mult           = "Stamina Mult for normal attacks";
inline std::string player_pattack_mult          = "Stamina Mult for power attacks";
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
inline std::string player_running_cost   = "Stamina cost for running";

inline std::string stamina_av_mult_onehanded       = "Scale relevant stamina costs by Onehanded value";
inline std::string stamina_av_mult_twohanded       = "Scale relevant stamina costs by TwoHanded value";
inline std::string stamina_av_mult_archery         = "Scale relevant stamina costs by Archery value";
inline std::string stamina_av_mult_block           = "Scale relevant stamina costs by Block value";
inline std::string stamina_av_mult_heavyarmor      = "Scale relevant stamina costs by HeavyArmor value";
inline std::string stamina_av_mult_lightarmor      = "Scale relevant stamina costs by LightArmor value";
inline std::string stamina_av_mult_sneak           = "Scale relevant stamina costs by Sneak value";
inline std::string stamina_av_mult_alteration      = "Scale relevant stamina costs by Alteration value";
inline std::string stamina_av_mult_illusion        = "Scale relevant stamina costs by Illusion value";
inline std::string stamina_av_mult_destruction     = "Scale relevant stamina costs by Destruction value";
inline std::string stamina_av_mult_conjuration     = "Scale relevant stamina costs by Conjuration value";
inline std::string stamina_av_mult_restoration     = "Scale relevant stamina costs by Restoration value";
inline std::string stamina_av_mult_speedmult       = "Scale relevant stamina costs by SpeedMult value";
inline std::string stamina_av_mult_inventoryweight = "Scale relevant stamina costs by InventoryWeight value";
} // namespace TOOL

namespace VAR
{

inline bool npc_weight_mode{};
inline float npc_attack_cost{};
inline float npc_attack_mult{};
inline float npc_pattack_mult{};
inline float npc_sprint_cost{};
inline float npc_swim_cost{};
inline float npc_sneak_cost{};
inline float npc_cast_cost{};
inline float npc_jump_cost{};
inline float npc_horseride_sprint_cost{};
inline float npc_bow_draw_cost{};
inline float npc_bow_shoot_cost{};
inline float npc_running_cost{};

inline bool player_weight_mode{};
inline float player_attack_cost{};
inline float player_attack_mult{};
inline float player_pattack_mult{};
inline float player_sprint_cost{};
inline float player_swim_cost{};
inline float player_sneak_cost{};
inline float player_cast_cost{};
inline float player_jump_cost{};
inline float player_horseride_sprint_cost{};
inline float player_bow_draw_cost{};
inline float player_bow_shoot_cost{};
inline float player_running_cost{};

inline float stamina_av_mult_onehanded{};
inline float stamina_av_mult_twohanded{};
inline float stamina_av_mult_archery{};
inline float stamina_av_mult_block{};
inline float stamina_av_mult_heavyarmor{};
inline float stamina_av_mult_lightarmor{};
inline float stamina_av_mult_sneak{};
inline float stamina_av_mult_alteration{};
inline float stamina_av_mult_illusion{};
inline float stamina_av_mult_destruction{};
inline float stamina_av_mult_conjuration{};
inline float stamina_av_mult_restoration{};
inline float stamina_av_mult_speedmult{};
inline float stamina_av_mult_inventoryweight{};
} // namespace VAR

} // namespace MENU
} // namespace EXCO
