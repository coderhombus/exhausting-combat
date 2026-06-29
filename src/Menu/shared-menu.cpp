#include "shared-menu.h"

#include "config.h"


void EXCO::MENU::SHARED::LoadFromConfig()
{

    using namespace VAR;
    // npcs
    npc_attack_cost           = CONFIG::stamina_cost_attack_npc.GetValue();
    npc_attack_mult           = CONFIG::stamina_mult_attack_npc.GetValue();
    npc_pattack_mult          = CONFIG::stamina_mult_pattack_npc.GetValue();
    npc_sprint_cost           = CONFIG::stamina_cost_sprint_npc.GetValue();
    npc_swim_cost             = CONFIG::stamina_cost_swim_npc.GetValue();
    npc_cast_cost             = CONFIG::stamina_cost_casting_npc.GetValue();
    npc_horseride_sprint_cost = CONFIG::stamina_cost_sprint_horse_npc.GetValue();
    npc_weight_mode           = CONFIG::equipped_weight_only_npc.GetValue();
    npc_jump_cost             = CONFIG::stamina_cost_jump_npc.GetValue();
    npc_sneak_cost            = CONFIG::stamina_cost_sneak_npc.GetValue();
    npc_bow_draw_cost         = CONFIG::stamina_cost_bow_drain_npc.GetValue();
    npc_bow_shoot_cost        = CONFIG::stamina_cost_bow_npc.GetValue();
    npc_running_cost          = CONFIG::stamina_cost_running_npc.GetValue();
    // player
    player_attack_cost           = CONFIG::stamina_cost_attack_player.GetValue();
    player_attack_mult           = CONFIG::stamina_mult_attack_player.GetValue();
    player_pattack_mult          = CONFIG::stamina_mult_pattack_player.GetValue();
    player_sprint_cost           = CONFIG::stamina_cost_sprint_player.GetValue();
    player_swim_cost             = CONFIG::stamina_cost_swim_player.GetValue();
    player_cast_cost             = CONFIG::stamina_cost_casting_player.GetValue();
    player_horseride_sprint_cost = CONFIG::stamina_cost_sprint_horse_player.GetValue();
    player_weight_mode           = CONFIG::equipped_weight_only_player.GetValue();
    player_jump_cost             = CONFIG::stamina_cost_jump_player.GetValue();
    player_sneak_cost            = CONFIG::stamina_cost_sneak_player.GetValue();
    player_bow_draw_cost         = CONFIG::stamina_cost_bow_drain_player.GetValue();
    player_bow_shoot_cost        = CONFIG::stamina_cost_bow_player.GetValue();
    player_running_cost          = CONFIG::stamina_cost_running_player.GetValue();
}

