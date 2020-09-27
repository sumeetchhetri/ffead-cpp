if ! nc -z localhost 5432 ; then   
  /run.sh
fi

ulimit -c unlimited
export FFEAD_CPP_PATH=/tmp/ffead-cpp-mongo-raw/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-mongo-raw
cp -f web/te-benchmark-um-mgr/config/cachememory.xml web/te-benchmark-um-mgr/config/cache.xml
sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop
if [ $# -eq 0 ]; then
	cp /server_orig.sh server.sh
	nohup bash -c "./server.sh > ffead.log &"
	sleep 10
	echo "ffead-cpp with mongo-raw support launched"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/json"
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/plaintext" -s /pipeline.lua -- 16
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/fortunes"
	mongo admin --eval 'var stats = db.runCommand( { serverStatus: 1});print(stats["opcounters"]["query"])'
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/db"
	mongo admin --eval 'var stats = db.runCommand( { serverStatus: 1});print(stats["opcounters"]["query"])'
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/queries?queries=20"
	mongo admin --eval 'var stats = db.runCommand( { serverStatus: 1});print(stats["opcounters"]["query"])'
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/updates?queries=20"
	mongo admin --eval 'var stats = db.runCommand( { serverStatus: 1});print(stats["opcounters"]["update"])'
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9781/te-benchmark-um-mgr/cached-worlds?count=100"
	sleep 10
	pkill ffead-cpp
fi
if [ "$1" == "g" ]; then
	cp /server.sh .
	gdb ffead-cpp
fi