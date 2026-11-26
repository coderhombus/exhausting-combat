#pragma once

#include <chrono>
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
///
/// Flushing: every line at debug or above is flushed immediately, so one-off events (attacks,
/// casts, jumps) are in the file as soon as they happen. Per-frame trace lines are flushed at
/// least once a second instead of per line, which would be too slow.
inline void SetLogLevel(std::int32_t a_level)
{
    const auto level = static_cast<spdlog::level::level_enum>(a_level);
    if (auto* logger = spdlog::default_logger_raw()) {
        logger->set_level(level);
        logger->flush_on(level < spdlog::level::debug ? spdlog::level::debug : level);
    }
    static const bool periodicFlushStarted = [] {
        spdlog::flush_every(std::chrono::seconds(1));
        return true;
    }();
    (void)periodicFlushStarted;
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

/// Display name for log lines ("Lydia", "Iron Dagger"); "<none>" for null.
[[nodiscard]] inline std::string Describe(const RE::TESObjectREFR* a_ref)
{
    if (!a_ref) {
        return "<none>";
    }
    const char* name = a_ref->GetName();
    return name && name[0] ? name : "<unnamed>";
}

[[nodiscard]] inline std::string Describe(const RE::TESForm* a_form)
{
    if (!a_form) {
        return "<none>";
    }
    const char* name = a_form->GetName();
    return name && name[0] ? name : "<unnamed>";
}
}  // namespace ExhaustingCombat