void EXCO::MENU::SHARED::RestoreDefaults()
{

    using namespace VAR;
    // npcs
    npc_attack_cost           = 9.F;
    npc_attack_mult           = 1.414F;
    npc_pattack_mult          = 2.0F;
    npc_sprint_cost           = 7.F;
    npc_swim_cost             = 5.F;
    npc_cast_cost             = 10.F;
    npc_horseride_sprint_cost = 7.F;
    npc_weight_mode           = true;
    npc_jump_cost             = 10.F;
    npc_sneak_cost            = 2.F;
    npc_bow_draw_cost         = 5.F;
    npc_bow_shoot_cost        = 10.F;
    npc_running_cost          = 1.F;
    // player
    player_attack_cost           = 9.F;
    player_attack_mult           = 1.414F;
    player_pattack_mult          = 2.0F;
    player_sprint_cost           = 7.F;
    player_swim_cost             = 5.F;
    player_cast_cost             = 10.F;
    player_horseride_sprint_cost = 7.F;
    player_weight_mode           = true;
    player_jump_cost             = 10.F;
    player_sneak_cost            = 2.F;
    player_bow_draw_cost         = 5.F;
    player_bow_shoot_cost        = 10.F;
    player_running_cost          = 1.F;

    stamina_av_mult_onehanded       = 0.F;
    stamina_av_mult_twohanded       = 0.F;
    stamina_av_mult_archery         = 0.F;
    stamina_av_mult_block           = 0.F;
    stamina_av_mult_heavyarmor      = 0.F;
    stamina_av_mult_lightarmor      = 0.F;
    stamina_av_mult_sneak           = 0.F;
    stamina_av_mult_alteration      = 0.F;
    stamina_av_mult_illusion        = 0.F;
    stamina_av_mult_destruction     = 0.F;
    stamina_av_mult_conjuration     = 0.F;
    stamina_av_mult_restoration     = 0.F;
    stamina_av_mult_speedmult       = 0.F;
    stamina_av_mult_inventoryweight = 0.F;

    // jump and horse sprint do not work for NPCs afaik.
    // TODO: test anim event for npc jump as replacer
    /*
    CONFIG::stamina_cost_sprint_horse_npc.SetValue(npc_horseride_sprint_cost);
    CONFIG::stamina_cost_jump_npc.SetValue(npc_jump_cost);
    */
    // jump and horse sprint do not work for NPCs afaik.
    // TODO: test anim event for npc jump as replacer

    CONFIG::stamina_cost_attack_npc.SetValue(npc_attack_cost);
    CONFIG::stamina_mult_attack_npc.SetValue(npc_attack_mult);
    CONFIG::stamina_mult_pattack_npc.SetValue(npc_pattack_mult);
    CONFIG::stamina_cost_sprint_npc.SetValue(npc_sprint_cost);
    CONFIG::stamina_cost_swim_npc.SetValue(npc_swim_cost);
    CONFIG::stamina_cost_casting_npc.SetValue(npc_cast_cost);
    CONFIG::stamina_cost_sneak_npc.SetValue(npc_sneak_cost);
    CONFIG::stamina_cost_bow_drain_npc.SetValue(npc_bow_draw_cost);
    CONFIG::stamina_cost_bow_npc.SetValue(npc_bow_shoot_cost);
    CONFIG::equipped_weight_only_npc.SetValue(npc_weight_mode);
    CONFIG::stamina_cost_running_npc.SetValue(npc_running_cost);
    // player
    CONFIG::stamina_cost_attack_player.SetValue(player_attack_cost);
    CONFIG::stamina_mult_attack_player.SetValue(player_attack_mult);
    CONFIG::stamina_mult_pattack_player.SetValue(player_pattack_mult);
    CONFIG::stamina_cost_sprint_horse_player.SetValue(player_horseride_sprint_cost);
    CONFIG::equipped_weight_only_player.SetValue(player_weight_mode);
    CONFIG::stamina_cost_jump_player.SetValue(player_jump_cost);
    CONFIG::stamina_cost_swim_player.SetValue(player_swim_cost);
    CONFIG::stamina_cost_sprint_player.SetValue(player_sprint_cost);
    CONFIG::stamina_cost_casting_player.SetValue(player_cast_cost);
    CONFIG::stamina_cost_sneak_player.SetValue(player_sneak_cost);
    CONFIG::stamina_cost_bow_drain_player.SetValue(player_bow_draw_cost);
    CONFIG::stamina_cost_bow_player.SetValue(player_bow_shoot_cost);
    CONFIG::stamina_cost_running_player.SetValue(player_running_cost);

    CONFIG::stamina_av_mult_onehanded.SetValue(stamina_av_mult_onehanded);
    CONFIG::stamina_av_mult_twohanded.SetValue(stamina_av_mult_twohanded);
    CONFIG::stamina_av_mult_archery.SetValue(stamina_av_mult_archery);
    CONFIG::stamina_av_mult_block.SetValue(stamina_av_mult_block);
    CONFIG::stamina_av_mult_heavyarmor.SetValue(stamina_av_mult_heavyarmor);
    CONFIG::stamina_av_mult_lightarmor.SetValue(stamina_av_mult_lightarmor);
    CONFIG::stamina_av_mult_sneak.SetValue(stamina_av_mult_sneak);
    CONFIG::stamina_av_mult_alteration.SetValue(stamina_av_mult_alteration);
    CONFIG::stamina_av_mult_illusion.SetValue(stamina_av_mult_illusion);
    CONFIG::stamina_av_mult_destruction.SetValue(stamina_av_mult_destruction);
    CONFIG::stamina_av_mult_conjuration.SetValue(stamina_av_mult_conjuration);
    CONFIG::stamina_av_mult_restoration.SetValue(stamina_av_mult_restoration);
    CONFIG::stamina_av_mult_speedmult.SetValue(stamina_av_mult_speedmult);
    CONFIG::stamina_av_mult_inventoryweight.SetValue(stamina_av_mult_inventoryweight);

    CONFIG::UpdateSettings(true);
}
