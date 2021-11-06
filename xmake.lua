set_project("ffead-cpp")

set_xmakever("2.3.2")

set_version("6.0", {build = "%Y%m%d%H%M"})

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
	after_check(function (option)
		if option:enabled() then
			option:set("configvar", "SRV_EMB", 1)
		end
	end)
option_end()

option("MOD_MEMORY")
	set_default(true)
	set_showmenu(true)
	set_description("Enable Memory Cache module")
	after_check(function (option)
		if option:enabled() then
			option:set("configvar", "INC_MEMORYCACHE", 1)
		end
	end)
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
	        option:set("configvar", "HAVE_MEMCACHEDINC", 1)
			option:set("configvar", "HAVE_MEMCACHEDLIB", 1)
			option:set("configvar", "INC_MEMCACHED", 1)
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
	        option:set("configvar", "HAVE_REDISINC", 1)
			option:set("configvar", "HAVE_REDISLIB", 1)
			option:set("configvar", "INC_REDISCACHE", 1)
	    end
    end)
option_end()

option("MOD_SER_BIN")
	set_default(false)
	set_showmenu(true)
	set_description("Enable Binary Serialization module")
	after_check(function (option)
		if option:enabled() then
			option:set("configvar", "INC_BINSER", 1)
		end
	end)
option_end()

option("MOD_JOBS")
	set_default(true)
	set_showmenu(true)
	set_description("Enable Jobs module")
	after_check(function (option)
		if option:enabled() then
			option:set("configvar", "INC_JOBS", 1)
		end
	end)
option_end()

option("LIBPQ")
	set_default(true)
	set_description("Check whether libpq is present")
	after_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("pq")
		if l then
			import("lib.detect.find_path")
			local p = find_path("libpq-fe.h", {"/usr/include", "/usr/local/include", 
												"/usr/include/postgresql", "/usr/local/include/postgresql", 
												"/usr/include/pgsql", "/usr/local/include/pgsql"})
			if not p then
				raise('libpq headers not found')
			end
			option:add({includedirs = p})
	        option:set("configvar", "HAVE_PQHDR", 1)
	        option:set("configvar", "HAVE_LIBPQ", 1)
	        option:add(l)
	        
	        import("lib.detect.find_path")
			local p = find_path("libpq-fe.h", {"/usr/include", "/usr/local/include", "/usr/include/postgresql", "/usr/local/include/postgresql"})
			if not p then
				p = find_path("libpq-fe.h", {"/usr/include/pgsql", "/usr/local/include/pgsql"})
				if not p then
					raise('libpq headers not found')
				end
			end
			option:add({includedirs = p})
			
			import("lib.detect.has_cfuncs")
			local ok = has_cfuncs("PQenterBatchMode", {includes = "libpq-fe.h"})
			if not ok then
				ok = has_cfuncs("PQenterBatchMode", {includes = "postgresql/libpq-fe.h"})
				if not ok then
					ok = has_cfuncs("PQenterBatchMode", {includes = "pgsql/ibpq-fe.h"})
					if ok then
						option:set("configvar", "HAVE_LIBPQ_BATCH", 1)
					end
				else
					option:set("configvar", "HAVE_LIBPQ_BATCH", 1)
				end
			else
				option:set("configvar", "HAVE_LIBPQ_BATCH", 1)
			end
			ok = has_cfuncs("PQenterPipelineMode", {includes = "libpq-fe.h"})
			if not ok then
				ok = has_cfuncs("PQenterPipelineMode", {includes = "postgresql/libpq-fe.h"})
				if not ok then
					ok = has_cfuncs("PQenterPipelineMode", {includes = "pgsql/ibpq-fe.h"})
					if ok then
						option:set("configvar", "HAVE_LIBPQ_PIPELINE", 1)
					end
				else
					option:set("configvar", "HAVE_LIBPQ_PIPELINE", 1)
				end
			else
				option:set("configvar", "HAVE_LIBPQ_PIPELINE", 1)
			end
		end
    end)
option_end()

option("CURL")
	set_description("Check whether curl is present")
	on_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("curl")
		if l then
			option:add(l)
			option:enable(true)
			option:set("configvar", "HAVE_CURLLIB", 1)
		end
    end)
option_end()

option("ZLIB")
	set_description("Check whether zlib is present")
	on_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("z")
		if not l then
			raise('z library not found')
		end
		option:add(l)
		option:enable(true)
    end)
option_end()

option("SSL")
	set_description("Check whether ssl is present")
	after_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("ssl")
		local l1 = find_package("crypto")
		if l and l1 then
			option:set("configvar", "HAVE_SSLINC", 1)
			option:set("configvar", "HAVE_SSLLIB", 1)
			option:set("configvar", "HAVE_CRYPTOLIB", 1)
			option:add(l)
			option:add(l1)
			option:enable(true)
		end
    end)
