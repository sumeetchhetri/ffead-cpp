#!/bin/bash

apt update -yqq && apt install --no-install-recommends -yqq autoconf-archive unzip uuid-dev odbc-postgresql unixodbc unixodbc-dev \
	apache2 apache2-dev libapr1-dev libaprutil1-dev memcached libmemcached-dev redis-server libssl-dev \
	zlib1g-dev cmake make clang-format-11 ninja-build libmongoc-dev libpq-dev

#redis will not start correctly on bionic with this config
sed -i "s/bind .*/bind 127.0.0.1/g" /etc/redis/redis.conf

service apache2 stop
service memcached stop
service redis-server stop

cd $IROOT
wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make install
cd $IROOT
rm -rf libcuckoo-master

wget -q https://github.com/redis/hiredis/archive/v1.0.0.tar.gz
tar xf v1.0.0.tar.gz
rm -f v1.0.0.tar.gz
cd hiredis-1.0.0/
cmake . && make install
cd $IROOT
rm -rf hiredis-1.0.0

wget -q https://github.com/sewenew/redis-plus-plus/archive/refs/tags/1.3.3.tar.gz
tar xf 1.3.3.tar.gz
rm -f 1.3.3.tar.gz
cd redis-plus-plus-1.3.3/
mkdir build
cd build
cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=17 .. && make && make install
cd $IROOT
rm -rf redis-plus-plus-1.3.3

mkdir -p /usr/lib/x86_64-linux-gnu/odbc
wget -q https://downloads.mysql.com/archives/get/p/10/file/mysql-connector-odbc-8.0.19-linux-ubuntu18.04-x86-64bit.tar.gz
tar xf mysql-connector-odbc-8.0.19-linux-ubuntu18.04-x86-64bit.tar.gz
mv mysql-connector-odbc-8.0.19-linux-ubuntu18.04-x86-64bit/lib/libmyodbc8* /usr/lib/x86_64-linux-gnu/odbc/
mysql-connector-odbc-8.0.19-linux-ubuntu18.04-x86-64bit/bin/myodbc-installer -d -a -n "MySQL" -t "DRIVER=/usr/lib/x86_64-linux-gnu/odbc/libmyodbc8w.so;"
rm -f mysql-connector-odbc-8.0.19-linux-ubuntu18.04-x86-64bit.tar.gz
rm -rf mysql-connector-odbc-8.0.19-linux-ubuntu18.04-x86-64bit

cd $IROOT
wget -q https://github.com/microsoft/mimalloc/archive/v1.6.3.tar.gz
tar xf mimalloc-1.6.3.tar.gz
cd mimalloc-1.6.3
mkdir -p out/release
cmake ../.. -DCMAKE_BUILD_TYPE=Release
make && make install
cd $IROOT
rm -rf mimalloc-1.6.3

wget -q https://github.com/microsoft/snmalloc/archive/0.4.2.tar.gz
tar xf snmalloc-0.4.2.tar.gz
cd snmalloc-0.4.2
mkdir build
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release
ninja
cd $IROOT
