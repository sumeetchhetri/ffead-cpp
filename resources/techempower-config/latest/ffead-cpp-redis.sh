#!/bin/bash

cd $IROOT

apt update -yqq && apt install -yqq software-properties-common
add-apt-repository -y ppa:chris-lea/redis-server
apt install redis-server

wget https://github.com/redis/hiredis/archive/v0.13.3.tar.gz
tar xvf v0.13.3.tar.gz
cd hiredis-0.13.3/
make
PREFIX=${IROOT}/ make install
