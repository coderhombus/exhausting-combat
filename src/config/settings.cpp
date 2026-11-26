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
    const std::string npcJumpNote   = n.isNpc ? " NPC jumps may not reach the hooked function, so this may have no effect (untested)." : "";
    const std::string npcHorseNote  = n.isNpc ? " Applies to horses ridden by NPCs and to riderless horses (untested)." : "";

    return ActorProfile{
        .equippedWeightOnly = MakeBool(n, "EquippedWeightOnly", "Equipped Weight Only",
                                       "When on, only equipped items count as carried weight for jump, sprint, swim and run costs. "
                                       "When off, the whole inventory weight counts.",
                                       d.equippedWeightOnly),
        .combatOnly         = MakeBool(n, "CombatOnly", "Combat Only", "When on, this mod charges no stamina outside combat.", d.combatOnly),
        .attackCost         = MakeFloat(n, "StaminaCostAttack", "Attack Cost",
                                        "Base stamina cost of a melee attack or bash, before item weight (x1 to x2), multipliers, skill scaling "
                                        "and perks. Unarmed attacks add no cost (the vanilla cost applies).",
                                        d.attackCost, kMinCost, kMaxCost),
        .attackMult         = MakeFloat(n, "StaminaMultAttack", "Attack Mult", "Multiplier applied to every melee attack and bash cost.",
                                        d.attackMult, kMinMult, kMaxMult),
        .powerAttackMult    = MakeFloat(n, "StaminaMultPAttack", "Power Attack Mult",
                                        "Additional multiplier for power attacks, applied on top of Attack Mult.", d.powerAttackMult, kMinMult,
                                        kMaxMult),
        .sprintCost         = MakeFloat(n, "StaminaCostSprint", "Sprint Cost",
                                        "Stamina per second while sprinting on foot, raised by up to 50% at full carry weight.", d.sprintCost,
                                        kMinCost, kMaxCost),
        .swimCost           = MakeFloat(n, "StaminaCostSwim", "Swim Cost",
                                        "Stamina per second while swimming, raised by up to 50% at full carry weight.", d.swimCost, kMinCost,
                                        kMaxCost),
        .sneakCost          = MakeFloat(n, "StaminaCostSneak", "Sneak Cost",
                                        "Stamina per second while moving in sneak mode, reduced by Sneak skill scaling.", d.sneakCost, kMinCost,
                                        kMaxCost),
        .runCost            = MakeFloat(n, "StaminaCostRunning", "Running Cost",
                                        "Stamina per second while running (not sprinting, sneaking or swimming), raised by up to 50% at full "
                                        "carry weight.",
                                        d.runCost, kMinCost, kMaxCost),
        .castCost           = MakeFloat(n, "StaminaCostCast", "Casting Cost",
                                        "Base stamina cost per spell cast (per second for concentration spells). The spell's magicka cost adds "
                                        "up to 25 more; reduced by the spell school's skill scaling. 0 disables spell costs.",
                                        d.castCost, kMinCost, kMaxCost),
        .bowDrawCost        = MakeFloat(n, "StaminaCostBowDrain", "Bow Drawing Cost",
                                        "Stamina per second while holding a drawn bow without zooming in. Also drains during the whole "
                                        "crossbow reload animation.",
                                        d.bowDrawCost, kMinCost, kMaxCost),
        .bowShotCost        = MakeFloat(n, "StaminaCostBow", "Bow Shooting Cost",
                                        "Stamina cost per arrow or bolt fired, reduced by Archery skill scaling. The shot fails if it would "
                                        "leave the shooter with no stamina.",
                                        d.bowShotCost, kMinCost, kMaxCost),
        .jumpCost           = MakeFloat(n, "StaminaCostJump", "Jumping Cost",
                                        "Stamina cost per jump, raised by up to 50% at full carry weight." + npcJumpNote, d.jumpCost, kMinCost,
                                        kMaxCost),
        .mountSprintCost    = MakeFloat(n, "StaminaCostSprintHorse", "Horse Sprint Cost",
                                        "Stamina per second a horse pays while sprinting. A ridden horse uses its rider's settings and perks." +
                                            npcHorseNote,
                                        d.mountSprintCost, kMinCost, kMaxCost),
    };
}

FloatParameter MakeSkill(std::string_view a_keyName, std::string_view a_label, std::string_view a_whatIsReduced, bool a_implemented = true)
{
    std::string description = a_implemented ?
                                  Join("Fraction of ", a_whatIsReduced, " stamina costs removed at skill 100, scaling linearly with skill (0 = off, 1 = free at 100).") :
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
                                         .description  = "Log detail: 0 trace (every per-frame drain), 1 debug (every one-off charge with its "
                                                         "inputs), 2 info, 3 warnings, 4 errors, 5 critical, 6 off. 0 and 1 write large logs.",
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
          .description  = "When on, an NPC whose maximum stamina is lower than the cost of an attack, spell or bow shot performs it for "
                          "free. Many vanilla NPCs have 0 stamina and could otherwise never act.",
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
    all.reserve(48);

    all.emplace_back(&general.logLevel);

    const auto addProfile = [&all](ActorProfile& a_profile, BoolParameter* a_afterCombatOnly) {
        all.emplace_back(&a_profile.equippedWeightOnly);
        all.emplace_back(&a_profile.combatOnly);
        if (a_afterCombatOnly) {
            all.emplace_back(a_afterCombatOnly);
        }
        for (FloatParameter* p : { &a_profile.attackCost, &a_profile.attackMult, &a_profile.powerAttackMult, &a_profile.sprintCost,
                                   &a_profile.swimCost, &a_profile.sneakCost, &a_profile.runCost, &a_profile.castCost,
                                   &a_profile.bowDrawCost, &a_profile.bowShotCost, &a_profile.jumpCost, &a_profile.mountSprintCost }) {
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
