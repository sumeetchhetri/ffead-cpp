#!/bin/sh

export MALLOC_CHECK_=0
IS_OS_DARWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"darwin") != 0 {print "darwin"}'`
IS_BSD=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"bsd") != 0 {print "bsd"}'`
if [ "$IS_OS_DARWIN" != "" ]; then
	alias nproc="sysctl -n hw.ncpu"
	export FFEAD_CPP_PATH=`cd "$(dirname server.sh)" && ABSPATH=$(pwd) && cd -`
elif [ "$IS_BSD" != "" ]; then
	alias nproc="sysctl -n hw.ncpu"
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
	install_name_tool -change "@rpath/libffead-modules.dylib" "${FFEAD_CPP_PATH}/lib/libffead-modules.dylib" ffead-cpp
	install_name_tool -change "@rpath/libffead-framework.dylib" "${FFEAD_CPP_PATH}/lib/libffead-framework.dylib" ffead-cpp
	if [ -d "${FFEAD_CPP_PATH}/web/default" ]; then
		install_name_tool -change "@rpath/libdefault.dylib" "${FFEAD_CPP_PATH}/lib/libdefault.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/flexApp" ]; then
		install_name_tool -change "@rpath/libflexApp.dylib" "${FFEAD_CPP_PATH}/lib/libflexApp.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/oauthApp" ]; then
		install_name_tool -change "@rpath/liboauthApp.dylib" "${FFEAD_CPP_PATH}/lib/liboauthApp.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/markers" ]; then
		install_name_tool -change "@rpath/libmarkers.dylib" "${FFEAD_CPP_PATH}/lib/libmarkers.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/peer-server" ]; then
		install_name_tool -change "@rpath/libpeer_server.dylib" "${FFEAD_CPP_PATH}/lib/libpeer_server.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/te-benchmark" ]; then
		install_name_tool -change "@rpath/libte_benchmark.dylib" "${FFEAD_CPP_PATH}/lib/libte_benchmark.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/te-benchmark-um" ]; then
		install_name_tool -change "@rpath/libte_benchmark_um.dylib" "${FFEAD_CPP_PATH}/lib/libte_benchmark_um.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/te-benchmark-um-mgr" ]; then
		install_name_tool -change "@rpath/libte_benchmark_um_mgr.dylib" "${FFEAD_CPP_PATH}/lib/libte_benchmark_um_mgr.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/te-benchmark-um-pq" ]; then
		install_name_tool -change "@rpath/libte_benchmark_um_pq.dylib" "${FFEAD_CPP_PATH}/lib/libte_benchmark_um_pq.dylib" ffead-cpp
	fi
	if [ -d "${FFEAD_CPP_PATH}/web/te-benchmark-um-pq-async" ]; then
		install_name_tool -change "@rpath/libte_benchmark_um_pq_async.dylib" "${FFEAD_CPP_PATH}/lib/libte_benchmark_um_pq_async.dylib" ffead-cpp
	fi
	export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LD_LIBRARY_PATH
	export DYLD_FALLBACK_LIBRARY_PATH=$DYLD_LIBRARY_PATH
	cp ${FFEAD_CPP_PATH}/lib/libinter.dylib ${FFEAD_CPP_PATH}/lib/libdinter.dylib .
	#break set -E C++
	#frame select 1
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

if [ "$IS_OS_DARWIN" != "" ]; then
	lldb ffead-cpp
else
	gdb ffead-cpp
fi
