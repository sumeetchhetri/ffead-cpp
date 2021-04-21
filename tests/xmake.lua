add_includedirs("./")

local bindir = "$(projectdir)/ffead-cpp-6.0-bin"

target("tests")
	set_languages("c++17")
	add_deps("ffead-framework")
	add_options("ALL")
	set_kind("binary")
	add_options("ALL")
	on_load(setIncludes)
	add_files("$(projectdir)/tests/*.cpp")
	set_installdir(bindir)