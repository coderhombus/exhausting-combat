# In-game test checklist

Unit tests cover formulas, rules and configuration. Everything below needs Skyrim AE 1.6.1170.
Set `iLogLevel = 0` in the TOML first: every charge is then logged with its inputs and result,
so each check is "do the action, compare the log line with the expected formula".

## Loading
- [ ] Log shows `config loaded ... (0 warnings, 0 errors)` and `installed 4 virtual and 4 call hooks`.
- [ ] With PEPE installed: `Perk Entry Point Extender detected`. Without: one warning, no errors later.
- [ ] Debug log lists `entry point Mod Power Attack Stamina ...: condition tabs [...]`; note the tab names (confirms what the condition item is matched against).
- [ ] Temporarily list a different runtime in `kSupportedRuntimes`: the plugin logs a critical error and does not load.

## Movement (one cost at a time)
- [ ] Run, sprint, sneak-walk, sneak-run, swim: exactly one `[run]` / `[sprint]` / `[sneak]` / `[swim]` line per frame.
- [ ] Sprinting no longer also produces `[run]` lines (stacking fix).
- [ ] Sneak + sprint (roll): `[sprint]` only.
- [ ] Riding: the player produces no movement lines; the horse produces `[mount-sprint]` / `[run]` / `[swim]` with `(rider Player ...)`, paid by the horse.
- [ ] Riderless horse and NPC-ridden horse: charged with NPC settings (`Horse Sprint Cost##npc`).
- [ ] NPC drains run with a sensible `dt` (~0.016 at 60 FPS); under slow-time effects NPC `dt` stays real-time.

## Actions
- [ ] Melee: weapon weight multiplier and skill factor in the `[attack]` line; power attack applies the power multiplier.
- [ ] Shield bash and torch bash use the shield/torch weight; bash with only a weapon uses the weapon.
- [ ] Unarmed attack: `no weapon, shield or torch identified; no cost added`, vanilla cost applies.
- [ ] Attack diagonally backwards: one charge, the duplicate shows `duplicate event within cooldown`.
- [ ] Jump on foot charged; jump on horseback charged to the horse (or not at all, see hook comment).
- [ ] Bow draw without zoom drains; zooming in stops it; crossbow drains for the whole reload.
- [ ] Bow shot with stamina left fires; the shot that empties stamina is blocked.
- [ ] NPC with 0 max stamina: attacks, casts and bow shots all succeed for free (`exempt`).
- [ ] Fire-and-forget cast with too little stamina is interrupted; concentration spell stops when stamina hits 0.
- [ ] Combat-only on: no lines out of combat; god mode (tgm): no player lines.

## Menu
- [ ] Every setting except the two "not implemented" skill multipliers appears; NPC jump and NPC horse sprint are visible.
- [ ] Sliders for costs and multipliers stop at 20.
- [ ] Changing a value takes effect immediately; closing the menu saves to `exhausting-combat.toml` (only if something changed) and the file keeps its comments.
- [ ] Reset Defaults restores the shipped values; Log Level changes apply immediately.
- [ ] Values survive a game restart.
