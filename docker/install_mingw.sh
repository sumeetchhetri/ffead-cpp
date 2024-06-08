wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip -qq master.zip
mv ffead-cpp-master ffead-cpp-src
rm -f master.zip

#cd /tmp/ffead-cpp-src/docker/files
#unzip -qq mingw64-dlfcn.zip
#cd mingw64
#cp bin/* /mingw64/bin/
#cp include/* /mingw64/include/
#cp lib/* /mingw64/lib/
#cd /tmp/ffead-cpp-src/docker/files
#rm -rf mingw64
#unzip -qq mingw64-unixodbc.zip
#cd mingw64
#cp bin/* /mingw64/bin/
#cp include/* /mingw64/include/
#cp -rf lib/* /mingw64/lib/
#cp -rf share/* /mingw64/share/
#cp -rf etc/* /mingw64/etc/

TARGET=x86_64-w64-mingw32

cd /tmp
wget -q https://github.com/sean-/ossp-uuid/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd ossp-uuid-master
cp /tmp/ffead-cpp-src/docker/files/config.sub .
env CC=${TARGET}-gcc CFLAGS="-D_FORTIFY_SOURCE=2" LDFLAGS="-lssp" ./configure --prefix=/mingw64/ \
	--without-pgsql --without-perl --without-php
make
mv uuid.exe uuid
make install
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid.po uuid.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_cli.po uuid_cli.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_dce.po uuid_dce.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_mac.po uuid_mac.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_md5.po uuid_md5.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_prng.po uuid_prng.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_sha1.po uuid_sha1.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_str.po uuid_str.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_time.po uuid_time.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_ui128.po uuid_ui128.c
${TARGET}-gcc -D_FORTIFY_SOURCE=2 -c -fPIC -o uuid_ui64.po uuid_ui64.c
${TARGET}-gcc -shared -fPIC -Wl,-soname,libossp-uuid.dll -lssp -o libossp-uuid.dll *.po
cp libossp-uuid.dll /mingw64/lib/
cp /mingw64/lib/libuuid.a /mingw64/lib/libossp-uuid.dll.a
cd /tmp
rm -rf ossp-uuid-master

wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd /tmp/libcuckoo-master
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw64/ .
mingw32-make install
cd /tmp
rm -rf libcuckoo-master

#wget -q https://github.com/kkos/oniguruma/releases/download/v6.9.6/onig-6.9.6.tar.gz
#tar xf onig-6.9.6.tar.gz
#cd onig-6.9.6
#cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw64/ -DENABLE_BINARY_COMPATIBLE_POSIX_API=on .
#mingw32-make -j4 install
#cd /tmp
#rm -rf onig-6.9.6

#wget -q https://github.com/redis/hiredis/archive/8e0264cfd6889b73c241b60736fe96ba1322ee6e.zip
#unzip 8e0264cfd6889b73c241b60736fe96ba1322ee6e.zip
#rm -f 8e0264cfd6889b73c241b60736fe96ba1322ee6e.zip
#cd hiredis-8e0264cfd6889b73c241b60736fe96ba1322ee6e
#cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw64/ . && mingw32-make -j4 install
#cd /tmp
#rm -rf hiredis-8e0264cfd6889b73c241b60736fe96ba1322ee6e

#wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.16.2/mongo-c-driver-1.16.2.tar.gz
#tar xf mongo-c-driver-1.16.2.tar.gz
#rm -f mongo-c-driver-1.16.2.tar.gz
#cd mongo-c-driver-1.16.2/
#cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw64/ -DCMAKE_C_FLAGS="-D__USE_MINGW_ANSI_STDIO=1" .
#mingw32-make -j4 install
#cd /tmp
#rm -rf mongo-c-driver-1.16.2

