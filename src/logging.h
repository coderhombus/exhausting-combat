#pragma once

#include <cstdint>
#include <format>
#include <string>

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace ExhaustingCombat
{
namespace logs = SKSE::log;

/// Severity conventions:
///  trace    per-frame values (movement and bow-draw drains)
///  debug    one line per discrete charge (attack, cast, shot, jump) with inputs and result
///  info     state changes: config loaded/saved, PEPE detected, cast interrupted, shot blocked
///  warn     recoverable problems: invalid config values, missing optional dependencies
///  error    failures that disable a feature
///  critical the plugin will not run (unsupported runtime)
inline void SetLogLevel(std::int32_t a_level)
{
    const auto level = static_cast<spdlog::level::level_enum>(a_level);
    if (auto* logger = spdlog::default_logger_raw()) {
        logger->set_level(level);
        logger->flush_on(level < spdlog::level::info ? spdlog::level::info : level);
    }
}

/// Cheap check for hot paths, so arguments of disabled trace logs are not even computed.
[[nodiscard]] inline bool IsTraceEnabled()
{
    auto* logger = spdlog::default_logger_raw();
    return logger && logger->should_log(spdlog::level::trace);
}

[[nodiscard]] inline bool IsDebugEnabled()
{
    auto* logger = spdlog::default_logger_raw();
    return logger && logger->should_log(spdlog::level::debug);
}

/// "Lydia (000A2C94)" style identifier for log lines.
[[nodiscard]] inline std::string Describe(const RE::TESObjectREFR* a_ref)
{
    if (!a_ref) {
        return "<none>";
    }
    const char* name = a_ref->GetName();
    return std::format("{} ({:08X})", name && name[0] ? name : "<unnamed>", a_ref->GetFormID());
}

[[nodiscard]] inline std::string Describe(const RE::TESForm* a_form)
{
    if (!a_form) {
        return "<none>";
    }
    const char* name = a_form->GetName();
    return std::format("{} ({:08X})", name && name[0] ? name : "<unnamed>", a_form->GetFormID());
}
}  // namespace ExhaustingCombat
