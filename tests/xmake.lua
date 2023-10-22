add_includedirs("./")

local bindir = "$(projectdir)/ffead-cpp-7.0-bin"

target("tests")
	set_languages("c++17")
	add_deps("ffead-framework")
	add_options(getOptions())
	set_kind("binary")
	on_load(setIncludes)
	add_files("$(projectdir)/tests/*.cpp")
	set_installdir(bindir)