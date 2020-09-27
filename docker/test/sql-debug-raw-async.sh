if ! nc -z localhost 5432 ; then   
  /run.sh
fi

ulimit -c unlimited
export FFEAD_CPP_PATH=/tmp/ffead-cpp-sql-raw-async/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-sql-raw-async
cp -f web/te-benchmark-um-pq-async/config/cachememory.xml web/te-benchmark-um-pq-async/config/cache.xml
sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop
if [ $# -eq 0 ]; then
	cp /server_orig.sh server.sh
	nohup bash -c "./server.sh > ffead.log &"
	sleep 10
	echo "ffead-cpp with sql-raw-async support launched"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/json"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/plaintext" -s /pipeline.lua -- 16
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/fortunes"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/db"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/queries?queries=20"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/updates?queries=20"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9785/te-benchmark-um-pq-async/cached-worlds?count=100"
	sleep 10
	pkill ffead-cpp
fi
if [ "$1" == "g" ]; then

	cp /server.sh .
	gdb ffead-cpp
fi