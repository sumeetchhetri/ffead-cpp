#!/bin/sh

FFEAD_CPPPTH=$1
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}

IS_OS_MINGW=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"mingw") != 0 {print "mingw"}'`

cd $FFEAD_CPP_PATH/rtdcf/autotools
if [ "$IS_OS_MINGW" != "" ]; then
	mingw32-make libdinter.la
else
	make libdinter.la
fi
cp -f .libs/*dinter* $FFEAD_CPP_PATH/lib/