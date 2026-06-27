#include "stamina-manager.h"

#include "API/PerkEntryPointExtenderAPI.h"
#include "RE/A/ActorValues.h"
#include "RE/T/TESObjectWEAP.h"
#include "Utility/util.h"
#include "config.h"
#include "mod-data.h"
#include "versionlibdb.h"

namespace EXCO
{

void StaminaCost::ManageSneakStamina(RE::Actor* a_actor, float a_deltaTime)
{
    if (!ActorValidAndNotGod(a_actor))
        return;

    if (!a_actor->IsSneaking() || !ActorUtil::IsMoving(a_actor))
        return;

    if (a_actor->GetActorValue(RE::ActorValue::kStamina) <= 0.f)
        return;

    auto cond_item     = GetRelevantWeapon(a_actor);
    float default_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_sneak_player.GetValue()
                                                : CONFIG::stamina_cost_sneak_npc.GetValue();

    float multAV = AVScaling(a_actor, RE::ActorValue::kSneak);
    float reduce = default_cost * multAV * a_deltaTime;

    // PEPE Perk
    RE::HandleEntryPoint(ENTRIES::sneakStamEP, a_actor, reduce, ENTRIES::sneakStam, cond_item);

    a_actor->DamageActorValue(RE::ActorValue::kStamina, reduce);
}

void StaminaCost::ManageJumpStamina(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
        return;
    float jump_cost = CalculateJumpCost(a_actor);
    auto cond_item  = GetRelevantWeapon(a_actor);

    RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(a_actor);
    RE::HandleEntryPoint(ENTRIES::jumpStamEP, a_actor, jump_cost, ENTRIES::jumpStam, cond_item);

    damaged_actor->DamageActorValue(RE::ActorValue::kStamina, jump_cost);
}

void StaminaCost::ManageSprintStamina(RE::Actor* a_actor, float a_deltaTime)
{
    if (!ActorValidAndNotGod(a_actor))
    {
        return;
    }
    if (!a_actor->IsSprinting() || !ActorUtil::IsMoving(a_actor))
    {
        return;
    }

    if (a_actor->GetActorValue(RE::ActorValue::kStamina) <= 0)
    {
        return;
    }

    // Special case for horse.
    // Since this runs also in the ActorUpdate loop, i can't just get the
    // mount as this runs on the horse as well
    if (a_actor->IsAMount())
    {
        RE::ActorPtr rider{};
        a_actor->GetMountedBy(rider);

        auto apply_on   = rider ? rider.get() : a_actor;
        float base_cost = CalculateHorseSprintCost(apply_on);
        float cost      = base_cost * a_deltaTime;
        auto cond_item  = GetRelevantWeapon(apply_on);

        RE::HandleEntryPoint(ENTRIES::mountSprintStamEP, apply_on, cost, ENTRIES::mountSprintStam, cond_item);

        a_actor->DamageActorValue(RE::ActorValue::kStamina, cost);
        // return to not have it run on the rider as well and double apply it
        return;
    }

    float default_cost = CalculateSprintCost(a_actor);
    float reduce       = default_cost * a_deltaTime;
    auto weapon        = GetRelevantWeapon(a_actor);

    RE::HandleEntryPoint(ENTRIES::sprintStamEP, a_actor, reduce, ENTRIES::sprintStam, weapon);

    a_actor->DamageActorValue(RE::ActorValue::kStamina, reduce);
}

bool StaminaCost::ManageCastStamina(RE::ActorMagicCaster* a_caster)
{
    if (!a_caster)
        return true;

    // not sure if casting type scroll is a thing, but better exclude it here
    if (ActorUtil::CasterIsCastingObject(a_caster))
    {
        return true;
    }

    if (!a_caster->currentSpell)
    {
        return true;
    }

    auto spell = a_caster->currentSpell;

    // concentration is handled in the update ActorMagicCaster update loop
    if (spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration)
    {
        return true;
    }

    auto actor = a_caster->GetCasterAsActor();
    if (!actor)
    {
        return true;
    }

    if (!MagicUtil::IsSpellPlayable(spell))
    {
        return true;
    }

    float stamina_cost = CalculateCastCost(a_caster, spell);

    RE::HandleEntryPoint(ENTRIES::castStamEP, actor, stamina_cost, ENTRIES::castStam, spell);
    RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(actor);

    damaged_actor->DamageActorValue(RE::ActorValue::kStamina, stamina_cost);

    return damaged_actor->GetActorValue(RE::ActorValue::kStamina) > 0.f;
}

// Concentration, as always, needs special handling.
// needs to be in update loop otherwise it only costs once when activating
bool StaminaCost::ManageConcentrationStamina(RE::ActorMagicCaster* a_caster, float a_deltaTime)
{
    if (!a_caster)
    {
        return true;
    }
    if (ActorUtil::CasterIsCastingObject(a_caster))
    {
        return true;
    }
    if (!a_caster->currentSpell)
    {
        return true;
    }
    auto actor = a_caster->GetCasterAsActor();
    if (!actor)
    {
        return true;
    }

    auto spell = a_caster->currentSpell;

    if (!MagicUtil::IsSpellPlayable(spell))
    {
        return true;
    }

    float stamina_cost = CalculateCastCost(a_caster, spell) * a_deltaTime;

    RE::HandleEntryPoint(ENTRIES::castStamEP, actor, stamina_cost, ENTRIES::castStam, spell);

    RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(actor);
    damaged_actor->DamageActorValue(RE::ActorValue::kStamina, stamina_cost);

    return damaged_actor->GetActorValue(RE::ActorValue::kStamina) > 0.f;
}

float StaminaCost::ManageAttackStamina(RE::Actor* a_actor, const RE::BGSAttackData* a_attack)
{
    if (!ActorValidAndNotGod(a_actor))
        return 0;
    if (!a_attack)
        return 0;

    const bool isBash        = a_attack->data.flags.any(RE::AttackData::AttackFlag::kBashAttack);
    const bool isPowerAttack = a_attack->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);

