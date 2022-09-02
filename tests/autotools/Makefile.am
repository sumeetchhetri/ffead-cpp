AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CPPFLAGS=-I"../../src/modules/common" \
			-I"../../src/modules/client-util" \
			-I"../../src/modules/client-util/ssl" \
			-I"../../src/modules/server-util" \
			-I"../../src/modules/http" \
			-I"../../src/modules/threads" \
			-I"../../src/modules/reflection" \
			-I"../../src/modules/ssl" \
			-I"../../src/modules/serialization" \
			-I"../../src/modules/serialization/json"
			

prefix=${abs_builddir}

noinst_PROGRAMS = tests
tests_SOURCES = ../FfeadServerTestSuite.cpp
tests_LDFLAGS= -no-undefined
tests_LDADD = -L"../../src/autotools/libs/.libs" -lffead-modules
