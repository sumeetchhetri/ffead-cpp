export FFEAD_CPP_PATH=/tmp/ffead-cpp-sql-raw/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-sql-raw
if [ $# -eq 0 ]; then
	nohup bash -c "./server.sh > ffead.log &"
	sleep 10
	echo "ffead-cpp with sql-raw support launched"
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/json
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/plaintext
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/fortunes
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/db
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/queries?queries=1
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/updates?queries=1
	wrk -t1 -c1 -d5s http://localhost:9783/te-benchmark-um-pq/cached-worlds?queries=1
fi
if [ "$1" == "g" ]; then
	gdb ffead-cpp
fi