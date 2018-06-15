
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -I"../../modules/common"

prefix=${abs_builddir}

bin_PROGRAMS = CHS
CHS_SOURCES = ../../server/CHServer.cpp
CHS_LDFLAGS= -no-undefined -L../libs/.libs/
CHS_LDADD = -lffead_common -lffead_framework @ADD_LIBDL@

