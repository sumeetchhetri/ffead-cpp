
module_sources = []
module_includes = ['/usr/local/include','common','cache','cache/memory','cache/redis',
	'cache/memcached','client-util','http','http/http11','http/http20', 'common/picoev',
	'ioc','jobs','reflection','sdorm','sdorm/sql','sdorm/sql/libpq','../framework', 'search/elasticsearch',
	'client-util/ssl','sdorm/mongo','sdorm/mongo/raw','search','serialization', 'search/solr',
	'serialization/xml','serialization/json','server-util','ssl','threads', 'sdorm/scylla/raw']
module_includes += global_includes

c = run_command(meson_grabber, 'common', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'client-util', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'server-util', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'http', 'all', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'ioc', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'reflection', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'threads', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if libssl.found()
	c = run_command(meson_grabber, 'ssl', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
	
	c = run_command(meson_grabber, 'client-util/ssl', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'cache', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_MEMORY') == true
	c = run_command(meson_grabber, 'cache/memory', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_REDIS') == true
	c = run_command(meson_grabber, 'cache/redis', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_MEMCACHED') == true
	c = run_command(meson_grabber, 'cache/memcached', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'search', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_SOLR') == true
	c = run_command(meson_grabber, 'search/solr', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_ELASTIC') == true
	c = run_command(meson_grabber, 'search/elasticsearch', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'sdorm', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_SDORM_SQL') == true
	c = run_command(meson_grabber, 'sdorm/sql', 'all', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_SDORM_MONGO') == true
	c = run_command(meson_grabber, 'sdorm/mongo', 'all', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('MOD_SDORM_SCYLLA') == true
	c = run_command(meson_grabber, 'sdorm/scylla', 'all', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

c = run_command(meson_grabber, 'serialization', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'serialization/xml', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

c = run_command(meson_grabber, 'serialization/json', check: true)
tmp = c.stdout().strip().split('\n')
module_sources += tmp

if get_option('MOD_SER_BIN') == true
	c = run_command(meson_grabber, 'serialization/binary', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
	module_includes += ['serialization/binary']
endif

if get_option('MOD_JOBS') == true
	c = run_command(meson_grabber, 'jobs', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

if get_option('WITH_PICOEV') == true
	c = run_command(meson_grabber, 'common/picoev', 'all', check: true)
	tmp = c.stdout().strip().split('\n')
	module_sources += tmp
endif

libffeadmodules = shared_library('ffead-modules', module_sources, include_directories: module_includes, dependencies: global_libs, install: true, install_dir: bin_dir+'/lib')
