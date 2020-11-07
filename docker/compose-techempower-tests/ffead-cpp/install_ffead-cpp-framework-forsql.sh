#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))

WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

cd $IROOT

wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make install
cd $IROOT
rm -rf libcuckoo-master

cd $IROOT

wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip -qq master.zip
rm -f master.zip
mv ffead-cpp-master ffead-cpp-src
mv ${TROOT}/ffead-cpp-src ffead-cpp-src
cd ffead-cpp-src/

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
cp -f ${TROOT}/server.sh script/
sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' resources/server.prop
sed -i 's|LOGGING_ENABLED=true|LOGGING_ENABLED=false|g' resources/server.prop

rm -rf web/default web/oauthApp web/flexApp web/markers web/peer-server

sed -i 's|localhost|db|g' web/te-benchmark/config/sdorm.xml
sed -i 's|localhost|db|g' web/te-benchmark/config/sdormmongo.xml
sed -i 's|localhost|db|g' web/te-benchmark/config/sdormmysql.xml
sed -i 's|localhost|db|g' web/te-benchmark/config/sdormpostgresql.xml
sed -i 's|localhost|db|g' web/te-benchmark-um/config/sdorm.xml
sed -i 's|localhost|db|g' web/te-benchmark-um/config/sdormmongo.xml
sed -i 's|localhost|db|g' web/te-benchmark-um/config/sdormmysql.xml
sed -i 's|localhost|db|g' web/te-benchmark-um/config/sdormpostgresql.xml
sed -i 's|localhost|db|g' web/te-benchmark-um-mgr/config/sdorm.xml
sed -i 's|localhost|db|g' web/te-benchmark-um-pq/config/sdorm.xml
sed -i 's|localhost|db|g' web/te-benchmark-um-pq-async/config/sdorm.xml
sed -i 's|127.0.0.1|db|g' resources/sample-odbcinst.ini
sed -i 's|127.0.0.1|db|g' resources/sample-odbc.ini
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/default)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/flexApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/oauthApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/markers)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/peer-server)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/default/libdefault${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/flexApp/libflexApp${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/oauthApp/liboauthApp${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/markers/libmarkers${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/peer-server/libpeer_server${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|web/default/src/autotools/Makefile||g' configure.ac
sed -i 's|web/flexApp/src/autotools/Makefile||g' configure.ac
sed -i 's|web/oauthApp/src/autotools/Makefile||g' configure.ac
sed -i 's|web/markers/src/autotools/Makefile||g' configure.ac
sed -i 's|web/peer-server/src/autotools/Makefile||g' configure.ac

cmake -DSRV_EMB=on -DMOD_APACHE=on -DMOD_NGINX=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on .

cp resources/sample-odbcinst.ini ${IROOT}/odbcinst.ini
cp resources/sample-odbc.ini ${IROOT}/odbc.ini

cd ${IROOT}/ffead-cpp-src/

#Build for sql now
cp -f web/te-benchmark/sql-src/TeBkWorldsql.h web/te-benchmark/include/TeBkWorld.h
cp -f web/te-benchmark/sql-src/TeBkWorldsql.cpp web/te-benchmark/src/TeBkWorld.cpp
cp -f web/te-benchmark-um/sql-src/TeBkUmWorldsql.h web/te-benchmark-um/include/TeBkUmWorld.h
cp -f web/te-benchmark-um/sql-src/TeBkUmWorldsql.cpp web/te-benchmark-um/src/TeBkUmWorld.cpp
make install -j4
cd ffead-cpp-5.0-bin
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

cd ${IROOT}/ffead-cpp-src/
cp -rf ffead-cpp-5.0-bin ${IROOT}/ffead-cpp-5.0
rm -rf ffead-cpp-5.0-bin

cd ${IROOT}/ffead-cpp-5.0
cp -f ${TROOT}/run_ffead.sh ./

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
chmod 755 $FFEAD_CPP_PATH/*.sh
rm -f $FFEAD_CPP_PATH/*.cntrl
rm -f $FFEAD_CPP_PATH/tmp/*.sess
#cache related dockerfiles will add the cache.xml accordingly whenever needed
rm -f web/te-benchmark/config/cache.xml
rm -f web/te-benchmark-um/config/cache.xml
