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
cp lib/* /mingw64/lib/
cp -rf share/* /mingw64/share/
cp -rf etc/* /mingw64/etc/

cd /tmp
wget -q https://github.com/sean-/ossp-uuid/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd ossp-uuid-master
cp /tmp/ffead-cpp-src/docker/files/config.sub .
./configure --prefix=/mingw64/ --without-pgsql --without-perl --without-php --disable-static --enable-shared
make
mv uuid.exe uuid
make install
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

cd /tmp/ffead-cpp-src
cmake -G "MinGW Makefiles" -DSRV_EMB=on -DMOD_REDIS=on -DCMAKE_INC_PATH=/mingw64/ .
mingw32-make install -j4
mv /tmp/ffead-cpp-src/ffead-cpp-4.0-bin /tmp/
cd /tmp
rm -rf /tmp/ffead-cpp-src


wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.16.2/mongo-c-driver-1.16.2.tar.gz
tar xf mongo-c-driver-1.16.2.tar.gz
rm -f mongo-c-driver-1.16.2.tar.gz
cd mongo-c-driver-1.16.2/
CC=/mingw64/bin/gcc.exe /mingw64/bin/cmake -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX="C:/msys64/mingw64/" -DCMAKE_C_FLAGS="-D__USE_MINGW_ANSI_STDIO=1"
make install
cd /tmp
rm -rf mongo-c-driver-1.16.2
