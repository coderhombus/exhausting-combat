#pragma once

// Mounted melee for the player. The attack-cost hook may not fire for attacks made on
// horseback, so while the player is mounted we also listen to the player's melee swing
// animation events ("weaponSwing" / "weaponLeftSwing") and charge the horse from there.
// Costs::ChargeMountedSwing merges both sources, so an attack is charged once.

#include <RE/Skyrim.h>

namespace ExhaustingCombat::Stamina::MountedCombat
{
/// Call on every player update. Starts listening when the player mounts and stops on dismount
/// (re-registering covers animation graphs rebuilt by loading a save).
void UpdatePlayer(RE::PlayerCharacter& a_player);
}  // namespace ExhaustingCombat::Stamina::MountedCombat
