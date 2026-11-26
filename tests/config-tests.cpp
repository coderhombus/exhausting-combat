#include <doctest/doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <variant>

#include "config/config-store.h"
#include "config/settings.h"

using namespace ExhaustingCombat::Config;

namespace
{
bool HasDiagnostic(const LoadReport& a_report, Severity a_severity, std::string_view a_needle)
{
    for (const auto& d : a_report.diagnostics) {
        if (d.severity == a_severity && d.message.find(a_needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string ReadFile(const std::filesystem::path& a_path)
{
    std::ifstream     file(a_path, std::ios::binary);
    std::stringstream text;
    text << file.rdbuf();
    std::string content = text.str();
    std::erase(content, '\r');  // tolerate a CRLF checkout on Windows
    return content;
}
}  // namespace

TEST_CASE("Parameter table is internally consistent")
{
    Settings              settings;
    std::set<std::string> keys;
    std::set<std::string> labels;

    for (const auto& ref : settings.All()) {
        std::visit(
            [&](auto* p) {
                const auto& s = p->Spec();
                CAPTURE(s.key);
                CHECK_FALSE(s.section.empty());
                CHECK_FALSE(s.key.empty());
                CHECK_FALSE(s.label.empty());
                CHECK_FALSE(s.description.empty());
                CHECK(s.description.find('%') == std::string::npos);    // tooltips are printf-style formats
                CHECK(s.description.find("\n\n") == std::string::npos);  // no empty comment lines
                CHECK(s.description.back() != '\n');
                CHECK_FALSE(s.maxValue < s.minValue);
                CHECK(p->InRange(s.defaultValue));
                CHECK(p->GetValue() == s.defaultValue);
                CHECK(keys.insert(s.section + "." + s.key).second);  // unique key
                CHECK(labels.insert(s.label).second);                 // unique ImGui id
            },
            ref);
    }
    CHECK(keys.size() == 46);
}

TEST_CASE("Player defaults are canonical and NPC defaults are identical copies")
{
    Settings s;
    const auto pairs = { std::pair{ &s.player.attackCost, &s.npc.attackCost }, { &s.player.attackMult, &s.npc.attackMult },
                         { &s.player.powerAttackMult, &s.npc.powerAttackMult }, { &s.player.sprintCost, &s.npc.sprintCost },
                         { &s.player.swimCost, &s.npc.swimCost }, { &s.player.sneakCost, &s.npc.sneakCost },
                         { &s.player.runCost, &s.npc.runCost }, { &s.player.castCost, &s.npc.castCost },
                         { &s.player.concentrationMult, &s.npc.concentrationMult },
                         { &s.player.bowDrawCost, &s.npc.bowDrawCost }, { &s.player.bowShotCost, &s.npc.bowShotCost },
                         { &s.player.jumpCost, &s.npc.jumpCost }, { &s.player.mountSprintCost, &s.npc.mountSprintCost } };
    for (const auto& [player, npc] : pairs) {
        CAPTURE(player->Spec().key);
        CHECK(player->Spec().defaultValue == npc->Spec().defaultValue);
        CHECK(player->Spec().minValue == npc->Spec().minValue);
        CHECK(player->Spec().maxValue == npc->Spec().maxValue);
    }
    // The shipped values (decision 1.2).
    CHECK(s.player.attackCost.GetValue() == 9.0f);
    CHECK(s.player.jumpCost.GetValue() == 10.0f);
    CHECK(s.player.castCost.GetValue() == 10.0f);
    CHECK(s.player.mountSprintCost.GetValue() == 7.0f);
    CHECK(s.player.bowDrawCost.GetValue() == 5.0f);
    CHECK(s.skills.oneHanded.GetValue() == 0.0f);
    CHECK(s.exemptNpcsWithLowMaxStamina.GetValue());
    CHECK(s.general.logLevel.GetValue() == kLogLevelInfo);
}

TEST_CASE("Costs and multipliers use the 0-20 range; skill scaling 0-1")
{
    Settings s;
    CHECK(s.player.attackCost.Spec().maxValue == 20.0f);
    CHECK(s.player.attackMult.Spec().maxValue == 20.0f);
    CHECK(s.npc.castCost.Spec().maxValue == 20.0f);
    CHECK(s.skills.archery.Spec().maxValue == 1.0f);
}

TEST_CASE("Menu visibility: unimplemented skill multipliers hidden, NPC jump and horse sprint shown")
{
    Settings s;
    CHECK_FALSE(s.skills.speedMult.Spec().visibleInMenu);
    CHECK_FALSE(s.skills.inventoryWeight.Spec().visibleInMenu);
    CHECK(s.skills.speedMult.Spec().description.find("Not implemented") != std::string::npos);
    CHECK(s.npc.jumpCost.Spec().visibleInMenu);
    CHECK(s.npc.mountSprintCost.Spec().visibleInMenu);
}

TEST_CASE("SetValue clamps to the valid range")
{
    Settings s;
    s.player.attackCost.SetValue(500.0f);
    CHECK(s.player.attackCost.GetValue() == 20.0f);
    s.player.attackCost.SetValue(-1.0f);
    CHECK(s.player.attackCost.GetValue() == 0.0f);
    s.general.logLevel.SetValue(42);
    CHECK(s.general.logLevel.GetValue() == kLogLevelOff);
}

TEST_CASE("Serialize -> Load round-trips every value with no diagnostics")
{
    Settings source;
    source.player.attackCost.SetValue(12.5f);
    source.npc.combatOnly.SetValue(false);
    source.skills.block.SetValue(0.25f);
    source.general.logLevel.SetValue(1);
    source.npc.attackMult.SetValue(1.414f);

    const std::string text = Serialize(source);

    Settings   loaded;
    const auto report = LoadFromString(text, loaded);
    CHECK(report.parsed);
    CHECK(report.diagnostics.empty());
    CHECK(loaded.player.attackCost.GetValue() == 12.5f);
    CHECK_FALSE(loaded.npc.combatOnly.GetValue());
    CHECK(loaded.skills.block.GetValue() == 0.25f);
    CHECK(loaded.general.logLevel.GetValue() == 1);
    CHECK(loaded.npc.attackMult.GetValue() == 1.414f);
    CHECK(Serialize(loaded) == text);
}

TEST_CASE("Serialized file documents purpose, default and range for every setting")
{
    Settings          s;
    const std::string text = Serialize(s);
    CHECK(text.find("# Default: 9.0 | Valid: 0.0 to 20.0\nfPlayerStaminaCostAttack = 9.0") != std::string::npos);
    CHECK(text.find("# Default: true | Valid: true or false\nbNPCCombatOnly = true") != std::string::npos);
    CHECK(text.find("# Default: 2 | Valid: 0 to 6\niLogLevel = 2") != std::string::npos);
    CHECK(text.find("Not implemented yet") != std::string::npos);
    CHECK(text.find("[Stamina.ActorValue]") != std::string::npos);
    SUBCASE("multi-line descriptions become one comment line each")
    {
        CHECK(text.find("# Base stamina cost of a melee attack or bash.\n# Then scaled by item weight") != std::string::npos);
        std::istringstream lines(text);
        for (std::string line; std::getline(lines, line);) {
            CAPTURE(line);
            const bool isComment = line.starts_with("#"), isSection = line.starts_with("["), isBlank = line.empty();
            const bool isAssignment = line.find(" = ") != std::string::npos;
            CHECK((isComment || isSection || isBlank || isAssignment));
        }
    }
}

TEST_CASE("Concentration multiplier: own setting per profile, default 1.0 (no change in cost)")
{
    Settings s;
    CHECK(s.player.concentrationMult.GetValue() == 1.0f);
    CHECK(s.player.concentrationMult.Spec().key == "fPlayerStaminaMultConcentration");
    CHECK(s.npc.concentrationMult.Spec().key == "fNPCStaminaMultConcentration");
    CHECK(s.npc.concentrationMult.Spec().maxValue == 20.0f);
    CHECK(s.npc.concentrationMult.Spec().visibleInMenu);
}

TEST_CASE("Loading: missing keys, wrong types, ranges, NaN and unknown keys")
{
    Settings s;
    s.player.attackCost.SetValue(3.0f);  // stale value must be reset by loading

    const auto report = LoadFromString(R"(
[General]
iLogLevel = 1.5

[Stamina.Player]
fPlayerStaminaCostJump = 15
fPlayerStaminaCostSprint = "fast"
fPlayerStaminaCostSwim = 99.0
fPlayerStaminaCostSneak = nan
bPlayerCombatOnly = 0
fPlayerStaminaCostAtack = 4.0

[Stamina.ActorValue]
fAVBlockMult = -0.5
)",
                                       s);
    REQUIRE(report.parsed);

    CHECK(s.player.attackCost.GetValue() == 9.0f);  // missing -> default (stale 3.0 discarded)
    CHECK(HasDiagnostic(report, Severity::Info, "fPlayerStaminaCostAttack: missing"));

    CHECK(s.player.jumpCost.GetValue() == 15.0f);  // integer accepted for a float

    CHECK(s.player.sprintCost.GetValue() == 7.0f);
    CHECK(HasDiagnostic(report, Severity::Warning, "fPlayerStaminaCostSprint: expected a number"));

    CHECK(s.player.swimCost.GetValue() == 20.0f);
    CHECK(HasDiagnostic(report, Severity::Warning, "fPlayerStaminaCostSwim: 99.0 is outside 0.0..20.0; clamped to 20.0"));

    CHECK(s.player.sneakCost.GetValue() == 2.0f);
    CHECK(HasDiagnostic(report, Severity::Warning, "fPlayerStaminaCostSneak: nan is not a finite number"));

    CHECK(s.player.combatOnly.GetValue());
    CHECK(HasDiagnostic(report, Severity::Warning, "bPlayerCombatOnly: expected true or false"));

    CHECK(s.general.logLevel.GetValue() == kLogLevelInfo);
    CHECK(HasDiagnostic(report, Severity::Warning, "iLogLevel: expected an integer"));

    CHECK(s.skills.block.GetValue() == 0.0f);
    CHECK(HasDiagnostic(report, Severity::Warning, "fAVBlockMult: -0.5 is outside 0.0..1.0; clamped to 0.0"));

    CHECK(HasDiagnostic(report, Severity::Warning, "unknown setting 'Stamina.Player.fPlayerStaminaCostAtack'"));
}

TEST_CASE("Unparsable file leaves every setting at its default")
{
    Settings s;
    s.npc.castCost.SetValue(1.0f);
    const auto report = LoadFromString("[Stamina.NPC\nfNPCStaminaCostCast = 3.0\n", s);
    CHECK_FALSE(report.parsed);
    CHECK(report.Count(Severity::Error) == 1);
    CHECK(s.npc.castCost.GetValue() == 10.0f);
}

TEST_CASE("Old shipped file (pre-refactor key order and NPC values) still loads")
{
    Settings   s;
    const auto report = LoadFromString(R"([Stamina.NPC]
bNPCEquippedWeightOnly = true
bNPCCombatOnly = true
bExcludeNoStaminaNPCs = true
fNPCStaminaCostJump = 10.0
fNPCStaminaCostAttack = 10.0
fNPCStaminaCostCast = 9.0
fNPCStaminaCostSprintHorse = 10.0
)",
                                       s);
    CHECK(report.parsed);
    CHECK(report.Count(Severity::Warning) == 0);
    CHECK(s.npc.attackCost.GetValue() == 10.0f);  // a user's explicit value wins over the new default
    CHECK(s.npc.castCost.GetValue() == 9.0f);
}

TEST_CASE("File save and load use the same file")
{
    const auto path = std::filesystem::temp_directory_path() / "exhausting-combat-test.toml";
    std::filesystem::remove(path);

    Settings missing;
    const auto notFound = LoadFromFile(path, missing);
    CHECK_FALSE(notFound.fileFound);
    CHECK(HasDiagnostic(notFound, Severity::Info, "not found"));

    Settings source;
    source.npc.bowShotCost.SetValue(4.0f);
    std::string error;
    REQUIRE(SaveToFile(path, source, error));
    source.npc.bowShotCost.SetValue(6.0f);
    REQUIRE(SaveToFile(path, source, error));  // overwrites in place
    CHECK_FALSE(std::filesystem::exists(path.string() + ".tmp"));

    Settings   loaded;
    const auto report = LoadFromFile(path, loaded);
    CHECK(report.fileFound);
    CHECK(report.diagnostics.empty());
    CHECK(loaded.npc.bowShotCost.GetValue() == 6.0f);
    std::filesystem::remove(path);
}

#ifdef EXHAUSTING_COMBAT_SOURCE_DIR
TEST_CASE("Shipped release TOML is exactly the generated default file")
{
    const std::filesystem::path shipped = std::filesystem::path(EXHAUSTING_COMBAT_SOURCE_DIR) / "release" / "exhausting-combat.toml";
    Settings                    defaults;
    INFO("Regenerate with: exhausting-combat-tests --write-default-config release/exhausting-combat.toml");
    CHECK(ReadFile(shipped) == Serialize(defaults));
}
#endif
