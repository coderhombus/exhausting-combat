#include "integrations/perk-entry-points.h"

#include <array>

// Third-party header we may not modify; silence its warnings here so --werror builds work.
#pragma warning(push, 0)
#include "API/PerkEntryPointExtenderAPI.h"
#pragma warning(pop)
#include "logging.h"
#include "stamina/formulas.h"

namespace ExhaustingCombat::Integrations::PerkEntryPoints
{
namespace
{
bool g_available = false;

std::string_view ToString(PEPE::RequestResult a_result)
{
    using R = PEPE::RequestResult;
    switch (a_result) {
        case R::Success:
            return "Success";
        case R::InvalidAPI:
            return "InvalidAPI";
        case R::Unsupported:
            return "Unsupported";
        case R::UnmatchedArgs:
            return "UnmatchedArgs";
        case R::BadFormArg:
            return "BadFormArg";
        case R::BadEntryPoint:
            return "BadEntryPoint";
        case R::BadString:
            return "BadString";
        case R::NoActor:
            return "NoActor";
        case R::InvalidActor:
            return "InvalidActor";
        case R::InvalidOut:
            return "InvalidOut";
        case R::UnknownFlags:
            return "UnknownFlags";
        case R::BadExtraData:
            return "BadExtraData";
        case R::UnknownError:
            return "UnknownError";
    }
    return "?";
}
}  // namespace

void Initialize()
{
    // RequestInterface() looks the DLL up and caches the interface; when PEPE is missing it
    // would repeat the lookup (and log) on every call, so we only ever ask once.
    g_available = PerkEntryPointExtenderAPI::RequestInterface() != nullptr;
    if (g_available) {
        logs::info("Perk Entry Point Extender detected: perk categories are active");
    } else {
        logs::warn("Perk Entry Point Extender not found: stamina perks from PEPE categories will not apply");
    }
}

bool IsAvailable()
{
    return g_available;
}

float Apply(const Entry& a_entry, RE::Actor* a_perkOwner, float a_value, RE::TESForm* a_conditionItem)
{
    if (g_available && a_perkOwner) {
        const float before = a_value;
        const auto  result = RE::HandleEntryPoint(a_entry.entryPoint, a_perkOwner, a_value, a_entry.category, a_conditionItem);
        if (result != PEPE::RequestResult::Success) {
            logs::debug("PEPE {} on {}: {}", a_entry.category, Describe(a_perkOwner), ToString(result));
        } else if (before != a_value && IsTraceEnabled()) {
            logs::trace("PEPE {} on {}: {} -> {}", a_entry.category, Describe(a_perkOwner), before, a_value);
        }
    }
    return Stamina::ClampCost(a_value);
}

void LogEntryPointSignatures()
{
    if (!IsDebugEnabled()) {
        return;
    }
    constexpr std::array kEntryPoints{ RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina,
                                       RE::BGSEntryPoint::ENTRY_POINTS::kCanDualCastSpell,
                                       RE::BGSEntryPoint::ENTRY_POINTS::kGetMaxCarryWeight };
    for (const auto entryPoint : kEntryPoints) {
        const auto* info = RE::BGSEntryPoint::GetEntryPoint(entryPoint);
        if (!info) {
            continue;
        }
        std::string parameters;
        for (std::uint32_t i = 0; i < info->parameters.count; ++i) {
            const char* name = info->parameters.data ? info->parameters.data[i].name : nullptr;
            parameters += std::format("{}{}", i ? ", " : "", name ? name : "?");
        }
        logs::debug("entry point {} ({}): condition tabs [{}]", info->name ? info->name : "?", static_cast<int>(entryPoint), parameters);
    }
}
}  // namespace ExhaustingCombat::Integrations::PerkEntryPoints
