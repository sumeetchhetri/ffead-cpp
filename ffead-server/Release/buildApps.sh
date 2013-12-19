#!/bin/sh

ENVVARS_FILE=envvars.mk

IS_OS_CYGWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"cygwin") != 0 {print "cygwin"}'`
IS_OS_BSD=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"bsd") != 0 {print "bsd"}'`
IS_OS_SOLARIS=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"sunos") != 0 {print "sunos"}'`
IS_OS_LINUX=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"linux") != 0 {print "linux"}'`

MAKE_EXE=make
RES_LIB_EXT=so

if [ "$IS_OS_BSD" != "" ]; then
MAKE_EXE=gmake
elif [ "$IS_OS_SOLARIS" != "" ]; then
MAKE_EXE=gmake
elif [ "$IS_OS_CYGWIN" != "" ]; then
RES_LIB_EXT=a
fi

cd ../tests
$MAKE_EXE clean all
mkdir ../Release/ffead-server/tests
cp -f tests test.csv testValues.prop runTests.sh security.prop *.pem ../Release/ffead-server/tests
cd ../web/default/src/Release
$MAKE_EXE clean all
cp -f libdefault.$RES_LIB_EXT ../../../../Release/ffead-server/lib
cd ../../../flexApp/src/Release
$MAKE_EXE clean all
cp -f libflexApp.$RES_LIB_EXT ../../../../Release/ffead-server/lib
cd ../../../oauthApp/src/Release
$MAKE_EXE clean all
cp -f liboauthApp.$RES_LIB_EXT ../../../../Release/ffead-server/lib