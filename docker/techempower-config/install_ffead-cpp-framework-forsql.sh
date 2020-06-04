#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))

WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

cd $IROOT

wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip master.zip
mv ffead-cpp-master ffead-cpp-src
cd $IROOT

SRV_TYPE=SRV_EMB

if [ "$1" = "lithium" ]
then
	SRV_TYPE=SRV_LITHIUM
fi

if [ "$1" = "cinatra" ]
then
	SRV_TYPE=SRV_CINATRA
	git clone https://github.com/sumeetchhetri/cinatra.git
fi

if [ "$1" = "drogon" ]
then
	SRV_TYPE=SRV_DROGON
	git clone --recurse-submodules https://github.com/an-tao/drogon
	mkdir build
	cd build
	cmake ..
	make && sudo make install
fi

if [ "$1" = "libreactor" ]
then
	apt-get install -y build-essential libjansson-dev wget
	cd $IROOT
	wget https://github.com/fredrikwidlund/libdynamic/releases/download/v1.3.0/libdynamic-1.3.0.tar.gz
	tar fvxz libdynamic-1.3.0.tar.gz
	cd libdynamic-1.3.0
	./configure --prefix=/usr AR=gcc-ar NM=gcc-nm RANLIB=gcc-ranlib
	make install
	cd $IROOT
	wget https://github.com/fredrikwidlund/libreactor/releases/download/v1.0.1/libreactor-1.0.1.tar.gz
	tar fvxz libreactor-1.0.1.tar.gz
	cd libreactor-1.0.1
	./configure --prefix=/usr AR=gcc-ar NM=gcc-nm RANLIB=gcc-ranlib
	make install
	cd $IROOT
	cd ffead-cpp-src/lang-server-backends/c/libreactor
	make
	cp libreactor-ffead-cpp $IROOT/
fi

if [ "$1" = "crystal-http" ]
then
	curl -sL "https://keybase.io/crystal/pgp_keys.asc" | sudo apt-key add -
	echo "deb https://dist.crystal-lang.org/apt crystal main" | sudo tee /etc/apt/sources.list.d/crystal.list
	apt-get update -y
	apt install -y crystal
	cd ffead-cpp-src/lang-server-backends/crystal/crystal
	crystal build --release --no-debug crystal-ffead-cpp.cr -o crystal-ffead-cpp.out
	cp crystal-ffead-cpp.out $IROOT/
fi

if [ "$1" = "crystal-h2o" ]
then
	apt-get update -y
	apt-get install -yqq libh2o-evloop-dev libwslay-dev libyaml-0-2 libevent-dev libpcre3-dev \
    	gcc wget git libssl-dev libuv1-dev ca-certificates --no-install-recommends
    wget -q https://github.com/crystal-lang/crystal/releases/download/0.26.1/crystal-0.26.1-1-linux-x86_64.tar.gz
	tar --strip-components=1 -xzf crystal-0.26.1-1-linux-x86_64.tar.gz -C /usr/
	rm -f *.tar.gz
	cd ffead-cpp-src/lang-server-backends/crystal/h2o.cr
	shards install
	gcc -shared -O3 lib/h2o/src/ext/h2o.c -I/usr/include -fPIC -o h2o.o
	CRYSTAL_PATH=lib:/usr/share/crystal/src crystal build --prelude=empty --no-debug --release -Dgc_none -Dfiber_none -Dexcept_none -Dhash_none -Dtime_none -Dregex_none -Dextreme h2o-evloop-ffead-cpp.cr --link-flags="-Wl,-s $PWD/h2o.o -DH2O_USE_LIBUV=0" -o h2o-evloop-ffead-cpp.out
	cp h2o-evloop-ffead-cpp $IROOT/
fi

if [ "$1" = "rust-actix" ]
then
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	source ~/.cargo/env
	cd ffead-cpp-src/lang-server-backends/rust/actix-ffead-cpp/
	cargo build --release
	cp target/release/actix-ffead-cpp $IROOT/
fi

if [ "$1" = "rust-hyper" ]
then
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	source ~/.cargo/env
	cd ffead-cpp-src/lang-server-backends/rust/hyper-ffead-cpp/
	cargo build --release
	cp target/release/hyper-ffead-cpp $IROOT/
fi

if [ "$1" = "rust-thruster" ]
then
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	source ~/.cargo/env
	cd ffead-cpp-src/lang-server-backends/rust/thruster-ffead-cpp/
	cargo build --release
	cp target/release/thruster-ffead-cpp $IROOT/
fi

if [ "$1" = "rust-rocket" ]
then
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
	source ~/.cargo/env
	rustup default nightly
	cd ffead-cpp-src/lang-server-backends/rust/rocket-ffead-cpp/
	cargo build --release
	cp target/release/actix-ffead-cpp $IROOT/
fi

if [ "$1" = "go-fasthttp" ]
then
	wget https://dl.google.com/go/go1.14.4.linux-amd64.tar.gz
	tar -C /usr/local -xzf go1.14.4.linux-amd64.tar.gz
	export PATH=$PATH:/usr/local/go/bin
	cd ffead-cpp-src/lang-server-backends/go/fasthttp
	make
	cp fasthttp-ffead-cpp $IROOT/
fi

if [ "$1" = "go-gnet" ]
then
	wget https://dl.google.com/go/go1.14.4.linux-amd64.tar.gz
	tar -C /usr/local -xzf go1.14.4.linux-amd64.tar.gz
	export PATH=$PATH:/usr/local/go/bin
	cd ffead-cpp-src/lang-server-backends/go/gnet
	make
	cp gnet-ffead-cpp $IROOT/
