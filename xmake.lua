-- set xmake version
set_xmakever("2.9.4")

-- include local folders
includes("lib/commonlibob64")

-- set project
set_project("ue4ss_Loader")
set_version("2.0.0")
set_license("GPL-3.0")

-- set defaults
set_arch("x64")
set_languages("c++23")
set_optimize("faster")
set_warnings("allextra", "error")
set_defaultmode("releasedbg")

-- enable lto
set_policy("build.optimization.lto", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- setup targets
target("ue4ss_Loader")
    -- bind local dependencies
    add_deps("commonlibob64")

    -- add commonlibob64 plugin
    add_rules("commonlibob64.plugin", {
        name = "ue4ss_Loader",
        author = "xEpicBradx"
    })

    -- add source files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/PCH.h")

    -- add extra files
    add_extrafiles(".clang-format")
