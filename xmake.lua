set_project("ffead-cpp-5.0")

set_xmakever("2.3.2")

set_version("5.4", {build = "%Y%m%d%H%M"})

if is_mode("debug") then
	set_warnings("all", "error")
	set_configvar("DEBUG_MODE", 1)
elseif is_mode("release") then
	set_warnings("none")
end

add_rules("mode.release", "mode.debug")

add_cxxflags("-fPIC")
if is_plat("windows") then
    add_cxxflags("/EHsc")
end

option("SRV_EMB")
	set_default(true)
	set_showmenu(true)
	set_description("Embedded Http Server Engine")
option_end()

option("MOD_MEMORY")
	set_default(true)
	set_showmenu(true)
	set_description("Enable Memory Cache module")
	add_defines("INC_MEMORYCACHE")
option_end()

option("MOD_MEMCACHED")
	set_default(false)
	set_showmenu(true)
	set_description("Enable Memcached Cache module")
	after_check(function (option)
		if option:enabled() then
			import("lib.detect.find_package")
			local l = find_package("memcached")
			if not l then
				raise('memcached library not found')
			end
	        option:add(l)
	        l = find_package("memcachedutil")
			if not l then
				raise('memcachedutil library not found')
			end
	        option:add(l)
	    end
    end)
option_end()

option("MOD_REDIS")
	set_default(false)
	set_showmenu(true)
	set_description("Enable Redis Cache module")
	after_check(function (option)
		if option:enabled() then
			import("lib.detect.find_package")
			local l = find_package("hiredis")
			if not l then
				raise('hiredis library not found')
			end
	        option:add(l)
	    end
    end)
option_end()

option("MOD_SER_BIN")
	set_default(false)
	set_showmenu(true)
	set_description("Enable Binary Serialization module")
option_end()

option("MOD_JOBS")
	set_default(true)
	set_showmenu(true)
	set_description("Enable Jobs module")
option_end()

option("LIBPQ_BATCH")
	set_default(true)
option_end()

option("LIBPQ")
	add_deps("LIBPQ_BATCH")
	set_default(true)
	set_description("Check whether libpq is present")
	after_check(function (option)
		if option:dep("LIBPQ_BATCH"):enabled() then
			import("lib.detect.find_package")
			local l = find_package("pq")
			if l then
				import("lib.detect.has_cfuncs")
				local ok = has_cfuncs("PQenterBatchMode", {includes = "postgresql/libpq-fe.h"})
				if not ok then
					ok = has_cfuncs("PQenterBatchMode", {includes = "pgsql/libpq-fe.h"})
					if not ok then
						option:dep("LIBPQ_BATCH"):enable(false)
					else
						set_configvar("HAVE_PQHDR", 1)
					end
				else
					set_configvar("HAVE_PQHDR", 1)
				end
			end
		end
    end)
option_end()

option("CURL")
	set_default(true)
	set_description("Check whether curl is present")
	after_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("curl")
		if l then
			option:add(l)
		else
			option:enable(false)
		end
    end)
option_end()

option("ZLIB")
	set_default(true)
	add_defines("HAVE_ZLIB")
	set_description("Check whether zlib is present")
	after_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("z")
		if not l then
			raise('z library not found')
		end
		option:add(l)
    end)
option_end()

option("SSL")
	set_default(true)
	set_description("Check whether ssl is present")
	after_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("ssl")
		if not l then
			option:enable(false)
		else
			option:add(l)
		end
    end)
option_end()

option("SSL_CRYPTO")
	set_default(true)
	set_description("Check whether crypto is present")
	after_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("crypto")
		if not l then
			option:enable(false)
		else
			option:add(l)
		end
    end)
option_end()

option("REGEX")
	set_default(true)
	set_description("Check whether regex is present")
	on_check(function (option)
		import("lib.detect.has_cfuncs")
		local ok = has_cfuncs("regcomp", {includes = "regex.h"})
		if not ok then
			option:enable(false)
		end
    end)
