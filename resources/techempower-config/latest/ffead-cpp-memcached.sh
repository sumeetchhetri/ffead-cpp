#!/bin/bash

cd $IROOT

apt update -yqq && apt install -yqq memcached 

wget https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-1.0.18.tar.gz
tar xvf libmemcached-1.0.18.tar.gz
cd ${IROOT}/libmemcached-1.0.18/
./configure --prefix=${IROOT}
make
make install
