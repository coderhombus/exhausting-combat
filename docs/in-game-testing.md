# In-game test checklist

Unit tests cover formulas, rules and configuration. Everything below needs Skyrim AE 1.6.1170.
Set `iLogLevel = 1` (or `0` for per-frame drains) in the TOML first: every event is then logged
with its inputs and result, so each check is "do the action, compare the log line with the
expected formula". A missing line now means the event was not detected; skipped events log
`skipped: <reason>`.

## Loading
- [ ] Log shows `config loaded ... (0 warnings, 0 errors)` and `installed 4 virtual and 4 call hooks`.
- [ ] With PEPE installed: `Perk Entry Point Extender detected`. Without: one warning, no errors later.
- [ ] Debug log lists `entry point Mod Power Attack Stamina ...: condition tabs [...]`; note the tab names (confirms what the condition item is matched against).
- [ ] Temporarily remove 1.6.1170 from `kSupportedRuntimes`: the plugin logs a critical error listing the supported runtimes and does not load.

## Logging
- [ ] Dagger, sword, war axe, two-hander, bow bash and shield bash attacks each produce an `[attack]` line immediately (no waiting for other output).
- [ ] No FormIDs in lines; every charge line shows `payer:`.
- [ ] `powerMult=1.000` on normal attacks, the configured value on power attacks.
- [ ] If an attack still produces no line at all, the attack-cost event did not fire for it: note weapon, attack type and whether sprinting/sneaking/mounted.

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
- [ ] Jump on foot charged.
- [ ] Jump on horseback: exactly one `[jump]` line per jump, `actor` = player, `payer:` = horse, using player settings. A second report shows `already charged for this jump`.
- [ ] Mounted melee: one `[attack-mounted]` `paid (...)` line per swing-attack, payer = horse, horse stamina drops; the source in brackets says whether the attack-cost event or a swing event detected it. Note which one appears; if only swing events do, the attack-cost hook does not fire on horseback.
- [ ] Mounted power attack with several swings: charged once (`already charged for this attack` for later swings).
- [ ] Two quick consecutive mounted attacks: both charged (if the second shows `already charged`, the 600 ms merge window is too long).
- [ ] Bow draw without zoom drains; zooming in stops it; crossbow drains for the whole reload.
- [ ] Bow shot with stamina left fires; the shot that empties stamina is blocked.
- [ ] NPC with 0 max stamina: attacks, casts and bow shots all succeed for free (`exempt`).
- [ ] Fire-and-forget cast with too little stamina is interrupted; concentration spell stops when stamina hits 0.
- [ ] Concentration Mult: 0.5 halves the per-second `[concentration]` cost; fire-and-forget casts unchanged.
- [ ] Combat-only on: no lines out of combat; god mode (tgm): no player lines.

## Menu
- [ ] Every setting except the two "not implemented" skill multipliers appears; NPC jump and NPC horse sprint are visible.
- [ ] Sliders for costs and multipliers stop at 20; Concentration Mult present for player and NPCs.
- [ ] Tooltips show line breaks and no stray characters where the old text had percent signs.
- [ ] Changing a value takes effect immediately; closing the menu saves to `exhausting-combat.toml` (only if something changed) and the file keeps its comments.
- [ ] Reset Defaults restores the shipped values; Log Level changes apply immediately.
- [ ] Values survive a game restart.
