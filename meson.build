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
elif cc.has_header('uuid.h')
	conf_data.set('HAVE_OSSPUUIDINC_2', 1)
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
	librredispp = cc.find_library('redis++', dirs: ['/usr/local/lib'])
	if librredispp.found()
		conf_data.set('HAVE_REDIS_CLUSTERLIB', 1)
		if cc.has_header('sw/redis++/redis++.h')
			conf_data.set('HAVE_REDIS_CLUSTERINC', 1)
			conf_data.set('INC_REDISCACHE', 1)
		endif
		global_libs += [librredispp]
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
	pqincpath = '/usr/include'
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
		endif
		conf_data.set('HAVE_PQHDR', 1)
		conf_data.set('HAVE_LIBPQ', 1)
		global_libs += [libpq]
	endif
	if libpq.found() and cc.has_function('PQenterBatchMode', prefix: '#include <libpq-fe.h>', args: '-I'+pqincpath, dependencies: libpq)
		# regcomp in libc, everything is fine, no lib needed
		conf_data.set('HAVE_LIBPQ_BATCH', 1)
	endif
	if libpq.found() and cc.has_function('PQenterPipelineMode', prefix: '#include <libpq-fe.h>', args: '-I'+pqincpath, dependencies: libpq)
		# regcomp in libc, everything is fine, no lib needed
		conf_data.set('HAVE_LIBPQ_PIPELINE', 1)
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

if get_option('MOD_SDORM_SCYLLA') == true
	if cc.has_header('cassandra.h')
		conf_data.set('HAVE_SCYLLAINC', 1)
	else
		error('scylla-cpp-driver headers not found')
	endif
	libscylla = cc.find_library('scylla-cpp-driver')
	if libscylla.found()
		conf_data.set('HAVE_SCYLLALIB', 1)
		global_libs += [libscylla]
	else
		error('scylla-cpp-driver lib not found')
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

if get_option('MOD_SOLR') == true
	conf_data.set('HAVE_SOLR', 1)
endif

if get_option('MOD_ELASTIC') == true
	if cc.has_header('elasticlient/client.h', args : '-I/usr/local/include/')
		conf_data.set('HAVE_ELASTIC', 1)
	else
		error('elasticlient headers not found')
	endif
	libelastic = cc.find_library('elasticlient')
	if libelastic.found()
		global_libs += [libelastic]
		conf_data.set('HAVE_ELASTICLIB', 1)
	else
		error('elastic lib not found')
	endif
	if cc.has_header('cpr/response.h', args : '-I/usr/local/include/')
		conf_data.set('HAVE_CPR', 1)
	else
		error('cpr headers not found')
	endif
	libcpr = cc.find_library('cpr')
	if libcpr.found()
		global_libs += [libcpr]
		conf_data.set('HAVE_CPRLIB', 1)
	else
		error('cpr lib not found')
	endif
endif

if not cc.has_header('libcuckoo/cuckoohash_map.hh', args : '-I/usr/local/include/')
	error('libcuckoo headers not found')
endif

if get_option('WITH_RAPIDJSON') == true and cc.has_header('rapidjson/document.h', args : '-I/usr/local/include/')
	conf_data.set('HAVE_RAPID_JSON', 1)
endif

if get_option('WITH_PUGIXML') == true
	if cc.has_header('pugixml.hpp', args : '-I/usr/local/include/')
		conf_data.set('HAVE_PUGI_XML', 1)
	else
		error('pugixml headers not found')
	endif
	libpxml = cc.find_library('pugixml')
	if libpxml.found()
		global_libs += [libpxml]
	else
		error('pugixml lib not found')
	endif
endif

if get_option('MOD_MEMORY') == true
	conf_data.set('INC_MEMORYCACHE', 1)
endif

if get_option('WITH_PICOEV') == true
	conf_data.set('USE_PICOEV', 1)
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
	conf_data.set('HAVE_TCP_QUICKACK', 1)
endif

if cc.compiles('#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_DEFER_ACCEPT; }')
	conf_data.set('HAVE_TCP_DEFER_ACCEPT', 1)
endif

if cc.compiles('#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_FASTOPEN; }')
	conf_data.set('HAVE_TCP_FASTOPEN', 1)
endif

if cc.compiles('#include <sys/socket.h>\nint test() { return SO_REUSEPORT; }')
	conf_data.set('HAVE_SO_REUSEPORT', 1)
endif

if cc.compiles('#include <sys/socket.h>\nint test() { return SO_REUSEADDR; }')
	conf_data.set('HAVE_SO_REUSEADDR', 1)
endif

