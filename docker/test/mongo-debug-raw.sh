export FFEAD_CPP_PATH=/tmp/ffead-cpp-mongo-raw/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-mongo-raw
if [ $# -eq 0 ]; then
	nohup bash -c "./server.sh > ffead.log &"
	sleep 10
	echo "ffead-cpp with mongo-raw support launched"
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/json
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/plaintext
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/fortunes
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/db
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/queries?queries=1
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/updates?queries=1
	curl -vvv http://localhost:9781/te-benchmark-um-mgr/cached-worlds?queries=1
fi
if [ "$1" == "g" ]; then
	gdb ffead-cpp
fi