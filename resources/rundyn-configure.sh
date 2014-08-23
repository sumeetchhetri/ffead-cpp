#!/bin/sh

if [ "$#" -gt 1 -a -n "$2" ]
then
	FFEAD_CPPPTH=$2
	export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
elif [ "$#" -gt 0 -a -n "$1" ]
then
	FFEAD_CPPPTH=$1
	export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
fi

export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:$LD_LIBRARY_PATH
#echo $LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
#echo $PATH

cd $FFEAD_CPP_PATH/rtdcf/autotools
#rm -f $FFEAD_CPP_PATH/lib/*inter.*
#rm -f Makefile

#if [ -f configure ]; then
#	echo 'Configure file already exists'
#else
#	ls
#	echo $PATH
#	./autogen.sh
#	echo $PATH
#fi

./configure
