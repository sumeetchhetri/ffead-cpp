project('ffead-cpp-inter', 'cpp', version : '5.4', license : 'Apache', default_options : ['cpp_std=c++17'])

cc = meson.get_compiler('cpp')

meson_lister = meson.source_root() + '/meson_lister.sh'

libdir = meson.source_root() + '/../lib'

wd_libs = []
wd_includes = []

c = run_command(meson_lister, '../web')
webdirs = c.stdout().strip().split('\n')

foreach wd: webdirs
	libwd = cc.find_library(wd, dirs: [libdir])
	wd_libs += [libwd]
	wd_includes += ['../web/'+wd+'/include']
endforeach

pqincpath = ''
if not cc.has_header('libpq-fe.h')
	if not cc.has_header('libpq-fe.h', args : '-I/usr/include/postgresql')
		if not cc.has_header('libpq-fe.h', args : '-I/usr/include/pgsql')
			if not cc.has_header('libpq-fe.h', args : '-I/usr/local/include/postgresql')
				if cc.has_header('libpq-fe.h', args : '-I/usr/local/include/pgsql')
					pqincpath = '/usr/local/include/pgsql'
				endif
			else
				pqincpath = '/usr/local/include/postgresql'
			endif
		else
			pqincpath = '/usr/include/pgsql'
		endif
	else
		pqincpath = '/usr/include/postgresql'
	endif
	wd_includes += [pqincpath]
endif

if not cc.has_header('bson.h', args : '-I/usr/include/libbson-1.0')
	if cc.has_header('bson.h', args : '-I/usr/local/include/libbson-1.0')
		wd_includes += ['/usr/local/include/libbson-1.0']
	endif
else
	wd_includes += ['/usr/include/libbson-1.0']
endif
if not cc.has_header('mongoc.h', args : '-I/usr/include/libmongoc-1.0')
	if cc.has_header('mongoc.h', args : '-I/usr/local/include/libmongoc-1.0')
		wd_includes += ['/usr/local/include/libmongoc-1.0']
	endif
else
	wd_includes += ['/usr/include/libmongoc-1.0']
endif

wd_includes += ['../include']

libffeadmod = cc.find_library('ffead-modules', dirs: [libdir])
if libffeadmod.found()
	wd_libs += [libffeadmod]
endif

libffeadfrm = cc.find_library('ffead-framework', dirs: [libdir])
if libffeadfrm.found()
	wd_libs += [libffeadfrm]
endif

inter_sources = ['ReflectorInterface.cpp', 'SerializeInterface.cpp', 'AjaxInterface.cpp', 'WsInterface.cpp']
dinter_sources = ['DCPInterface.cpp', 'TemplateInterface.cpp']

shared_library('inter', inter_sources, include_directories: wd_includes, dependencies: wd_libs, install: true, install_dir: libdir)
shared_library('dinter', dinter_sources, include_directories: wd_includes, dependencies: wd_libs, install: true, install_dir: libdir)
