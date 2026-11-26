# Exhausting Combat

[nexus link](https://www.nexusmods.com/skyrimspecialedition/mods/181654)

SKSE plugin that adds stamina costs to attacks, movement, jumping, archery and spellcasting.
**Supports Skyrim Anniversary Edition 1.6.1170 only**; the plugin refuses to load on any other
runtime (see `src/plugin-info.h` to change that while testing).

## Build

Clone with `--recursive`, then:

```
xmake            # plugin -> Distr/SKSE/Plugins
xmake build exhausting-combat-tests && xmake run exhausting-combat-tests   # unit tests
```

## Layout

| Path | What it holds | Engine-dependent |
| --- | --- | --- |
| `src/stamina/formulas.h`, `src/stamina/rules.h` | cost math and decision rules | no (unit-tested) |
| `src/config/` | every setting (key, default, range, label, tooltip), TOML load/validate/save | no (unit-tested) |
| `src/stamina/actor-context.*` | facts about an actor: profile, gates, payer, movement, attack item | yes |
| `src/stamina/costs.*` | one function per charged action | yes |
| `src/integrations/perk-entry-points.*` | adapter for Perk Entry Point Extender (PEPE) | yes |
| `src/hooks.*`, `src/menu/`, `src/main.cpp` | engine hooks, FUCK menu, plugin entry | yes |
| `src/API/` | third-party API headers: **do not modify** | |

## Configuration

`Data/SKSE/Plugins/exhausting-combat.toml` is the only config file. It is read at startup and
rewritten when settings change in the in-game (FUCK) menu. Every setting in it is documented
with its purpose, default and valid range. The shipped file is generated from
`src/config/settings.cpp`; after changing a default run
`xmake run exhausting-combat-tests --write-default-config release/exhausting-combat.toml`
(a unit test fails while the two differ).

Set `iLogLevel = 1` (debug) to log every charge with its inputs, or `0` (trace) to also log
per-frame drains. The log is `Documents/My Games/Skyrim Special Edition/SKSE/styyx-stamina-combat.log`.

In-game checks that unit tests cannot cover are listed in `docs/in-game-testing.md`.
