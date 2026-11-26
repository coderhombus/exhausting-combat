// Characterisation tests: the expected values were computed from the pre-refactor formulas in
// stamina-manager.cpp (commit 9c11c58 / c43f7e6) and must not change without a deliberate
// gameplay decision.
#include <doctest/doctest.h>

#include <cmath>
#include <limits>

#include "stamina/formulas.h"

using namespace ExhaustingCombat::Stamina;
using doctest::Approx;

TEST_CASE("ClampCost never returns a negative or NaN cost")
{
    CHECK(ClampCost(5.0f) == 5.0f);
    CHECK(ClampCost(0.0f) == 0.0f);
    CHECK(ClampCost(-3.0f) == 0.0f);
    CHECK(ClampCost(std::numeric_limits<float>::quiet_NaN()) == 0.0f);
    CHECK(ClampCost(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
}

TEST_CASE("WeightMultiplier maps vanilla weapon weights 2..33 onto 1..2")
{
    CHECK(WeightMultiplier(-5.0f) == Approx(1.0f));
    CHECK(WeightMultiplier(0.0f) == Approx(1.0f));   // weightless items count as 1, below the band
    CHECK(WeightMultiplier(2.0f) == Approx(1.0f));
    CHECK(WeightMultiplier(17.5f) == Approx(1.5f));
    CHECK(WeightMultiplier(33.0f) == Approx(2.0f));
    CHECK(WeightMultiplier(50.0f) == Approx(2.0f));
}

TEST_CASE("EncumbranceScaledCost adds up to 50% at full load and is not capped above it")
{
    CHECK(EncumbranceScaledCost(10.0f, 0.0f, 100.0f) == Approx(10.0f));
    CHECK(EncumbranceScaledCost(10.0f, 50.0f, 100.0f) == Approx(12.5f));
    CHECK(EncumbranceScaledCost(10.0f, 100.0f, 100.0f) == Approx(15.0f));
    CHECK(EncumbranceScaledCost(10.0f, 200.0f, 100.0f) == Approx(20.0f));
    SUBCASE("zero or negative carry weight is treated as 1")
    {
        CHECK(EncumbranceScaledCost(10.0f, 2.0f, 0.0f) == Approx(20.0f));
        CHECK(EncumbranceScaledCost(10.0f, 2.0f, -50.0f) == Approx(20.0f));
    }
    CHECK(EncumbranceScaledCost(0.0f, 500.0f, 100.0f) == Approx(0.0f));
}

TEST_CASE("SkillCostFactor reduces cost linearly with skill and never goes negative")
{
    CHECK(SkillCostFactor(100.0f, 0.1f) == Approx(0.9f));
    CHECK(SkillCostFactor(50.0f, 1.0f) == Approx(0.5f));
    CHECK(SkillCostFactor(0.0f, 1.0f) == Approx(1.0f));
    CHECK(SkillCostFactor(100.0f, 0.0f) == Approx(1.0f));  // shipped default: no scaling
    CHECK(SkillCostFactor(150.0f, 1.0f) == Approx(0.0f));  // fortified skill > 100
    SUBCASE("multiplier outside [0, 1] is clamped")
    {
        CHECK(SkillCostFactor(100.0f, 2.0f) == Approx(0.0f));
        CHECK(SkillCostFactor(100.0f, -1.0f) == Approx(1.0f));
    }
}

TEST_CASE("CastCost = (base + min(sqrt(magicka) * 0.7, 25)) * skill")
{
    CHECK(CastCost(10.0f, 100.0f, 1.0f) == Approx(17.0f));
    CHECK(CastCost(10.0f, 100.0f, 0.5f) == Approx(8.5f));
    SUBCASE("magicka below 1 counts as 1")
    {
        CHECK(CastCost(10.0f, 0.0f, 1.0f) == Approx(10.7f));
        CHECK(CastCost(10.0f, -20.0f, 1.0f) == Approx(10.7f));
    }
    SUBCASE("magicka contribution is capped at 25")
    {
        CHECK(CastCost(10.0f, 10000.0f, 1.0f) == Approx(35.0f));
        CHECK(CastCost(10.0f, 1275.5f, 1.0f) == Approx(35.0f));
    }
    SUBCASE("base <= 0 disables the cost and is returned unchanged")
    {
        CHECK(CastCost(0.0f, 100.0f, 1.0f) == Approx(0.0f));
        CHECK(CastCost(-1.0f, 100.0f, 1.0f) == Approx(-1.0f));
    }
}

TEST_CASE("AttackCost = base * weightMult * attackMult * powerAttackMult * skill")
{
    AttackCostInputs in;
    in.baseCost        = 9.0f;
    in.itemWeight      = 17.5f;  // weightMult 1.5
    in.attackMult      = 1.414f;
    in.powerAttackMult = 2.0f;
    in.skillFactor     = 1.0f;

    in.isPowerAttack = false;
    CHECK(AttackCost(in) == Approx(19.089f));
    in.isPowerAttack = true;
    CHECK(AttackCost(in) == Approx(38.178f));

    in.skillFactor = 0.5f;
    CHECK(AttackCost(in) == Approx(19.089f));

    in.itemWeight = 0.0f;  // e.g. a weightless torch: counts as 1 -> weightMult 1
    CHECK(AttackCost(in) == Approx(9.0f * 1.414f * 2.0f * 0.5f));
}

TEST_CASE("AppliedPowerAttackMult is the setting for power attacks only")
{
    AttackCostInputs in;
    in.powerAttackMult = 2.5f;
    in.isPowerAttack   = false;
    CHECK(AppliedPowerAttackMult(in) == 1.0f);
    in.isPowerAttack = true;
    CHECK(AppliedPowerAttackMult(in) == 2.5f);
}

TEST_CASE("ConcentrationCostPerSecond: mult 1.0 keeps the previous cost")
{
    CHECK(ConcentrationCostPerSecond(17.0f, 1.0f) == Approx(17.0f));
    CHECK(ConcentrationCostPerSecond(17.0f, 0.5f) == Approx(8.5f));
    CHECK(ConcentrationCostPerSecond(17.0f, 0.0f) == Approx(0.0f));
}
