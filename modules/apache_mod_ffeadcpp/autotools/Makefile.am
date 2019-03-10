AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APR_INCLUDEDIR=`$(APXS) -q APR_INCLUDEDIR`
AM_CPPFLAGS =  -I"../../../src/framework" \
			    -I"../../../src/modules/cache" \
				-I"../../../src/modules/cache/memcached" \
				-I"../../../src/modules/cache/memory" \
				-I"../../../src/modules/cache/redis" \
				-I"../../../src/modules/client-util" \
				-I"../../../src/modules/component" \
				-I"../../../src/modules/distocache" \
				-I"../../../src/modules/http" \
				-I"../../../src/modules/http/http11" \
				-I"../../../src/modules/http/http20" \
				-I"../../../src/modules/ioc" \
				-I"../../../src/modules/jobs" \
				-I"../../../src/modules/methinvoker" \
				-I"../../../src/modules/msghandler" \
				-I"../../../src/modules/reflection" \
				-I"../../../src/modules/sdorm" \
				-I"../../../src/modules/sdorm/gtm" \
				-I"../../../src/modules/sdorm/mongo" \
				-I"../../../src/modules/sdorm/sql" \
				-I"../../../src/modules/serialization" \
				-I"../../../src/modules/serialization/binary" \
				-I"../../../src/modules/serialization/json" \
				-I"../../../src/modules/serialization/xml" \
				-I"../../../src/modules/server-util" \
				-I"../../../src/modules/ssl" \
				-I"../../../src/modules/threads"
AM_CPPFLAGS += -I"$(APXS_INCLUDEDIR)" -I"$(APR_INCLUDEDIR)" -fpermissive -w

packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
fprefix=../../../${packageIdentifier}
prefix=${abs_builddir}

noinst_LTLIBRARIES = mod_ffeadcpp.la
mod_ffeadcpp_la_SOURCES = ../mod_ffeadcpp.cpp
mod_ffeadcpp_la_LIBADD = -L"${fprefix}/lib" -lffead-modules -lffead-framework -ldl -lcrypto -lssl @LIB_TYPE@apr-1 @LIB_TYPE@aprutil-1 @APACHE_HTTPD_LIB@
mod_ffeadcpp_la_LDFLAGS = -avoid-version -shared -module -rpath ${prefix}/lib -no-undefined 


# install the shared object file into Apache 
install-data-hook:
	cp ${prefix}/.libs/mod_ffeadcpp.@LIBRARY_EXTENSION@ ${prefix}/.libs/mod_ffeadcpplib.so
	$(APXS) -i -n 'ffead_cpp_module' ${prefix}/.libs/mod_ffeadcpplib.so

#dist_noinst_SCRIPTS = autogen.sh