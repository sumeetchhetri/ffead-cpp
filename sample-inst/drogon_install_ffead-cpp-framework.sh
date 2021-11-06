#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))

WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

rm -rf nginxfc
rm -rf ffead-cpp-6.0/

if [ ! -d "drogon" ]
then
	git clone --recurse-submodules https://github.com/an-tao/drogon
	cd drogon
	mkdir build
	cd build
	cmake ..
	make && sudo make install
fi

cd ffead-cpp/

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' resources/server.prop
sed -i 's|ENABLE_CRS=true|ENABLE_CRS=false|g' resources/server.prop
sed -i 's|ENABLE_SEC=true|ENABLE_SEC=false|g' resources/server.prop
sed -i 's|ENABLE_FLT=true|ENABLE_FLT=false|g' resources/server.prop
sed -i 's|ENABLE_CNT=false|ENABLE_CNT=true|g' resources/server.prop
sed -i 's|ENABLE_CNT_MPG=true|ENABLE_CNT_MPG=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_PTH=true|ENABLE_CNT_PTH=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_EXT=true|ENABLE_CNT_EXT=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_RST=false|ENABLE_CNT_RST=true|g' resources/server.prop
sed -i 's|ENABLE_EXT=false|ENABLE_EXT=true|g' resources/server.prop
sed -i 's|ENABLE_SCR=true|ENABLE_SCR=false|g' resources/server.prop
sed -i 's|ENABLE_SWS=true|ENABLE_SWS=false|g' resources/server.prop
#sed -i 's|LOGGING_ENABLED=true|LOGGING_ENABLED=false|g' resources/server.prop

#rm -rf web/default web/oauthApp web/flexApp web/markers

#sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/default)||g' CMakeLists.txt
#sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/flexApp)||g' CMakeLists.txt
#sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/oauthApp)||g' CMakeLists.txt
#sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/markers)||g' CMakeLists.txt
#sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/default/libdefault${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
#sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/flexApp/libflexApp${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
#sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/oauthApp/liboauthApp${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
#sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/markers/libmarkers${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
#sed -i 's|web/default/src/autotools/Makefile||g' configure.ac
#sed -i 's|web/flexApp/src/autotools/Makefile||g' configure.ac
#sed -i 's|web/oauthApp/src/autotools/Makefile||g' configure.ac
#sed -i 's|web/markers/src/autotools/Makefile||g' configure.ac

cmake -DSRV_DROGON=on -DDEBUG=on -DMOD_APACHE=off -DMOD_NGINX=off -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on .
cp -f web/te-benchmark/sql-src/TeBkWorldmongo.h web/te-benchmark/include/TeBkWorld.h
cp -f web/te-benchmark/sql-src/TeBkWorldmongo.cpp web/te-benchmark/src/TeBkWorld.cpp
cp -f web/t1/sql-src/TeBkUmWorldmongo.h web/t1/include/TeBkUmWorld.h
cp -f web/t1/sql-src/TeBkUmWorldmongo.cpp web/t1/src/TeBkUmWorld.cpp
make install -j${MAX_THREADS}

rm -f /usr/local/lib/libffead-*
rm -f /usr/local/lib/libt1.so*
rm -f /usr/local/lib/libt2.so*
rm -f /usr/local/lib/libt3.so*
rm -f /usr/local/lib/libt4.so*
rm -f /usr/local/lib/libt5.so*
rm -f /usr/local/lib/libinter.so
rm -f /usr/local/lib/libdinter.so

cd ffead-cpp-6.0-bin
chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
./server.sh &
while [ ! -f lib/libinter.so ]
do
	sleep 1
done
while [ ! -f lib/libdinter.so ]
do
	sleep 1
done
pkill ffead-cpp

cd /root/ffead-cpp/
cp -rf ffead-cpp-6.0-bin /root/ffead-cpp-6.0
rm -rf ffead-cpp-6.0-bin

cd /root/ffead-cpp-6.0
cp -f lib/* /usr/local/lib
ldconfig

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
#rm -f web/te-benchmark/config/cache.xml
#rm -f web/t1/config/cache.xml
