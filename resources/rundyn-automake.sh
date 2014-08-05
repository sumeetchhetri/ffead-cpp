#!/bin/sh

if [ "$#" -gt 0 -a -n "$1" ]
then
	FFEAD_CPPPTH=$1
	export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
fi

cd $FFEAD_CPP_PATH/rtdcf/autotools
make clean
make all
cp -f .libs/*inter* $FFEAD_CPP_PATH/lib/