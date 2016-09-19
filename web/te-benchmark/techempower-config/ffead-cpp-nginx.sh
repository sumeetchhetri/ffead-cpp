#!/bin/bash

RETCODE=$(fw_exists ${IROOT}/ffead-cpp-nginx.installed)
[ ! "$RETCODE" == 0 ] || { return 0; }

sudo apt-get remove libodbc1 unixodbc unixodbc-dev
sudo apt-get install build-essential
sudo apt-get install -y uuid-dev libmyodbc odbc-postgresql

fw_get -o unixODBC-2.3.4.tar.gz ftp://ftp.unixodbc.org/pub/unixODBC/unixODBC-2.3.4.tar.gz
fw_untar unixODBC-2.3.4.tar.gz
cd unixODBC-2.3.4
./configure --enable-stats=no --enable-gui=no --enable-drivers=no --enable-iconv --with-iconv-char-enc=UTF8 --with-iconv-ucode-enc=UTF16LE --libdir=/usr/lib/x86_64-linux-gnu --prefix=/usr --sysconfdir=/etc

fw_get -o ffead-cpp-2.0.tar.gz https://github.com/sumeetchhetri/ffead-cpp/releases/download/v2.0-Draft-TLV-Fixed-TE_Benchmark/ffead-cpp-2.0-tlfixed-bin.tar.gz
fw_untar ffead-cpp-2.0.tar.gz

rm -rf ${TROOT}/ffead-cpp-2.0-bin
cp -R ffead-cpp-2.0-bin/ ${TROOT}
mv ${TROOT}/ffead-cpp-2.0-bin ${TROOT}/ffead-cpp-2.0
rm -rf ffead-cpp-2.0/

fw_get -o nginx-1.11.3.tar.gz http://nginx.org/download/nginx-1.11.3.tar.gz
tar xvzf nginx-1.11.3.tar.gz
cd nginx-1.11.3
./configure --with-ld-opt="-lstdc++ -L${TROOT}/ffead-cpp-2.0/lib" --add-module=${TROOT}/ffead-cpp-2.0/ngx_mod --with-cc-opt="${TROOT}/ffead-cpp-2.0/include -w -fpermissive"
sudo make install

sudo sed -i 's|localhost|'${DBHOST}'|g' ${TROOT}/ffead-cpp-2.0/web/te-benchmark/config/sdorm.xml

sudo rm -f /etc/odbcinst.ini
sudo rm -f /etc/odbc.ini

sudo cp ${TROOT}/ffead-cpp-2.0/resources/sample-odbcinst.ini /etc/odbcinst.ini
sudo cp ${TROOT}/ffead-cpp-2.0/resources/sample-odbc.ini /etc/odbc.ini

sudo sed -i 's|localhost|'${DBHOST}'|g' /etc/odbc.ini

wget https://github.com/mongodb/mongo-c-driver/releases/download/1.4.0/mongo-c-driver-1.4.0.tar.gz
tar -xzf mongo-c-driver-1.4.0.tar.gz
cd mongo-c-driver-1.4.0/
./configure --prefix=${IROOT} --libdir=${IROOT} --disable-automatic-init-and-cleanup
make && sudo make install

sudo bash -c 'cat > /usr/local/nginx/conf/nginx.conf <<EOL
Listen 8080
FFEAD_CPP_PATH '"${FFEADROOT}"'
<VirtualHost *:8080>
	DocumentRoot '"${FFEADROOT}"'/web
	SetHandler ffead_cpp_module
	<Directory '"${FFEADROOT}"'>
		Options FollowSymLinks
		AllowOverride None
		Require all denied
	</Directory>
	<Directory '"${FFEADROOT}"'/web/>
		Options -Indexes +FollowSymLinks +MultiViews
		AllowOverride All
		Require all granted
	</Directory>
</VirtualHost>
EOL'

sudo chown -R www-data:www-data ${TROOT}/ffead-cpp-2.0
sudo chmod -R g+rw ${TROOT}/ffead-cpp-2.0

touch ${IROOT}/ffead-cpp-nginx.installed
