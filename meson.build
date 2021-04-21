project('ffead-cpp', 'cpp', version : '6.0', license : 'Apache', default_options : ['cpp_std=c++17'])

cc = meson.get_compiler('cpp')
conf_data = configuration_data()
global_libs = []

libzlib = [dependency('zlib')]
global_libs += [libzlib]

libuuid = dependency('uuid', required: false)
if libuuid.found()
	libuuid = [ libuuid ]
elif cc.has_function('uuid_generate', prefix: '#include <uuid/uuid.h>')
	# uuid_generate in libc, everything is fine, no lib needed
	libuuid = []
	conf_data.set('HAVE_UUIDINC', 1)
else
	libuuid = cc.find_library('uuid')
	if not(cc.has_function('uuid_generate',
			dependencies: libuuid,
			prefix: '#include <uuid/uuid.h>'
		))
		error('Couldn\'t find uuid/uuid.h or uuid_generate in lib c and uuid')
	endif
	global_libs += [libuuid]
	conf_data.set('HAVE_UUIDINC', 1)
endif
if not cc.has_header('uuid/uuid.h') and cc.has_header('ossp/uuid.h')
	libuuid = cc.find_library('ossp-uuid')
	if not libuuid.found()
		error('libuuid not found')
	endif
	conf_data.set('HAVE_OSSPUUIDINC', 1)
endif
conf_data.set('HAVE_UUIDLIB', 1)

libregex = []
if cc.has_function('regcomp', prefix: '#include <regex.h>')
	# regcomp in libc, everything is fine, no lib needed
	libregex = []
	conf_data.set('HAVE_REGEX', 1)
else
	libregex = cc.find_library('onig', dirs: ['/usr/local/lib'])
	if not(cc.has_function('regcomp',
			args: defs,
			prefix: '#include <onigposix.h>'
		))
		error('Couldn\'t find regex.h or onigposix.h')
	conf_data.set('HAVE_ONIG_REGEX', 1)
	conf_data.set('HAVE_ONIG_REGEX_LIB', 1)
	global_libs += [libregex]
	endif
endif

libssl = []
libcrypto = []
if build_machine.system().contains('darwin')
	libssl = cc.find_library('ssl', required: false, dirs: ['/usr/local/opt/openssl/lib/', '/usr/local/lib'])
	libcrypto = cc.find_library('crypto', required: false, dirs: ['/usr/local/opt/openssl/lib/', '/usr/local/lib'])
else
	libssl = cc.find_library('ssl', required: false)
	libcrypto = cc.find_library('crypto', required: false)
endif
if libssl.found()
	if cc.has_header('openssl/ssl.h')
		conf_data.set('HAVE_SSLINC', 1)
	endif
	conf_data.set('HAVE_SSLLIB', 1)
	global_libs += [libssl]
endif
if libcrypto.found()
	conf_data.set('HAVE_CRYPTOLIB', 1)
	global_libs += [libcrypto]
endif

libcurl = dependency('curl', required : false)
if libcurl.found()
	conf_data.set('HAVE_CURLLIB', 1)
	if cc.has_header('curl/curl.h')
		conf_data.set('HAVE_CURLINC', 1)
	endif
	global_libs += [libcurl]
endif

libmemcached = []
if get_option('MOD_MEMCACHED') == true
	libmemcached = cc.find_library('memcached', dirs: ['/usr/local/lib'])
	libmemcachedutil = cc.find_library('memcachedutil', dirs: ['/usr/local/lib'])
	if libmemcached.found() and libmemcachedutil.found()
		conf_data.set('HAVE_MEMCACHEDLIB', 1)
		if cc.has_header('libmemcached/memcached.h')
			conf_data.set('HAVE_MEMCACHEDINC', 1)
			conf_data.set('INC_MEMCACHED', 1)
		endif
		libmemcached = [libmemcached, libmemcachedutil]
		global_libs += [libmemcached]
	endif
endif

libhiredis = []
if get_option('MOD_REDIS') == true
	libhiredis = cc.find_library('hiredis', dirs: ['/usr/local/lib'])
	if libhiredis.found()
		conf_data.set('HAVE_REDISLIB', 1)
		if cc.has_header('hiredis/hiredis.h')
			conf_data.set('HAVE_REDISINC', 1)
			conf_data.set('INC_REDISCACHE', 1)
		endif
		global_libs += [libhiredis]
	endif
endif

global_includes = ['/usr/local/include']

