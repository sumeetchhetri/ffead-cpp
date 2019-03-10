AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../../../include" -I"../../include"

packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
fprefix=../../../../${packageIdentifier}
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
libmarkers_la_LIBADD = -L"${fprefix}/lib" -lffead-modules -lffead-framework

#dist_noinst_SCRIPTS = autogen.sh