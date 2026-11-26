#include "stamina/actor-context.h"

#include <styyx-utils.h>

#include "logging.h"
#include "stamina/formulas.h"

namespace ExhaustingCombat::Stamina
{
namespace
{
constexpr bool kLeftHand  = true;
constexpr bool kRightHand = false;

/// Skill multiplier setting for an actor value, or null if that value has none.
/// SpeedMult and InventoryWeight have settings but are not implemented, so they map to null.
const Config::FloatParameter* SkillScalingFor(RE::ActorValue a_value)
{
    const auto& skills = Config::Get().skills;
    switch (a_value) {
        case RE::ActorValue::kOneHanded:
            return &skills.oneHanded;
        case RE::ActorValue::kTwoHanded:
            return &skills.twoHanded;
        case RE::ActorValue::kArchery:
            return &skills.archery;
        case RE::ActorValue::kBlock:
            return &skills.block;
        case RE::ActorValue::kHeavyArmor:
            return &skills.heavyArmor;
        case RE::ActorValue::kLightArmor:
            return &skills.lightArmor;
        case RE::ActorValue::kSneak:
            return &skills.sneak;
        case RE::ActorValue::kAlteration:
            return &skills.alteration;
        case RE::ActorValue::kIllusion:
            return &skills.illusion;
        case RE::ActorValue::kDestruction:
            return &skills.destruction;
        case RE::ActorValue::kConjuration:
            return &skills.conjuration;
        case RE::ActorValue::kRestoration:
            return &skills.restoration;
        default:
            return nullptr;
    }
}
}  // namespace

const Config::ActorProfile& ProfileFor(const RE::Actor& a_actor)
{
    return Config::Get().Profile(a_actor.IsPlayerRef());
}

bool ShouldChargeStamina(const RE::Actor* a_actor)
{
    if (!a_actor) {
        return false;
    }
    const ChargeGateFacts facts{
        .isPlayerInGodMode = a_actor->IsPlayerRef() && RE::PlayerCharacter::IsGodMode(),
        .combatOnly        = ProfileFor(*a_actor).combatOnly.GetValue(),
        .inCombat          = a_actor->IsInCombat(),
    };
    return Stamina::ShouldChargeStamina(facts);
}

RE::Actor& PayerFor(RE::Actor& a_actor)
{
    if (!a_actor.IsPlayerRef()) {
        return a_actor;
    }
    RE::ActorPtr mount;
    if (a_actor.GetMount(mount) && mount) {
        // Forms (and so the mount) outlive this call; the handle only guards the lookup.
        return *mount;
    }
    return a_actor;
}

MountRole GetMountRole(const RE::Actor& a_actor)
{
    if (a_actor.IsAMount()) {
        return MountRole::Mount;
    }
    if (a_actor.IsOnMount()) {
        return MountRole::Rider;
    }
    return MountRole::None;
}

RE::ActorPtr GetRider(RE::Actor& a_mount)
{
    RE::ActorPtr rider;
    if (!a_mount.GetMountedBy(rider)) {
        return {};
    }
    return rider;
}

MovementFlags ReadMovementFlags(const RE::Actor& a_actor)
{
    return MovementFlags{
        .moving    = StyyxUtil::ActorUtil::IsMoving(&a_actor),
        .sneaking  = a_actor.IsSneaking(),
        .running   = a_actor.IsRunning(),
        .sprinting = a_actor.IsSprinting(),
        .swimming  = a_actor.IsSwimming(),
    };
}

float GetStamina(RE::Actor& a_actor)
{
    return a_actor.GetActorValue(RE::ActorValue::kStamina);
}

float EncumbranceScaledCostFor(RE::Actor& a_actor, float a_baseCost)
{
    const bool  equippedOnly = ProfileFor(a_actor).equippedWeightOnly.GetValue();
    // GetEquippedWeight() is not const in CommonLib; taking a non-const actor avoids a const_cast.
    const float carried      = equippedOnly ? a_actor.GetEquippedWeight() : a_actor.GetActorValue(RE::ActorValue::kInventoryWeight);
    const float carryWeight  = a_actor.GetActorValue(RE::ActorValue::kCarryWeight);
    return EncumbranceScaledCost(a_baseCost, carried, carryWeight);
}

float SkillFactorFor(RE::Actor& a_actor, RE::ActorValue a_skill)
{
    const auto* scaling = SkillScalingFor(a_skill);
    if (!scaling) {
        return 1.0f;
    }
    return SkillCostFactor(a_actor.GetActorValue(a_skill), scaling->GetValue());
}

RE::ActorValue AssociatedSkill(const RE::TESObjectWEAP& a_weapon)
{
    if (a_weapon.IsOneHandedAxe() || a_weapon.IsOneHandedDagger() || a_weapon.IsOneHandedMace() || a_weapon.IsOneHandedSword() ||
        a_weapon.IsHandToHandMelee() || a_weapon.IsStaff()) {
        return RE::ActorValue::kOneHanded;
    }
    if (a_weapon.IsTwoHandedAxe() || a_weapon.IsTwoHandedSword()) {
        return RE::ActorValue::kTwoHanded;
    }
    if (a_weapon.IsBow() || a_weapon.IsCrossbow()) {
        return RE::ActorValue::kArchery;
    }
    logs::debug("weapon {} has no associated skill; no skill scaling applied", Describe(&a_weapon));
    return RE::ActorValue::kNone;
}

RE::TESObjectWEAP* GetRelevantWeapon(RE::Actor& a_actor)
{
    if (a_actor.IsHumanoid() && a_actor.IsAttacking()) {
        return StyyxUtil::ActorUtil::GetWieldingWeapon(&a_actor);
    }

    auto* left  = a_actor.GetEquippedObject(kLeftHand);
    auto* right = a_actor.GetEquippedObject(kRightHand);

    auto* leftWeapon  = left ? left->As<RE::TESObjectWEAP>() : nullptr;
    auto* rightWeapon = right ? right->As<RE::TESObjectWEAP>() : nullptr;

    // Dual wielding (or both hands holding something unusual): attribute to the right hand.
    // Staff in one hand / unarmed in the other: the staff. Nothing: null (unarmed).
    return rightWeapon ? rightWeapon : leftWeapon;
}

AttackItem ResolveAttackItem(RE::Actor& a_actor, bool a_isBash)
{
    auto*       shield    = a_actor.GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
    const bool  hasShield = shield && shield->IsShield();
    auto*       leftItem  = a_actor.GetEquippedObject(kLeftHand);
    auto*       torch     = leftItem ? leftItem->As<RE::TESObjectLIGH>() : nullptr;
    auto*       weapon    = GetRelevantWeapon(a_actor);

    AttackItem item;
    item.kind        = ChooseAttackItem(a_isBash, hasShield, torch != nullptr, weapon != nullptr);
    item.skillFactor = a_isBash ? SkillFactorFor(a_actor, RE::ActorValue::kBlock) : 1.0f;

    switch (item.kind) {
        case AttackItemKind::Shield:
            if (shield->IsHeavyArmor()) {
                item.skillFactor *= SkillFactorFor(a_actor, RE::ActorValue::kHeavyArmor);
            } else if (shield->IsLightArmor()) {
                item.skillFactor *= SkillFactorFor(a_actor, RE::ActorValue::kLightArmor);
            }
            item.form   = shield;
            item.weight = shield->GetWeight();
            break;
        case AttackItemKind::Torch:
            item.form   = torch;
            item.weight = torch->GetWeight();
            break;
        case AttackItemKind::Weapon:
            item.skillFactor *= SkillFactorFor(a_actor, AssociatedSkill(*weapon));
            item.form   = weapon;
            item.weight = weapon->GetWeight();
            break;
        case AttackItemKind::None:
            // Unarmed: no item, no weight, cost 0 by design (see ChooseAttackItem).
            break;
    }
    return item;
}

bool IsDrawingBowWithoutZoom(const RE::Actor& a_actor)
{
    if (a_actor.IsPlayerRef()) {
        const auto* camera = RE::PlayerCamera::GetSingleton();
        if (camera && (camera->bowZoomedIn || camera->zoomInput > 0.0f)) {
            return false;  // zoomed in: the vanilla bow-zoom drain applies instead
        }
    }

    switch (a_actor.GetAttackState()) {
        case RE::ATTACK_STATE_ENUM::kBowDrawn:
        case RE::ATTACK_STATE_ENUM::kBowAttached:
            break;
        default:
            return false;
    }

    const auto* object = a_actor.GetEquippedObject(kRightHand);
    const auto* weapon = object ? object->As<RE::TESObjectWEAP>() : nullptr;
    return weapon && (weapon->GetWeaponType() == RE::WEAPON_TYPE::kBow || weapon->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow);
}

void InterruptAttack(RE::Actor& a_actor)
{
    a_actor.NotifyAnimationGraph("attackStop");
    a_actor.NotifyAnimationGraph("blockStop");
}
}  // namespace ExhaustingCombat::Stamina
