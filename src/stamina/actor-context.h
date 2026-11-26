#pragma once

// Engine queries about an actor that the stamina rules need: which profile applies, whether to
// charge at all, who pays, how it is moving, what it attacks with. Pure decisions live in
// rules.h / formulas.h; this file only gathers facts from RE::Actor.

#include <RE/Skyrim.h>

#include "config/settings.h"
#include "stamina/rules.h"

namespace ExhaustingCombat::Stamina
{
/// Player profile for the player, NPC profile for everyone else (including horses).
[[nodiscard]] const Config::ActorProfile& ProfileFor(const RE::Actor& a_actor);

/// False for null, the player in god mode, or combat-only mode while out of combat.
[[nodiscard]] bool ShouldChargeStamina(const RE::Actor* a_actor);

/// Who pays for one-off actions (jump, cast, shot, bow draw): a mounted *player* pays with the
/// horse, because the player's stamina is not used while riding. Everyone else pays themselves.
[[nodiscard]] RE::Actor& PayerFor(RE::Actor& a_actor);

[[nodiscard]] MountRole GetMountRole(const RE::Actor& a_actor);

/// The rider of a mount, or null. The returned handle keeps the rider alive while used.
[[nodiscard]] RE::ActorPtr GetRider(RE::Actor& a_mount);

[[nodiscard]] MovementFlags ReadMovementFlags(const RE::Actor& a_actor);

[[nodiscard]] float GetStamina(RE::Actor& a_actor);

/// `a_baseCost` raised by carried weight (equipped or whole inventory, per profile setting).
[[nodiscard]] float EncumbranceScaledCostFor(RE::Actor& a_actor, float a_baseCost);

/// Skill-based cost factor for any actor value; 1.0 for values without a skill multiplier.
[[nodiscard]] float SkillFactorFor(RE::Actor& a_actor, RE::ActorValue a_skill);

/// The skill that scales attacks with this weapon (kNone if none applies).
[[nodiscard]] RE::ActorValue AssociatedSkill(const RE::TESObjectWEAP& a_weapon);

/// The weapon an action is attributed to: the swinging weapon while a humanoid attacks,
/// otherwise the equipped weapon (right hand preferred when both hands hold one).
[[nodiscard]] RE::TESObjectWEAP* GetRelevantWeapon(RE::Actor& a_actor);

struct AttackItem
{
    AttackItemKind kind        = AttackItemKind::None;
    RE::TESForm*   form        = nullptr;  // weight source and PEPE condition item
    float          weight      = 0.0f;
    float          skillFactor = 1.0f;     // product of all skill factors that apply
};

[[nodiscard]] AttackItem ResolveAttackItem(RE::Actor& a_actor, bool a_isBash);

/// Bow or crossbow drawn/attached without the player zooming in (crossbows: whole reload).
[[nodiscard]] bool IsDrawingBowWithoutZoom(const RE::Actor& a_actor);

/// Stops a running attack or block animation. Currently unused; kept for planned features
/// (e.g. interrupting attacks when out of stamina).
void InterruptAttack(RE::Actor& a_actor);
}  // namespace ExhaustingCombat::Stamina