    logs::info("bash={} leftAttack={} event={}", isBash, a_attack->IsLeftAttack(), a_attack->event.c_str());
    float attack_cost = CalculateAttackCost(a_actor, isBash, isPowerAttack);

    if (auto* rightObj = a_actor->GetEquippedObject(true); isBash && rightObj && rightObj->As<RE::TESObjectLIGH>())
    {
        const auto torch = rightObj->As<RE::TESObjectLIGH>(); // was a pointer
        logs::info("global get torch name: {}", torch ? torch->GetName() : "none");
        RE::HandleEntryPoint(ENTRIES::attackStamEP, a_actor, attack_cost, ENTRIES::attackStam, torch);
    }

    else if (auto* shield = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
             shield && isBash && shield->IsShield())
    {
        logs::info("global get weapon name: {}", shield ? shield->GetName() : "none");
        RE::HandleEntryPoint(ENTRIES::attackStamEP, a_actor, attack_cost, ENTRIES::attackStam, shield);
    }

    else
    {
        const auto weapon = GetRelevantWeapon(a_actor);
        logs::info("global get weapon name: {}", weapon ? weapon->GetName() : "none");
        RE::HandleEntryPoint(ENTRIES::attackStamEP, a_actor, attack_cost, ENTRIES::attackStam, weapon);
    }
    // RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(a_actor);
    // damaged_actor->DamageActorValue(RE::ActorValue::kStamina, attack_cost);
    return attack_cost;
}

// this is the actual shot.
// still debating to leave it, but i think i am fine with bows costing while
// drawing and extra per shot compensates for the higher move stamina cost for
// melee
bool StaminaCost::ManageBowStamina(RE::Actor* a_actor, RE::TESObjectWEAP* a_weapon)
{
    if (!ActorValidAndNotGod(a_actor))
        return true;
    float stamina_cost =
        a_actor->IsPlayerRef() ? CONFIG::stamina_cost_bow_player.GetValue() : CONFIG::stamina_cost_bow_npc.GetValue();

    stamina_cost *= AVScaling(a_actor, RE::ActorValue::kArchery);

    RE::HandleEntryPoint(ENTRIES::bowStamEP, a_actor, stamina_cost, ENTRIES::bowStam, a_weapon);

    RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(a_actor);
    damaged_actor->DamageActorValue(RE::ActorValue::kStamina, stamina_cost);

    return damaged_actor->GetActorValue(RE::ActorValue::kStamina) > 0.f;
}

