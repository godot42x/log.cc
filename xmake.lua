
add_requires("fmt")
add_packages("fmt")

-- Add this to top_level xmake if msvc
if is_os("windows") then
	add_cxxflags("/Zc:preprocessor")
end

add_includedirs()

target("m_log")
	set_kind("shared")

	if is_os("linux") then
		add_cxflags("-fPIC")
	elseif is_os("windows") then
		add_defines("BUILD_SHARED_M_LOG")
	end

	--add_cxxflags("-std=c++20")

	add_includedirs(".")


	add_headerfiles("*.h")
	add_files("log.cpp")

	add_packages("fmt")
