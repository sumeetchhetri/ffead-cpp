
module_includes = ['/usr/local/include','../modules/common','../modules/cache','../modules/cache/memory','../modules/cache/redis',
	'../modules/cache/memcached','../modules/client-util','../modules/http','../modules/http/http11','../modules/http/http20',
	'../modules/ioc','../modules/jobs','../modules/reflection','../modules/sdorm','../modules/sdorm/sql','../modules/sdorm/sql/libpq',
	'../modules/client-util/ssl','../modules/sdorm/mongo','../modules/sdorm/mongo/raw','../modules/search','../modules/serialization',
	'../modules/serialization/xml','../modules/serialization/json','../modules/server-util','../modules/ssl','../modules/threads',
	'../modules/sdorm/scylla/raw','../modules/search/solr','../modules/search/elasticsearch']
module_includes += global_includes
module_libs = [global_libs]

c = run_command(meson_grabber, '.', check: true)
module_sources = c.stdout().strip().split('\n')

libffeadframework = shared_library('ffead-framework', module_sources, include_directories: module_includes, dependencies: module_libs, 
	link_with: libffeadmodules, install: true, install_dir: bin_dir+'/lib')