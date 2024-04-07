
# prepare PostgreSQL APT repository
cp pgdg.list /etc/apt/sources.list.d/
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add -

# prepare mysql APT repository
cp mysql.list /etc/apt/sources.list.d/
apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 8C718D3B5072E1F5

# prepare mongodb APT repository
apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 4B7C549A058F8B6B
echo "deb https://repo.mongodb.org/apt/ubuntu bionic/mongodb-org/4.2 multiverse" | tee /etc/apt/sources.list.d/mongodb-org.list

apt-get -yqq update > /dev/null
apt-get -yqq install locales autoconf-archive autoconf automake libtool gcc g++ cmake unzip libpcre3-dev zlib1g-dev \
	libpq-dev libssl-dev uuid-dev odbc-postgresql unixodbc unixodbc-dev libcurl4-openssl-dev libmemcached-dev wget netcat \
	memcached redis-server gdb ninja-build telnet net-tools vim

#POSTGRESQL
# install postgresql on database machine
apt-get -yqq update && apt-get -yqq install -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" postgresql postgresql-contrib && \
	rm -rf /var/lib/apt/lists/*

# Make sure all the configuration files in main belong to postgres
mv postgresql.conf /etc/postgresql/${PG_VERSION}/main/postgresql.conf
mv pg_hba.conf /etc/postgresql/${PG_VERSION}/main/pg_hba.conf

chown -Rf postgres:postgres /etc/postgresql/${PG_VERSION}/main

mkdir /ssd
cp -R -p /var/lib/postgresql/${PG_VERSION}/main /ssd/postgresql
cp /etc/postgresql/${PG_VERSION}/main/postgresql.conf /ssd/postgresql
mv 60-postgresql-shm.conf /etc/sysctl.d/60-postgresql-shm.conf

chown -Rf postgres:postgres /var/run/postgresql
chmod 2777 /var/run/postgresql
chown postgres:postgres /etc/sysctl.d/60-postgresql-shm.conf
chown postgres:postgres create-postgres*
chown -Rf postgres:postgres /ssd

#MYSQL
# https://bugs.mysql.com/bug.php?id=90695
["/bin/bash", "-c", "debconf-set-selections <<< \"mysql-server mysql-server/lowercase-table-names select Enabled\""]
["/bin/bash", "-c", "debconf-set-selections <<< \"mysql-community-server mysql-community-server/data-dir select 'Y'\""]
["/bin/bash", "-c", "debconf-set-selections <<< \"mysql-community-server mysql-community-server/root-pass password secret\""]
["/bin/bash", "-c", "debconf-set-selections <<< \"mysql-community-server mysql-community-server/re-root-pass password secret\""]
echo "Installing mysql-server version: $(apt-cache policy mysql-server | grep -oP "(?<=Candidate: )(.*)$")"
DEBIAN_FRONTEND=noninteractive apt-get -yqq update && apt-get -y install mysql-server > /dev/null

mv /etc/mysql/my.cnf /etc/mysql/my.cnf.orig
cp my.cnf /etc/mysql/my.cnf

rm -rf /ssd/mysql
rm -rf /ssd/log/mysql
cp -R -p /var/lib/mysql /ssd/
cp -R -p /var/log/mysql /ssd/log
mkdir -p /var/run/mysqld

# It may seem weird that we call `service mysql start` several times, but the RUN
# directive is a 1-time operation for building this image. Subsequent calls
# do not see running processes from prior calls; therefor, each command here
# that relies on the mysql server running will explicitly start the server and
# perform the work required.

#chown -R mysql:mysql /var/lib/mysql /var/log/mysql /var/run/mysqld /ssd && \
#    mysqld & \
#    until mysql -uroot -psecret -e "exit"; do sleep 1; done && \
#    mysqladmin -uroot -psecret flush-hosts && \
#    mysql -uroot -psecret < create.sql

chown -R mysql:mysql /var/lib/mysql /var/log/mysql /var/run/mysqld /ssd
#MYSQL


#MONGODB
DEBIAN_FRONTEND=noninteractive apt-get -yqq update && apt-get -yqq install apt-transport-https mongodb-org=4.2.10  mongodb-org-mongos=4.2.10 mongodb-org-server=4.2.10 mongodb-org-shell=4.2.10 mongodb-org-tools=4.2.10 > /dev/null

mkdir -p /data/db
chmod 777 /data/db