// costs stamina for the entire reload animation duration for crossbows
// requiem users will love this
void StaminaCost::ManageBowDraw(RE::Actor* a_actor, float a_deltaTime)
{
    if (!ActorValidAndNotGod(a_actor))
        return;
    if (!Util::IsBowDrawNoZoom(a_actor))
        return;

    auto equipped_weapon = skyrim_cast<RE::TESObjectWEAP*>(a_actor->GetEquippedObject(false));
    float base_cost      = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_bow_drain_player.GetValue()
                                                  : CONFIG::stamina_cost_bow_drain_npc.GetValue();
    float multAV         = AVScaling(a_actor, RE::ActorValue::kArchery);
    float stam_cost      = base_cost * multAV * a_deltaTime;

    RE::HandleEntryPoint(ENTRIES::bowDrawStamEP, a_actor, stam_cost, ENTRIES::bowDrawStam, equipped_weapon);

    RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(a_actor);
    damaged_actor->DamageActorValue(RE::ActorValue::kStamina, stam_cost);
}

float StaminaCost::GetAttackStaminaCost(RE::Actor* a_actor, bool isBash, bool isPowerAttack)
{
    return CalculateAttackCost(a_actor, isBash, isPowerAttack);
}

float StaminaCost::GetAttackStaminaCost_Safe(RE::ActorValueOwner* a_actor, RE::BGSAttackData* a_attack)
{
    const bool isBash        = a_attack->data.flags.any(RE::AttackData::AttackFlag::kBashAttack);
    const bool isPowerAttack = a_attack->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);
    const auto actor         = skyrim_cast<RE::Actor*>(a_actor);
    return CalculateAttackCost(actor, isBash, isPowerAttack);
}

// vanilla weapon weights
static constexpr float MIN_WEIGHT = 2.0;
static constexpr float MAX_WEIGHT = 33.0;

float StaminaCost::AVScaling(RE::Actor* a_actor, RE::ActorValue a_akValue)
{
    // scaling for 1H, 2H, Bash, Archery, Magic, Sneak, Acrobatics
    // Pass in  relevant skill, return relevant scaling
    // GetActorValue($1), scaled by CONFIG::SkillScaling($1)
    float mult;
    switch (a_akValue)
    {
        case RE::ActorValue::kOneHanded:
            mult = CONFIG::stamina_av_mult_onehanded.GetValue();
            break;
        case RE::ActorValue::kTwoHanded:
            mult = CONFIG::stamina_av_mult_twohanded.GetValue();
            break;
        case RE::ActorValue::kArchery:
            mult = CONFIG::stamina_av_mult_archery.GetValue();
            break;
        case RE::ActorValue::kBlock:
            mult = CONFIG::stamina_av_mult_block.GetValue();
            break;
        case RE::ActorValue::kHeavyArmor:
            mult = CONFIG::stamina_av_mult_heavyarmor.GetValue();
            break;
        case RE::ActorValue::kLightArmor:
            mult = CONFIG::stamina_av_mult_lightarmor.GetValue();
            break;
        case RE::ActorValue::kSneak:
            mult = CONFIG::stamina_av_mult_sneak.GetValue();
            break;
        case RE::ActorValue::kAlteration:
            mult = CONFIG::stamina_av_mult_alteration.GetValue();
            break;
        case RE::ActorValue::kIllusion:
            mult = CONFIG::stamina_av_mult_illusion.GetValue();
            break;
        case RE::ActorValue::kDestruction:
            mult = CONFIG::stamina_av_mult_destruction.GetValue();
            break;
        case RE::ActorValue::kConjuration:
            mult = CONFIG::stamina_av_mult_conjuration.GetValue();
            break;
        case RE::ActorValue::kRestoration:
            mult = CONFIG::stamina_av_mult_restoration.GetValue();
            break;
        case RE::ActorValue::kSpeedMult: // running faster costs more stamina?
            mult = CONFIG::stamina_av_mult_speedmult.GetValue();
            mult = 0;                    // return its own separate value here
            logs::warn("kSpeedMult Actor Value Scaling has not yet been implemented");
            break;
        case RE::ActorValue::kInventoryWeight: // replace other code with this function
            mult = CONFIG::stamina_av_mult_inventoryweight.GetValue();
            mult = 0;                          // return its own separate value here
            logs::warn("kInventoryWeight Actor Value Scaling has not yet been implemented");
            break;
        default:
            mult = 0.0f;
            break;
    }

    if (mult > 1.0f || mult < 0.0f)
        logs::warn("AVScaling clamp: value {} for {}", mult, (int)a_akValue);

    if (mult > 1.0f)
        mult = 1.0f;
    else if (mult < 0.0f)
        mult = 0.0f;

    const float skillLevel = a_actor->GetActorValue(a_akValue);

    // 100 skill & 0.1 mult in config -> 10% stamina cost reduction at skill 100
    const float scalar = skillLevel / 100.0f;
    const float res    = 1.0f - (scalar * mult);

    return std::max(0.0f, res);
}

