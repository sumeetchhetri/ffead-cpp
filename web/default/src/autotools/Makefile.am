AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS=-I"../../../../include" -I"../../include"

packageIdentifier=${PACKAGE_NAME}-${PACKAGE_VERSION}-bin
distdir=${PACKAGE_NAME}-${PACKAGE_VERSION}-src
fprefix=../../../../${packageIdentifier}
prefix=${abs_builddir}

lib_LTLIBRARIES = libdefault.la
libdefault_la_SOURCES = ../DefTemp.cpp \
				../DefaultController.cpp \
				../Dview.cpp \
				../Expose.cpp \
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
				../TestCronBasedJob.cpp \
				../DefaultWebSocketHandler.cpp \
				../GtmAccesController.cpp

libdefault_la_LDFLAGS = -no-undefined 
libdefault_la_LIBADD = -L"${fprefix}/lib" -lffead_common -lffead_framework

#dist_noinst_SCRIPTS = autogen.sh