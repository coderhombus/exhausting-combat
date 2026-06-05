#include "stamina-manager.h"

#include "Utility/util.h"
#include "config.h"
#include "mod-data.h"


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

    const auto cond_item     = ActorUtil::GetWieldingWeapon(a_actor);
    const float default_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_sneak_player.GetValue()
                                                      : CONFIG::stamina_cost_sneak_npc.GetValue();

    float reduce = default_cost * a_deltaTime;

    // PEPE Perk
    RE::HandleEntryPoint(ENTRIES::sneakStamEP, a_actor, reduce, ENTRIES::sneakStam, cond_item);

    a_actor->DamageActorValue(RE::ActorValue::kStamina, reduce);
}

void StaminaCost::ManageJumpStamina(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
        return;
    float jump_cost      = CalculateJumpCost(a_actor);
    const auto cond_item = ActorUtil::GetWieldingWeapon(a_actor);

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

        const auto apply_on   = rider ? rider.get() : a_actor;
        const float base_cost = CalculateHorseSprintCost(apply_on);
        float cost            = base_cost * a_deltaTime;
        const auto cond_item  = ActorUtil::GetWieldingWeapon(apply_on);

        RE::HandleEntryPoint(ENTRIES::mountSprintStamEP, apply_on, cost, ENTRIES::mountSprintStam, cond_item);

        a_actor->DamageActorValue(RE::ActorValue::kStamina, cost);
        // return to not have it run on the rider as well and double apply it
        return;
    }

    const float default_cost = CalculateSprintCost(a_actor);
    float reduce             = default_cost * a_deltaTime;
    const auto weapon        = ActorUtil::GetWieldingWeapon(a_actor);

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

    const auto spell = a_caster->currentSpell;

    // concentration is handled in the update ActorMagicCaster update loop
    if (spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration)
    {
        return true;
    }

    const auto actor = a_caster->GetCasterAsActor();
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
    const auto actor = a_caster->GetCasterAsActor();
    if (!actor)
    {
        return true;
    }

    const auto spell = a_caster->currentSpell;

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

    float attack_cost = CalculateAttackCost(a_actor);
    const auto weapon = ActorUtil::GetWieldingWeapon(a_actor);

    RE::HandleEntryPoint(ENTRIES::attackStamEP, a_actor, attack_cost, ENTRIES::attackStam, weapon);

    // looks like i don't need to manually damage the value, the game applies
    // the cost if returned
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

    const auto equipped_weapon = skyrim_cast<RE::TESObjectWEAP*>(a_actor->GetEquippedObject(false));
    const float base_cost      = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_bow_drain_player.GetValue()
                                                        : CONFIG::stamina_cost_bow_drain_npc.GetValue();
    float stam_cost            = base_cost * a_deltaTime;

    RE::HandleEntryPoint(ENTRIES::bowDrawStamEP, a_actor, stam_cost, ENTRIES::bowDrawStam, equipped_weapon);

    RE::Actor* damaged_actor = Util::GetDamagedActorIfMount(a_actor);
    damaged_actor->DamageActorValue(RE::ActorValue::kStamina, stam_cost);
}

float StaminaCost::GetAttackStaminaCost(RE::Actor* a_actor)
{
    return CalculateAttackCost(a_actor);
}

float StaminaCost::GetAttackStaminaCost_Safe(RE::ActorValueOwner* a_actor, RE::BGSAttackData* a_attack)
{
    const auto actor = skyrim_cast<RE::Actor*>(a_actor);
    return CalculateAttackCost(actor);
}

// vanilla weapon weights
static constexpr float MIN_WEIGHT = 2.0;
static constexpr float MAX_WEIGHT = 33.0;

float StaminaCost::CalculateAttackCost(RE::Actor* a_actor)
{
    const auto weapon    = a_actor->GetAttackingWeapon() ? a_actor->GetAttackingWeapon()->object : nullptr;
    const auto base_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_attack_player.GetValue()
                                                  : CONFIG::stamina_cost_attack_npc.GetValue();

    if (!weapon)
        return base_cost;

    const auto weap = weapon->As<RE::TESObjectWEAP>();
    if (!weap)
        return base_cost;

    if (weap->IsHandToHandMelee())
        return base_cost;
    // vanilla min: 2
    // vanilla max: 33
    // vanilla average ~17
    // formula: result = (weight - min) / (max - min)

    const auto weight   = std::max(weapon->GetWeight(), 1.f);
    const auto modifier = 1.f + std::clamp((weight - MIN_WEIGHT) / (MAX_WEIGHT - MIN_WEIGHT), 0.f, 1.f) * 0.3f;
    // up to 30% more stamina cost for heavy weapons

    return base_cost * modifier;
}

