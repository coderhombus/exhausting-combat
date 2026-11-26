// Unit-test entry point. Run with `xmake run exhausting-combat-tests` (see tests/README.md).
//
// Extra option, handled before doctest:
//   --write-default-config <path>   writes the documented default TOML (used to regenerate
//                                   release/exhausting-combat.toml after changing defaults)
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <cstdio>
#include <string>
#include <string_view>

#include "config/config-store.h"

int main(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string_view(argv[i]) == "--write-default-config") {
            ExhaustingCombat::Config::Settings defaults;
            std::string                        error;
            if (!ExhaustingCombat::Config::SaveToFile(argv[i + 1], defaults, error)) {
                std::fprintf(stderr, "%s\n", error.c_str());
                return 1;
            }
            std::printf("wrote %s\n", argv[i + 1]);
            return 0;
        }
    }

    doctest::Context context(argc, argv);
    return context.run();
}
