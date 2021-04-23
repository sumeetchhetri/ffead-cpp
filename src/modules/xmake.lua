
local bindir = "$(projectdir)/ffead-cpp-6.0-bin"

target("ffead-modules")
	set_languages("c++17")
	set_kind("shared")
	add_options(getOptions())
	on_load(setIncludes)
	add_files("$(projectdir)/src/modules/common/*.cpp")
	add_files("$(projectdir)/src/modules/client-util/*.cpp")
	add_files("$(projectdir)/src/modules/server-util/*.cpp")
	add_files("$(projectdir)/src/modules/http/*.cpp")
	add_files("$(projectdir)/src/modules/http/http11/*.cpp")
	add_files("$(projectdir)/src/modules/http/http20/*.cpp")
	add_files("$(projectdir)/src/modules/ioc/*.cpp")
	add_files("$(projectdir)/src/modules/reflection/*.cpp")
	add_files("$(projectdir)/src/modules/threads/*.cpp")
	if has_config("CURL") then
		add_links("curl")
	end
	if has_config("SSL") then
		add_files("$(projectdir)/src/modules/ssl/*.cpp")
		add_files("$(projectdir)/src/modules/client-util/ssl/*.cpp")
		add_links("ssl", "crypto")
	end
	add_files("$(projectdir)/src/modules/cache/*.cpp")
	add_files("$(projectdir)/src/modules/cache/memory/*.cpp")
	if has_config("MOD_REDIS") then
		add_files("$(projectdir)/src/modules/cache/redis/*.cpp")
		add_links("hiredis")
	end
    if has_config("MOD_MEMCACHED") then
		add_files("$(projectdir)/src/modules/cache/memcached/*.cpp")
		add_links("memcached","memcachedutil")
	end
	add_files("$(projectdir)/src/modules/sdorm/*.cpp")
    if has_config("MOD_SDORM_SQL") then
		add_files("$(projectdir)/src/modules/sdorm/sql/*.cpp")
		add_files("$(projectdir)/src/modules/sdorm/sql/libpq/*.cpp")
		add_links("odbc","pq","uuid")
	end
    if has_config("MOD_SDORM_MONGO") then
    	add_files("$(projectdir)/src/modules/sdorm/mongo/*.cpp")
    	add_files("$(projectdir)/src/modules/sdorm/mongo/raw/*.cpp")
    	add_links("bson-1.0","mongoc-1.0","uuid")
	end
	add_files("$(projectdir)/src/modules/serialization/*.cpp")
	add_files("$(projectdir)/src/modules/serialization/xml/*.cpp")
	add_files("$(projectdir)/src/modules/serialization/json/*.cpp")
    if has_config("MOD_SER_BIN") then
		add_files("$(projectdir)/src/modules/serialization/binary/*.cpp")
	end
    if has_config("MOD_JOBS") then
		add_files("$(projectdir)/src/modules/jobs/*.cpp")
	end
	set_installdir(bindir)
	before_install(function (target)
        os.run("rm -rf %s", bindir)
    end)

