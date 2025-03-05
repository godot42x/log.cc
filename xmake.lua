--print("Check if did not add 'fmt' packages in your toplevel xmake.lua")

target("log.cc")
do
    set_kind("shared")

    if is_os("linux") then
        add_cxflags("-fPIC")
    elseif is_os("windows") then
        add_defines("BUILD_SHARED_M_LOG")
    end

    set_languages("c++20")

    add_headerfiles("*.h")
    add_files("log.cpp")
    add_includedirs(".")

    on_config(function(target)
        if is_host("window") then
            -- Check if the /Zc:preprocessor flag is not added
            if not target:has_cxxflags("/Zc:preprocessor") then
                print(
                    '\27[31m' ..
                    '[NOTICE] Add these to top-level "xmake.lua" if you are using MSVC:' ..
                    '\n\tif is_plat("windows") then ' ..
                    '\n\t    add_cxflags("/Zc:preprocessor")' ..
                    '\n\tend' ..
                    '\27[Om'
                )
            end
        end
    end)
end