option_end()

option("ONIG_REGEX")
	add_deps("REGEX")
	set_default(true)
	set_description("Check whether onig-regex is present")
	after_check(function (option)
		if not option:dep("REGEX"):enabled() then
			import("lib.detect.has_cfuncs")
			local ok = has_cfuncs("regcomp", {includes = "onigposix.h"})
			if not ok then
				raise('onig headers not found')
			else
				import("lib.detect.find_package")
				local l = find_package("onig")
				if not l then
					raise('onig library not found')
				end
		        option:add(l)
			end
		else
			option:enable(false)
        end
    end)
option_end()

option("UUID")
	set_default(true)
	set_description("Check whether uuid is present")
	on_check(function (option)
		import("lib.detect.has_cfuncs")
		local ok = has_cfuncs("uuid_generate", {includes = "uuid/uuid.h"})
		if not ok then
			option:enable(false)
		else
			import("lib.detect.find_package")
			local l = find_package("uuid")
			if not l then
				raise('uuid library not found')
			end
	        option:add(l)
		end
    end)
option_end()

option("OSSP_UUID")
	add_deps("UUID")
	set_default(true)
	set_description("Check whether ossd-uuid is present")
	after_check(function (option)
		if not option:dep("UUID"):enabled() then
            import("lib.detect.find_package")
			local l = find_package("ossp-uuid")
			if not l then
				option:enable(false)
			end
		else
			option:enable(false)
        end
    end)
option_end()

option("MOD_SDORM_SQL")
	add_deps("OSSP_UUID")
	set_default(true)
	set_showmenu(true)
	set_description("Enable SQL Sdorm module")
	after_check(function (option)
		if option:enabled() then
			if not option:dep("UUID"):enabled() then
				if not option:dep("OSSP_UUID"):enabled() then
					raise('uuid library not found')
				end
			end
			import("lib.detect.find_package")
			local l = find_package("odbc")
			if not l then
				raise('odbc library not found')
			end
	        option:add(l)
	    end
    end)
option_end()

local mongo_inc_path = ""
option("MOD_SDORM_MONGO")
	add_deps("OSSP_UUID")
	set_default(false)
	set_showmenu(true)
	set_description("Enable MongoDB Sdorm module")
	after_check(function (option)
		if option:enabled() then
			if not option:dep("UUID"):enabled() then
				if not option:dep("OSSP_UUID"):enabled() then
					raise('uuid library not found')
				end
			end
			--[[ok = has_cincludes("bson.h")
			if not ok then
				raise('bson headers not found')
			end--]]
			import("lib.detect.find_package")
			local l = find_package("mongoc-1.0")
			if not l then
				raise('mongoc-1.0 library not found')
			end
	        option:add(l)
	        l = find_package("bson-1.0")
			if not l then
				raise('bson-1.0 library not found')
			end
	        option:add(l)
	    end
    end)
option_end()

option("LIBCUCKOO")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cxxincludes")
		local ok = has_cxxincludes({"libcuckoo/cuckoohash_map.hh"}, {target = target, configs = {languages = "cxx11"}})
		if not ok then
			raise('libcuckoo includes not found')
		end
	end)
option_end()

option("EXECINFO")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cincludes")
		ok = has_cincludes({"execinfo.h"})
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("SELECT")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cincludes")
		ok = has_cincludes("sys/select.h")
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("POLL")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cincludes")
		ok = has_cincludes("sys/poll.h")
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("DEVPOLL")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cincludes")
		ok = has_cincludes("sys/devpoll.h")
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("EPOLL")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		if is_plat("linux") then
			import("lib.detect.has_cincludes")
			ok = has_cincludes("sys/epoll.h")
			if not ok then
				option:enable(false)
			end
		else
			option:enable(false)
		end
	end)
option_end()

option("KQUEUE")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		if is_plat("macosx") then
			import("lib.detect.has_cincludes")
			ok = has_cincludes("sys/event.h")
			if not ok then
				option:enable(false)
			end
		else
			option:enable(false)
		end
	end)
