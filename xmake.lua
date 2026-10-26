--print("Check if did not add 'fmt' packages in your toplevel xmake.lua")

if is_plat("windows") then
    add_cxflags("/Zc:preprocessor")
end

target("log.cc")
do
    set_kind(is_plat("wasm") and "static" or "shared")

    set_languages("c++20")

    add_headerfiles("./src/**.h")
    add_files("./src/**.cpp")

    add_includedirs("./src/include/", { public = true })


    LogccHasPrint = false

    on_config(function(target)
        if is_host("window") then
            -- Check if the /Zc:preprocessor flag is not added

            if not LogccHasPrint then
                LogccHasPrint = true
                cprint(
                    '${yellow}' ..
                    '[WARNING] This project/package/plugin which name is [' .. target:name() .. ']' ..
                    'Just add "/Zc:preprocessor" to the whole compiler environment\n' ..
                    '\tBecause you maybe use the msvc.' ..
                    'And the __VA_OPT__ is required by this flag...'
                )
            end

            if target:get("kind") == "shared" then
                -- Named after the library that owns it, not after a generic BUILD_SHARED_* shape: this is a
                -- third-party-shaped plugin with no YA_ semantics, and a bare BUILD_SHARED_ family is
                -- exactly the prefix that collides with another library's header. Literal rather than
                -- derived from target:name() so the emitter and the reader in src/base.h are one string
                -- that a grep (and Script/automation/check_macro_prefix.py) can match.
                target:add("defines", "LOG_CC_BUILD_SHARED=1")
            end
        end
    end)
end


target("log.cc.test")
do
    if bDebug then
        print("add test unit:", target_name)
    end
    set_group("test")
    set_kind("binary")
    add_deps("log.cc")
    add_files("./test/**.cpp")
end
