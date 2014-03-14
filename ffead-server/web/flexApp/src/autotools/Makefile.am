AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../include"

prefix=${abs_builddir}

lib_LTLIBRARIES = libflexApp.la
libflexApp_la_SOURCES = ../FlexAppController.cpp
libflexApp_la_LDFLAGS = -shared -no-undefined -L"../../../../src/autotools/.libs" -lweblib

#dist_noinst_SCRIPTS = autogen.sh