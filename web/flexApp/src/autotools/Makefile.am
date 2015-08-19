AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../../../include" -I"../../include"

prefix=${abs_builddir}

lib_LTLIBRARIES = libflexApp.la
libflexApp_la_SOURCES = ../FlexAppController.cpp
libflexApp_la_LDFLAGS = -no-undefined
libflexApp_la_LIBADD = -L"../../../../src/autotools/.libs" -lffead_common -lffead_framework

#dist_noinst_SCRIPTS = autogen.sh