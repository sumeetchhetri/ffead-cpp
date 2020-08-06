wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip -qq master.zip
mv ffead-cpp-master ffead-cpp-src
rm -f master.zip
cd /tmp/ffead-cpp-src/docker/files
unzip -qq mingw64-dlfcn.zip
cd mingw64
cp bin/* /mingw64/bin/
cp include/* /mingw64/include/
cp lib/* /mingw64/lib/
cd /tmp/ffead-cpp-src/docker/files
rm -rf mingw64
unzip -qq mingw64-unixodbc.zip
cd mingw64
cp bin/* /mingw64/bin/
cp include/* /mingw64/include/
cp -rf lib/* /mingw64/lib/
cp -rf share/* /mingw64/share/
cp -rf etc/* /mingw64/etc/

TARGET=x86_64-w64-mingw32
cd /tmp
wget -q https://github.com/sean-/ossp-uuid/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd ossp-uuid-master
env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar \
	RANLIB=${TARGET}-ranlib CFLAGS="-D_FORTIFY_SOURCE=2" LDFLAGS="-lssp" ./configure --prefix=/mingw64/ \
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

wget -q https://github.com/redis/hiredis/archive/8e0264cfd6889b73c241b60736fe96ba1322ee6e.zip
unzip 8e0264cfd6889b73c241b60736fe96ba1322ee6e.zip
rm -f 8e0264cfd6889b73c241b60736fe96ba1322ee6e.zip
cd hiredis-8e0264cfd6889b73c241b60736fe96ba1322ee6e
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw64/ . && mingw32-make install
cd /tmp
rm -rf hiredis-8e0264cfd6889b73c241b60736fe96ba1322ee6e

wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.16.2/mongo-c-driver-1.16.2.tar.gz
tar xf mongo-c-driver-1.16.2.tar.gz
rm -f mongo-c-driver-1.16.2.tar.gz
cd mongo-c-driver-1.16.2/
CC=/mingw64/bin/gcc.exe /mingw64/bin/cmake -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX="C:/msys64/mingw64/" -DCMAKE_C_FLAGS="-D__USE_MINGW_ANSI_STDIO=1"
make install
cd /tmp
rm -rf mongo-c-driver-1.16.2

cd /tmp/ffead-cpp-src
mkdir build
cd build
cmake -G "MinGW Makefiles" -DSRV_EMB=on -DMOD_REDIS=ON -DMOD_SDORM_MONGO=ON -DCMAKE_INC_PATH=/mingw64/ ..
mingw32-make install -j4
mv /tmp/ffead-cpp-src/ffead-cpp-4.0-bin /tmp/
cd /tmp/ffead-cpp-src
WORKDIR /tmp/ffead-cpp-src
chmod +x autogen.sh
sed -i'' -e "s|m4_include|#m4_include|g" configure.ac
sed -i'' -e "s|AX_CXX_COMPILE_STDCXX|#AX_CXX_COMPILE_STDCXX|g" configure.ac
sed -i'' -e "s|AC_CHECK_LIB(regex|#AC_CHECK_LIB(regex|g" configure.ac
./autogen.sh
env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc CXX=${TARGET}-g++ AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include \
	CPPFLAGS="-I/mingw64/include -std=c++17" LDFLAGS="-L/mingw64/lib -fstack-protector" ./configure --host="${TARGET}" --enable-srv_emb=yes --enable-mod_sdormmongo=yes \
	--enable-mod_sdormsql=yes --enable-mod_rediscache=yes --with-top_inc_dir=/mingw64/include
make install -j4
cd /tmp
rm -rf /tmp/ffead-cpp-src
