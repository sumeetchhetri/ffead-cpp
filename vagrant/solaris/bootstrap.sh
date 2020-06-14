#!/usr/bin/env sh

# Prepares a virtual machine for running ffead-cpp

# Install prerequisite tools
echo "Installing dependencies"

pkg install cmake openssl libmemcached gdb gcc system/header

pkgadd -d http://get.opencsw.org/now
/opt/csw/bin/pkgutil -U

/opt/csw/bin/pkgutil -y -i /opt/csw/bin/wget

cd /tmp
/opt/csw/bin/wget --no-check-certificate -q https://github.com/mongodb/mongo-c-driver/releases/download/1.4.0/mongo-c-driver-1.4.0.tar.gz 
tar xf mongo-c-driver-1.4.0.tar.gz
rm -f mongo-c-driver-1.4.0.tar.gz
cd mongo-c-driver-1.4.0/ && \
./configure --with-libbson=bundled --disable-automatic-init-and-cleanup && \
make && make install
cd /tmp
rm -rf mongo-c-driver-1.4.0

/opt/csw/bin/wget --no-check-certificate -q https://github.com/redis/hiredis/archive/v0.13.3.tar.gz
tar xvf v0.13.3.tar.gz
rm -f v0.13.3.tar.gz
cd hiredis-0.13.3/
make
PREFIX=/usr make install
cd /tmp
rm -rf hiredis-0.13.3

cd /tmp
/opt/csw/bin/wget --no-check-certificate -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip master.zip
rm -f master.zip
cd libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make install
cd /tmp
rm -rf libcuckoo-master

cd /tmp
/opt/csw/bin/wget --no-check-certificate -q http://www.unixodbc.org/unixODBC-2.3.7.tar.gz
tar xvf unixODBC-2.3.7.tar.gz
rm -f unixODBC-2.3.7.tar.gz
cd unixODBC-2.3.7
./configure
make
make install
cd /tmp
rm -rf unixODBC-2.3.7

mkdir /opt
cd /opt
rm -rf ffead-cpp-src
/opt/csw/bin/wget --no-check-certificate -q https://github.com/sumeetchhetri/ffead-cpp/archive/v4.0.zip
unzip v4.0.zip
rm -f v4.0.zip
mv ffead-cpp-4.0 ffead-cpp-src
cd ffead-cpp-src
cmake -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on .
make install -j4

# Setting up passwordless sudo
echo "vagrant ALL=(ALL:ALL) NOPASSWD: ALL" | sudo tee -a /etc/sudoers

# Set current shell to bash
#chsh -s /usr/local/bin/bash root
#chsh -s bash
#finger vivek