option_end()

option("EVPORT")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		if is_plat("solaris") then
			import("lib.detect.has_cincludes")
			ok = has_cincludes("port.h")
			if not ok then
				option:enable(false)
			end
		else
			option:enable(false)
		end
	end)
option_end()

option("IOURING")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		if is_plat("linux") then
			import("lib.detect.has_cincludes")
			ok = has_cincludes("liburing.h")
			if not ok then
				option:enable(false)
			end
		else
			option:enable(false)
		end
	end)
option_end()

option("ACCEPT4")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cfuncs")
		local ok = has_cfuncs("accept4", {includes = "sys/socket.h"})
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("TCP_QUICKACK")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.check_cxsnippets")
		local ok = check_cxsnippets({"#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_QUICKACK; }"})
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("TCP_DEFER_ACCEPT")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.check_cxsnippets")
		local ok = check_cxsnippets({"#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_DEFER_ACCEPT; }"})
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("TCP_FASTOPEN")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.check_cxsnippets")
		local ok = check_cxsnippets({"#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_FASTOPEN; }"})
		if not ok then
			option:enable(false)
		end
	end)
option_end()

option("NETWORK_LIBS")
	set_default(true)
	set_showmenu(false)
	add_deps("SELECT", "POLL", "DEVPOLL", "IOURING", "EPOLL", "KQUEUE", "EVPORT", "ACCEPT4", "TCP_QUICKACK", "TCP_DEFER_ACCEPT", "TCP_FASTOPEN")
	
option_end()

option("LIBS")
	set_default(true)
	set_showmenu(false)
	add_deps("LIBPQ", "ONIG_REGEX", "SSL", "SSL_CRYPTO", "ZLIB", "CURL", "EXECINFO")
option_end()

option("ALL")
	set_default(true)
	set_showmenu(false)
	add_deps("MOD_MEMORY","MOD_MEMCACHED", "MOD_REDIS", "MOD_SDORM_SQL", "MOD_SDORM_MONGO", "SRV_EMB", "LIBS", "NETWORK_LIBS")
option_end()

add_includedirs("/usr/local/include")
add_includedirs("$(projectdir)/src/modules/common")
add_includedirs("$(projectdir)/src/modules/cache")
add_includedirs("$(projectdir)/src/modules/cache/memory")
add_includedirs("$(projectdir)/src/modules/cache/redis")
add_includedirs("$(projectdir)/src/modules/cache/memcached")
add_includedirs("$(projectdir)/src/modules/client-util")
add_includedirs("$(projectdir)/src/modules/http")
add_includedirs("$(projectdir)/src/modules/http/http11")
add_includedirs("$(projectdir)/src/modules/http/http20")
add_includedirs("$(projectdir)/src/modules/ioc")
add_includedirs("$(projectdir)/src/modules/jobs")
add_includedirs("$(projectdir)/src/modules/reflection")
add_includedirs("$(projectdir)/src/modules/sdorm")
add_includedirs("$(projectdir)/src/modules/sdorm/sql")
add_includedirs("$(projectdir)/src/modules/sdorm/sql/libpq")
add_includedirs("$(projectdir)/src/modules/client-util/ssl")
add_includedirs("$(projectdir)/src/modules/sdorm/mongo")
add_includedirs("$(projectdir)/src/modules/sdorm/mongo/raw")
add_includedirs("$(projectdir)/src/modules/search")
add_includedirs("$(projectdir)/src/modules/serialization")
add_includedirs("$(projectdir)/src/modules/serialization/xml")
add_includedirs("$(projectdir)/src/modules/serialization/json")
add_includedirs("$(projectdir)/src/modules/server-util")
add_includedirs("$(projectdir)/src/modules/ssl")
add_includedirs("$(projectdir)/src/modules/threads")
add_includedirs("$(projectdir)/src/framework")

local libext = ".so"
if is_plat("macosx") then
	add_includedirs("/usr/local/opt/openssl/include")
	add_linkdirs("/usr/local/opt/openssl/lib/")
	libext = ".dylib"
