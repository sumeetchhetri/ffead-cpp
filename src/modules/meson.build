
module_sources = []
module_includes = ['/usr/local/include','common','cache','cache/memory','cache/redis',
	'cache/memcached','client-util','http','http/http11','http/http20',
	'ioc','jobs','reflection','sdorm','sdorm/sql','sdorm/sql/libpq','../framework',
	'client-util/ssl','sdorm/mongo','sdorm/mongo/raw','search','serialization',
	'serialization/xml','serialization/json','server-util','ssl','threads']
module_includes += global_includes

c = run_command(meson_grabber, 'common')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'client-util')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'server-util')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'http', 'all')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'ioc')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'reflection')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'threads')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_JOBS') == true
	c = run_command(meson_grabber, 'ssl')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
	
	c = run_command(meson_grabber, 'client-util/ssl')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'cache')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'cache/memory')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_REDIS') == true
	c = run_command(meson_grabber, 'cache/redis')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_MEMCACHED') == true
	c = run_command(meson_grabber, 'cache/memcached')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'sdorm')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_SDORM_SQL') == true
	c = run_command(meson_grabber, 'sdorm/sql', 'all')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_SDORM_MONGO') == true
	c = run_command(meson_grabber, 'sdorm/mongo', 'all')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'serialization')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'serialization/xml')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'serialization/json')
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_SER_BIN') == true
	c = run_command(meson_grabber, 'serialization/binary')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
	module_includes += ['serialization/binary']
endif

if get_option('MOD_JOBS') == true
	c = run_command(meson_grabber, 'jobs')
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

libffeadmodules = shared_library('ffead-modules', module_sources, include_directories: module_includes, dependencies: global_libs, install: true, install_dir: bin_dir+'/lib')