fi

if [ "$1" = "v-vweb" ]
then
	git clone https://github.com/vlang/v
	cd v
	make
	./v symlink
	cd ffead-cpp-src/lang-server-backends/v/vweb
	./build.sh
	cp vweb $IROOT/
fi

if [ "$1" = "java-firenio" ]
then
	apt install -y default-jre maven
	cd ffead-cpp-src/lang-server-backends/java/firenio
	mvn package
	cp target/firenio-ffead-cpp-0.1-jar-with-dependencies.jar $IROOT/
fi

if [ "$1" = "java-rapidoid" ]
then
	apt install -y default-jre maven
	cd ffead-cpp-src/lang-server-backends/java/rapidoid
	mvn package
	cp target/rapidoid-ffead-cpp-1.0-jar-with-dependencies.jar $IROOT/
fi

if [ "$1" = "java-wizzardo-http" ]
then
	apt install -y default-jre gradle
	cd ffead-cpp-src/lang-server-backends/java/wizzardo-http
	gradle fatJar
	cp build/libs/wizzardo-ffead-cpp-all-1.0.jar $IROOT/
fi

cd $IROOT
cd ffead-cpp-src/

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
rm -rf web/te-benchmark-um
cp -f ${TROOT}/server.sh script/
cp -rf ${TROOT}/te-benchmark-um web/
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
sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop

rm -rf web/default web/oauthApp web/flexApp web/markers web/te-benchmark web/peer-server

sed -i 's|localhost|tfb-database|g' web/te-benchmark-um/config/sdorm.xml
sed -i 's|localhost|tfb-database|g' web/te-benchmark-um/config/sdormmongo.xml
sed -i 's|localhost|tfb-database|g' web/te-benchmark-um/config/sdormmysql.xml
sed -i 's|localhost|tfb-database|g' web/te-benchmark-um/config/sdormpostgresql.xml
sed -i 's|127.0.0.1|tfb-database|g' resources/sample-odbcinst.ini
sed -i 's|127.0.0.1|tfb-database|g' resources/sample-odbc.ini
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/default)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/flexApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/oauthApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/markers)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/te-benchmark)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/peer-server)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/default/libdefault${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/flexApp/libflexApp${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/oauthApp/liboauthApp${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/markers/libmarkers${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/te-benchmark/libte_benchmark${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_SOURCE_DIR}/web/peer-server/libpeer_server${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|web/default/src/autotools/Makefile||g' configure.ac
sed -i 's|web/flexApp/src/autotools/Makefile||g' configure.ac
sed -i 's|web/oauthApp/src/autotools/Makefile||g' configure.ac
sed -i 's|web/markers/src/autotools/Makefile||g' configure.ac
sed -i 's|web/te-benchmark/src/autotools/Makefile||g' configure.ac
sed -i 's|web/peer-server/src/autotools/Makefile||g' configure.ac

#./autogen.sh
#./configure --enable-debug=no --enable-apachemod=yes --enable-nginxmod=yes --enable-mod_sdormmongo=yes --enable-mod_sdormsql=yes --enable-mod_rediscache=yes --enable-mod_memcached=yes CPPFLAGS="$CPPFLAGS -I${IROOT}/include/libmongoc-1.0 -I${IROOT}/include/libbson-1.0 -I${IROOT}/include/" LDFLAGS="$LDFLAGS -L${IROOT} -L${IROOT}/lib"
#make install
cmake -D$SRV_TYPE=on -DMOD_APACHE=on -DMOD_NGINX=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on .
#make install -j4

cp resources/sample-odbcinst.ini ${IROOT}/odbcinst.ini
cp resources/sample-odbc.ini ${IROOT}/odbc.ini

cd ${IROOT}/ffead-cpp-src/
cp -f web/te-benchmark-um/sql-src/TeBkUmWorldsql.h web/te-benchmark-um/include/TeBkUmWorld.h
cp -f web/te-benchmark-um/sql-src/TeBkUmWorldsql.cpp web/te-benchmark-um/src/TeBkUmWorld.cpp
make install -j${MAX_THREADS}

rm -f /usr/local/lib/libffead-*
rm -f /usr/local/lib/libte_benc*
rm -f /usr/local/lib/libinter.so
rm -f /usr/local/lib/libdinter.so

cd ffead-cpp-4.0-bin
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
cp -rf ffead-cpp-4.0-bin ${IROOT}/ffead-cpp-4.0
rm -rf ffead-cpp-4.0-bin

ln -s ${IROOT}/ffead-cpp-4.0/lib/libte_benchmark_um.so /usr/local/lib/libte_benchmark_um.so
ln -s ${IROOT}/ffead-cpp-4.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so
ln -s ${IROOT}/ffead-cpp-4.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so
ln -s ${IROOT}/ffead-cpp-4.0/lib/libinter.so /usr/local/lib/libinter.so
ln -s ${IROOT}/ffead-cpp-4.0/lib/libdinter.so /usr/local/lib/libdinter.so
ldconfig

cd ${IROOT}/ffead-cpp-4.0
cp -f ${TROOT}/run_ffead.sh ./

chmod 755 *.sh resources/*.sh rtdcf/autotools/*.sh
chmod 755 $FFEAD_CPP_PATH/*.sh
rm -f $FFEAD_CPP_PATH/*.cntrl
rm -f $FFEAD_CPP_PATH/tmp/*.sess
#cache related dockerfiles will add the cache.xml accordingly whenever needed
rm -f web/te-benchmark-um/config/cache.xml
