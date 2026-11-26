#pragma once

#include <cstddef>

namespace ExhaustingCombat::Hooks
{
/// Bytes of SKSE trampoline Install() needs: one 14-byte branch per call hook.
inline constexpr std::size_t kCallHookCount      = 4;
inline constexpr std::size_t kBytesPerCallHook   = 14;
inline constexpr std::size_t kTrampolineBytes    = kCallHookCount * kBytesPerCallHook;

/// Installs every hook. Requires SKSE::AllocTrampoline(kTrampolineBytes) first.
void Install();
}  // namespace ExhaustingCombat::Hooks
