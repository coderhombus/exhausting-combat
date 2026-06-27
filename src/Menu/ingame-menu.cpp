#include "ingame-menu.h"

#include "API/FUCK_API.h"
#include "Menu/shared-menu.h"
#include "config.h"
#include "st-fui.h"

namespace EXCO
{

void FMenu::StaminaTool::Draw()
{
    if (FUCK::CollapsingHeader(MENU::TITLE::npc_section.c_str()))
    {

        DrawNPCs();
    }
    if (FUCK::CollapsingHeader(MENU::TITLE::player_section.c_str()))
    {

        DrawPlayer();
    }
    if (FUCK::CollapsingHeader(MENU::TITLE::av_section.c_str()))
    {
        DrawAV();
    }
    if (FUCK::CollapsingHeader(MENU::TITLE::system_section.c_str()))
    {
        DrawSystem();
    }
}
void FMenu::StaminaTool::OnOpen()
{
    MENU::SHARED::LoadFromConfig();
}
void FMenu::StaminaTool::OnClose()
{
    CONFIG::UpdateSettings(true);
}
void FMenu::StaminaTool::DrawNPCs()
{

    using namespace MENU;
    FUCKUtil::GreenTitleText("Stamina for NPCs", 5);
    FUCK::Indent();

    FUCKUtil::SliderFloat(LAB::npc_attack_cost.c_str(), VAR::npc_attack_cost, CONFIG::stamina_cost_attack_npc, 0.0,
                          50.0, TOOL::npc_attack_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_sprint_cost.c_str(), VAR::npc_sprint_cost, CONFIG::stamina_cost_sprint_npc, 0.0,
                          20.0, TOOL::npc_sprint_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_swim_cost.c_str(), VAR::npc_swim_cost, CONFIG::stamina_cost_swim_npc, 0.0, 20.0,
                          TOOL::npc_swim_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_sneak_cost.c_str(), VAR::npc_sneak_cost, CONFIG::stamina_cost_sneak_npc, 0.0, 20.0,
                          TOOL::npc_sneak_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_cast_cost.c_str(), VAR::npc_cast_cost, CONFIG::stamina_cost_casting_npc, 0.0, 50.0,
                          TOOL::npc_cast_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_bow_draw_cost.c_str(), VAR::npc_bow_draw_cost, CONFIG::stamina_cost_bow_drain_npc,
                          0.0, 20.0, TOOL::npc_bow_draw_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_bow_shoot_cost.c_str(), VAR::npc_bow_shoot_cost, CONFIG::stamina_cost_bow_npc, 0.0,
                          50.0, TOOL::npc_bow_shoot_cost.c_str());
    FUCKUtil::SliderFloat(LAB::npc_running_cost.c_str(), VAR::npc_running_cost, CONFIG::stamina_cost_running_npc, 0.0,
                          20.0, TOOL::npc_running_cost.c_str());
    FUCK::Unindent();
};
void FMenu::StaminaTool::DrawPlayer()
{

    using namespace MENU;
    FUCKUtil::GreenTitleText("Stamina for Player", 5);
    FUCK::Indent();

    FUCKUtil::SliderFloat(LAB::player_attack_cost.c_str(), VAR::player_attack_cost, CONFIG::stamina_cost_attack_player,
                          0.0, 50.0, TOOL::player_attack_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_attack_mult.c_str(), VAR::player_attack_mult, CONFIG::stamina_mult_attack_player, 
                          0.0, 50.0, TOOL::player_attack_mult.c_str());
    FUCKUtil::SliderFloat(LAB::player_sprint_cost.c_str(), VAR::player_sprint_cost, CONFIG::stamina_cost_sprint_player,
                          0.0, 20.0, TOOL::player_sprint_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_swim_cost.c_str(), VAR::player_swim_cost, CONFIG::stamina_cost_swim_player, 0.0,
                          20.0, TOOL::player_swim_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_sneak_cost.c_str(), VAR::player_sneak_cost, CONFIG::stamina_cost_sneak_player,
                          0.0, 20.0, TOOL::player_sneak_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_cast_cost.c_str(), VAR::player_cast_cost, CONFIG::stamina_cost_casting_player,
                          0.0, 50.0, TOOL::player_cast_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_bow_draw_cost.c_str(), VAR::player_bow_draw_cost,
                          CONFIG::stamina_cost_bow_drain_player, 0.0, 20.0, TOOL::player_bow_draw_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_bow_shoot_cost.c_str(), VAR::player_bow_shoot_cost,
                          CONFIG::stamina_cost_bow_player, 0.0, 50.0, TOOL::player_bow_shoot_cost.c_str());
    FUCKUtil::Checkbox(LAB::player_weight_mode.c_str(), VAR::player_weight_mode, CONFIG::equipped_weight_only_player,
                       TOOL::player_weight_mode.c_str());
    FUCKUtil::SliderFloat(LAB::player_jump_cost.c_str(), VAR::player_jump_cost, CONFIG::stamina_cost_jump_player, 0.0,
                          50.0, TOOL::player_jump_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_horseride_sprint_cost.c_str(), VAR::player_horseride_sprint_cost,
                          CONFIG::stamina_cost_sprint_horse_player, 0.0, 20.0,
                          TOOL::player_horseride_sprint_cost.c_str());
    FUCKUtil::SliderFloat(LAB::player_running_cost.c_str(), VAR::player_running_cost,
                          CONFIG::stamina_cost_running_player, 0.0, 20.0, TOOL::player_running_cost.c_str());
    FUCK::Unindent();
}
void FMenu::StaminaTool::DrawAV()
{

    using namespace MENU;
    FUCKUtil::GreenTitleText("Stamina scaling by Actor Values, 5");
    FUCK::Indent();
    FUCKUtil::SliderFloat(LAB::stamina_av_mult_onehanded.c_str(), VAR::stamina_av_mult_onehanded,
                             CONFIG::stamina_av_mult_onehanded, 0.0, 1.0, TOOL::stamina_av_mult_onehanded.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_twohanded.c_str(), VAR::stamina_av_mult_twohanded,
                             CONFIG::stamina_av_mult_twohanded, 0.0, 1.0, TOOL::stamina_av_mult_twohanded.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_archery.c_str(), VAR::stamina_av_mult_archery,
                             CONFIG::stamina_av_mult_archery, 0.0, 1.0, TOOL::stamina_av_mult_archery.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_block.c_str(), VAR::stamina_av_mult_block,
                             CONFIG::stamina_av_mult_block, 0.0, 1.0, TOOL::stamina_av_mult_block.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_heavyarmor.c_str(), VAR::stamina_av_mult_heavyarmor,
                             CONFIG::stamina_av_mult_heavyarmor, 0.0, 1.0, TOOL::stamina_av_mult_heavyarmor.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_lightarmor.c_str(), VAR::stamina_av_mult_lightarmor,
                             CONFIG::stamina_av_mult_lightarmor, 0.0, 1.0, TOOL::stamina_av_mult_lightarmor.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_sneak.c_str(), VAR::stamina_av_mult_sneak,
                             CONFIG::stamina_av_mult_sneak, 0.0, 1.0, TOOL::stamina_av_mult_sneak.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_alteration.c_str(), VAR::stamina_av_mult_alteration,
                             CONFIG::stamina_av_mult_alteration, 0.0, 1.0, TOOL::stamina_av_mult_alteration.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_illusion.c_str(), VAR::stamina_av_mult_illusion,
                             CONFIG::stamina_av_mult_illusion, 0.0, 1.0, TOOL::stamina_av_mult_illusion.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_destruction.c_str(), VAR::stamina_av_mult_destruction,
                             CONFIG::stamina_av_mult_destruction, 0.0, 1.0, TOOL::stamina_av_mult_destruction.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_conjuration.c_str(), VAR::stamina_av_mult_conjuration,
                             CONFIG::stamina_av_mult_conjuration, 0.0, 1.0, TOOL::stamina_av_mult_conjuration.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_restoration.c_str(), VAR::stamina_av_mult_restoration,
                             CONFIG::stamina_av_mult_restoration, 0.0, 1.0, TOOL::stamina_av_mult_restoration.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_speedmult.c_str(), VAR::stamina_av_mult_speedmult,
                             CONFIG::stamina_av_mult_speedmult, 0.0, 1.0, TOOL::stamina_av_mult_speedmult.c_str());

    FUCKUtil::SliderFloat(LAB::stamina_av_mult_inventoryweight.c_str(), VAR::stamina_av_mult_inventoryweight,
                             CONFIG::stamina_av_mult_inventoryweight, 0.0, 1.0,
                             TOOL::stamina_av_mult_inventoryweight.c_str());
    FUCK::Unindent();
}
void FMenu::StaminaTool::DrawSystem()
{
    if (FUCK::Button(MENU::TITLE::restore_button.c_str()))
    {
        MENU::SHARED::RestoreDefaults();
    }
}
void FMenu::RegisterFMenu()
{
    if (FUCK::Connect("styyx-stamina-combat", 1))
    {
        FUCK::RegisterTool(&g_stamTool);
    }
}
} // namespace EXCO
