if ! nc -z localhost 5432 ; then   
  /run.sh
fi

ulimit -c unlimited
export FFEAD_CPP_PATH=/tmp/ffead-cpp-sql-orm/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-sql-orm
cp -f web/te-benchmark-um/config/cachememory.xml web/te-benchmark-um/config/cache.xml
if [ $# -eq 0 ]; then
	cp /server_orig.sh server.sh
	nohup bash -c "./server.sh > ffead.log &"
	sleep 10
	echo "ffead-cpp with sql-orm support launched"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/json"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/plaintext" -s /pipeline.lua -- 16
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/fortunes"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/db"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/queries?queries=20"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/updates?queries=20"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9784/te-benchmark-um/cached-worlds?count=100"
	sleep 10
	pkill ffead-cpp
fi
if [ "$1" == "g" ]; then
	gdb ffead-cpp
fi