mongod --fork --logpath /var/log/mongodb.log --bind_ip_all && sleep 10 && mongo < create.js && sleep 10
#MONGODB

#Install libcuckoo headers
cd  /tmp
wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd  /tmp/libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ .
make install
cd  /tmp
rm -rf /tmp/libcuckoo-master

wget -q https://github.com/redis/hiredis/archive/v1.0.0.tar.gz
tar xf v1.0.0.tar.gz
rm -f v1.0.0.tar.gz
cd hiredis-1.0.0/ && cmake . && make install
cd  /tmp
rm -rf hiredis-1.0.0

wget -q https://github.com/sewenew/redis-plus-plus/archive/refs/tags/1.3.10.tar.gz
tar xf 1.3.10.tar.gz
rm -f 1.3.10.tar.gz
cd redis-plus-plus-1.3.10/
mkdir build
cd build
cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=17 .. && make && make install
cd /tmp
rm -rf redis-plus-plus-1.3.10

#Install mongodb c driver
#wget -q https://github.com/mongodb/mongo-c-driver/releases/download/1.4.2/mongo-c-driver-1.4.2.tar.gz
#tar xf mongo-c-driver-1.4.2.tar.gz
#rm -f mongo-c-driver-1.4.2.tar.gz
#cd mongo-c-driver-1.4.2/ && ./configure --disable-automatic-init-and-cleanup --disable-tests --disable-ssl --disable-sasl && make && make install
#cd  /tmp
#rm -rf mongo-c-driver-1.4.2
ENV VERSION=1.26.2
RUN wget "https://github.com/mongodb/mongo-c-driver/archive/refs/tags/$VERSION.tar.gz" --output-document="mongo-c-driver-$VERSION.tar.gz"
RUN tar xf "mongo-c-driver-$VERSION.tar.gz"
RUN rm -f "mongo-c-driver-$VERSION.tar.gz"
RUN cd mongo-c-driver-$VERSION/ && mkdir _build && cmake -S . -B _build \
-D ENABLE_EXTRA_ALIGNMENT=OFF \
-D ENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
-D CMAKE_BUILD_TYPE=RelWithDebInfo \
-D BUILD_VERSION="$VERSION" \
-D ENABLE_SSL=OFF \
-D ENABLE_SASL=OFF \
-D ENABLE_MONGOC=ON && cmake --build _build --config RelWithDebInfo --parallel && cmake --install _build
RUN rm -rf "mongo-c-driver-$VERSION"

#WRK
cd  /tmp/wrk
apt-get -yqq update && apt-get -yqq install libluajit-5.1-dev libssl-dev luajit && rm -rf /var/lib/apt/lists/* && \
	curl -sL https://github.com/wg/wrk/archive/4.1.0.tar.gz | tar xz --strip-components=1
export LDFLAGS="-O3 -march=native -flto"
export CFLAGS="-I /usr/include/luajit-2.1 $LDFLAGS"
make WITH_LUAJIT=/usr WITH_OPENSSL=/usr -j "$(nproc)"
cp wrk /usr/local/bin
#WRK

cd  /tmp
wget -q https://github.com/axboe/liburing/archive/liburing-0.7.tar.gz
tar xf liburing-0.7.tar.gz
rm -f liburing-0.7.tar.gz
cd liburing-liburing-0.7 && ./configure --prefix=/usr/local && make install
cd /tmp && rm -rf liburing-liburing-0.7

#Install ffead-cpp
cd  /tmp
wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip -qq master.zip
mv ffead-cpp-master ffead-cpp-src
rm -f master.zip

apt remove -yqq libpq-dev
apt autoremove -yqq
rm -f /usr/lib/x86_64-linux-gnu/libpq.*
apt update && apt install -y bison flex libreadline-dev
cd  /tmp
wget -nv https://github.com/postgres/postgres/archive/b787d4ce6d910080065025bcd5f968544997271f.zip
unzip -q b787d4ce6d910080065025bcd5f968544997271f.zip
cd  /tmp/postgres-b787d4ce6d910080065025bcd5f968544997271f
wget -nv https://www.postgresql.org/message-id/attachment/115223/v22-0001-libpq-batch.patch
git apply ./v22-0001-libpq-batch.patch
./configure --prefix=/usr CFLAGS='-O3 -march=native -flto'
cd  src/interfaces/libpq
make all install -j4
cp ../../../src/include/postgres_ext.h ../../../src/include/pg_config_ext.h libpq-fe.h /usr/include