RE::ActorValue StaminaCost::AssociatedSkillWeapon(RE::TESObjectWEAP* weap)
{
    if (weap->IsOneHandedAxe() || weap->IsOneHandedDagger() || weap->IsOneHandedMace() || weap->IsOneHandedSword())
        return RE::ActorValue::kOneHanded;
    else if (weap->IsTwoHandedAxe() || weap->IsTwoHandedSword())
        return RE::ActorValue::kTwoHanded;
    // add magic, archery, unarmed -> scale with Hand to Hand / Bruiser / Vicn Hand to Hand

    logs::warn("Weapon is not an Axe, Dagger, Mace, Sword, BattleAxe or Greatsword: {}", weap->GetFullName());
    return RE::ActorValue::kNone;
}

RE::TESObjectWEAP* StaminaCost::GetRelevantWeapon(RE::Actor* a_actor)
{
    auto* left  = a_actor->GetEquippedObject(false);
    auto* right = a_actor->GetEquippedObject(true);

    auto* leftWeap  = left ? left->As<RE::TESObjectWEAP>() : nullptr;
    auto* rightWeap = right ? right->As<RE::TESObjectWEAP>() : nullptr;

    // if dual wielding, or bashing with no weapons, yet both weapons are equipped (doesn't occur in vanilla)
    if (leftWeap && rightWeap)
        return rightWeap;
    else if (leftWeap)
        return leftWeap;
    else if (rightWeap)
        //  could be staff in left hand, unarmed in right
        return rightWeap;
    else
        // runs if attacking when leftWeap and rightWeap are non-weapon items or Dual Unarmed
        return nullptr;


    return nullptr;
}
float StaminaCost::CalculateAttackCost(RE::Actor* a_actor, bool isBash, bool isPowerAttack)
{
    // CONFIG values
    const auto base_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_attack_player.GetValue()
                                                  : CONFIG::stamina_cost_attack_npc.GetValue();

    const float mult = a_actor->IsPlayerRef() ? CONFIG::stamina_mult_attack_player.GetValue()
                                              : CONFIG::stamina_mult_attack_npc.GetValue();
    float multPA     = a_actor->IsPlayerRef() ? CONFIG::stamina_mult_pattack_player.GetValue()
                                              : CONFIG::stamina_mult_pattack_npc.GetValue();

    if (!isPowerAttack) // multPA only applies during a power attack
        multPA = 1.0f;

    float multAV = 1.0f; // stacking multiplier from actor values
    float weight = 1.0f; // item weight resolved below

    // stamina cost modified by block skill when bashing
    if (isBash)
        multAV *= AVScaling(a_actor, RE::ActorValue::kBlock);

    // Check Shields and Torches first
    if (auto* shield = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
        shield && isBash && shield->IsShield())
    {
        // additionally scales with heavy/light armor skill depending on shield type
        if (shield->IsHeavyArmor())
            multAV *= AVScaling(a_actor, RE::ActorValue::kHeavyArmor);
        else if (shield->IsLightArmor())
            multAV *= AVScaling(a_actor, RE::ActorValue::kLightArmor);
        weight = std::max(shield->GetWeight(), 1.0f);
        logs::info("Shield Bash: ({})", shield->GetName());
    }

    // Check if this works with any other light sources / lanterns etc
    else if (auto* rightObj = a_actor->GetEquippedObject(true); rightObj && rightObj->As<RE::TESObjectLIGH>())
    {
        weight = std::max(rightObj->GetWeight(), 1.0f);
        logs::info("Torch Bash: ({})", rightObj->GetName());
    }
    // Weapon
    else if (auto const weap = GetRelevantWeapon(a_actor); weap && weap->As<RE::TESObjectWEAP>())
    {
        // vanilla min/max weight: 2/33
        // vanilla average weight ~17
        // formula: result = (weight - min) / (max - min)

        // If bashing without a shield
        // GetRelevantWeapon() gets and checks each hand's equipped weapons

        // scales with 1H/2H skill depending on weapon type
        multAV *= AVScaling(a_actor, AssociatedSkillWeapon(weap));

        weight = std::max(weap->GetWeight(), 1.0f);
        logs::info("Weapon: ({})", weap->GetName());
    }
    else
    {
        logs::info("Item belonging to ({}) not identified", a_actor->GetName());
    }

    const float modifier =
        (1.0f + std::clamp((weight - MIN_WEIGHT) / (MAX_WEIGHT - MIN_WEIGHT), 0.0f, 1.0f)) * mult * multPA * multAV;

    const float result = base_cost * modifier;

    logs::info("mult: {}, multPA {}, weight_mult {}", mult, multPA,
               (1.0 + std::clamp((weight - MIN_WEIGHT) / (MAX_WEIGHT - MIN_WEIGHT), 0.0f, 1.0f)));
    logs::info("base_cost = {}, multiplier = {}, stamina cost = {}, weight = {}, isBash = {}, isPA = {}", base_cost,
               modifier, result, weight, isBash, isPowerAttack);

    return result;
}

