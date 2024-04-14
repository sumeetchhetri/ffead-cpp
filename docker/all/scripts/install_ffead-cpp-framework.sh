#!/bin/bash

#Set the number of threads ----
MAX_THREADS=$(( 3 * `nproc` / 2 ))

WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

#temporary workaround till this gets fixed by the framework author of CppServer
sed -i 's|explicit constexpr UUID(const char\* uuid, size_t size);|explicit UUID(const char\* uuid, size_t size);|g' /usr/local/include/system/uuid.h
sed -i 's|constexpr CppCommon::UUID operator ""|CppCommon::UUID operator ""|g' /usr/local/include/system/uuid.h
sed -i 's|inline constexpr UUID::UUID|inline UUID::UUID|g' /usr/local/include/system/uuid.inl

git clone https://github.com/sumeetchhetri/ffead-cpp
#git checkout 92c3a9e3d5ec1de4a909fe688d649d7f31e050c0 -b 6.0
cd ffead-cpp
rm -rf .git
cd ..
mv ffead-cpp ffead-cpp-src
mv ffead-cpp-src/lang-server-backends ${IROOT}/

cd /tmp
git clone https://github.com/Tencent/rapidjson
cp -rf rapidjson/include/rapidjson /usr/include/
rm -rf rapidjson

cd $IROOT/ffead-cpp-src/

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
#rm -rf web/t1
#rm -rf web/t2
#rm -rf web/t3
#rm -rf web/t4
#rm -rf web/t5
#rm -rf web/t6
#rm -rf web/t7
mv ${IROOT}/server.sh script/
#mv ${IROOT}/t1 web/
#mv ${IROOT}/t2 web/
#mv ${IROOT}/t3 web/
#mv ${IROOT}/t4 web/
#mv ${IROOT}/t5 web/
#mv ${IROOT}/t6 web/
#mv ${IROOT}/t7 web/
sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' resources/server.prop
sed -i 's|ENABLE_CRS=true|ENABLE_CRS=false|g' resources/server.prop
sed -i 's|ENABLE_SEC=true|ENABLE_SEC=false|g' resources/server.prop
sed -i 's|ENABLE_FLT=true|ENABLE_FLT=false|g' resources/server.prop
sed -i 's|ENABLE_CNT=true|ENABLE_CNT=true|g' resources/server.prop
sed -i 's|ENABLE_EXT_CNT=true|ENABLE_EXT_CNT=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_MPG=true|ENABLE_CNT_MPG=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_PTH=true|ENABLE_CNT_PTH=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_EXT=true|ENABLE_CNT_EXT=false|g' resources/server.prop
sed -i 's|ENABLE_CNT_RST=true|ENABLE_CNT_RST=true|g' resources/server.prop
sed -i 's|ENABLE_EXT=true|ENABLE_EXT=true|g' resources/server.prop
sed -i 's|ENABLE_SCR=true|ENABLE_SCR=false|g' resources/server.prop
sed -i 's|ENABLE_SWS=true|ENABLE_SWS=false|g' resources/server.prop
sed -i 's|ENABLE_JOBS=true|ENABLE_JOBS=false|g' resources/server.prop
sed -i 's|LOGGING_ENABLED=true|LOGGING_ENABLED=false|g' resources/server.prop
sed -i 's|EVH_SINGLE=true|EVH_SINGLE=false|g' resources/server.prop

rm -rf web/default web/oauthApp web/flexApp web/markers web/te-benchmark web/peer-server
#rm -rf web/t1 web/t2