libpq = []
libodbc = []
if get_option('MOD_SDORM_SQL') == true
	libodbc = cc.find_library('odbc', dirs: ['/usr/local/lib'])
	if libodbc.found()
		if cc.has_header('sql.h')
			conf_data.set('HAVE_SQLINC', 1)
			conf_data.set('INC_SDORM', 1)
			conf_data.set('INC_SDORM_SQL', 1)
		endif
		conf_data.set('HAVE_ODBCLIB', 1)
		global_libs += [libodbc]
	endif
	libpq = cc.find_library('pq', dirs: ['/usr/local/lib'], required: false)
	pqincpath = ''
	if libpq.found()
		if not cc.has_header('libpq-fe.h')
			if not cc.has_header('libpq-fe.h', args : '-I/usr/include/postgresql')
				if not cc.has_header('libpq-fe.h', args : '-I/usr/include/pgsql')
					if not cc.has_header('libpq-fe.h', args : '-I/usr/local/include/postgresql')
						if not cc.has_header('libpq-fe.h', args : '-I/usr/local/include/pgsql')
							error('libpq headers not found')
						else
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
			global_includes += [pqincpath]
			conf_data.set('HAVE_PQHDR', 1)
			conf_data.set('HAVE_LIBPQ', 1)
			global_libs += [libpq]
		endif
	endif
	if libpq.found() and cc.has_function('PQenterBatchMode', prefix: '#include <libpq-fe.h>', args: '-I'+pqincpath)
		# regcomp in libc, everything is fine, no lib needed
		conf_data.set('HAVE_LIBPQ_BATCH', 1)
	endif
endif

if build_machine.system().contains('darwin')
	global_includes += ['/usr/local/opt/openssl/include']
endif

libbson = []
libmongoc = []
if get_option('MOD_SDORM_MONGO') == true
	libbson = cc.find_library('bson-1.0', dirs: ['/usr/local/lib'])
	libmongoc = cc.find_library('mongoc-1.0', dirs: ['/usr/local/lib'])
	if not cc.has_header('bson.h', args : '-I/usr/include/libbson-1.0')
		if not cc.has_header('bson.h', args : '-I/usr/local/include/libbson-1.0')
			error('bson headers not found')
		else
			global_includes += ['/usr/local/include/libbson-1.0']
		endif
	else
		global_includes += ['/usr/include/libbson-1.0']
	endif
	if not cc.has_header('mongoc.h', args : '-I/usr/include/libmongoc-1.0')
		if not cc.has_header('mongoc.h', args : '-I/usr/local/include/libmongoc-1.0')
			error('mongoc headers not found')
		else
			global_includes += ['/usr/local/include/libmongoc-1.0']
		endif
	else
		global_includes += ['/usr/include/libmongoc-1.0']
	endif
	if libmongoc.found()
		conf_data.set('HAVE_MONGOINC', 1)
		conf_data.set('HAVE_MONGOCLIB', 1)
		conf_data.set('HAVE_BSONINC', 1)
		conf_data.set('HAVE_BSONLIB', 1)
		conf_data.set('INC_SDORM', 1)
		conf_data.set('INC_SDORM_MONGO', 1)
		global_libs += [libbson, libmongoc]
	endif
endif

libdl = cc.find_library('dl')
if libdl.found()
	global_libs += [libdl]
endif

libpthread = cc.find_library('pthread')
if libdl.found()
	global_libs += [libpthread]
endif

if get_option('MOD_SER_BIN') == true
	conf_data.set('INC_BINSER', 1)
endif

if get_option('MOD_JOBS') == true
	conf_data.set('INC_JOBS', 1)
endif

if not cc.has_header('libcuckoo/cuckoohash_map.hh', args : '-I/usr/local/include/')
	error('libcuckoo headers not found')
endif

if get_option('SRV_EMB') == true
	conf_data.set('SRV_EMB', 1)
endif

if build_machine.system().contains('darwin')
	conf_data.set('OS_DARWIN', 1)
	conf_data.set('APPLE', 1)
elif build_machine.system().contains('cygwin')
	conf_data.set('CYGWIN', 1)
elif build_machine.system().contains('emscripten')
	conf_data.set('EMSCRIPTEN', 1)
elif build_machine.system().contains('windows')
	conf_data.set('OS_MINGW', 1)
	conf_data.set('MINGW', 1)
	conf_data.set('USE_WIN_IOCP', 1)
elif build_machine.system().contains('linux')
	conf_data.set('OS_LINUX', 1)
