#!/usr/bin/env sh

# Prepares a virtual machine for running ffead-cpp

# Install prerequisite tools
echo "Installing dependencies"

pkg install cmake openssl libmemcached gdb gcc system/header autoconf automake libtool

#pkgadd -d http://get.opencsw.org/now
#/opt/csw/bin/pkgutil -U

#cd /tmp
#wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.4.0/mongo-c-driver-1.4.0.tar.gz 
#tar xf mongo-c-driver-1.4.0.tar.gz
#rm -f mongo-c-driver-1.4.0.tar.gz
#cd mongo-c-driver-1.4.0/ && \
#./configure --with-libbson=bundled --disable-automatic-init-and-cleanup && \
#make && make install
#cd /tmp
#rm -rf mongo-c-driver-1.4.0

wget -q https://github.com/redis/hiredis/archive/v1.0.0.tar.gz
gtar xzf v1.0.0.tar.gz
rm -f v1.0.0.tar.gz
cd hiredis-1.0.0/
cmake . && make install
cd /tmp
rm -rf hiredis-1.0.0

wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip master.zip
rm -f master.zip
cd libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make install
cd /tmp
rm -rf libcuckoo-master

cd /tmp
wget -q https://ftp.osuosl.org/pub/blfs/conglomeration/unixODBC/unixODBC-2.3.7.tar.gz
tar xf unixODBC-2.3.7.tar.gz
rm -f unixODBC-2.3.7.tar.gz
cd unixODBC-2.3.7
./configure
make
make install
cd /tmp
rm -rf unixODBC-2.3.7

crle -u -s /usr/local/lib/
crle -64 -u -s /usr/local/lib/

wget -q https://github.com/ninja-build/ninja/archive/v1.10.2.zip
unzip v1.10.2.zip
cd ninja-1.10.2/
cmake -Bbuild-cmake -H.
cmake --build build-cmake
cd build-cmake/
mv ninja /usr/bin/
cd /tmp
rm -rf ninja-1.10.2/ v1.10.2.zip

mkdir /opt
cd /opt
rm -rf ffead-cpp-src
wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip master.zip
rm -f master.zip
mv ffead-cpp-master ffead-cpp-src
cd ffead-cpp-src
cmake -GNinja -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DDEBUG=on .
ninja install
