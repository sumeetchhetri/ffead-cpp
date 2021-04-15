mkdir /tmp/profile-data
cd /tmp/ffead-cpp-src
rm -rf /tmp/ffead-cpp-src/web/default && rm -rf /tmp/ffead-cpp-src/web/flexApp && \
	rm -rf /tmp/ffead-cpp-src/web/oauthApp && rm -rf /tmp/ffead-cpp-src/web/te-benchmark && \
	rm -rf /tmp/ffead-cpp-src/web/peer-server && rm -rf /tmp/ffead-cpp-src/web/markers && \
	rm -rf /tmp/ffead-cpp-src/web/te-benchmark-um && \
	rm -rf /tmp/ffead-cpp-src/web/te-benchmark-um-mgr && \
	rm -rf /tmp/ffead-cpp-src/web/te-benchmark-um-pq
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/default)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/flexApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/oauthApp)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/markers)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/te-benchmark)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/peer-server)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/te-benchmark-um)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/te-benchmark-um-mgr)||g' CMakeLists.txt
sed -i 's|add_subdirectory(${PROJECT_SOURCE_DIR}/web/te-benchmark-um-pq)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/default/libdefault${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/flexApp/libflexApp${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/oauthApp/liboauthApp${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/markers/libmarkers${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/te-benchmark/libte-benchmark${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/peer-server/libpeer-server${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/te-benchmark-um/libte-benchmark-um${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/te-benchmark-um-mgr/libte-benchmark-um-mgr${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
sed -i 's|install(FILES ${PROJECT_BINARY_DIR}/web/te-benchmark-um-pq/libte-benchmark-um-pq${LIB_EXT} DESTINATION ${PROJECT_NAME}-bin/lib)||g' CMakeLists.txt
mkdir build

cd /tmp/ffead-cpp-src/build
cmake -GNinja -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DCMAKE_EXE_LINKER_FLAGS="-fprofile-dir=/tmp/profile-data -fprofile-generate" \
	-DCMAKE_CXX_FLAGS="-flto -march=native -fprofile-dir=/tmp/profile-data  -fprofile-generate" ..
ninja install && \
	cp -f /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq-async/config/cachememory.xml /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq-async/config/cache.xml && \
	mv /tmp/ffead-cpp-src/ffead-cpp-5.0-bin /tmp/ffead-cpp-sql-raw

cd /

if ! nc -z localhost 5432 ; then
  ./run.sh
fi

sed -i 's|cmake |cmake -DCMAKE_EXE_LINKER_FLAGS="-fprofile-dir=/tmp/profile-data -fprofile-generate" -DCMAKE_CXX_FLAGS="-march=native -fprofile-dir=/tmp/profile-data -fprofile-generate" |g' /tmp/ffead-cpp-sql-raw/resources/rundyn-automake.sh
chmod +x *.sh && ./install_ffead-cpp-profile.sh
ls /tmp/profile-data
rm -rf /tmp/ffead-cpp-sql-raw

cd /tmp/ffead-cpp-src/build
rm -f CMakeCache.txt
cmake -GNinja -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DCMAKE_CXX_FLAGS="-flto -march=native -fprofile-dir=/tmp/profile-data -fprofile-use -fprofile-correction" ..
ninja install && \
	cp -f /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq-async/config/cachememory.xml /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq/config/cache.xml && \
	mv /tmp/ffead-cpp-src/ffead-cpp-5.0-bin /tmp/ffead-cpp-sql-raw

cd /

sed -i 's|cmake |cmake -DCMAKE_CXX_FLAGS="-march=native -fprofile-dir=/tmp/profile-data -fprofile-use -fprofile-correction" |g' /tmp/ffead-cpp-sql-raw/resources/rundyn-automake.sh
./install_ffead-cpp-profile.sh
rm -rf /tmp/ffead-cpp-sql-raw

apt remove -y libpq-dev
apt update && apt install -y bison flex

commit=b787d4ce6d910080065025bcd5f968544997271f
wget -nv https://github.com/postgres/postgres/archive/$commit.zip
unzip -q $commit.zip
cd postgres-$commit
wget -nv https://www.postgresql.org/message-id/attachment/115223/v22-0001-libpq-batch.patch
git apply ./v22-0001-libpq-batch.patch
./configure --prefix=/usr CFLAGS='-O3 -march=native -flto'
cd src/interfaces/libpq
make all install -j4
cp ../../../src/include/postgres_ext.h ../../../src/include/pg_config_ext.h /usr/include

cd /tmp/ffead-cpp-src
rm -rf build
mkdir build
cd /tmp/ffead-cpp-src/build
cmake -GNinja -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DCMAKE_EXE_LINKER_FLAGS="-fprofile-dir=/tmp/profile-data  -fprofile-generate" \
	-DCMAKE_CXX_FLAGS="-march=native -fprofile-dir=/tmp/profile-data  -fprofile-generate" ..
ninja install && \
	cp -f /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq-async/config/cachememory.xml /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq/config/cache.xml && \
	mv /tmp/ffead-cpp-src/ffead-cpp-5.0-bin /tmp/ffead-cpp-sql-raw

cd /

sed -i 's|cmake .|cmake -DCMAKE_EXE_LINKER_FLAGS="-fprofile-dir=/tmp/profile-data -fprofile-generate" -DCMAKE_CXX_FLAGS="-march=native -fprofile-dir=/tmp/profile-data  -fprofile-generate" .|g' /tmp/ffead-cpp-sql-raw/resources/rundyn-automake.sh
./install_ffead-cpp-profile.sh
ls /tmp/profile-data
rm -rf /tmp/ffead-cpp-sql-raw

cd /tmp/ffead-cpp-src/build
rm -f CMakeCache.txt
cmake -GNinja -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on -DCMAKE_CXX_FLAGS="-march=native -fprofile-dir=/tmp/profile-data -fprofile-use -fprofile-correction" ..
ninja install && \
	cp -f /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq-async/config/cachememory.xml /tmp/ffead-cpp-src/ffead-cpp-5.0-bin/web/te-benchmark-um-pq/config/cache.xml && \
	mv /tmp/ffead-cpp-src/ffead-cpp-5.0-bin /tmp/ffead-cpp-sql-raw

cd /

sed -i 's|cmake |cmake -DCMAKE_CXX_FLAGS="-march=native -fprofile-dir=/tmp/profile-data -fprofile-use -fprofile-correction" |g' /tmp/ffead-cpp-sql-raw/resources/rundyn-automake.sh
./install_ffead-cpp-profile.sh
rm -rf /tmp/ffead-cpp-sql-raw
