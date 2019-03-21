#!/bin/bash

FFEAD_CPPPTH=$1
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}

DEBG="TO_REPLACE_DEBUG"
PCFL="TO_REPLACE_CPPFLAGS"
PLDFL="TO_REPLACE_LDFLAGS"
PACFL="TO_REPLACE_AM_CXXFLAGS"

PACFL=${PACFL//-std=gnu++11/}
PACFL=${PACFL//-std=gnu++0x/}
PACFL=${PACFL//-std=c++11/}
PACFL=${PACFL//+std=c++11/}
PACFL=${PACFL//-std=c++0x/}

if [ "$DEBG" = "yes" ]; then
	DEBG="--enable-debug=yes"
else
	DEBG=""
fi

#if [ "$#" -gt 1 -a -n "$2" ]
#then
#	DEBG=$2
#elif [ "$#" -gt 0 -a -n "$1" ]
#then
#	DEBG=
#fi

export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
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

./configure $DEBG CPPFLAGS="$CPPFLAGS $PCFL $PACFL" LDFLAGS="$LDFLAGS $PLDFL"
