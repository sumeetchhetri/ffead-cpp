#!/bin/sh

cd tmp
wget http://nginx.org/download/nginx-1.11.3.tar.gz
tar xvzf nginx-1.11.3.tar.gz
cd nginx-1.11.3
./configure --with-ld-opt="-lstdc++ ${3} -L${1}/lib" --add-module=${4} --with-cc-opt="${2} -I${1}/include -w -fpermissive"
make install