sed -i 's|localhost|tfb-database|g' web/t1/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/t2/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/t3/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/t4/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/t5/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/t6/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/t7/config/sdorm.xml
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/default)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/flexApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/oauthApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/markers)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/te-benchmark)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/peer-server)||g' CMakeLists.txt
#sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/t1)||g' CMakeLists.txt
#sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/t2)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/default/libdefault${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/flexApp/libflexApp${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/oauthApp/liboauthApp${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/markers/libmarkers${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/te-benchmark/libte-benchmark${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/peer-server/libpeer-server${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
#sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/t1/libt1${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
#sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/t2/libt2{LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/default")||g' CMakeLists.txt
sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/flexApp")||g' CMakeLists.txt
sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/oauthApp")||g' CMakeLists.txt
sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/markers")||g' CMakeLists.txt
sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/peer-server")||g' CMakeLists.txt
sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/te-benchmark")||g' CMakeLists.txt
#sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/t1")||g' CMakeLists.txt
#sed -i 's|install(DIRECTORY DESTINATION "${PROJECT_NAME}-bin/web/t2")||g' CMakeLists.txt
sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/default/ DESTINATION ${PROJECT_NAME}-bin/web/default)||g' CMakeLists.txt
sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/flexApp/ DESTINATION ${PROJECT_NAME}-bin/web/flexApp)||g' CMakeLists.txt
sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/oauthApp/ DESTINATION ${PROJECT_NAME}-bin/web/oauthApp)||g' CMakeLists.txt
sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/markers/ DESTINATION ${PROJECT_NAME}-bin/web/markers)||g' CMakeLists.txt
sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/peer-server/ DESTINATION ${PROJECT_NAME}-bin/web/peer-server)||g' CMakeLists.txt
sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/te-benchmark/ DESTINATION ${PROJECT_NAME}-bin/web/te-benchmark)||g' CMakeLists.txt
#sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/t1/ DESTINATION ${PROJECT_NAME}-bin/web/t1)||g' CMakeLists.txt
#sed -i 's|install(DIRECTORY ${PROJECT_SOURCE_DIR}/web/t2/ DESTINATION ${PROJECT_NAME}-bin/web/t2)||g' CMakeLists.txt
sed -i 's|web/default/src/autotools/Makefile||g' configure.ac
sed -i 's|web/flexApp/src/autotools/Makefile||g' configure.ac
sed -i 's|web/oauthApp/src/autotools/Makefile||g' configure.ac
sed -i 's|web/markers/src/autotools/Makefile||g' configure.ac
sed -i 's|web/te-benchmark/src/autotools/Makefile||g' configure.ac
sed -i 's|web/peer-server/src/autotools/Makefile||g' configure.ac
#sed -i 's|web/t1/src/autotools/Makefile||g' configure.ac
#sed -i 's|web/t2/src/autotools/Makefile||g' configure.ac

#./autogen.sh
#./configure --enable-debug=no --enable-apachemod=yes --enable-nginxmod=yes --enable-mod_sdormmongo=yes --enable-mod_sdormsql=yes --enable-mod_rediscache=yes --enable-mod_memcached=yes CPPFLAGS="$CPPFLAGS -I${IROOT}/include/libmongoc-1.0 -I${IROOT}/include/libbson-1.0 -I${IROOT}/include/" LDFLAGS="$LDFLAGS -L${IROOT} -L${IROOT}/lib"
cmake -DSRV_ALL=on -DCINATRA_INCLUDES=${IROOT}/cinatra/include -DMOD_APACHE=on -DMOD_NGINX=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DDEBUG=${DEBUG} -DWITH_RAPIDJSON=on -DWITH_PUGIXML=on -GNinja .

cp resources/sample-odbcinst.ini ${IROOT}/odbcinst.ini
cp resources/sample-odbc.ini ${IROOT}/odbc.ini

sed -i 's|127.0.0.1|tfb-database|g' ${IROOT}/odbc.ini

#Start building for mongodb as the World model is different for SQL use case
cd ${IROOT}/ffead-cpp-src/
cp -f web/t1/sql-src/TeBkUmWorldmongo.h web/t1/include/TeBkUmWorld.h
cp -f web/t1/sql-src/TeBkUmWorldmongo.cpp web/t1/src/TeBkUmWorld.cpp
ninja install

rm -f /usr/local/lib/libffead-*
rm -f /usr/local/lib/libt1.so*
rm -f /usr/local/lib/libt2.so*
rm -f /usr/local/lib/libt3.so*
rm -f /usr/local/lib/libt4.so*
rm -f /usr/local/lib/libt5.so*
rm -f /usr/local/lib/libinter.so
rm -f /usr/local/lib/libdinter.so

if [ ! -d "ffead-cpp-7.0-bin" ]
then
	exit 1
fi

