## More tweaks
  - movement stamina with heavy and light armor
  - heavy armor and light armor is more stamina-intensive, but scales back with skill level

## block/allow/scale with low stamina:

    Melee attacks;
    Bashes;
    Bow shots (do not confuse with crossbow);
    Blocking;
    Jumping;
    Running.


## Action cost depends on weight of appropriate part. Below is a list with them:

-[v] Melee attack — attacking weapon (or fist); | Values -> Base of 9, Mult of 1.414 -> expose mult in ini settings
-[v] Bash — bashing item (weapon, bow or shield); -> Dual wield bash with Just a block still uses RH
-[v] Bow shot — bow weight; -> already a feature
-[v] Bowstring taut — bow weight; -> already a feature
-[v] Blocking — blocked item (weapon or shield) + attacking item; - should be done by timed blocking skse plugin instead
-[v] Jumping — curiass; | Use only equipped weight -> or just use containerize to manage such items -> no bc loot
-[v] Swimming — curiass; -> already a feature
-[v] Running — total equipped weight. -> already a feature

- toggle in/out of combat ->  unless has magic effect with keyword -> magnitude is a multplier (40 points -> 1.4x)
## Level influence

If you want to get somewhere else, you must run at least twice as fast as that!
As the level increases, the costs of the action increase. To be able to perform same number of actions during you playthrough, you should upgrade stamina every ‘k’ levels (configurable).

## Skill influence

As the skill increases, the costs of the action decrease. You become more and more efficient warrior, you less tired. The skill naturally corresponds to the action. There are no skills affected to swimming, jumping, running.
Notes -> for acrobatic, based on count -> if number of light armor (not counting shield) > heavy armor pieces -> light armor. or just cuirass

## Regen

Regen rate scales with current health and stamina (can be inversely proprotional too - berserk mode) - Styyx Static Regeneration instead

## Equip Weight
-[v] Instead of carry weight / inventory weight

## Moving type
-[v] Standing still or moving -> into Styyx Static Regeneration instead


###
optional integration with acrobatics AV


---

struct ArmorCounts
{
    std::uint32_t heavy = 0;
    std::uint32_t light = 0;
    std::uint32_t clothing = 0;
};

ArmorCounts CountWornArmor(RE::Actor* a_actor)
{
    ArmorCounts counts;

    const auto inventory = a_actor->GetInventory();

    for (const auto& [item, data] : inventory) {
        if (!data.second->IsWorn()) {
            continue;
        }

        auto* armor = item->As<RE::TESObjectARMO>();
        if (!armor) {
            continue;
        }

        if (armor->IsHeavyArmor()) {
            ++counts.heavy;
        } else if (armor->IsLightArmor()) {
            ++counts.light;
        } else {
            // clothing, robes, circlets, etc.
            ++counts.clothing;
        }
    }

    return counts;
}