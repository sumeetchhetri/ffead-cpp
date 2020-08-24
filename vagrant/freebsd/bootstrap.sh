#!/usr/bin/env sh

# Prepares a virtual machine for running ffead-cpp

# Install prerequisite tools
echo "Installing dependencies"
export BATCH=yes
env PAGER=cat portsnap fetch extract --not-running-from-cron

pkg install -y perl5 cmake openssl unixODBC hiredis libmemcached e2fsprogs-libuuid wget gdb bash

cd /tmp
wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.4.0/mongo-c-driver-1.4.0.tar.gz
tar xf mongo-c-driver-1.4.0.tar.gz
rm -f mongo-c-driver-1.4.0.tar.gz
cd mongo-c-driver-1.4.0/ && \
./configure --with-libbson=bundled --disable-automatic-init-and-cleanup && \
make && make install
cd /tmp
rm -rf mongo-c-driver-1.4.0

cd /tmp
wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip master.zip
rm -f master.zip
cd libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make install
cd /tmp
rm -rf libcuckoo-master

mkdir /opt
cd /opt
rm -rf ffead-cpp-src
wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/v4.0.zip
unzip v4.0.zip
rm -f v4.0.zip
mv ffead-cpp-5.0 ffead-cpp-src
cd ffead-cpp-src
cmake -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on .
make install -j4

# Setting up passwordless sudo
echo "vagrant ALL=(ALL:ALL) NOPASSWD: ALL" | sudo tee -a /etc/sudoers

# Set current shell to bash
#chsh -s /usr/local/bin/bash root
#chsh -s bash
#finger vivek
