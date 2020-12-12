#!/bin/sh

FFEAD_CPPPTH=$1
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}

IS_OS_MINGW=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"mingw") != 0 {print "mingw"}'`

cd $FFEAD_CPP_PATH/rtdcf/autotools
if [ "$IS_OS_MINGW" != "" ]; then
	mingw32-make clean
	mingw32-make all
else
	make clean
	make all
fi
cp -f .libs/*inter* $FFEAD_CPP_PATH/lib/