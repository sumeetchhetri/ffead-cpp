AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../include"

prefix=${abs_builddir}

lib_LTLIBRARIES = liboauthApp.la
liboauthApp_la_SOURCES = ../OAUTHController.cpp \
		../OAUTH2Controller.cpp
liboauthApp_la_LDFLAGS = -shared -no-undefined -L"../../../../src/autotools/.libs" -lweblib

#dist_noinst_SCRIPTS = autogen.sh