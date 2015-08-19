AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../../../include" -I"../../include"

prefix=${abs_builddir}

lib_LTLIBRARIES = libmarkers.la
libmarkers_la_SOURCES = ../MarkerController.cpp \
				../MarkerDview.cpp \
				../MarkerExpose.cpp \
				../MarkerTest.cpp \
				../MarkerDefTemp.cpp \
				../MarkerTesting.cpp \
				../MarkerIOFilter.cpp \
				../MarkerRestController.cpp \
				../MarkerSecurityProvider.cpp
				
libmarkers_la_LDFLAGS = -no-undefined 
libmarkers_la_LIBADD = -L"../../../../src/autotools/.libs" -lffead_common -lffead_framework

#dist_noinst_SCRIPTS = autogen.sh