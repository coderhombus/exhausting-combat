#include "shared-menu.h"

#include "config.h"


void EXCO::MENU::SHARED::LoadFromConfig()
{

    using namespace VAR;
    // npcs
    npc_attack_cost           = CONFIG::stamina_cost_attack_npc.GetValue();
    npc_sprint_cost           = CONFIG::stamina_cost_sprint_npc.GetValue();
    npc_swim_cost             = CONFIG::stamina_cost_swim_npc.GetValue();
    npc_cast_cost             = CONFIG::stamina_cost_casting_npc.GetValue();
    npc_horseride_sprint_cost = CONFIG::stamina_cost_sprint_horse_npc.GetValue();
    npc_jump_cost             = CONFIG::stamina_cost_jump_npc.GetValue();
    npc_sneak_cost            = CONFIG::stamina_cost_sneak_npc.GetValue();
    npc_bow_draw_cost         = CONFIG::stamina_cost_bow_drain_npc.GetValue();
    npc_bow_shoot_cost        = CONFIG::stamina_cost_bow_npc.GetValue();
    // player
    player_attack_cost           = CONFIG::stamina_cost_attack_player.GetValue();
    player_sprint_cost           = CONFIG::stamina_cost_sprint_player.GetValue();
    player_swim_cost             = CONFIG::stamina_cost_swim_player.GetValue();
    player_cast_cost             = CONFIG::stamina_cost_casting_player.GetValue();
    player_horseride_sprint_cost = CONFIG::stamina_cost_sprint_horse_player.GetValue();
    player_jump_cost             = CONFIG::stamina_cost_jump_player.GetValue();
    player_sneak_cost            = CONFIG::stamina_cost_sneak_player.GetValue();
    player_bow_draw_cost         = CONFIG::stamina_cost_bow_drain_player.GetValue();
    player_bow_shoot_cost        = CONFIG::stamina_cost_bow_player.GetValue();
}

void EXCO::MENU::SHARED::RestoreDefaults()
{

    using namespace VAR;
    // npcs
    npc_attack_cost           = 10.F;
    npc_sprint_cost           = 7.F;
    npc_swim_cost             = 5.F;
    npc_cast_cost             = 10.F;
    npc_horseride_sprint_cost = 7.F;
    npc_jump_cost             = 10.F;
    npc_sneak_cost            = 2.F;
    npc_bow_draw_cost         = 5.F;
    npc_bow_shoot_cost        = 10.F;
    // player
    player_attack_cost           = 10.F;
    player_sprint_cost           = 7.F;
    player_swim_cost             = 5.F;
    player_cast_cost             = 10.F;
    player_horseride_sprint_cost = 7.F;
    player_jump_cost             = 10.F;
    player_sneak_cost            = 2.F;
    player_bow_draw_cost         = 5.F;
    player_bow_shoot_cost        = 10.F;

    // jump and horse sprint do not work for NPCs afaik. 
    // TODO: test anim event for npc jump as replacer 
    /*
    CONFIG::stamina_cost_sprint_horse_npc.SetValue(npc_horseride_sprint_cost);
    CONFIG::stamina_cost_jump_npc.SetValue(npc_jump_cost);
    */
    CONFIG::stamina_cost_attack_npc.SetValue(npc_attack_cost);
    CONFIG::stamina_cost_sprint_npc.SetValue(npc_sprint_cost);
    CONFIG::stamina_cost_swim_npc.SetValue(npc_swim_cost);
    CONFIG::stamina_cost_casting_npc.SetValue(npc_cast_cost);
    CONFIG::stamina_cost_sneak_npc.SetValue(npc_sneak_cost);
    CONFIG::stamina_cost_bow_drain_npc.SetValue(npc_bow_draw_cost);
    CONFIG::stamina_cost_bow_npc.SetValue(npc_bow_shoot_cost);
    // player
    CONFIG::stamina_cost_attack_player.SetValue(player_attack_cost);
    CONFIG::stamina_cost_sprint_horse_player.SetValue(player_horseride_sprint_cost);
    CONFIG::stamina_cost_jump_player.SetValue(player_jump_cost);
    CONFIG::stamina_cost_swim_player.SetValue(player_swim_cost);
    CONFIG::stamina_cost_sprint_player.SetValue(player_sprint_cost);
    CONFIG::stamina_cost_casting_player.SetValue(player_cast_cost);
    CONFIG::stamina_cost_sneak_player.SetValue(player_sneak_cost);
    CONFIG::stamina_cost_bow_drain_player.SetValue(player_bow_draw_cost);
    CONFIG::stamina_cost_bow_player.SetValue(player_bow_shoot_cost);

    CONFIG::UpdateSettings(true);
}