static constexpr float CAST_MODIFIER     = 0.70f;
static constexpr float MAX_CAST_COST_ADD = 25.f;
float StaminaCost::CalculateCastCost(const RE::ActorMagicCaster* a_caster, const RE::MagicItem* a_spell)
{
    // is probably never null, but better safe than sorry
    if (!a_caster || !a_spell)
        return 0.f;
    auto actor = a_caster->GetCasterAsActor();
    if (!ActorValidAndNotGod(actor))
        return 0.f;

    auto base_cost = actor->IsPlayerRef() ? CONFIG::stamina_cost_casting_player.GetValue()
                                          : CONFIG::stamina_cost_casting_npc.GetValue();
    // prevent 0 magicka cost from costing 0 stamina
    auto mag_cost = std::max(a_spell->CalculateMagickaCost(actor), 1.0f);

    // magicka cost can only add up to 25 stamina cost to base
    auto cast_cost_addition = std::min(std::sqrt(mag_cost) * CAST_MODIFIER, MAX_CAST_COST_ADD);
    auto cost               = base_cost + cast_cost_addition;

    const float multAV = AVScaling(a_caster->GetCasterAsActor(), a_spell->GetAssociatedSkill());
    return cost * multAV;
}

bool StaminaCost::DumpSpecificVersion()
{
    VersionDb db;

    // Try to load database of version 1.5.62.0 regardless of running executable version.
    if (!db.Load(1, 6, 1170, 0))
    {
        logs::warn("Failed to load database for 1.6.1170.0!");
        return false;
    }

    // Write out a file called offsets-1.5.62.0.txt where each line is the ID and offset.
    db.Dump("offsets-1.6.1170.0.txt");
    logs::info("Dumped offsets for 1.6.1170.0");
    return true;
}

float StaminaCost::CalculateJumpCost(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
        return 0.f;
    float base_cost =
        a_actor->IsPlayerRef() ? CONFIG::stamina_cost_jump_player.GetValue() : CONFIG::stamina_cost_jump_npc.GetValue();

    // DumpSpecificVersion();
    return CalculateWeightModi(base_cost, a_actor);
}

float StaminaCost::CalculateSprintCost(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
        return 0.f;
    float base_sprint_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_sprint_player.GetValue()
                                                    : CONFIG::stamina_cost_sprint_npc.GetValue();

    return CalculateWeightModi(base_sprint_cost, a_actor);
}

float StaminaCost::CalculateWeightModi(float a_baseCost, const RE::Actor* a_actor)
{
    // full weight adds 50% of the base
    const float carry_weight    = std::max(a_actor->GetActorValue(RE::ActorValue::kCarryWeight), 1.0f);
    const float inventoryWeight = a_actor->GetActorValue(RE::ActorValue::kInventoryWeight);
    const float equippedWeight  = const_cast<RE::Actor*>(a_actor)->GetEquippedWeight();
    const bool playerOrNPC      = a_actor->IsPlayerRef() ? CONFIG::equipped_weight_only_player.GetValue()
                                                         : CONFIG::equipped_weight_only_npc.GetValue();

    const float controlWeight = playerOrNPC ? equippedWeight : inventoryWeight;
    const float modi          = controlWeight / carry_weight;

    // logs::info("inventoryWeight: {}", inventoryWeight);
    // logs::info("equippedWeight: {}", equippedWeight);
    // logs::info("modi: {}", modi);

    return a_baseCost + modi * (a_baseCost / 2);
}