if cc.compiles('#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\n#include <linux/bpf.h>\n#include <linux/filter.h>\n#include <sys/sysinfo.h>\nint main() { return SO_ATTACH_REUSEPORT_CBPF; }')
	conf_data.set('HAVE_SO_ATTACH_REUSEPORT_CBPF', 1)
endif

if cc.has_header('sys/sysinfo.h')
	conf_data.set('HAVE_SYSINFO', 1)
endif

if cc.has_header('liburing', args : '-I/usr/local/include')
	conf_data.set('USE_IO_URING', 1)
	conf_data.set('USE_PICOEV', 0)
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

if cc.has_header('sys/sendfile.h')
	conf_data.set('IS_SENDFILE', 1)
endif

#message(conf_data.keys())

configure_file(input : 'src/modules/common/AppDefines.meson.h.in', output : 'AppDefines.h', configuration : conf_data)

conf_file_path = meson.current_build_dir() + '/AppDefines.h'
run_command('mv', conf_file_path, 'src/modules/common/', check: true)
run_command('chmod', '+x', 'meson_grabber.sh', check: true)
run_command('chmod', '+x', 'meson_postinstall.sh', check: true)

meson_grabber = meson.source_root() + '/meson_grabber.sh'
meson_post_install = meson.source_root() + '/meson_postinstall.sh'

bin_dir = meson.current_source_dir()+'/'+meson.project_name()+'-'+meson.project_version()+'-bin/'
run_command('rm', '-rf', bin_dir, check: true)
run_command('mkdir', bin_dir, check: true)
run_command('mkdir', bin_dir+'web', check: true)

subdir('src/modules')
subdir('src/framework')
subdir('tests')

#Add all web application directories here
subdir('web/default')
subdir('web/flexApp')
subdir('web/markers')
subdir('web/oauthApp')
subdir('web/peer-server')
subdir('web/te-benchmark')
subdir('web/t1')
if get_option('MOD_SDORM_MONGO') == true
	subdir('web/t2')
endif
if get_option('MOD_SDORM_SQL') == true
	subdir('web/t3')
	subdir('web/t4')
	subdir('web/t5')
endif

emb_includes = ['src/modules/common','src/modules/cache','src/modules/cache/memory','src/modules/cache/redis',
	'src/modules/cache/memcached','src/modules/client-util','src/modules/http','src/modules/http/http11','src/modules/http/http20',
	'src/modules/ioc','src/modules/jobs','src/modules/reflection','src/modules/sdorm','src/modules/sdorm/sql','src/modules/sdorm/sql/libpq',
	'src/modules/client-util/ssl','src/modules/sdorm/mongo','src/modules/sdorm/mongo/raw','src/modules/search','src/modules/serialization',
	'src/modules/serialization/xml','src/modules/serialization/json','src/modules/server-util','src/modules/ssl','src/modules/threads',
	'src/framework','src/modules/search/solr','src/modules/search/elasticsearch']
	
foreach inc: emb_includes
	c = run_command(meson_grabber, inc, 'all', '*.h', check: true)
	incs = c.stdout().strip().split('\n')
	install_headers(incs, install_dir: bin_dir+'include')
endforeach

emb_includes += [global_includes]

executable('ffead-cpp', 'src/server/embedded/CHServer.cpp', include_directories: emb_includes, dependencies: global_libs, 
	link_with: [libffeadmodules, libffeadframework], install: true, install_dir: bin_dir)

install_subdir('resources', install_dir: bin_dir)
install_subdir('public', install_dir: bin_dir)
install_subdir('rtdcf', install_dir: bin_dir)
#install_subdir('web', install_dir: bin_dir)
install_subdir('web/default', install_dir: bin_dir+'web')
install_subdir('web/flexApp', install_dir: bin_dir+'web')
install_subdir('web/markers', install_dir: bin_dir+'web')
install_subdir('web/oauthApp', install_dir: bin_dir+'web')
install_subdir('web/peer-server', install_dir: bin_dir+'web')
install_subdir('web/te-benchmark', install_dir: bin_dir+'web')
install_subdir('web/t1', install_dir: bin_dir+'web')
if get_option('MOD_SDORM_MONGO') == true
	install_subdir('web/t2', install_dir: bin_dir+'web')
endif
if get_option('MOD_SDORM_SQL') == true
	install_subdir('web/t3', install_dir: bin_dir+'web')
	install_subdir('web/t4', install_dir: bin_dir+'web')
	install_subdir('web/t5', install_dir: bin_dir+'web')
endif

meson.add_install_script(meson_post_install, bin_dir, meson.source_root())
