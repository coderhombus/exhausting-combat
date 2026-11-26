#pragma once

// Engine-independent decision rules: who pays, which movement cost applies, when a cost is
// skipped. The engine layer (actor-context / costs) gathers plain facts from RE::Actor and asks
// these functions; keeping them pure makes every branch unit-testable.

#include <chrono>
#include <cstdint>
#include <string_view>

namespace ExhaustingCombat::Stamina
{
// ---------------------------------------------------------------------------------------------
// Movement
// ---------------------------------------------------------------------------------------------

/// Raw movement flags as the engine reports them (ActorState1 bits). The engine sets these
/// independently, so several can be true at once (e.g. `running` stays set while sprinting).
struct MovementFlags
{
    bool moving    = false;  // any of movingForward/Back/Left/Right
    bool sneaking  = false;
    bool running   = false;
    bool sprinting = false;
    bool swimming  = false;
};

/// Exactly one locomotion state per actor per frame, so at most one movement cost is charged.
enum class MovementState : std::uint8_t
{
    Idle,       // not moving: no movement cost
    Walking,    // moving, no other flag: no movement cost
    Running,
    Sneaking,
    Sprinting,
    Swimming,
};

/// Collapses the independent engine flags into one state. Priority (highest first):
/// Swimming > Sprinting > Sneaking > Running > Walking. Before the refactor each flag was
/// checked separately, so e.g. sprinting with the `running` bit still set paid sprint + run.
[[nodiscard]] constexpr MovementState ClassifyMovement(const MovementFlags& a_flags) noexcept
{
    if (!a_flags.moving) {
        return MovementState::Idle;
    }
    if (a_flags.swimming) {
        return MovementState::Swimming;
    }
    if (a_flags.sprinting) {
        return MovementState::Sprinting;
    }
    if (a_flags.sneaking) {
        return MovementState::Sneaking;
    }
    if (a_flags.running) {
        return MovementState::Running;
    }
    return MovementState::Walking;
}

/// How an actor relates to riding.
enum class MountRole : std::uint8_t
{
    None,   // on foot
    Rider,  // sitting on a mount: the mount pays movement costs, the rider pays nothing
    Mount,  // a horse (ridden or not): pays its own movement costs
};

/// Which movement cost (if any) a state charges. Mounts and people use different sprint costs.
enum class MovementCost : std::uint8_t
{
    None,
    Run,
    Sneak,
    Sprint,
    MountSprint,
    Swim,
};

[[nodiscard]] constexpr MovementCost MovementCostFor(MovementState a_state, MountRole a_role) noexcept
{
    if (a_role == MountRole::Rider) {
        return MovementCost::None;  // the mount's own update charges for this movement
    }
    switch (a_state) {
        case MovementState::Swimming:
            return MovementCost::Swim;
        case MovementState::Sprinting:
            return a_role == MountRole::Mount ? MovementCost::MountSprint : MovementCost::Sprint;
        case MovementState::Sneaking:
            return MovementCost::Sneak;
        case MovementState::Running:
            return MovementCost::Run;
        case MovementState::Walking:
        case MovementState::Idle:
            return MovementCost::None;
    }
    return MovementCost::None;
}

/// Sneak and sprint drains historically stop once the payer is at 0 stamina; swim and run keep
/// charging (stamina is clamped by the engine). Preserved as-is.
[[nodiscard]] constexpr bool RequiresPositiveStamina(MovementCost a_cost) noexcept
{
    return a_cost == MovementCost::Sneak || a_cost == MovementCost::Sprint || a_cost == MovementCost::MountSprint;
}

[[nodiscard]] constexpr std::string_view ToString(MovementState a_state) noexcept
{
    switch (a_state) {
        case MovementState::Idle:
            return "idle";
        case MovementState::Walking:
            return "walking";
        case MovementState::Running:
            return "running";
        case MovementState::Sneaking:
            return "sneaking";
        case MovementState::Sprinting:
            return "sprinting";
        case MovementState::Swimming:
            return "swimming";
    }
    return "unknown";
}

[[nodiscard]] constexpr std::string_view ToString(MovementCost a_cost) noexcept
{
    switch (a_cost) {
        case MovementCost::None:
            return "none";
        case MovementCost::Run:
            return "run";
        case MovementCost::Sneak:
            return "sneak";
        case MovementCost::Sprint:
            return "sprint";
        case MovementCost::MountSprint:
            return "mount-sprint";
        case MovementCost::Swim:
            return "swim";
    }
    return "unknown";
}

// ---------------------------------------------------------------------------------------------
// Gates
// ---------------------------------------------------------------------------------------------

struct ChargeGateFacts
{
    bool isPlayerInGodMode = false;
    bool combatOnly        = false;  // the actor's profile setting
    bool inCombat          = false;
};

/// False when no stamina cost should be applied at all: god mode, or combat-only mode while out
/// of combat.
[[nodiscard]] constexpr bool ShouldChargeStamina(const ChargeGateFacts& a_facts) noexcept
{
    if (a_facts.isPlayerInGodMode) {
        return false;
    }
    return !a_facts.combatOnly || a_facts.inCombat;
}

/// "Exclude NPCs without stamina": many vanilla NPCs have 0 (or very low) max stamina. When the
/// option is on, an NPC whose max stamina is below the cost of an action performs it for free
/// instead of being unable to attack/cast/shoot at all. Never applies to the player.
[[nodiscard]] constexpr bool IsExemptForLowMaxStamina(bool a_isPlayer, bool a_exemptionEnabled, float a_maxStamina, float a_cost) noexcept
{
    return !a_isPlayer && a_exemptionEnabled && a_maxStamina < a_cost;
}

// ---------------------------------------------------------------------------------------------
// Attacks
// ---------------------------------------------------------------------------------------------

/// Which item an attack's weight (and perk conditions) come from.
enum class AttackItemKind : std::uint8_t
{
    None,  // nothing identified, e.g. unarmed. Intentionally costs 0 (see ChooseAttackItem).
    Shield,
    Torch,
    Weapon,
};

/// Shield and torch only count for bashes; otherwise the relevant weapon is used.
///
/// Unarmed attacks resolve to None and therefore cost 0 from this mod (the engine's own cost
/// is used instead, see ResolveAttackHookCost). This is intentional: fists have no item weight.
/// Future support for unarmed perk mods or "fist weapons" (TESObjectWEAP hand-to-hand) should
/// give them a weight here rather than special-casing the formula.
[[nodiscard]] constexpr AttackItemKind ChooseAttackItem(bool a_isBash, bool a_hasShield, bool a_hasTorchInLeftHand, bool a_hasWeapon) noexcept
{
    if (a_isBash && a_hasShield) {
        return AttackItemKind::Shield;
    }
    if (a_isBash && a_hasTorchInLeftHand) {
        return AttackItemKind::Torch;
    }
    if (a_hasWeapon) {
        return AttackItemKind::Weapon;
    }
    return AttackItemKind::None;
}

/// Value returned to the engine from the attack-stamina-cost hook.
///  - inside the player's double-event cooldown: 0 (the event already paid)
///  - continuous attacks: vanilla cost, untouched
///  - otherwise our cost if positive, else the vanilla cost
[[nodiscard]] constexpr float ResolveAttackHookCost(float a_vanillaCost, float a_modCost, bool a_isContinuousAttack, bool a_suppressedByCooldown) noexcept
{
    if (a_suppressedByCooldown) {
        return 0.0f;
    }
    if (a_isContinuousAttack) {
        return a_vanillaCost;
    }
    return a_modCost > 0.0f ? a_modCost : a_vanillaCost;
}

/// The attack-cost event fires twice when attacking while moving diagonally backwards. A short
/// cooldown (player only) stops the second event from charging again. 10 ms was too short;
/// 60 ms works, up to ~150 ms may be safe but is untested.
class AttackCooldown
{
public:
    using Clock     = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    static constexpr auto kDuration = std::chrono::milliseconds(60);

    /// True if an attack may be charged at `a_now`; starts a new cooldown when it is.
    [[nodiscard]] bool TryStart(TimePoint a_now) noexcept
    {
        if (a_now < m_allowedFrom) {
            return false;
        }
        m_allowedFrom = a_now + kDuration;
        return true;
    }

private:
    TimePoint m_allowedFrom{};
};
}  // namespace ExhaustingCombat::Stamina