VERSION=1.26.2
wget "https://github.com/mongodb/mongo-c-driver/archive/refs/tags/$VERSION.tar.gz" --output-document="mongo-c-driver-$VERSION.tar.gz"
tar xf "mongo-c-driver-$VERSION.tar.gz"
rm -f "mongo-c-driver-$VERSION.tar.gz"
cd mongo-c-driver-$VERSION/ && mkdir _build && cmake -G "MinGW Makefiles" -S . -B _build \
-D ENABLE_EXTRA_ALIGNMENT=OFF \
-D ENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -D ENABLE_TESTS=OFF -D ENABLE_EXAMPLES=OFF \
-D CMAKE_BUILD_TYPE=RelWithDebInfo \
-D BUILD_VERSION="$VERSION" \
-D ENABLE_SSL=OFF \
-D ENABLE_SASL=OFF -DCMAKE_INSTALL_PREFIX=/mingw64/ -DCMAKE_C_FLAGS="-D__USE_MINGW_ANSI_STDIO=1" \
-D ENABLE_MONGOC=ON && cmake --build _build --config RelWithDebInfo --parallel && cmake --install _build
rm -rf "mongo-c-driver-$VERSION"

cd /tmp/ffead-cpp-src
mkdir build
cd build
cmake -G "MinGW Makefiles" -DSRV_EMB=on -DMOD_REDIS=ON -DMOD_SDORM_MONGO=ON -DCMAKE_INC_PATH=/mingw64/ ..
mingw32-make -j4 install
mv /tmp/ffead-cpp-src/ffead-cpp-7.0-bin /tmp/
cd /tmp/ffead-cpp-7.0-bin && chmod +x *.sh
export PATH=/tmp/ffead-cpp-7.0-bin/lib:/mingw64/bin:/mingw64/lib:$PATH
nohup bash -c "./server.sh > ffead.log &"
echo "Waiting for ffead-cpp to launch on port 8080..."
COUNTER=0
while [ ! -f lib/libinter.dll ]
do
  sleep 1
  COUNTER=$((COUNTER+1))
  if [ "$COUNTER" = 600 ]
  then
  	cat ffead.log
  	cat logs/jobs.log
    echo "ffead-cpp exiting due to failure...."
    exit 1
  fi
done
COUNTER=0
while [ ! -f lib/libdinter.dll ]
do
  sleep 1
  COUNTER=$((COUNTER+1))
  if [ "$COUNTER" = 120 ]
  then
  	cat ffead.log
  	cat logs/jobs.log
    echo "ffead-cpp exiting due to failure....dlib"
    exit 1
  fi
done
echo "ffead-cpp start successful"
#sleep 5
#cd tests && chmod +x *.sh && ./runTests.sh
#cd -
echo "ffead-cpp normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp
#cd /tmp/ffead-cpp-src
#chmod +x autogen.sh
#sed -i'' -e "s|m4_include|#m4_include|g" configure.ac
#sed -i'' -e "s|AX_CXX_COMPILE_STDCXX|#AX_CXX_COMPILE_STDCXX|g" configure.ac
#sed -i'' -e "s|AC_CHECK_LIB(regex|#AC_CHECK_LIB(regex|g" configure.ac
#sed -i'' -e 's|LIBS="-lwsock32 -lws2_32 -lkernel32 -lregex -lssp|LIBS="-lwsock32 -lws2_32 -lkernel32 -lregex -lssp -ldl|g' configure.ac
#./autogen.sh
#env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc CXX=${TARGET}-g++ AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include \
#	CPPFLAGS="-I/mingw64/include -std=c++17" LDFLAGS="-L/mingw64/lib -fstack-protector" lt_cv_deplibs_check_method=pass_all ./configure --host="${TARGET}" \
#	--enable-srv_emb=yes --enable-mod_sdormmongo=yes --enable-mod_sdormsql=yes --enable-mod_rediscache=yes --with-top_inc_dir=/mingw64/include
#make install -j4
#cd /tmp
#rm -rf /tmp/ffead-cpp-src