option_end()

option("CHECK_REGEX")
	set_description("Check whether regex is present")
	on_check(function (option)
		import("lib.detect.has_cfuncs")
		local ok = has_cfuncs("regcomp", {includes = "regex.h"})
		if ok then
			option:add(l)
			option:set("configvar", "HAVE_REGEX", 1)
		else
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
		        option:set("configvar", "HAVE_ONIG_REGEX", 1)
		        option:set("configvar", "HAVE_ONIG_REGEX_LIB", 1)
		        
		        import("lib.detect.find_path")
				local p = find_path("uuid.h", {"/usr/include/ossp", "/usr/local/include/ossp"})
				if not p then
					p = find_path("uuid.h", {"/usr/include", "/usr/local/include"})
					if not p then
						raise('uuid headers not found')
					end
				end
				option:add({includedirs = p})
			end
		end
		option:enable(true)
    end)
option_end()

option("CHECK_UUID")
	set_description("Check whether uuid is present")
	on_check(function (option)
		import("lib.detect.find_package")
		local l = find_package("uuid")
		if not l then
			import("lib.detect.find_package")
			local l = find_package("ossp-uuid")
			if not l then
				raise('uuid library not found')
			else
				import("lib.detect.find_path")
				local p = find_path("uuid.h", {"/usr/include/ossp", "/usr/local/include/ossp"})
				if not p then
					p = find_path("uuid.h", {"/usr/include", "/usr/local/include"})
					if not p then
						raise('uuid headers not found')
					end
					option:set("configvar", "HAVE_OSSPUUIDINC_2", 1)
				else
					option:set("configvar", "HAVE_OSSPUUIDINC", 1)
				end
			end
			option:set("configvar", "HAVE_UUIDLIB", 1)
			option:add(l)
		else
			import("lib.detect.has_cfuncs")
			local ok = has_cfuncs("uuid_generate", {includes = "uuid/uuid.h"})
			if not ok then
				raise('uuid headers not found')
			end
			option:set("configvar", "HAVE_UUIDINC", 1)
			option:set("configvar", "HAVE_UUIDLIB", 1)
			option:add(l)
		end
		option:enable(true)
    end)
option_end()

option("MOD_SDORM_SQL")
	add_deps("CHECK_UUID")
	set_default(true)
	set_showmenu(true)
	set_description("Enable SQL Sdorm module")
	after_check(function (option)
		if option:enabled() and option:dep("CHECK_UUID"):enabled() then
			import("lib.detect.find_package")
			local l = find_package("odbc")
			if not l then
				raise('odbc library not found')
			end
	        option:add(l)
	        option:set("configvar", "HAVE_SQLINC", 1)
			option:set("configvar", "HAVE_ODBCLIB", 1)
			option:set("configvar", "INC_SDORM", 1)
			option:set("configvar", "INC_SDORM_SQL", 1)
	    end
    end)
option_end()

option("MOD_SDORM_MONGO")
	add_deps("CHECK_UUID")
	set_default(false)
	set_showmenu(true)
	set_description("Enable MongoDB Sdorm module")
	after_check(function (option)
		if option:enabled() and option:dep("CHECK_UUID"):enabled() then
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
	        option:set("configvar", "HAVE_MONGOINC", 1)
			option:set("configvar", "HAVE_MONGOCLIB", 1)
			option:set("configvar", "HAVE_BSONINC", 1)
			option:set("configvar", "HAVE_BSONLIB", 1)
			option:set("configvar", "INC_SDORM", 1)
			option:set("configvar", "INC_SDORM_MONGO", 1)
			
			import("lib.detect.find_path")
			local p = find_path("mongoc.h", {"/usr/include/libmongoc-1.0", "/usr/local/include/libmongoc-1.0"})
			if not p then
				raise('mongoc headers not found')
			end
			option:add("includedirs", p)
			
			p = find_path("bson.h", {"/usr/include/libbson-1.0", "/usr/local/include/libbson-1.0"})
			if not p then
				raise('bson headers not found')
			end
			option:add("includedirs", p)
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
		option:set("configvar", "HAVE_CKOHMINC", 1)
	end)
option_end()

option("WITH_RAPIDJSON")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cxxincludes")
		local ok = has_cxxincludes({"rapidjson/document.h"}, {target = target})
		if ok then
			option:set("configvar", "HAVE_RAPID_JSON", 1)
		end
	end)
option_end()

