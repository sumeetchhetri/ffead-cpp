AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../include"

prefix=${abs_builddir}

lib_LTLIBRARIES = libdefault.la
libdefault_la_SOURCES = ../DefTemp.cpp \
				../DefaultController.cpp \
				../Dview.cpp \
				../Expose.cpp \
				../Service1.cpp \
				../Service2.cpp \
				../Tempo.cpp \
				../Test.cpp \
				../Test2.cpp \
				../Test3.cpp \
				../Test4.cpp \
				../TestMany.cpp \
				../Testing.cpp \
				../DefaultOAUTHController.cpp \
				../DefaultIOFilter.cpp \
				../TestPage.cpp \
				../TestForm.cpp \
				../DefaultRestController.cpp \
				../TestFormController.cpp \
				../TestingWS.cpp \
				../TestObject.cpp \
				../TestObject1.cpp \
				../TestCronBasedJob.cpp
libdefault_la_LDFLAGS = -shared -no-undefined -L"../../../../src/autotools/.libs" -lweblib

#dist_noinst_SCRIPTS = autogen.sh