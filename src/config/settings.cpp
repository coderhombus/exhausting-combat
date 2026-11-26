#include "config/settings.h"

#include <string>
#include <string_view>

namespace ExhaustingCombat::Config
{
namespace
{
// ---------------------------------------------------------------------------------------------
// Canonical defaults. Player values are canonical; NPCs start with identical copies.
// To change a default, change it here: the shipped TOML (release/exhausting-combat.toml) is
// generated from this table and a unit test fails if the two drift apart.
// ---------------------------------------------------------------------------------------------
struct ProfileDefaults
{
    bool  equippedWeightOnly = true;
    bool  combatOnly         = true;
    float attackCost         = 9.0f;
    float attackMult         = 1.414f;
    float powerAttackMult    = 2.0f;
    float sprintCost         = 7.0f;
    float swimCost           = 5.0f;
    float sneakCost          = 2.0f;
    float runCost            = 1.0f;
    float castCost           = 10.0f;
    float concentrationMult  = 1.0f;  // 1.0 keeps concentration costs as before this setting existed
    float bowDrawCost        = 5.0f;
    float bowShotCost        = 10.0f;
    float jumpCost           = 10.0f;
    float mountSprintCost    = 7.0f;
};

inline constexpr ProfileDefaults kCanonicalProfileDefaults{};

// Ranges shared by all costs/multipliers of a profile.
inline constexpr float kMinCost = 0.0f;
inline constexpr float kMaxCost = 20.0f;
inline constexpr float kMinMult = 0.0f;
inline constexpr float kMaxMult = 20.0f;

// Skill scaling: fraction of the cost removed at skill 100.
inline constexpr float kMinSkillScaling     = 0.0f;
inline constexpr float kMaxSkillScaling     = 1.0f;
inline constexpr float kDefaultSkillScaling = 0.0f;

inline constexpr std::string_view kGeneralSection      = "General";
inline constexpr std::string_view kSkillScalingSection = "Stamina.ActorValue";

struct ProfileNames
{
    std::string_view who;          // key infix: "Player" / "NPC"
    std::string_view section;      // TOML table
    std::string_view labelSuffix;  // ImGui id suffix
    Group            group;
    bool             isNpc;
};

inline constexpr ProfileNames kPlayerNames{ "Player", "Stamina.Player", "##player", Group::Player, false };
inline constexpr ProfileNames kNpcNames{ "NPC", "Stamina.NPC", "##npc", Group::NPC, true };

std::string Join(std::string_view a, std::string_view b, std::string_view c = {})
{
    std::string out;
    out.reserve(a.size() + b.size() + c.size());
    out.append(a).append(b).append(c);
    return out;
}

BoolParameter MakeBool(const ProfileNames& a_names, std::string_view a_keyStem, std::string_view a_label, std::string a_description,
                       bool a_default)
{
    return BoolParameter{ { .section       = std::string(a_names.section),
                            .key           = Join("b", a_names.who, a_keyStem),
                            .label         = Join(a_label, a_names.labelSuffix),
                            .description   = std::move(a_description),
                            .defaultValue  = a_default,
                            .minValue      = false,
                            .maxValue      = true,
                            .group         = a_names.group,
                            .visibleInMenu = true } };
}

FloatParameter MakeFloat(const ProfileNames& a_names, std::string_view a_keyStem, std::string_view a_label, std::string a_description,
                         float a_default, float a_min, float a_max)
{
    return FloatParameter{ { .section       = std::string(a_names.section),
                             .key           = Join("f", a_names.who, a_keyStem),
                             .label         = Join(a_label, a_names.labelSuffix),
                             .description   = std::move(a_description),
                             .defaultValue  = a_default,
                             .minValue      = a_min,
                             .maxValue      = a_max,
                             .group         = a_names.group,
                             .visibleInMenu = true } };
}

ActorProfile MakeProfile(const ProfileNames& n, const ProfileDefaults& d)
{
    // Descriptions are shown as menu tooltips (one line per '\n') and as TOML comments.
    // Never use '%': FUCK passes tooltip text to ImGui as a printf-style format string.
    const std::string npcJumpNote  = n.isNpc ? "\nNPC jumps may not reach the hooked function, so this may have no effect (untested)." : "";
    const std::string npcHorseNote = n.isNpc ? "\nApplies to horses ridden by NPCs and to riderless horses (untested)." : "";
    const std::string loadNote     = "\nRaised by carried weight: x1.5 at full carry weight, more when over-encumbered.";

    return ActorProfile{
        .equippedWeightOnly = MakeBool(n, "EquippedWeightOnly", "Equipped Weight Only",
                                       "On: only equipped items count as carried weight for jump, sprint, swim and run costs.\n"
                                       "Off: the whole inventory weight counts.",
                                       d.equippedWeightOnly),
        .combatOnly         = MakeBool(n, "CombatOnly", "Combat Only", "On: this mod charges no stamina outside combat.", d.combatOnly),
        .attackCost         = MakeFloat(n, "StaminaCostAttack", "Attack Cost",
                                        "Base stamina cost of a melee attack or bash.\n"
                                        "Then scaled by item weight (x1 to x2), Attack Mult, Power Attack Mult, skill scaling and perks.\n"
                                        "Unarmed attacks add no cost (the vanilla cost applies).",
                                        d.attackCost, kMinCost, kMaxCost),
        .attackMult         = MakeFloat(n, "StaminaMultAttack", "Attack Mult", "Multiplier applied to every melee attack and bash cost.",
                                        d.attackMult, kMinMult, kMaxMult),
        .powerAttackMult    = MakeFloat(n, "StaminaMultPAttack", "Power Attack Mult",
                                        "Additional multiplier for power attacks only, applied on top of Attack Mult.", d.powerAttackMult,
                                        kMinMult, kMaxMult),
        .sprintCost         = MakeFloat(n, "StaminaCostSprint", "Sprint Cost", "Stamina per second while sprinting on foot." + loadNote,
                                        d.sprintCost, kMinCost, kMaxCost),
        .swimCost           = MakeFloat(n, "StaminaCostSwim", "Swim Cost", "Stamina per second while swimming." + loadNote, d.swimCost,
                                        kMinCost, kMaxCost),
        .sneakCost          = MakeFloat(n, "StaminaCostSneak", "Sneak Cost",
                                        "Stamina per second while moving in sneak mode.\nReduced by Sneak skill scaling.", d.sneakCost, kMinCost,
                                        kMaxCost),
        .runCost            = MakeFloat(n, "StaminaCostRunning", "Running Cost",
                                        "Stamina per second while running (not sprinting, sneaking or swimming)." + loadNote, d.runCost,
                                        kMinCost, kMaxCost),
        .castCost           = MakeFloat(n, "StaminaCostCast", "Casting Cost",
                                        "Base stamina cost per spell cast (per second for concentration spells).\n"
                                        "The spell's magicka cost adds up to 25 more; reduced by the spell school's skill scaling.\n"
                                        "0 disables spell costs.",
                                        d.castCost, kMinCost, kMaxCost),
        .concentrationMult  = MakeFloat(n, "StaminaMultConcentration", "Concentration Mult",
                                        "Multiplier for concentration spells only, applied on top of the casting cost per second.\n"
                                        "1.0 = a second of concentration costs the same as one fire-and-forget cast.",
                                        d.concentrationMult, kMinMult, kMaxMult),
        .bowDrawCost        = MakeFloat(n, "StaminaCostBowDrain", "Bow Drawing Cost",
                                        "Stamina per second while holding a drawn bow without zooming in.\n"
                                        "Also drains during the whole crossbow reload animation.",
                                        d.bowDrawCost, kMinCost, kMaxCost),
        .bowShotCost        = MakeFloat(n, "StaminaCostBow", "Bow Shooting Cost",
                                        "Stamina cost per arrow or bolt fired, reduced by Archery skill scaling.\n"
                                        "The shot fails if it would leave the shooter with no stamina.",
                                        d.bowShotCost, kMinCost, kMaxCost),
        .jumpCost           = MakeFloat(n, "StaminaCostJump", "Jumping Cost", "Stamina cost per jump." + loadNote + npcJumpNote, d.jumpCost,
                                        kMinCost, kMaxCost),
        .mountSprintCost    = MakeFloat(n, "StaminaCostSprintHorse", "Horse Sprint Cost",
                                        "Stamina per second a horse pays while sprinting.\nA ridden horse uses its rider's settings and perks." +
                                            npcHorseNote,
                                        d.mountSprintCost, kMinCost, kMaxCost),
    };
}

FloatParameter MakeSkill(std::string_view a_keyName, std::string_view a_label, std::string_view a_whatIsReduced, bool a_implemented = true)
{
    std::string description = a_implemented ?
                                  Join("Fraction of ", a_whatIsReduced, " stamina costs removed at skill 100, scaling linearly with skill.\n0 = off, 1 = free at skill 100.") :
                                  std::string("Not implemented yet: this value currently has no effect.");
    return FloatParameter{ { .section       = std::string(kSkillScalingSection),
                             .key           = Join("fAV", a_keyName, "Mult"),
                             .label         = Join(a_label, " Scaling##skill"),
                             .description   = std::move(description),
                             .defaultValue  = kDefaultSkillScaling,
                             .minValue      = kMinSkillScaling,
                             .maxValue      = kMaxSkillScaling,
                             .group         = Group::SkillScaling,
                             .visibleInMenu = a_implemented } };
}
}  // namespace

const FloatParameter& SkillScaling::For(Skill a_skill) const noexcept
{
    switch (a_skill) {
        case Skill::OneHanded:
            return oneHanded;
        case Skill::TwoHanded:
            return twoHanded;
        case Skill::Archery:
            return archery;
        case Skill::Block:
            return block;
        case Skill::HeavyArmor:
            return heavyArmor;
        case Skill::LightArmor:
            return lightArmor;
        case Skill::Sneak:
            return sneak;
        case Skill::Alteration:
            return alteration;
        case Skill::Illusion:
            return illusion;
        case Skill::Destruction:
            return destruction;
        case Skill::Conjuration:
            return conjuration;
        case Skill::Restoration:
            return restoration;
        case Skill::SpeedMult:
            return speedMult;
        case Skill::InventoryWeight:
            return inventoryWeight;
    }
    return oneHanded;  // unreachable for valid enum values
}

Settings::Settings() :
    general{ .logLevel = IntParameter{ { .section      = std::string(kGeneralSection),
                                         .key          = "iLogLevel",
                                         .label        = "Log Level##general",
                                         .description  = "Log detail: 0 trace, 1 debug, 2 info, 3 warnings, 4 errors, 5 critical, 6 off.\n"
                                                         "1 logs every attack, cast, shot and jump with its inputs; 0 also logs per-frame drains.\n"
                                                         "0 and 1 write large logs.",
                                         .defaultValue = kLogLevelInfo,
                                         .minValue     = kLogLevelTrace,
                                         .maxValue     = kLogLevelOff,
                                         .group        = Group::General } } },
    player(MakeProfile(kPlayerNames, kCanonicalProfileDefaults)),
    npc(MakeProfile(kNpcNames, kCanonicalProfileDefaults)),
    exemptNpcsWithLowMaxStamina(BoolParameter{
        { .section      = std::string(kNpcNames.section),
          .key          = "bExcludeNoStaminaNPCs",
          .label        = "No Stamina Exception##npc",
          .description  = "On: an NPC whose maximum stamina is lower than the cost of an attack, spell or bow shot performs it for free.\n"
                          "Many vanilla NPCs have 0 stamina and could otherwise never act.",
          .defaultValue = true,
          .minValue     = false,
          .maxValue     = true,
          .group        = Group::NPC } }),
    skills{
        .oneHanded       = MakeSkill("OneHanded", "One-Handed", "one-handed weapon attack"),
        .twoHanded       = MakeSkill("TwoHanded", "Two-Handed", "two-handed weapon attack"),
        .archery         = MakeSkill("Archery", "Archery", "bow and crossbow (shot, draw and bash)"),
        .block           = MakeSkill("Block", "Block", "bash"),
        .heavyArmor      = MakeSkill("HeavyArmor", "Heavy Armor", "heavy shield bash"),
        .lightArmor      = MakeSkill("LightArmor", "Light Armor", "light shield bash"),
        .sneak           = MakeSkill("Sneak", "Sneak", "sneak movement"),
        .alteration      = MakeSkill("Alteration", "Alteration", "Alteration spell"),
        .illusion        = MakeSkill("Illusion", "Illusion", "Illusion spell"),
        .destruction     = MakeSkill("Destruction", "Destruction", "Destruction spell"),
        .conjuration     = MakeSkill("Conjuration", "Conjuration", "Conjuration spell"),
        .restoration     = MakeSkill("Restoration", "Restoration", "Restoration spell"),
        .speedMult       = MakeSkill("SpeedMult", "Speed Mult", "", false),
        .inventoryWeight = MakeSkill("InventoryWeight", "Inventory Weight", "", false),
    }
{}

std::vector<ParameterRef> Settings::All()
{
    std::vector<ParameterRef> all;
    all.reserve(52);

    all.emplace_back(&general.logLevel);

    const auto addProfile = [&all](ActorProfile& a_profile, BoolParameter* a_afterCombatOnly) {
        all.emplace_back(&a_profile.equippedWeightOnly);
        all.emplace_back(&a_profile.combatOnly);
        if (a_afterCombatOnly) {
            all.emplace_back(a_afterCombatOnly);
        }
        for (FloatParameter* p : { &a_profile.attackCost, &a_profile.attackMult, &a_profile.powerAttackMult, &a_profile.sprintCost,
                                   &a_profile.swimCost, &a_profile.sneakCost, &a_profile.runCost, &a_profile.castCost,
                                   &a_profile.concentrationMult, &a_profile.bowDrawCost, &a_profile.bowShotCost, &a_profile.jumpCost, &a_profile.mountSprintCost }) {
            all.emplace_back(p);
        }
    };
    addProfile(player, nullptr);
    addProfile(npc, &exemptNpcsWithLowMaxStamina);

    for (FloatParameter* p : { &skills.oneHanded, &skills.twoHanded, &skills.archery, &skills.block, &skills.heavyArmor, &skills.lightArmor,
                               &skills.sneak, &skills.alteration, &skills.illusion, &skills.destruction, &skills.conjuration,
                               &skills.restoration, &skills.speedMult, &skills.inventoryWeight }) {
        all.emplace_back(p);
    }
    return all;
}

void Settings::ResetToDefaults()
{
    for (const auto& ref : All()) {
        std::visit([](auto* a_parameter) { a_parameter->ResetToDefault(); }, ref);
    }
}

Settings& Get()
{
    static Settings settings;
    return settings;
}
}  // namespace ExhaustingCombat::Config