cd ffead-cpp-7.0-bin
#cache related dockerfiles will add the cache.xml accordingly whenever needed
chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
./server.sh &
COUNTER=0
while [ ! -f lib/libinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure...."
    	exit 1
    fi
done
COUNTER=0
while [ ! -f lib/libdinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure....ddlib"
    	exit 1
    fi
done
echo "ffead-cpp start successful"
sleep 20
cd tests && rm -f test.csv && cp ${IROOT}/ffead-cpp-src/tests/test-te-all.csv test.csv && chmod +x *.sh && ./runTests.sh
echo "ffead-cpp normal shutdown"
pkill ffead-cpp

cd ${IROOT}/ffead-cpp-src/
cp -rf ffead-cpp-7.0-bin ${IROOT}/ffead-cpp-7.0
rm -rf ffead-cpp-7.0-bin
mv ${IROOT}/nginxfc ${IROOT}/nginx-ffead-mongo

cd ${IROOT}/ffead-cpp-7.0

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
chmod 755 *.sh
rm -f *.cntrl
rm -f tmp/*.sess
#Done building for mongodb


#Start building for sql as the World model is different for mongodb use case
cd ${IROOT}/ffead-cpp-src/
cp -f web/t1/sql-src/TeBkUmWorldsql.h web/t1/include/TeBkUmWorld.h
cp -f web/t1/sql-src/TeBkUmWorldsql.cpp web/t1/src/TeBkUmWorld.cpp
ninja install

if [ ! -d "ffead-cpp-7.0-bin" ]
then
	exit 1
fi

cd ffead-cpp-7.0-bin
#cache related dockerfiles will add the cache.xml accordingly whenever needed
chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
./server.sh &
COUNTER=0
while [ ! -f lib/libinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure...."
    	exit 1
    fi
done
COUNTER=0
while [ ! -f lib/libdinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure....ddlib"
    	exit 1
    fi
done
echo "ffead-cpp start successful"
sleep 20
cd tests && rm -f test.csv && cp ${IROOT}/ffead-cpp-src/tests/test-te-all.csv test.csv && chmod +x *.sh && ./runTests.sh
echo "ffead-cpp normal shutdown"
pkill ffead-cpp

cd ${IROOT}/ffead-cpp-src/
cp -rf ffead-cpp-7.0-bin ${IROOT}/ffead-cpp-7.0-sql
rm -rf ffead-cpp-7.0-bin
mv ${IROOT}/nginxfc ${IROOT}/nginx-ffead-sql

cd ${IROOT}/ffead-cpp-7.0-sql

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
chmod 755 *.sh
rm -f *.cntrl
rm -f tmp/*.sess
#Done building for sql

#Start building for picoev backend
cd ${IROOT}/ffead-cpp-src/
rm -rf CMakeCache.txt CMakeFiles
cmake -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DDEBUG=${DEBUG} -DWITH_RAPIDJSON=on -DWITH_PUGIXML=on -DWITH_PICOEV=on -GNinja .
ninja install
if [ ! -d "ffead-cpp-7.0-bin" ]
then
	exit 1
fi

cd ffead-cpp-7.0-bin
#cache related dockerfiles will add the cache.xml accordingly whenever needed
chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
./server.sh &
COUNTER=0
while [ ! -f lib/libinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure...."
    	exit 1
    fi
done
COUNTER=0
while [ ! -f lib/libdinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure....ddlib"
    	exit 1
    fi
done
echo "ffead-cpp start successful - picoev backend"
sleep 20
cd tests && rm -f test.csv && cp ${IROOT}/ffead-cpp-src/tests/test-te-all.csv test.csv && chmod +x *.sh && ./runTests.sh
echo "ffead-cpp normal shutdown - picoev backend"
pkill ffead-cpp

cd ${IROOT}/ffead-cpp-src/
cp -rf ffead-cpp-7.0-bin ${IROOT}/ffead-cpp-7.0-picoev
rm -rf ffead-cpp-7.0-bin

cd ${IROOT}/ffead-cpp-7.0-picoev

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
chmod 755 *.sh
rm -f *.cntrl
rm -f tmp/*.sess
#Done

#Start building for io_uring backend
cd /tmp
wget -q https://github.com/axboe/liburing/archive/liburing-2.2.tar.gz
tar xf liburing-2.2.tar.gz
rm -f liburing-2.2.tar.gz
cd liburing-liburing-2.2 && ./configure --prefix=/usr/local && make install
cd /tmp && rm -rf liburing-liburing-2.2

cd ${IROOT}/ffead-cpp-src/
rm -rf CMakeCache.txt CMakeFiles
cmake -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DDEBUG=${DEBUG} -DWITH_RAPIDJSON=on -DWITH_PUGIXML=on -DWITH_IOURING=on -GNinja .
ninja install
if [ ! -d "ffead-cpp-7.0-bin" ]
then
	exit 1
fi

cd ffead-cpp-7.0-bin
#ulimit -l 102400000
sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop
sed -i 's|REQUEST_HANDLER=RequestReaderHandler|REQUEST_HANDLER=RequestHandler2|g' resources/server.prop
#cache related dockerfiles will add the cache.xml accordingly whenever needed
chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
./server.sh &
COUNTER=0
while [ ! -f lib/libinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure...."
    	exit 1
    fi
done
COUNTER=0
while [ ! -f lib/libdinter.so ]
do
    sleep 1
    COUNTER=$((COUNTER+1))
    if [ "$COUNTER" = 120 ]
    then
    	cat logs/jobs.log
    	echo "ffead-cpp exiting exiting due to failure....ddlib"
    	exit 1
    fi
done
echo "ffead-cpp start successful - io_uring backend"
sleep 20
cd tests && rm -f test.csv && cp ${IROOT}/ffead-cpp-src/tests/test-te-all.csv test.csv && chmod +x *.sh && ./runTests.sh
echo "ffead-cpp normal shutdown - io_uring backend"
pkill ffead-cpp

cd ${IROOT}/ffead-cpp-src/
cp -rf ffead-cpp-7.0-bin ${IROOT}/ffead-cpp-7.0-io_uring
rm -rf ffead-cpp-7.0-bin

cd ${IROOT}/ffead-cpp-7.0-io_uring

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
chmod 755 *.sh
rm -f *.cntrl
rm -f tmp/*.sess
#Done