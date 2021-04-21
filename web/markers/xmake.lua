add_includedirs("include/")

local bindir = "$(projectdir)/ffead-cpp-6.0-bin"

target("markers")
	set_languages("c++17")
	add_deps("ffead-framework")
	add_options("ALL")
	set_kind("shared")
	add_options("ALL")
	on_load(setIncludes)
	add_files("src/*.cpp")
	set_installdir(bindir)
