set_project("ffead-cpp-inter")

set_xmakever("2.3.2")

set_version("7.0", {build = "%Y%m%d%H%M"})

function setup(target)
	local outdata = os.iorun("$(projectdir)/meson_lister.sh $(projectdir)/../web")
    for dir in outdata:gmatch("[^\r\n]+") do
            target:add({includedirs = "$(projectdir)/../web/"..dir.."/include"})
            target:add({links = dir})
    end
	
	target:add({includedirs = "/usr/local/include"})
	target:add({includedirs = "../include"})
	target:add({links = 'ffead-modules'})
	target:add({links = 'ffead-framework'})
	
	import("lib.detect.find_path")
	local p = find_path("mongoc.h", {"/usr/include/libmongoc-1.0", "/usr/local/include/libmongoc-1.0", "/opt/homebrew/include/libmongoc-1.0"})
	if not p then
		raise('mongoc headers not found')
	else
		target:add({includedirs = p})
	end
	p = find_path("bson.h", {"/usr/include/libbson-1.0", "/usr/local/include/libbson-1.0", "/opt/homebrew/include/libbson-1.0"})
	if not p then
		raise('bson headers not found')
	else
		target:add({includedirs = p})
	end
	p = find_path("uuid.h", {"/usr/include/uuid", "/usr/local/include/uuid", "/usr/include", "/usr/local/include", "/opt/homebrew/include", "/opt/homebrew/include/uuid"})
	if not p then
		p = find_path("uuid.h", {"/usr/include/ossp", "/usr/local/include/ossp", "/opt/homebrew/include/ossp"})
		if not p then
			raise('uuid headers not found')
		else
			target:add({includedirs = p})
		end
	else
		target:add({includedirs = p})
	end
	p = find_path("libpq-fe.h", {"/usr/include", "/usr/local/include", "/usr/include/postgresql", "/usr/local/include/postgresql", "/opt/homebrew/include/postgresql"})
	if not p then
		p = find_path("libpq-fe.h", {"/usr/include/pgsql", "/usr/local/include/pgsql", "/opt/homebrew/include/pgsql"})
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
	l = find_package("uuid")
	if not l then
		l = find_package("ossp-uuid")
		if not l then
			raise('uuid library not found')
		end
		target:add({links = "ossp-uuid"})
		p = find_path("uuid.h", {"/usr/include/ossp", "/usr/local/include/ossp", "/opt/homebrew/include/ossp"})
		if not p then
			p = find_path("uuid.h", {"/usr/include", "/usr/local/include", "/opt/homebrew/include"})
			if not p then
				raise('uuid headers not found')
			end
		else
			target:add({includedirs = p})
		end
	else
		target:add({links = "uuid"})
	end
	target:add({linkdirs = "/usr/local/lib"})
	target:add({linkdirs = "/opt/homebrew/lib"})
	target:add({linkdirs = "../lib"})
end

target("inter")
	set_languages("c++17")
	set_kind("shared")
	on_load(setup)
	add_files("ReflectorInterface.cpp", "SerializeInterface.cpp", "AjaxInterface.cpp", "WsInterface.cpp")
	set_installdir("$(projectdir)")
	after_install(function (target)
		os.run("chmod +x xmake-install.sh")
		os.run("./xmake-install.sh libinter")
    end)

target("dinter")
	set_languages("c++17")
	set_kind("shared")
	on_load(setup)
	add_files("DCPInterface.cpp", "TemplateInterface.cpp")
	set_installdir("$(projectdir)")
	after_install(function (target)
		os.run("chmod +x xmake-install.sh")
		os.run("./xmake-install.sh libdinter")
    end)
	