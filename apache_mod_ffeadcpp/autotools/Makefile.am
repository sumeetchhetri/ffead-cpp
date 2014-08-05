AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
AM_CPPFLAGS=-I"../../include" -I"$(APXS_INCLUDEDIR)" -I"/usr/include/apr-1" -I"/usr/include/apr-1.0" -fpermissive -w

prefix=${abs_builddir}

noinst_LTLIBRARIES = mod_ffeadcpp.la
mod_ffeadcpp_la_SOURCES = ../mod_ffeadcpp.cpp
mod_ffeadcpp_la_LDFLAGS = -shared -module -rpath ${prefix}/lib -no-undefined -L"../../src/autotools/.libs" -lweblib -ldl -lcrypto -lssl -lapr-1 -laprutil-1 @APACHE_HTTPD_LIB@


# install the shared object file into Apache 
install-data-hook:
	mv ${prefix}/.libs/mod_ffeadcpp.so.0.0.0 ${prefix}/.libs/mod_ffeadcpplib.so
	$(APXS) -i -n 'ffead_cpp_module' ${prefix}/.libs/mod_ffeadcpplib.so

#dist_noinst_SCRIPTS = autogen.sh