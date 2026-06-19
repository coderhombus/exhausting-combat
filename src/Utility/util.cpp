#include "util.h"

// needed cause apparently, while on a horse your stamina doesn't exist
RE::Actor* EXCO::Util::GetDamagedActorIfMount(RE::Actor* a_actor)
{
    if (!a_actor)
        return nullptr;

    if (!a_actor->IsPlayerRef())
        return a_actor;

    RE::ActorPtr honse{};
    GetMount(a_actor, &honse);
    if (honse)
        return honse.get();
    return a_actor;
}

bool EXCO::Util::IsPlayerAndZoom(const RE::Actor* a_actor)
{
    const auto player_camera = RE::PlayerCamera::GetSingleton();
    if (const auto player = RE::PlayerCharacter::GetSingleton(); a_actor != player)
        return false;
    return player_camera->bowZoomedIn || player_camera->zoomInput > 0;
}

bool EXCO::Util::IsBowDrawNoZoom(const RE::Actor* a_actor)
{
    if (!a_actor)
        return false;

    if (IsPlayerAndZoom(a_actor))
        return false;

    const auto attack_state    = a_actor->GetAttackState();
    const auto equipped_weapon = skyrim_cast<RE::TESObjectWEAP*>(a_actor->GetEquippedObject(false));
    if (!equipped_weapon)
        return false;
    switch (attack_state)
    {
        case RE::ATTACK_STATE_ENUM::kBowDrawn:
        case RE::ATTACK_STATE_ENUM::kBowAttached:
            if (equipped_weapon->GetWeaponType() == RE::WEAPON_TYPE::kBow ||
                equipped_weapon->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow)
                return true;
        default:
            break;
    }
    return false;
}

void EXCO::Util::InterruptAttack(RE::Actor* a_actor)
{
    a_actor->NotifyAnimationGraph("attackStop");
    a_actor->NotifyAnimationGraph("blockStop");
}

void EXCO::Util::LookupStaminaCostGlobal()
{
    stam_cost_pl_glob = RE::TESForm::LookupByEditorID<RE::TESGlobal>("SST_StaminaCostPlayer");
}
