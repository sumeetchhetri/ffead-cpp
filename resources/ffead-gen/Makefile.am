AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../../../include" -I"../../include"

packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
fprefix=../../../../${packageIdentifier}
prefix=${abs_builddir}

lib_LTLIBRARIES = lib@APP_NAME@.la
lib@APP_NAME@_la_SOURCES = @APP_CLASSES@

lib@APP_NAME@_la_LDFLAGS = -no-undefined 
lib@APP_NAME@_la_LIBADD = -L"${fprefix}/lib" -lffead_common -lffead_framework @FAPP_LIBS@

#dist_noinst_SCRIPTS = autogen.sh