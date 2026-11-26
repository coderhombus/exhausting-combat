# Unit tests

Engine-independent tests (doctest) for `src/stamina/formulas.h`, `src/stamina/rules.h` and
`src/config/`. They do not include CommonLib, so they build with any C++23 compiler.

```
xmake build exhausting-combat-tests
xmake run exhausting-combat-tests                 # all tests
xmake run exhausting-combat-tests -tc="*Attack*"  # filter by test-case name
```

Without xmake (doctest and toml11 headers on the include path):

```
g++ -std=c++23 -Isrc -I<doctest> -I<toml11>/include -DEXHAUSTING_COMBAT_SOURCE_DIR=\"$PWD\" tests/*.cpp src/config/*.cpp -o tests.out && ./tests.out
```

`formulas-tests.cpp` pins the pre-refactor formulas; changing an expected value there is a
gameplay change and needs a deliberate decision.
