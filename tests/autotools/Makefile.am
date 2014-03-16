AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../include"

prefix=${abs_builddir}

noinst_PROGRAMS = tests
tests_SOURCES = ../FfeadServerTestSuite.cpp
tests_LDADD = -L"../../src/autotools/" -lweblib
