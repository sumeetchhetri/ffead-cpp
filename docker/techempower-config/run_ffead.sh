#!/bin/sh

echo never > /sys/kernel/mm/transparent_hugepage/enabled
echo 'echo never > /sys/kernel/mm/transparent_hugepage/enabled' >> /etc/rc.local
sysctl vm.overcommit_memory=1

export PATH=${IROOT}/nginxfc/sbin:${PATH}
export LD_LIBRARY_PATH=${IROOT}/:${IROOT}/lib:${FFEAD_CPP_PATH}/lib:/usr/local/lib:$LD_LIBRARY_PATH
export ODBCINI=${IROOT}/odbc.ini
export ODBCSYSINI=${IROOT}

cd $FFEAD_CPP_PATH

#use below settings only for debugging
#echo '/tmp/core.%h.%e.%t' > /proc/sys/kernel/core_pattern
#ulimit -c unlimited

service redis-server stop
service apache2 stop
service memcached stop

rm -f /tmp/cache.lock
rm -f web/te-benchmark-um/config/cache.xml

if [ $2 = "redis" ]
then
	service redis-server start
	cp -f web/te-benchmark-um/config/cacheredis.xml web/te-benchmark-um/config/cache.xml
	cp -f web/te-benchmark-um/config/sdormmongo.xml web/te-benchmark-um/config/sdorm.xml
fi

if [ $2 = "memcached" ]
then
	service memcached start
	cp -f web/te-benchmark-um/config/cachememcached.xml web/te-benchmark-um/config/cache.xml
	cp -f web/te-benchmark-um/config/sdormmongo.xml web/te-benchmark-um/config/sdorm.xml
fi

if [ $2 = "mongo" ]
then
	cp -f web/te-benchmark-um/config/sdormmongo.xml web/te-benchmark-um/config/sdorm.xml
fi

if [ $2 = "mysql" ]
then
	cp -f web/te-benchmark-um/config/sdormmysql.xml web/te-benchmark-um/config/sdorm.xml
fi

if [ $2 = "postgresql" ]
then
	cp -f web/te-benchmark-um/config/sdormpostgresql.xml web/te-benchmark-um/config/sdorm.xml
fi

rm -f rtdcf/*.d rtdcf/*.o 
rm -f *.cntrl
rm -f tmp/*.sess
if [ ! -d tmp ]; then
mkdir tmp
fi
chmod 700 ffead-cpp*
chmod 700 resources/*.sh
chmod 700 tests/*
chmod 700 rtdcf/*

if [ $1 = "emb" ]
then
	for i in $(seq 0 $(($(nproc --all)-1))); do
	  taskset -c $i ./ffead-cpp $FFEAD_CPP_PATH &
	done
fi

if [ $1 = "apache" ]
then
	sed -i 's|<pool-size>30</pool-size>|<pool-size>3</pool-size>|g' $FFEAD_CPP_PATH/web/te-benchmark-um/config/sdorm.xml
	sed -i 's|<pool-size>10</pool-size>|<pool-size>2</pool-size>|g' $FFEAD_CPP_PATH/web/te-benchmark-um/config/cache.xml
	apachectl -D FOREGROUND
fi

if [ $1 = "nginx" ]
then
	sed -i 's|<pool-size>30</pool-size>|<pool-size>3</pool-size>|g' $FFEAD_CPP_PATH/web/te-benchmark-um/config/sdorm.xml
	sed -i 's|<pool-size>10</pool-size>|<pool-size>2</pool-size>|g' $FFEAD_CPP_PATH/web/te-benchmark-um/config/cache.xml
	nginx -g 'daemon off;'
fi

if [ $1 = "libreactor" ]
then
	cd ${IROOT}
	./libreactor-ffead-cpp $FFEAD_CPP_PATH 8080
fi

if [ $1 = "crystal-http" ]
then
	cd ${IROOT}
	./crystal-ffead-cpp.out --ffead-cpp-dir=$FFEAD_CPP_PATH --to=8080
fi

if [ $1 = "crystal-h2o" ]
then
	cd ${IROOT}
	./h2o-evloop-ffead-cpp --ffead-cpp-dir=$FFEAD_CPP_PATH --to=8080
fi

if [ $1 = "rust-actix" ]
then
	cd ${IROOT}
	./actix-ffead-cpp $FFEAD_CPP_PATH 8080
fi

if [ $1 = "rust-hyper" ]
then
	cd ${IROOT}
	./hyper-ffead-cpp $FFEAD_CPP_PATH 8080
fi

if [ $1 = "rust-thruster" ]
then
	cd ${IROOT}
	./thruster-ffead-cpp $FFEAD_CPP_PATH 8080
fi

if [ $1 = "rust-rocket" ]
then
	cd ${IROOT}
	./rocket-ffead-cpp $FFEAD_CPP_PATH 8080
fi

if [ $1 = "go-fasthttp" ]
then
	cd ${IROOT}
	./fasthttp-ffead-cpp --server_directory=$FFEAD_CPP_PATH -addr=8080
fi

if [ $1 = "go-gnet" ]
then
	cd ${IROOT}
	./gnet-ffead-cpp --server_directory=$FFEAD_CPP_PATH --port=8080
fi

if [ $1 = "v-vweb" ]
then
	cd ${IROOT}
	./vweb --server_dir=$FFEAD_CPP_PATH --server_port=8080
fi

if [ $1 = "java-firenio" ]
then
	cd ${IROOT}
	java -classpath firenio-ffead-cpp-0.1-jar-with-dependencies.jar com.firenio.ffeadcpp.FirenioFfeadCppServer $FFEAD_CPP_PATH 8080
fi

if [ $1 = "java-rapidoid" ]
then
	cd ${IROOT}
	java -classpath rapidoid-ffead-cpp-1.0-jar-with-dependencies.jar com.rapidoid.ffeadcpp.Main $FFEAD_CPP_PATH 8080
fi

if [ $1 = "java-wizzardo-http" ]
then
	cd ${IROOT}
	java -jar wizzardo-ffead-cpp-all-1.0.jar $FFEAD_CPP_PATH 8080
fi

wait
