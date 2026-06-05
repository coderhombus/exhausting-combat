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
    FUCKUtil::GreenTitleText("Stamina for NPCs");
    FUCKUtil::SetSliderFloat(LAB::npc_attack_cost.c_str(), VAR::npc_attack_cost, CONFIG::stamina_cost_attack_npc, 0.0,
                             50.0, TOOL::npc_attack_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::npc_sprint_cost.c_str(), VAR::npc_sprint_cost, CONFIG::stamina_cost_sprint_npc, 0.0,
                             20.0, TOOL::npc_sprint_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::npc_swim_cost.c_str(), VAR::npc_swim_cost, CONFIG::stamina_cost_swim_npc, 0.0, 20.0,
                             TOOL::npc_swim_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::npc_sneak_cost.c_str(), VAR::npc_sneak_cost, CONFIG::stamina_cost_sneak_npc, 0.0,
                             20.0, TOOL::npc_sneak_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::npc_cast_cost.c_str(), VAR::npc_cast_cost, CONFIG::stamina_cost_casting_npc, 0.0,
                             50.0, TOOL::npc_cast_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::npc_bow_draw_cost.c_str(), VAR::npc_bow_draw_cost, CONFIG::stamina_cost_bow_drain_npc,
                             0.0, 20.0, TOOL::npc_bow_draw_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::npc_bow_shoot_cost.c_str(), VAR::npc_bow_shoot_cost, CONFIG::stamina_cost_bow_npc,
                             0.0, 50.0, TOOL::npc_bow_shoot_cost.c_str());
};
void FMenu::StaminaTool::DrawPlayer()
{

    using namespace MENU;
    FUCKUtil::GreenTitleText("Stamina for Player");
    FUCKUtil::SetSliderFloat(LAB::player_attack_cost.c_str(), VAR::player_attack_cost,
                             CONFIG::stamina_cost_attack_player, 0.0, 50.0, TOOL::player_attack_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_sprint_cost.c_str(), VAR::player_sprint_cost,
                             CONFIG::stamina_cost_sprint_player, 0.0, 20.0, TOOL::player_sprint_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_swim_cost.c_str(), VAR::player_swim_cost, CONFIG::stamina_cost_swim_player,
                             0.0, 20.0, TOOL::player_swim_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_sneak_cost.c_str(), VAR::player_sneak_cost, CONFIG::stamina_cost_sneak_player,
                             0.0, 20.0, TOOL::player_sneak_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_cast_cost.c_str(), VAR::player_cast_cost, CONFIG::stamina_cost_casting_player,
                             0.0, 50.0, TOOL::player_cast_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_bow_draw_cost.c_str(), VAR::player_bow_draw_cost,
                             CONFIG::stamina_cost_bow_drain_player, 0.0, 20.0, TOOL::player_bow_draw_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_bow_shoot_cost.c_str(), VAR::player_bow_shoot_cost,
                             CONFIG::stamina_cost_bow_player, 0.0, 50.0, TOOL::player_bow_shoot_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_jump_cost.c_str(), VAR::player_jump_cost, CONFIG::stamina_cost_jump_player,
                             0.0, 50.0, TOOL::player_jump_cost.c_str());
    FUCKUtil::SetSliderFloat(LAB::player_horseride_sprint_cost.c_str(), VAR::player_horseride_sprint_cost,
                             CONFIG::stamina_cost_sprint_horse_player, 0.0, 20.0,
                             TOOL::player_horseride_sprint_cost.c_str());
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