#include "skse-menu.h"

#include "API/SKSEMenuFramework.h"
#include "config.h"
#include "shared-menu.h"
#include "st-ui.h"


namespace EXCO
{

void SKSEMenu::RegisterSKSEMenu()
{
    if (!SKSEMenuFramework::IsInstalled())
    {
        return;
    }
    SKSEMenuFramework::SetSection(MENU::TITLE::mod_name);
    SKSEMenuFramework::AddSectionItem(MENU::TITLE::settings_section, RenderMenu);
    RegisterMenuOpenCloseEventSKSE();
}

void SKSEMenu::OnMenuOpen()
{
    MENU::SHARED::LoadFromConfig();
}
void SKSEMenu::OnMenuClose()
{
    CONFIG::UpdateSettings(true);
}

static void __stdcall MenuEventListener(SKSEMenuFramework::Model::EventType eventType)
{
    switch (eventType)
    {
        case SKSEMenuFramework::Model::kOpenMenu:
            SKSEMenu::OnMenuOpen();
            break;
        case SKSEMenuFramework::Model::kCloseMenu:
            SKSEMenu::OnMenuClose();
            break;
        default:
            break;
    }
}

void SKSEMenu::RegisterMenuOpenCloseEventSKSE()
{
    static SKSEMenuFramework::Model::Event event(MenuEventListener, 0);
}
void SKSEMenu::DrawNPCSection()
{
    using namespace MENU;
    UIUtil::GreenTitleText("Stamina for NPCs");
    UIUtil::SetSliderFloat(LAB::npc_attack_cost.c_str(), VAR::npc_attack_cost, CONFIG::stamina_cost_attack_npc, 0.0,
                           50.0, TOOL::npc_attack_cost.c_str());
    UIUtil::SetSliderFloat(LAB::npc_sprint_cost.c_str(), VAR::npc_sprint_cost, CONFIG::stamina_cost_sprint_npc, 0.0,
                           20.0, TOOL::npc_sprint_cost.c_str());
    UIUtil::SetSliderFloat(LAB::npc_swim_cost.c_str(), VAR::npc_swim_cost, CONFIG::stamina_cost_swim_npc, 0.0, 20.0,
                           TOOL::npc_swim_cost.c_str());
    UIUtil::SetSliderFloat(LAB::npc_sneak_cost.c_str(), VAR::npc_sneak_cost, CONFIG::stamina_cost_sneak_npc, 0.0, 20.0,
                           TOOL::npc_sneak_cost.c_str());
    UIUtil::SetSliderFloat(LAB::npc_cast_cost.c_str(), VAR::npc_cast_cost, CONFIG::stamina_cost_casting_npc, 0.0, 50.0,
                           TOOL::npc_cast_cost.c_str());
    UIUtil::SetSliderFloat(LAB::npc_bow_draw_cost.c_str(), VAR::npc_bow_draw_cost, CONFIG::stamina_cost_bow_drain_npc,
                           0.0, 20.0, TOOL::npc_bow_draw_cost.c_str());
    UIUtil::SetSliderFloat(LAB::npc_bow_shoot_cost.c_str(), VAR::npc_bow_shoot_cost, CONFIG::stamina_cost_bow_npc, 0.0,
                           50.0, TOOL::npc_bow_shoot_cost.c_str());
}
void SKSEMenu::DrawPlayerSection()
{
    using namespace MENU;
    UIUtil::GreenTitleText("Stamina for Player");
    UIUtil::SetSliderFloat(LAB::player_attack_cost.c_str(), VAR::player_attack_cost, CONFIG::stamina_cost_attack_player,
                           0.0, 50.0, TOOL::player_attack_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_sprint_cost.c_str(), VAR::player_sprint_cost, CONFIG::stamina_cost_sprint_player,
                           0.0, 20.0, TOOL::player_sprint_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_swim_cost.c_str(), VAR::player_swim_cost, CONFIG::stamina_cost_swim_player, 0.0,
                           20.0, TOOL::player_swim_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_sneak_cost.c_str(), VAR::player_sneak_cost, CONFIG::stamina_cost_sneak_player,
                           0.0, 20.0, TOOL::player_sneak_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_cast_cost.c_str(), VAR::player_cast_cost, CONFIG::stamina_cost_casting_player,
                           0.0, 50.0, TOOL::player_cast_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_bow_draw_cost.c_str(), VAR::player_bow_draw_cost,
                           CONFIG::stamina_cost_bow_drain_player, 0.0, 20.0, TOOL::player_bow_draw_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_bow_shoot_cost.c_str(), VAR::player_bow_shoot_cost,
                           CONFIG::stamina_cost_bow_player, 0.0, 50.0, TOOL::player_bow_shoot_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_jump_cost.c_str(), VAR::player_jump_cost, CONFIG::stamina_cost_jump_player, 0.0,
                           50.0, TOOL::player_jump_cost.c_str());
    UIUtil::SetSliderFloat(LAB::player_horseride_sprint_cost.c_str(), VAR::player_horseride_sprint_cost,
                           CONFIG::stamina_cost_sprint_horse_player, 0.0, 20.0,
                           TOOL::player_horseride_sprint_cost.c_str());
}
void SKSEMenu::DrawResetMenuSection()
{
    if (Button(MENU::TITLE::restore_button.c_str()))
    {
        MENU::SHARED::RestoreDefaults();
    }
}

void __stdcall SKSEMenu::RenderMenu()
{
    if (CollapsingHeader(MENU::TITLE::player_section.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawPlayerSection();
    }
    if (CollapsingHeader(MENU::TITLE::npc_section.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawNPCSection();
    }
    if (CollapsingHeader(MENU::TITLE::system_section.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawResetMenuSection();
    }
}

} // namespace EXCO