option("WITH_PUGIXML")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cxxincludes")
		local ok = has_cxxincludes({"pugixml.hpp"}, {target = target})
		if ok then
			import("lib.detect.find_package")
			local l = find_package("pugixml")
			if not l then
				raise('pugixml library not found')
			end
			option:set("configvar", "HAVE_PUGI_XML", 1)
	        option:add(l)
		end
	end)
option_end()

option("GENERIC")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cincludes")
		ok = has_cincludes({"execinfo.h"})
		if ok then
			option:set("configvar", "HAVE_EXECINFOINC", 1)
		end
		import("lib.detect.has_cincludes")
		ok = has_cincludes({"sys/sysinfo.h"})
		if ok then
			option:set("configvar", "HAVE_SYSINFO", 1)
		end
	end)
option_end()

option("SELECT")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.has_cincludes")
		ok = has_cincludes("sys/select.h")
		if ok then
			option:set("configvar", "USE_SELECT", 1)
		else
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
		if ok then
			option:set("configvar", "USE_POLL", 1)
		else
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
		if ok then
			option:set("configvar", "USE_DEVPOLL", 1)
		else
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
			local ok = has_cincludes("sys/epoll.h")
			if ok then
				option:set("configvar", "USE_EPOLL", 1)
			else
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
			local ok = has_cincludes("sys/event.h")
			if ok then
				option:set("configvar", "USE_KQUEUE", 1)
			else
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
			local ok = has_cincludes("port.h")
			if ok then
				option:set("configvar", "USE_EVPORT", 1)
			else
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
			local ok = has_cincludes("liburing.h")
			if ok then
				option:set("configvar", "USE_IO_URING", 1)
			else
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
		if ok then
			option:set("configvar", "HAVE_ACCEPT4", 1)
		else
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
		if ok then
			option:set("configvar", "HAVE_TCP_QUICKACK", 1)
		else
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
		if ok then
			option:set("configvar", "HAVE_TCP_DEFER_ACCEPT", 1)
		else
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
		if ok then
			option:set("configvar", "HAVE_TCP_FASTOPEN", 1)
		else
			option:enable(false)
		end
	end)
option_end()

option("SO_ATTACH_REUSEPORT_CBPF")
	set_default(true)
	set_showmenu(false)
	after_check(function (option)
		import("lib.detect.check_cxsnippets")
		local ok = check_cxsnippets({"#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\n#include <linux/bpf.h>\n#include <linux/filter.h>\n#include <sys/sysinfo.h>\nint test() { return SO_ATTACH_REUSEPORT_CBPF; }"})
		if ok then
			option:set("configvar", "HAVE_SO_ATTACH_REUSEPORT_CBPF", 1)
		else
			option:enable(false)
		end
	end)
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

if is_plat("linux") then
	set_configvar("OS_LINUX", 1)
elseif is_plat("macosx") then
	set_configvar("APPLE", 1)
	set_configvar("OS_DARWIN", 1)
	add_includedirs("/usr/local/opt/openssl/include")
	add_linkdirs("/usr/local/opt/openssl/lib/")
	libext = ".dylib"
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

add_cflags("-w")

function setIncludes(target)
	import("lib.detect.find_package")
	local l = find_package("dl")
	if l then
		target:add({links = "dl"})
	end
	target:add({linkdirs = "/usr/local/lib"})
	import("lib.detect.has_cincludes")
	local ok = has_cincludes("sys/sendfile.h")
	if ok then
		target:set("configvar", "IS_SENDFILE", 1)
	end
end

local bindir = "$(projectdir)/ffead-cpp-6.0-bin"

function getOptions()
	return {"CHECK_UUID", "LIBPQ", "CHECK_REGEX", "SSL", "ZLIB", "CURL", "GENERIC", "LIBCUCKOO", 
			 "SELECT", "POLL", "DEVPOLL", "IOURING", "EPOLL", "KQUEUE", "EVPORT", "ACCEPT4", "TCP_QUICKACK", 
			 "TCP_DEFER_ACCEPT", "TCP_FASTOPEN", "MOD_MEMORY","MOD_MEMCACHED", "MOD_REDIS", "MOD_SDORM_SQL", 
			 "MOD_SDORM_MONGO", "MOD_SER_BIN", "MOD_JOBS", "SRV_EMB", "WITH_RAPIDJSON", "WITH_PUGIXML"}
end

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
includes("web/t1")
includes("web/t2")
includes("web/t3")
includes("web/t4")
includes("web/t5")

target("ffead-cpp")
	set_languages("c++17")
	add_deps("tests")
	add_options(getOptions())
	set_kind("binary")
	on_load(setIncludes)
	add_files("$(projectdir)/src/server/embedded/*.cpp")

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
