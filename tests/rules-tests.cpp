#include <doctest/doctest.h>

#include "stamina/rules.h"

using namespace ExhaustingCombat::Stamina;

namespace
{
MovementFlags Moving(bool a_sneak, bool a_run, bool a_sprint, bool a_swim)
{
    return MovementFlags{ .moving = true, .sneaking = a_sneak, .running = a_run, .sprinting = a_sprint, .swimming = a_swim };
}
}  // namespace

TEST_CASE("ClassifyMovement picks exactly one state with priority swim > sprint > sneak > run")
{
    CHECK(ClassifyMovement({}) == MovementState::Idle);
    CHECK(ClassifyMovement(MovementFlags{ .moving = false, .running = true, .sprinting = true }) == MovementState::Idle);
    CHECK(ClassifyMovement(Moving(false, false, false, false)) == MovementState::Walking);
    CHECK(ClassifyMovement(Moving(false, true, false, false)) == MovementState::Running);
    CHECK(ClassifyMovement(Moving(true, false, false, false)) == MovementState::Sneaking);
    CHECK(ClassifyMovement(Moving(false, false, true, false)) == MovementState::Sprinting);
    CHECK(ClassifyMovement(Moving(false, false, false, true)) == MovementState::Swimming);

    SUBCASE("overlapping engine flags no longer stack costs")
    {
        CHECK(ClassifyMovement(Moving(false, true, true, false)) == MovementState::Sprinting);  // B6: sprint + run bit
        CHECK(ClassifyMovement(Moving(true, true, false, false)) == MovementState::Sneaking);
        CHECK(ClassifyMovement(Moving(true, false, true, false)) == MovementState::Sprinting);  // sneak roll
        CHECK(ClassifyMovement(Moving(false, true, true, true)) == MovementState::Swimming);
    }
}

TEST_CASE("MovementCostFor: riders never pay, mounts use the mount sprint cost")
{
    for (auto state : { MovementState::Idle, MovementState::Walking, MovementState::Running, MovementState::Sneaking,
                        MovementState::Sprinting, MovementState::Swimming }) {
        CAPTURE(ToString(state));
        CHECK(MovementCostFor(state, MountRole::Rider) == MovementCost::None);
    }

    CHECK(MovementCostFor(MovementState::Sprinting, MountRole::None) == MovementCost::Sprint);
    CHECK(MovementCostFor(MovementState::Sprinting, MountRole::Mount) == MovementCost::MountSprint);
    CHECK(MovementCostFor(MovementState::Swimming, MountRole::None) == MovementCost::Swim);
    CHECK(MovementCostFor(MovementState::Swimming, MountRole::Mount) == MovementCost::Swim);
    CHECK(MovementCostFor(MovementState::Running, MountRole::Mount) == MovementCost::Run);
    CHECK(MovementCostFor(MovementState::Sneaking, MountRole::None) == MovementCost::Sneak);
    CHECK(MovementCostFor(MovementState::Walking, MountRole::None) == MovementCost::None);
    CHECK(MovementCostFor(MovementState::Idle, MountRole::Mount) == MovementCost::None);
}

TEST_CASE("RequiresPositiveStamina preserves the old per-action pre-checks")
{
    CHECK(RequiresPositiveStamina(MovementCost::Sneak));
    CHECK(RequiresPositiveStamina(MovementCost::Sprint));
    CHECK(RequiresPositiveStamina(MovementCost::MountSprint));
    CHECK_FALSE(RequiresPositiveStamina(MovementCost::Swim));
    CHECK_FALSE(RequiresPositiveStamina(MovementCost::Run));
}

TEST_CASE("ShouldChargeStamina truth table")
{
    // god mode always wins
    CHECK_FALSE(ShouldChargeStamina({ .isPlayerInGodMode = true, .combatOnly = false, .inCombat = true }));
    CHECK_FALSE(ShouldChargeStamina({ .isPlayerInGodMode = true, .combatOnly = true, .inCombat = true }));
    // combat-only off: always charge
    CHECK(ShouldChargeStamina({ .combatOnly = false, .inCombat = false }));
    CHECK(ShouldChargeStamina({ .combatOnly = false, .inCombat = true }));
    // combat-only on: only in combat
    CHECK_FALSE(ShouldChargeStamina({ .combatOnly = true, .inCombat = false }));
    CHECK(ShouldChargeStamina({ .combatOnly = true, .inCombat = true }));
}

TEST_CASE("IsExemptForLowMaxStamina only frees NPCs whose max stamina is below the cost")
{
    CHECK(IsExemptForLowMaxStamina(false, true, 0.0f, 10.0f));
    CHECK(IsExemptForLowMaxStamina(false, true, 9.9f, 10.0f));
    CHECK_FALSE(IsExemptForLowMaxStamina(false, true, 10.0f, 10.0f));
    CHECK_FALSE(IsExemptForLowMaxStamina(false, false, 0.0f, 10.0f));  // option off
    CHECK_FALSE(IsExemptForLowMaxStamina(true, true, 0.0f, 10.0f));    // never the player
    CHECK_FALSE(IsExemptForLowMaxStamina(false, true, 0.0f, 0.0f));    // free action stays free
}

TEST_CASE("ChooseAttackItem: shield and torch only for bashes, unarmed is None")
{
    CHECK(ChooseAttackItem(true, true, true, true) == AttackItemKind::Shield);
    CHECK(ChooseAttackItem(true, false, true, true) == AttackItemKind::Torch);
    CHECK(ChooseAttackItem(true, false, false, true) == AttackItemKind::Weapon);
    CHECK(ChooseAttackItem(false, true, true, true) == AttackItemKind::Weapon);
    CHECK(ChooseAttackItem(false, true, true, false) == AttackItemKind::None);
    CHECK(ChooseAttackItem(false, false, false, false) == AttackItemKind::None);  // unarmed
}

TEST_CASE("ResolveAttackHookCost")
{
    CHECK(ResolveAttackHookCost(5.0f, 20.0f, false, true) == 0.0f);   // cooldown beats everything
    CHECK(ResolveAttackHookCost(5.0f, 20.0f, true, false) == 5.0f);   // continuous: vanilla
    CHECK(ResolveAttackHookCost(5.0f, 20.0f, false, false) == 20.0f);
    CHECK(ResolveAttackHookCost(5.0f, 0.0f, false, false) == 5.0f);   // unarmed / exempt / free -> vanilla
}

TEST_CASE("AttackCooldown suppresses a second charge within 60 ms")
{
    using namespace std::chrono_literals;
    AttackCooldown cooldown;
    const auto t0 = AttackCooldown::TimePoint{} + 10s;

    CHECK(cooldown.TryStart(t0));
    CHECK_FALSE(cooldown.TryStart(t0 + 1ms));
    CHECK_FALSE(cooldown.TryStart(t0 + 59ms));
    CHECK(cooldown.TryStart(t0 + 60ms));
    CHECK_FALSE(cooldown.TryStart(t0 + 61ms));  // a new window started at 60 ms
    CHECK(cooldown.TryStart(t0 + 200ms));
}
