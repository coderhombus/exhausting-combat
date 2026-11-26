-- set required xmake version
set_xmakever("2.8.2")

-- includes (need xmake.lua file in the same directory)
includes("lib/commonlibsse")
includes("extern/styyx-utils")

local mod_name = "styyx-stamina-combat"  -- DLL name; also the FUCK plugin name (src/plugin-info.h)

set_project(mod_name)
set_version("1.0.0")
set_license("GPL-3.0")

-- language and warnings
set_languages("c++23")
set_warnings("allextra")

-- build modes
add_rules("mode.debug", "mode.releasedbg")
set_defaultmode("releasedbg")

-- reproducible dependencies: xmake writes xmake-requires.lock on first configure; commit it
set_policy("package.requires_lock", true)

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"}) -- clangd / CLion / VS Code
-- Visual Studio solution on demand: `xmake project -k vsxmake` (no longer regenerated every build)
includes("xmake-rules.lua")

-- Skyrim AE 1.6.1170 only. The exact runtime is enforced at load time in src/plugin-info.h.
set_config("skyrim_ae", true)
set_config("use-hook-utils", true)
set_config("use-fuck", true)

add_requires("toml11")
add_requires("doctest")

-- the SKSE plugin
target(mod_name)
    add_deps("commonlibsse", {public = true})
    add_deps("styyx-util", {public = true})
    add_packages("toml11")

    set_targetdir("build/skse/plugins")

    add_rules("skse-template.plugin", {
        name = mod_name,
        author = "styyx",
        description = "Add stamina cost to certain actions."
    })

    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

    after_build(function(target)
        local plugins = path.join(os.projectdir(), "Distr", "SKSE", "Plugins")
        os.mkdir(plugins)
        os.trycp(target:targetfile(), plugins)
        os.trycp(target:symbolfile(), plugins)
        os.trycp("$(projectdir)/release/**.toml", plugins)
        os.trycp("$(projectdir)/release/**.json", plugins)
    end)

-- engine-independent unit tests: `xmake build exhausting-combat-tests && xmake run exhausting-combat-tests`
-- They compile without CommonLib, so they also build with any host compiler.
target("exhausting-combat-tests")
    set_kind("binary")
    set_default(false)
    add_packages("toml11", "doctest")
    add_includedirs("src")
    add_files("tests/*.cpp", "src/config/*.cpp")
    add_defines("EXHAUSTING_COMBAT_SOURCE_DIR=\"" .. os.projectdir():gsub("\\", "/") .. "\"")
    set_rundir("$(projectdir)")
    if is_plat("windows") then
        add_cxflags("/utf-8")
    end
