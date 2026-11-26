#pragma once

// Adapter around Perk Entry Point Extender (PEPE, src/API/PerkEntryPointExtenderAPI.h, which
// must not be modified). Everything PEPE-specific stays behind this interface.
//
// How the mod uses PEPE: each stamina cost is passed through a vanilla perk entry point
// (mostly "Mod Power Attack Stamina") under a PEPE *category* string. A perk author adds an
// entry with that category to a perk; PEPE then applies it only to our call, not to vanilla
// power attacks. The category strings are therefore part of the mod's public interface.
//
// The "condition item": vanilla entry points evaluate perk conditions against fixed subjects.
// "Mod Power Attack Stamina" has two condition tabs, Perk Owner and Weapon. PEPE forwards the
// extra form we pass as the subject of the second tab, so a perk can say e.g. "sneaking costs
// less while holding a dagger" (Weapon tab: GetIsObjectType/WornHasKeyword...). It is not a
// dummy: dropping it changes the argument count, which PEPE rejects (RequestResult
// UnmatchedArgs) so the perk silently stops applying. Passing nullptr keeps the count and makes
// conditions on that tab fail. For spells we pass the spell, for bashes the shield or torch.
// LogEntryPointSignatures() prints the engine's parameter list per entry point to confirm this.

#include <string_view>

#include <RE/Skyrim.h>

namespace ExhaustingCombat::Integrations::PerkEntryPoints
{
struct Entry
{
    std::string_view                category;    // PEPE category perk authors target
    RE::BGSEntryPoint::ENTRY_POINT  entryPoint;  // vanilla entry point the category rides on
};

// Categories applied by this mod.
inline constexpr Entry kAttackStamina{ "NormalAttackStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kJumpStamina{ "JumpStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kSneakStamina{ "SneakStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kSprintStamina{ "SprintStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kMountSprintStamina{ "MountSprintStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kBowShotStamina{ "BowStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kBowDrawStamina{ "BowDrawStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kCastStamina{ "CastStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kSwimStamina{ "SwimStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };
inline constexpr Entry kRunStamina{ "RunningStamina", RE::BGSEntryPoint::ENTRY_POINTS::kModPowerAttackStamina };

// Currently unused: reserved for planned features (kept so the category names stay stable).
inline constexpr Entry kSetBowDrain{ "SetBowDrain", RE::BGSEntryPoint::ENTRY_POINTS::kCanDualCastSpell };
inline constexpr Entry kGetMaxCarryWeight{ "GetMaxCarryWeight", RE::BGSEntryPoint::ENTRY_POINTS::kGetMaxCarryWeight };

/// Detects PEPE once. Call at SKSE kPostLoad (all SKSE plugins are loaded by then).
void Initialize();

[[nodiscard]] bool IsAvailable();

/// Lets perks of `a_entry.category` owned by `a_perkOwner` modify `a_value`. `a_conditionItem`
/// is the subject of the entry point's second condition tab (see top of file); may be null.
/// The result is always clamped to >= 0 (and NaN to 0), whether or not PEPE is present.
[[nodiscard]] float Apply(const Entry& a_entry, RE::Actor* a_perkOwner, float a_value, RE::TESForm* a_conditionItem);

/// Logs, at debug level, the condition-tab parameters the engine defines for each entry point
/// used. Call at kDataLoaded.
void LogEntryPointSignatures();
}  // namespace ExhaustingCombat::Integrations::PerkEntryPoints
