AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../include" ${INTER_DINTER_INCLUDES}

prefix=${abs_builddir}

lib_LTLIBRARIES = libinter.la
libinter_la_SOURCES = ${INTER_SOURCES}
libinter_la_LDFLAGS = -shared -no-undefined -module -L"../../lib" -lweblib ${INTER_DINTER_LIBRARIES}

if BUILT_WITH_CONFGURE
lib_LTLIBRARIES += libdinter.la
libdinter_la_SOURCES = ${DINTER_SOURCES}
libdinter_la_LDFLAGS = -shared -no-undefined -module -L"../../lib" -lweblib ${INTER_DINTER_LIBRARIES}
endif

dist_noinst_SCRIPTS = autogen.sh