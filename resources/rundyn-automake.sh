#!/bin/sh

FFEAD_CPPPTH=$1
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${FFEAD_CPPPTH}/lib:/usr/local/lib

IS_OS_MINGW=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"mingw") != 0 {print "mingw"}'`

cd $FFEAD_CPP_PATH/rtdcf/
if [ -f "/usr/bin/ninja" ] || [ -f "/usr/local/bin/ninja" ] || [ -f "/mingw64/bin/ninja" ]
then
	rm -rf CMakeFiles CMakeCache.txt
	if [ "$IS_OS_MINGW" != "" ]; then
		cmake -G "MinGW Makefiles" -GNinja .
	else
		cmake -GNinja .
	fi
	ninja clean
	ninja
else
	rm -rf CMakeFiles CMakeCache.txt
	cmake .
	if [ "$IS_OS_MINGW" != "" ]; then
		mingw32-make clean
		mingw32-make -j 4
	else
		make clean
		make -j 4
	fi
fi
cp -f *inter* $FFEAD_CPP_PATH/lib/