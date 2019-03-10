
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -I"../../modules/common"

prefix=${abs_builddir}

if MOD_ALL
AM_CPPFLAGS +=  -I"../../framework" \
			    -I"../../modules/cache" \
				-I"../../modules/cache/memcached" \
				-I"../../modules/cache/memory" \
				-I"../../modules/cache/redis" \
				-I"../../modules/client-util" \
				-I"../../modules/component" \
				-I"../../modules/distocache" \
				-I"../../modules/http" \
				-I"../../modules/http/http11" \
				-I"../../modules/http/http20" \
				-I"../../modules/interpreter" \
				-I"../../modules/ioc" \
				-I"../../modules/jobs" \
				-I"../../modules/methinvoker" \
				-I"../../modules/msghandler" \
				-I"../../modules/reflection" \
				-I"../../modules/sdorm" \
				-I"../../modules/sdorm/gtm" \
				-I"../../modules/sdorm/mongo" \
				-I"../../modules/sdorm/sql" \
				-I"../../modules/serialization" \
				-I"../../modules/serialization/binary" \
				-I"../../modules/serialization/json" \
				-I"../../modules/serialization/xml" \
				-I"../../modules/server-util" \
				-I"../../modules/ssl" \
				-I"../../modules/threads"
else
if MOD_DEFAULT
AM_CPPFLAGS +=  -I"../../framework" \
	    -I"../../modules/cache" \
		-I"../../modules/cache/memory" \
		-I"../../modules/client-util" \
		-I"../../modules/http" \
		-I"../../modules/http/http11" \
		-I"../../modules/http/http20" \
		-I"../../modules/interpreter" \
		-I"../../modules/ioc" \
		-I"../../modules/jobs" \
		-I"../../modules/reflection" \
		-I"../../modules/sdorm" \
		-I"../../modules/serialization" \
		-I"../../modules/serialization/binary" \
		-I"../../modules/serialization/json" \
		-I"../../modules/serialization/xml" \
		-I"../../modules/server-util" \
		-I"../../modules/ssl" \
		-I"../../modules/threads"
else
if MOD_COMPONENT
AM_CPPFLAGS += \
				-I"../../modules/component"
endif
if MOD_METHINVOKER
AM_CPPFLAGS += \
				-I"../../modules/serialization/xml" \
				-I"../../modules/methinvoker"
endif
if MOD_MSGHANDLER
AM_CPPFLAGS += \
				-I"../../modules/serialization/binary" \
				-I"../../modules/msghandler"
endif
if MOD_SDORM_SQL
AM_CPPFLAGS += \
				-I"../../modules/sdorm/sql"
endif
if MOD_SDORM_MONGO
AM_CPPFLAGS += \
				-I"../../modules/sdorm/mongo"
endif
if MOD_DISTOCACHE
AM_CPPFLAGS += \
				-I"../../modules/serialization/binary" \
				-I"../../modules/distocache"
endif
if MOD_XMLSERIALIZE
AM_CPPFLAGS += \
				-I"../../modules/serialization/xml"
endif
if MOD_BINSERIALIZE
AM_CPPFLAGS += \
				-I"../../modules/serialization/binary"
endif
if MOD_INTERPRETER
AM_CPPFLAGS += \
				-I"../../modules/interpreter"
endif
if MOD_JOBS
AM_CPPFLAGS += \
				-I"../../modules/jobs"
endif
if MOD_MEMCACHED
AM_CPPFLAGS += \
				-I"../../modules/cache/memcached"
endif
if MOD_REDISCACHE
AM_CPPFLAGS += \
				-I"../../modules/cache/redis"
endif
if MOD_GTM
AM_CPPFLAGS += \
				-I"../../modules/sdorm/gtm"
endif
endif
endif

if MOD_SDORM_SQL
AM_CPPFLAGS += \
				-I"../../modules/sdorm/sql"
endif

if MOD_SDORM_MONGO
AM_CPPFLAGS += \
				-I"../../modules/sdorm/mongo"
endif
if MOD_MEMCACHED
AM_CPPFLAGS += \
				-I"../../modules/cache/memcached"
endif
if MOD_REDISCACHE
AM_CPPFLAGS += \
				-I"../../modules/cache/redis"
endif
if MOD_GTM
AM_CPPFLAGS += \
				-I"../../modules/sdorm/gtm"
endif

bin_PROGRAMS = ffead-cpp
ffead_cpp_SOURCES = ../../server/CHServer.cpp
ffead_cpp_LDFLAGS= -no-undefined -L../libs/.libs/
ffead_cpp_LDADD = -lffead-modules -lffead-framework @ADD_LIBDL@

