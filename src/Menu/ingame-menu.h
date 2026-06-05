#pragma once

#include <st-fui.h>

#include "API/FUCK_API.h"
#include "shared-menu.h"


namespace EXCO
{

namespace FMenu
{
void RegisterFMenu();

struct StaminaTool : FUCK::ITool
{

    const char* PluginName() const override { return "styyx-stamina-combat"; }
    const char* Name() const override { return "Exhausting Combat"; }
    const char* Group() const override { return "Styyx Mods"; }
    void Draw() override;
    void OnOpen() override;
    void OnClose() override;

  private:
    void DrawNPCs();
    void DrawPlayer();
    void DrawSystem();
};
inline StaminaTool g_stamTool;

} // namespace FMenu

} // namespace EXCO