float StaminaCost::CalculateHorseSprintCost(RE::Actor* a_actor)
{
    // horse stamina is a flat cost
    if (!ActorValidAndNotGod(a_actor))
        return 0.f;

    float ret = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_sprint_horse_player.GetValue()
                                       : CONFIG::stamina_cost_sprint_horse_npc.GetValue();
    return ret;
}

// convenience function.
// basically replaces (!a_actor) return; (!a_actor->IsPlayerRef() &&
// RE::PlayerCharacter::IsGodMode()) checks in every stamina related function
bool StaminaCost::ActorValidAndNotGod(RE::Actor* a_actor)
{
    if (!a_actor)
    {
        return false;
    }
    if (ActorUtil::IsGod(a_actor))
    {
        return false;
    }
    return true;
}

float StaminaCost::CalculateSwimCost(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
    {
        return 0.F;
    }
    float base_swim_cost =
        a_actor->IsPlayerRef() ? CONFIG::stamina_cost_swim_player.GetValue() : CONFIG::stamina_cost_swim_npc.GetValue();

    return CalculateWeightModi(base_swim_cost, a_actor);
}
float StaminaCost::CalculateRunningCost(RE::Actor* a_actor)
{

    if (!ActorValidAndNotGod(a_actor))
    {
        return 0.F;
    }
    float base_running_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_running_player.GetValue()
                                                     : CONFIG::stamina_cost_running_npc.GetValue();
    return CalculateWeightModi(base_running_cost, a_actor);
}


void StaminaCost::ManageSwimStamina(RE::Actor* a_actor, float a_deltaTime)
{

    if (!ActorValidAndNotGod(a_actor))
    {
        return;
    }
    if (!a_actor->IsSwimming() || !ActorUtil::IsMoving(a_actor))
    {
        return;
    }

    if (a_actor->IsAMount())
    {
        RE::ActorPtr rider{};
        a_actor->GetMountedBy(rider);
        auto apply_on   = rider.get() ? rider.get() : a_actor;
        float base_cost = CalculateSwimCost(apply_on);
        float cost      = base_cost * a_deltaTime;
        auto cond_item  = GetRelevantWeapon(rider.get());

        RE::HandleEntryPoint(ENTRIES::swimStamEP, apply_on, cost, ENTRIES::swimStam, cond_item);

        a_actor->DamageActorValue(RE::ActorValue::kStamina, cost);
        // return to not have it run on the rider as well and double apply it
        return;
    }

    float default_cost = CalculateSwimCost(a_actor);
    float reduce       = default_cost * a_deltaTime;
    auto weapon        = GetRelevantWeapon(a_actor);

    RE::HandleEntryPoint(ENTRIES::swimStamEP, a_actor, reduce, ENTRIES::swimStam, weapon);

    a_actor->DamageActorValue(RE::ActorValue::kStamina, reduce);
}
void StaminaCost::ManageRunningStamina(RE::Actor* a_actor, float a_deltaTime)
{

    if (!ActorValidAndNotGod(a_actor))
    {
        return;
    }

    if (!a_actor->IsRunning() || !ActorUtil::IsMoving(a_actor))
    {
        return;
    }

    if (a_actor->IsAMount())
    {
        RE::ActorPtr rider{};
        a_actor->GetMountedBy(rider);
        auto apply_on   = rider.get() ? rider.get() : a_actor;
        float base_cost = CalculateRunningCost(apply_on);
        float cost      = base_cost * a_deltaTime;
        auto cond_item  = GetRelevantWeapon(rider.get());

        RE::HandleEntryPoint(ENTRIES::runStamEP, apply_on, cost, ENTRIES::runStam, cond_item);

        a_actor->DamageActorValue(RE::ActorValue::kStamina, cost);
        // return to not have it run on the rider as well and double apply it
        return;
    }

    float default_cost = CalculateRunningCost(a_actor);
    float reduce       = default_cost * a_deltaTime;
    auto weapon        = GetRelevantWeapon(a_actor);

    RE::HandleEntryPoint(ENTRIES::runStamEP, a_actor, reduce, ENTRIES::runStam, weapon);

    a_actor->DamageActorValue(RE::ActorValue::kStamina, reduce);
}

} // namespace EXCO