end

add_cflags("-w")

function setIncludes(target)
	import("lib.detect.find_path")
	local p = find_path("mongoc.h", {"/usr/include/libmongoc-1.0", "/usr/local/include/libmongoc-1.0"})
	if not p then
		raise('mongoc headers not found')
	else
		target:add({includedirs = p})
	end
	p = find_path("bson.h", {"/usr/include/libbson-1.0", "/usr/local/include/libbson-1.0"})
	if not p then
		raise('bson headers not found')
	else
		target:add({includedirs = p})
	end
	p = find_path("uuid.h", {"/usr/include/uuid", "/usr/local/include/uuid"})
	if not p then
		p = find_path("uuid.h", {"/usr/include/ossp", "/usr/local/include/ossp"})
		if not p then
			raise('uuid headers not found')
		else
			target:add({includedirs = p})
		end
	else
		target:add({includedirs = p})
	end
	p = find_path("libpq-fe.h", {"/usr/include/postgresql", "/usr/local/include/postgresql"})
	if not p then
		p = find_path("libpq-fe.h", {"/usr/include/pgsql", "/usr/local/include/pgsql"})
		if not p then
			raise('libpq headers not found')
		else
			target:add({includedirs = p})
		end
	else
		target:add({includedirs = p})
	end
	import("lib.detect.find_package")
	local l = find_package("dl")
	if l then
		target:add({links = "dl"})
	end
	target:add({linkdirs = "/usr/local/lib"})
end

local bindir = "$(projectdir)/ffead-cpp-5.0-bin"

includes("src/modules")
includes("src/framework")
includes("tests")

-- add all web applications here below
includes("web/default")
includes("web/flexApp")
includes("web/markers")
includes("web/oauthApp")
includes("web/peer-server")
includes("web/te-benchmark")
includes("web/te-benchmark-um")
includes("web/te-benchmark-um-mgr")
includes("web/te-benchmark-um-pq")
includes("web/te-benchmark-um-pq-async")