static constexpr float CAST_MODIFIER     = 0.70f;
static constexpr float MAX_CAST_COST_ADD = 25.f;
float StaminaCost::CalculateCastCost(const RE::ActorMagicCaster* a_caster, const RE::MagicItem* a_spell)
{
    // is probably never null, but better safe than sorry
    if (!a_caster || !a_spell)
        return 0.f;
    const auto actor = a_caster->GetCasterAsActor();
    if (!ActorValidAndNotGod(actor))
        return 0.f;

    const auto base_cost = actor->IsPlayerRef() ? CONFIG::stamina_cost_casting_player.GetValue()
                                                : CONFIG::stamina_cost_casting_npc.GetValue();
    // prevent 0 magicka cost from costing 0 stamina
    const auto mag_cost = std::max(a_spell->CalculateMagickaCost(actor), 1.0f);

    // magicka cost can only add up to 25 stamina cost to base
    const auto cast_cost_addition = std::min(std::sqrt(mag_cost) * CAST_MODIFIER, MAX_CAST_COST_ADD);
    const auto cost               = base_cost + cast_cost_addition;

    return cost;
}

float StaminaCost::CalculateJumpCost(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
        return 0.f;
    const float base_cost =
        a_actor->IsPlayerRef() ? CONFIG::stamina_cost_jump_player.GetValue() : CONFIG::stamina_cost_jump_npc.GetValue();

    return CalculateWeightModi(base_cost, a_actor);
}

float StaminaCost::CalculateSprintCost(RE::Actor* a_actor)
{
    if (!ActorValidAndNotGod(a_actor))
        return 0.f;
    const float base_sprint_cost = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_sprint_player.GetValue()
                                                          : CONFIG::stamina_cost_sprint_npc.GetValue();

    return CalculateWeightModi(base_sprint_cost, a_actor);
}

float StaminaCost::CalculateWeightModi(float a_baseCost, const RE::Actor* a_actor)
{
    // full weight adds 50% of the base
    const float carry_weight = std::max(a_actor->GetActorValue(RE::ActorValue::kCarryWeight), 1.0f);
    const float modi         = a_actor->GetActorValue(RE::ActorValue::kInventoryWeight) / carry_weight;
    return a_baseCost + modi * (a_baseCost / 2);
}

float StaminaCost::CalculateHorseSprintCost(RE::Actor* a_actor)
{
    // horse stamina is a flat cost
    if (!ActorValidAndNotGod(a_actor))
        return 0.f;

    const float ret = a_actor->IsPlayerRef() ? CONFIG::stamina_cost_sprint_horse_player.GetValue()
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
    const float base_swim_cost =
        a_actor->IsPlayerRef() ? CONFIG::stamina_cost_swim_player.GetValue() : CONFIG::stamina_cost_swim_npc.GetValue();

    return CalculateWeightModi(base_swim_cost, a_actor);
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

        const auto apply_on   = rider ? rider.get() : a_actor;
        const float base_cost = CalculateSwimCost(apply_on);
        float cost            = base_cost * a_deltaTime;
        const auto cond_item  = ActorUtil::GetWieldingWeapon(rider.get());

        RE::HandleEntryPoint(ENTRIES::swimStamEP, apply_on, cost, ENTRIES::swimStam, cond_item);

        a_actor->DamageActorValue(RE::ActorValue::kStamina, cost);
        // return to not have it run on the rider as well and double apply it
        return;
    }

    const float default_cost = CalculateSwimCost(a_actor);
    float reduce             = default_cost * a_deltaTime;
    const auto weapon        = ActorUtil::GetWieldingWeapon(a_actor);

    RE::HandleEntryPoint(ENTRIES::swimStamEP, a_actor, reduce, ENTRIES::swimStam, weapon);

    a_actor->DamageActorValue(RE::ActorValue::kStamina, reduce);
}
} // namespace EXCO
