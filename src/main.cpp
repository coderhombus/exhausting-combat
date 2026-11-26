#include <algorithm>
#include <filesystem>

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include "config/config-store.h"
#include "config/settings.h"
#include "hooks.h"
#include "integrations/perk-entry-points.h"
#include "logging.h"
#include "menu/settings-menu.h"
#include "plugin-info.h"
#include "stamina/costs.h"

namespace
{
using namespace ExhaustingCombat;

bool IsSupportedRuntime(const REL::Version& a_runtime)
{
    if (std::ranges::find(Plugin::kSupportedRuntimes, a_runtime) != Plugin::kSupportedRuntimes.end()) {
        return true;
    }
    if (!Plugin::kEnforceSupportedRuntime) {
        logs::warn("runtime {} is not supported, but the runtime check is disabled in this build; hooks may be wrong", a_runtime.string());
        return true;
    }
    logs::critical("runtime {} is not supported (supported: 1.6.1170); {} will not load", a_runtime.string(), Plugin::kDisplayName);
    return false;
}

void LoadConfig()
{
    auto&      settings = Config::Get();
    const auto report   = Config::LoadFromFile(std::filesystem::path(Config::kConfigFilePath), settings);
    SetLogLevel(settings.general.logLevel.GetValue());

    for (const auto& diagnostic : report.diagnostics) {
        switch (diagnostic.severity) {
            case Config::Severity::Info:
                logs::info("config: {}", diagnostic.message);
                break;
            case Config::Severity::Warning:
                logs::warn("config: {}", diagnostic.message);
                break;
            case Config::Severity::Error:
                logs::error("config: {}", diagnostic.message);
                break;
        }
    }
    logs::info("config loaded from {} ({} warnings, {} errors)", Config::kConfigFilePath, report.Count(Config::Severity::Warning),
               report.Count(Config::Severity::Error));
}

void OnSkseMessage(SKSE::MessagingInterface::Message* a_message)
{
    if (!a_message) {
        return;
    }
    switch (a_message->type) {
        case SKSE::MessagingInterface::kPostLoad:
            Integrations::PerkEntryPoints::Initialize();
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            Stamina::InitializeForms();
            Integrations::PerkEntryPoints::LogEntryPointSignatures();
            Menu::Register();
            break;
        default:
            break;
    }
}
}  // namespace

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    SKSE::Init(a_skse);

    if (!IsSupportedRuntime(a_skse->RuntimeVersion())) {
        return false;
    }

    LoadConfig();  // before hooks: hooks read settings from the first frame

    SKSE::AllocTrampoline(Hooks::kTrampolineBytes);
    Hooks::Install();

    if (!SKSE::GetMessagingInterface()->RegisterListener(OnSkseMessage)) {
        logs::error("could not register for SKSE messages; perks, menu and forms will not initialise");
    }
    logs::info("{} loaded", Plugin::kDisplayName);
    return true;
}
