#!/bin/sh

export MALLOC_CHECK_=0
IS_OS_DARWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"darwin") != 0 {print "darwin"}'`
IS_BSD=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"bsd") != 0 {print "bsd"}'`
if [ "$IS_OS_DARWIN" != "" ]; then
	#alias nproc="sysctl -n hw.ncpu"
	export FFEAD_CPP_PATH=`cd "$(dirname server.sh)" && ABSPATH=$(pwd) && cd -`
elif [ "$IS_BSD" != "" ]; then
	#alias nproc="sysctl -n hw.ncpu"
	export FFEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
else
	export FFEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
fi

export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
echo $FFEAD_CPP_PATH
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
if [ "$IS_OS_DARWIN" != "" ]; then
	export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LD_LIBRARY_PATH
	export DYLD_FALLBACK_LIBRARY_PATH=$DYLD_LIBRARY_PATH
fi
export PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$PATH
echo $PATH
rm -f $FFEAD_CPP_PATH/rtdcf/*.d $FFEAD_CPP_PATH/rtdcf/*.o 
rm -f $FFEAD_CPP_PATH/*.cntrl
rm -f $FFEAD_CPP_PATH/tmp/*.sess
if [ ! -d tmp ]; then
mkdir tmp
fi
chmod 700 $FFEAD_CPP_PATH/ffead-cpp
chmod 700 $FFEAD_CPP_PATH/resources/*.sh
chmod 700 $FFEAD_CPP_PATH/tests/*
chmod 700 $FFEAD_CPP_PATH/rtdcf/*
chmod 700 $FFEAD_CPP_PATH/rtdcf/autotools/*
#/usr/sbin/setenforce 0

#if the event engine runs without a worker pool, then spwan "nproc" number of server processes
SINGLE_PROCESS=`grep "EVH_SINGLE=true" $FFEAD_CPP_PATH/resources/server.prop |wc -l`
if [ $SINGLE_PROCESS -ge 1 ]
then
	for i in $(seq 0 $(($(nproc --all)-1))); do
	  taskset -c $i ./ffead-cpp $FFEAD_CPP_PATH > ffead.$i.log 2>&1 &
	done
else
	./ffead-cpp $FFEAD_CPP_PATH > ffead.log 2>&1
fi
