AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APR_INCLUDEDIR=`$(APXS) -q APR_INCLUDEDIR`
AM_CPPFLAGS=-I"../../include" -I"$(APXS_INCLUDEDIR)" -I"$(APR_INCLUDEDIR)" -fpermissive -w

prefix=${abs_builddir}

noinst_LTLIBRARIES = mod_ffeadcpp.la
mod_ffeadcpp_la_SOURCES = ../mod_ffeadcpp.cpp
mod_ffeadcpp_la_LIBADD = -L"../../src/autotools/.libs" -lffead_common -lffead_framework -ldl -lcrypto -lssl @LIB_TYPE@apr-1 @LIB_TYPE@aprutil-1 @APACHE_HTTPD_LIB@
mod_ffeadcpp_la_LDFLAGS = -avoid-version -shared -module -rpath ${prefix}/lib -no-undefined 


# install the shared object file into Apache 
install-data-hook:
	cp ${prefix}/.libs/mod_ffeadcpp.@LIBRARY_EXTENSION@ ${prefix}/.libs/mod_ffeadcpplib.so
	$(APXS) -i -n 'ffead_cpp_module' ${prefix}/.libs/mod_ffeadcpplib.so

#dist_noinst_SCRIPTS = autogen.sh