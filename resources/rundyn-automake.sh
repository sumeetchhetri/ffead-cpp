#!/bin/sh

FFEAD_CPPPTH=$1
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${FFEAD_CPPPTH}/lib:/usr/local/lib

cd $FFEAD_CPP_PATH/rtdcf/
if [ -f "/usr/bin/ninja" ] || [ -f "/usr/local/bin/ninja" ]
then
	cmake -GNinja .
	ninja clean
	ninja
else
	cmake .
	make clean
	make -j 4
fi
cp -f *inter* $FFEAD_CPP_PATH/lib/