target("ffead-cpp")
	set_languages("c++17")
	add_deps("tests")
	add_options("ALL")
    set_kind("binary")
	on_load(setIncludes)
    add_files("$(projectdir)/src/server/embedded/*.cpp")
    if has_config("LIBPQ") then
		set_configvar("HAVE_PQHDR", 1)
		set_configvar("HAVE_LIBPQ", 1)
	end
    if has_config("LIBPQ_BATCH") then
		set_configvar("HAVE_LIBPQ_BATCH", 1)
	end
    if has_config("REGEX") then
		set_configvar("HAVE_REGEX", 1)
	end
    if has_config("ONIG_REGEX") then
		set_configvar("HAVE_ONIG_REGEX", 1)
		set_configvar("HAVE_ONIG_REGEX_LIB", 1)
	end
    if has_config("SSL") then
		set_configvar("HAVE_SSLINC", 1)
		set_configvar("HAVE_SSLLIB", 1)
	end
    if has_config("SSL_CRYPTO") then
		set_configvar("HAVE_CRYPTOLIB", 1)
	end
    if has_config("CURL") then
		set_configvar("HAVE_CURLLIB", 1)
	end
    if has_config("UUID") then
		set_configvar("HAVE_UUIDINC", 1)
		set_configvar("HAVE_UUIDLIB", 1)
	end
    if has_config("OSSP_UUID") then
		set_configvar("HAVE_OSSPUUIDINC", 1)
		set_configvar("HAVE_UUIDLIB", 1)
	end
    if has_config("MOD_REDIS") then
		set_configvar("HAVE_REDISINC", 1)
		set_configvar("HAVE_REDISLIB", 1)
		set_configvar("INC_REDISCACHE", 1)
	end
    if has_config("MOD_MEMCACHED") then
		set_configvar("HAVE_MEMCACHEDINC", 1)
		set_configvar("HAVE_MEMCACHEDLIB", 1)
		set_configvar("INC_MEMCACHED", 1)
	end
    if has_config("MOD_SDORM_SQL") then
		set_configvar("HAVE_SQLINC", 1)
		set_configvar("HAVE_ODBCLIB", 1)
		set_configvar("INC_SDORM", 1)
		set_configvar("INC_SDORM_SQL", 1)
	end
    if has_config("MOD_SDORM_MONGO") then
		set_configvar("HAVE_MONGOINC", 1)
		set_configvar("HAVE_MONGOCLIB", 1)
		set_configvar("HAVE_BSONINC", 1)
		set_configvar("HAVE_BSONLIB", 1)
		set_configvar("INC_SDORM", 1)
		set_configvar("INC_SDORM_MONGO", 1)
	end
    if has_config("MOD_SER_BIN") then
		set_configvar("INC_BINSER", 1)
	end
    if has_config("MOD_JOBS") then
		set_configvar("INC_JOBS", 1)
	end
    if has_config("SRV_EMB") then
		set_configvar("SRV_EMB", 1)
	end
    if has_config("LIBCUCKOO") then
    	set_configvar("HAVE_CKOHMINC", 1)
	end
    if has_config("EXECINFO") then
		set_configvar("HAVE_EXECINFOINC", 1)
	end
    if has_config("KQUEUE") then
		set_configvar("USE_KQUEUE", 1)
	elseif has_config("IOURING") then
		set_configvar("USE_IO_URING", 1)
	elseif has_config("EVPORT") then
		set_configvar("USE_EVPORT", 1)
	elseif has_config("EPOLL") then
		set_configvar("USE_EPOLL", 1)
	elseif has_config("DEVPOLL") then
		set_configvar("USE_DEVPOLL", 1)
	elseif has_config("POLL") then
		set_configvar("USE_POLL", 1)
	elseif has_config("SELECT") then
		set_configvar("USE_SELECT", 1)
	end
    if has_config("ACCEPT4") then
		set_configvar("HAVE_ACCEPT4", 1)
	end
	if is_plat("linux") then
		set_configvar("OS_LINUX", 1)
	elseif is_plat("macosx") then
		set_configvar("APPLE", 1)
		set_configvar("OS_DARWIN", 1)
	elseif is_plat("solaris") then
		set_configvar("OS_SOLARIS", 1)
	elseif is_plat("bsd") then
		set_configvar("OS_BSD", 1)
	elseif is_plat("android") then
		set_configvar("OS_ANDROID", 1)
	elseif is_plat("mingw") then
		set_configvar("OS_MINGW", 1)
		set_configvar("MINGW", 1)
		set_configvar("USE_WIN_IOCP", 1)
	elseif is_plat("cygwin") then
		set_configvar("CYGWIN", 1)
	end
    set_configdir("src/modules/common")
    add_configfiles("src/modules/common/AppDefines.xmake.h.in", {filename = "AppDefines.h"})
    
    set_installdir(bindir)
    add_installfiles("src/modules/common/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/cache/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/cache/memory/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/cache/redis/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/cache/memcached/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/client-util/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/http/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/http/http11/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/http/http20/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/ioc/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/jobs/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/reflection/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/sdorm/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/sdorm/sql/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/sdorm/sql/libpq/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/client-util/ssl/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/sdorm/mongo/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/sdorm/mongo/raw/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/search/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/serialization/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/serialization/xml/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/serialization/json/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/server-util/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/ssl/*.h", {prefixdir = "include"})
	add_installfiles("src/modules/threads/*.h", {prefixdir = "include"})
	add_installfiles("src/framework/*.h", {prefixdir = "include"})
	add_links("dl")
	
	after_install(function (target)
		os.run("chmod +x xmake-install.sh")
        os.run("./xmake-install.sh \"%s\" \"$(projectdir)\" %s %s", bindir, tostring(has_config("MOD_SDORM_MONGO")), tostring(has_config("MOD_SDORM_SQL")))
    end)