elif build_machine.system().contains('bsd')
	conf_data.set('OS_BSD', 1)
elif build_machine.system().contains('android')
	conf_data.set('OS_ANDROID', 1)
endif

if cc.has_function('accept4', prefix: '#include <sys/socket.h>')
	conf_data.set('ACCEPT4', 1)
endif

if cc.compiles('#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_QUICKACK; }')
	conf_data.set('TCP_QUICKACK', 1)
endif

if cc.compiles('#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_DEFER_ACCEPT; }')
	conf_data.set('TCP_DEFER_ACCEPT', 1)
endif

if cc.compiles('#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_FASTOPEN; }')
	conf_data.set('TCP_FASTOPEN', 1)
endif

if cc.has_header('liburing', args : '-I/usr/local/include')
	conf_data.set('USE_IO_URING', 1)
elif cc.has_header('sys/epoll.h')
	conf_data.set('USE_EPOLL', 1)
elif cc.has_header('sys/event.h')
	conf_data.set('USE_KQUEUE', 1)
elif cc.has_header('port.h')
	conf_data.set('USE_EVPORT', 1)
elif cc.has_header('sys/devpoll.h')
	conf_data.set('USE_DEVPOLL', 1)
elif cc.has_header('sys/poll.h')
	conf_data.set('USE_POLL', 1)
elif cc.has_header('sys/select.h')
	conf_data.set('USE_SELECT', 1)
endif

if cc.has_header('execinfo.h')
	conf_data.set('HAVE_EXECINFOINC', 1)
endif

#message(conf_data.keys())

configure_file(input : 'src/modules/common/AppDefines.meson.h.in', output : 'AppDefines.h', configuration : conf_data)

conf_file_path = meson.current_build_dir() + '/AppDefines.h'
run_command('mv', conf_file_path, 'src/modules/common/')
run_command('chmod', '+x', 'meson_grabber.sh')
run_command('chmod', '+x', 'meson_postinstall.sh')

meson_grabber = meson.source_root() + '/meson_grabber.sh'
meson_post_install = meson.source_root() + '/meson_postinstall.sh'

bin_dir = meson.current_source_dir()+'/'+meson.project_name()+'-'+meson.project_version()+'-bin/'
run_command('rm', '-rf', bin_dir)
run_command('mkdir', bin_dir)

subdir('src/modules')
subdir('src/framework')
subdir('tests')

#Add all web application directories herer
subdir('web/default')
subdir('web/flexApp')
subdir('web/markers')
subdir('web/oauthApp')
subdir('web/peer-server')
subdir('web/te-benchmark')
subdir('web/te-benchmark-um')
if get_option('MOD_SDORM_MONGO') == true
	subdir('web/te-benchmark-um-mgr')
endif
if get_option('MOD_SDORM_SQL') == true
	subdir('web/te-benchmark-um-pq')
	subdir('web/te-benchmark-um-pq-async')
endif

emb_includes = ['src/modules/common','src/modules/cache','src/modules/cache/memory','src/modules/cache/redis',
	'src/modules/cache/memcached','src/modules/client-util','src/modules/http','src/modules/http/http11','src/modules/http/http20',
	'src/modules/ioc','src/modules/jobs','src/modules/reflection','src/modules/sdorm','src/modules/sdorm/sql','src/modules/sdorm/sql/libpq',
	'src/modules/client-util/ssl','src/modules/sdorm/mongo','src/modules/sdorm/mongo/raw','src/modules/search','src/modules/serialization',
	'src/modules/serialization/xml','src/modules/serialization/json','src/modules/server-util','src/modules/ssl','src/modules/threads',
	'src/framework']
	
foreach inc: emb_includes
	c = run_command(meson_grabber, inc, 'all', '*.h')
	incs = c.stdout().strip().split('\n')
	install_headers(incs, install_dir: bin_dir+'include')
endforeach

emb_includes += [global_includes]

executable('ffead-cpp', 'src/server/embedded/CHServer.cpp', include_directories: emb_includes, dependencies: global_libs, 
	link_with: [libffeadmodules, libffeadframework], install: true, install_dir: bin_dir)

install_subdir('resources', install_dir: bin_dir)
install_subdir('public', install_dir: bin_dir)
install_subdir('rtdcf', install_dir: bin_dir)
install_subdir('web', install_dir: bin_dir)

meson.add_install_script(meson_post_install, bin_dir, meson.source_root())
