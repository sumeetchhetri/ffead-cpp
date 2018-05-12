#!/bin/bash

cd $IROOT

apt update -yqq && apt install -yqq unzip uuid-dev odbc-postgresql unixodbc unixodbc-dev 

# libmyodbc has been removed from apt

wget -q http://www.mirrorservice.org/sites/ftp.mysql.com/Downloads/Connector-ODBC/5.3/mysql-connector-odbc-5.3.10-linux-ubuntu16.04-x86-64bit.tar.gz
tar xf mysql-connector-odbc-5.3.10-linux-ubuntu16.04-x86-64bit.tar.gz
mkdir -p /usr/lib/x86_64-linux-gnu/odbc
mv mysql-connector-odbc-5.3.10-linux-ubuntu16.04-x86-64bit/lib/libmyodbc5* /usr/lib/x86_64-linux-gnu/odbc/ 
mysql-connector-odbc-5.3.10-linux-ubuntu16.04-x86-64bit/bin/myodbc-installer -d -a -n "MySQL" -t "DRIVER=/usr/lib/x86_64-linux-gnu/odbc/libmyodbc5w.so;"

# mongocdriver also used in all tests

wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.4.0/mongo-c-driver-1.4.0.tar.gz
tar xf mongo-c-driver-1.4.0.tar.gz
cd mongo-c-driver-1.4.0/ && \
    ./configure --prefix=${IROOT} --libdir=${IROOT} --disable-automatic-init-and-cleanup && \
    make && make install
	

	