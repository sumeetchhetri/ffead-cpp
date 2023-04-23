function do_setup() {
	LOG_MODE=1
	BUILD_PROJ_NAME=ffead-cpp
	BUILD_SYS=bazel
	BUILD_PLATFORM=c_cpp
	DEFS_FILE="src/modules/common/AppDefines.h"
}
function do_config() {
	configs+=$'MOD_DEBUG|Enable Debug mode|0\n'
	configs+=$'MOD_MEMORY|Enable Memory Cache module|1\n'
	configs+=$'MOD_MEMCACHED|Enable Memcached Cache module|1\n'
	configs+=$'MOD_REDIS|Enable Redis Cache module|1\n'
	configs+=$'MOD_SDORM_SQL|Enable SQL Sdorm module|1\n'
	configs+=$'MOD_SDORM_MONGO|Enable Mongo Sdorm module|1\n'
	configs+=$'MOD_SDORM_SCYLLA|Enable Scylla Sdorm module|0\n'
	configs+=$'MOD_SOLR|Enable SOLR Search module|1\n'
	configs+=$'MOD_ELASTIC|Enable Elasticsearch Search module|1\n'
	configs+=$'MOD_SER_BIN|Enable Binary Serialization module|1\n'
	configs+=$'MOD_JOBS|Enable Jobs module|1\n'
	configs+=$'SRV_ALL|Build All inbuilt HTTP Server Engines|1\n'
	configs+=$'SRV_EMB|Embedded Http Server Engine|1\n'
	configs+=$'WITH_RAPIDJSON|Use rapidjson for json handling|1\n'
	configs+=$'WITH_PUGIXML|Use pugixml for xml handling|1\n'
	configs+=$'WITH_PICOEV|Enable picoev engine|0\n'
	echo "$configs"
}
function do_start() {
	set_out "shellb_out"
	set_install "ffead-cpp-6.0-bin"
	finc_cpp_compiler "Please install a c++ compiler to proceed"
	cpp_flags "-std=c++17 -Wall"
	if is_config "MOD_DEBUG"; then
		cpp_flags "-g"
	fi
	l_flags ""
	add_lib_path "/usr/local/opt/openssl/lib" "/usr/local/lib"
	add_inc_path "/usr/local/opt/openssl/include" "/usr/local/include" "/usr/include/libmongoc-1.0"
	add_inc_path "/usr/include/libbson-1.0" "/usr/local/include/libmongoc-1.0" "/usr/local/include/libbson-1.0"
	add_inc_path "/usr/include/postgresql" "/usr/include/pgsql" "/usr/local/include/postgresql" "/usr/local/include/pgsql"
	add_def "OS_${OS_NAME}" "BUILD_BAZEL" "INC_WEBSVC" "INC_TPE" "INC_DVIEW" "INC_DCP" "INC_XMLSER"
	if [ "$OS_DARWIN" = "1" ]; then add_def "APPLE"; fi
	c_hdr "execinfo.h" "HAVE_EXECINFOINC"
	c_hdr "sys/sendfile.h" "IS_SENDFILE"
	c_hdr "sys/sysinfo.h" "HAVE_SYSINFO"
	if ! is_config "WITH_PICOEV"; then
		set_exclude_src "src/modules/common/picoev"
	else
		if ! c_hdr "liburing" "HAVE_LIBURING"; then
			add_def "USE_PICOEV"
		fi
	fi
	if c_hdr "liburing" "HAVE_LIBURING"; then
		add_def "USE_IO_URING"
	elif c_hdr "sys/epoll.h" "HAVE_EPOLL"; then
		add_def "USE_EPOLL"
	elif c_hdr "sys/event.h" "HAVE_KQUEUE"; then
		add_def "USE_KQUEUE"
	elif c_hdr "port.h" "HAVE_EVPORT"; then
		add_def "USE_EVPORT"
	elif c_hdr "sys/devpoll.h" "HAVE_DEVPOLL"; then
		add_def "USE_DEVPOLL"
	elif c_hdr "sys/poll.h" "HAVE_POLL"; then
		add_def "USE_POLL"
	elif c_hdr "sys/select.h" "HAVE_SELECT"; then
		add_def "USE_SELECT"
	fi
	c_code "#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_QUICKACK; }" "HAVE_TCP_QUICKACK"
	c_code "#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_DEFER_ACCEPT; }" "HAVE_TCP_DEFER_ACCEPT"
	c_code "#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\nint test() { return TCP_FASTOPEN; }" "HAVE_TCP_FASTOPEN"
	c_code "#include <sys/socket.h>\nint test() { return SO_REUSEPORT; }" "HAVE_SO_REUSEPORT"
	c_code "#include <sys/socket.h>\nint test() { return SO_REUSEADDR; }" "HAVE_SO_REUSEADDR"
	c_code "#include <sys/socket.h>\n#include <netinet/in.h>\n#include <netinet/tcp.h>\n#include <linux/bpf.h>\n#include <linux/filter.h>\n#include <sys/sysinfo.h>\nint main() { return SO_ATTACH_REUSEPORT_CBPF; }" "HAVE_SO_ATTACH_REUSEPORT_CBPF"
	if ! c_hdr "regex.h" "HAVE_REGEX"; then 
		c_hdr_lib "onigposix.h" "onig" "HAVE_ONIG_REGEX,HAVE_ONIG_REGEX_LIB" "regex devel not found"
	fi
	c_lib "z" "" "zlib lib not found"
	c_func "accept4" "HAVE_ACCEPT4"
	c_hdr_lib "uuid/uuid.h" "uuid" "HAVE_UUIDINC" "uuid devel not found"
	c_hdr_lib "curl/curl.h" "curl" "HAVE_CURLINC,HAVE_CURLLIB" "curl devel not found"
	if ! c_hdr_lib "openssl/ssl.h" "ssl" "HAVE_SSLINC,HAVE_SSLLIB" || ! c_lib "crypto" "HAVE_CRYPTOLIB"; then
		set_exclude_src "src/modules/ssl" "src/modules/client-util/ssl"
	fi
	c_hdr_lib "libpq-fe.h" "pq" "HAVE_PQHDR,HAVE_LIBPQ" "libpq devel not found"
	c_func "PQenterBatchMode" "HAVE_LIBPQ_BATCH"
	c_func "PQenterPipelineMode" "HAVE_LIBPQ_PIPELINE"
	c_hdr_lib "sql.h" "odbc" "HAVE_SQLINC,HAVE_ODBCLIB,HAVE_LIBODBC,INC_SDORM,INC_SDORM_SQL" "odbc devel not found"
	c_hdr_lib "mongoc.h" "mongoc-1.0" "HAVE_MONGOINC,HAVE_MONGOCLIB,INC_SDORM_MONGO" "libmongoc devel not found"
	c_hdr_lib "bson.h" "bson-1.0" "HAVE_BSONINC,HAVE_BSONLIB" "libbson devel not found"
	if ! is_config "MOD_SER_BIN"; then
		set_exclude_src "src/modules/serialization/binary"
	fi
	if ! is_config "MOD_MEMORY"; then
		set_exclude_src "src/modules/cache/memory"
	fi
	if ! is_config "MOD_REDIS" || ! c_hdr_lib "hiredis/hiredis.h" "hiredis" "HAVE_REDISINC,HAVE_REDISLIB,INC_REDISCACHE"; then
		set_exclude_src "src/modules/cache/redis"
	fi
	if defined "HAVE_REDISINC"; then
		cpp_hdr_lib "sw/redis++/redis++.h" "redis++" "HAVE_REDIS_CLUSTERINC,HAVE_REDIS_CLUSTERLIB,INC_REDISCACHE" ""
	fi
	if ! is_config "MOD_MEMCACHED" || ! c_hdr_lib "libmemcached/memcached.h" "memcached" "HAVE_MEMCACHEDINC,HAVE_MEMCACHEDLIB,INC_MEMCACHED"; then
		set_exclude_src "src/modules/cache/memcached"
	fi
	if defined "HAVE_MEMCACHEDINC"; then
		c_lib "memcachedutil"
	fi
	is_config "WITH_RAPIDJSON" && cpp_hdr "rapidjson/document.h" "HAVE_RAPID_JSON"
	is_config "WITH_PUGIXML" && cpp_hdr_lib "pugixml.hpp" "pugixml" "HAVE_PUGI_XML"
	if ! is_config "MOD_SDORM_SQL"; then
		set_exclude_src "src/modules/sdorm/sql"
	fi
	if ! is_config "MOD_SDORM_MONGO"; then
		set_exclude_src "src/modules/sdorm/mongo"
	fi
	if ! is_config "MOD_SDORM_SCYLLA" || ! cpp_hdr_lib "cassandra.h" "scylla-cpp-driver" "HAVE_SCYLLAINC,HAVE_SCYLLALIB"; then
		set_exclude_src "src/modules/sdorm/scylla"
	fi
	if ! is_config "MOD_ELASTIC" || ! cpp_hdr_lib "elasticlient/client.h" "elasticlient" "HAVE_ELASTIC,HAVE_ELASTICLIB" || ! cpp_hdr_lib "cpr/response.h" "elasticlient" "HAVE_CPR,HAVE_CPRLIB"; then
		set_exclude_src "src/modules/search/elasticsearch"
	fi
	if ! is_config "MOD_SOLR"; then
		set_exclude_src "src/modules/search/solr"
	else
		add_def "HAVE_SOLR"
	fi
	if ! is_config "MOD_JOBS"; then
		set_exclude_src "src/modules/jobs"
	fi
	set_exclude_src "src/modules/sdorm/gtm"
	if ! defined "OS_MINGW"; then
		set_exclude_src "src/modules/wepoll"
	fi
	add_inc_path "src/framework"
	set_src "src/modules" "shared:ffead-modules"
	set_src "src/framework" "shared:ffead-framework" "ffead-modules"
	set_src "tests" "binary:tests" "ffead-framework,ffead-modules"
	set_src "src/server/embedded" "binary:ffead-cpp" "ffead-framework,ffead-modules"
	
	apps_to_build="ffead-modules,ffead-framework,ffead-cpp,tests,default,flexApp,markers,oauthApp,peer-server,te-benchmark,t1"
	set_inc_src "web/default/include" "web/default/src" "shared:default" "ffead-framework,ffead-modules"
	set_inc_src "web/flexApp/include" "web/flexApp/src" "shared:flexApp" "ffead-framework,ffead-modules"
	set_inc_src "web/markers/include" "web/markers/src" "shared:markers" "ffead-framework,ffead-modules"
	set_inc_src "web/oauthApp/include" "web/oauthApp/src" "shared:oauthApp" "ffead-framework,ffead-modules"
	set_inc_src "web/peer-server/include" "web/peer-server/src" "shared:peer-server" "ffead-framework,ffead-modules"
	set_inc_src "web/te-benchmark/include" "web/te-benchmark/src" "shared:te-benchmark" "ffead-framework,ffead-modules"
	set_inc_src "web/t1/include" "web/t1/src" "shared:t1" "ffead-framework,ffead-modules"
	if is_config "MOD_SDORM_MONGO"; then
		apps_to_build+=",t2"
		set_inc_src "web/t2/include" "web/t2/src" "shared:t2" "ffead-framework,ffead-modules"
	fi
	if is_config "MOD_SDORM_SQL"; then
		apps_to_build+=",t3,t4,t5"
		set_inc_src "web/t3/include" "web/t3/src" "shared:t3" "ffead-framework,ffead-modules"
		set_inc_src "web/t4/include" "web/t4/src" "shared:t4" "ffead-framework,ffead-modules"
		set_inc_src "web/t5/include" "web/t5/src" "shared:t5" "ffead-framework,ffead-modules"
	fi
	templatize "rtdcf/inter-shellb.bazel.buck2.sh.tem" "rtdcf/inter-shellb.sh" "CPPFLAGS,LFLAGS,LIBS,BUILD_SYS"
	do_postbuild "$apps_to_build"
}
function do_install() {
	install_here "." "ffead-cpp" "script@*.sh,*.key,*.pem,*.crt" "resources" "public" "rtdcf"
	install_here "web"
	install_here "web" "web/default" "web/flexApp" "web/markers" "web/oauthApp" "web/peer-server" "web/te-benchmark" "web/t1"
	if is_config "MOD_SDORM_MONGO"; then
		install_here "web" "web/t2"
	fi
	if is_config "MOD_SDORM_SQL"; then
		install_here "web" "web/t3" "web/t4" "web/t5"
	fi
	install_here "lib" "lib*.${SHLIB_EXT}"
	install_here "include" "src/framework@*.h" "src/modules@*.h"
	install_here "logs"
	install_here "tests" "tests" "tests/@*.sh,*.pem,*.csv,*.prop"
}
