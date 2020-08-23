export FFEAD_CPP_PATH=/tmp/ffead-cpp-sql-orm/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-sql-orm
if [ $# -eq 0 ]; then
	nohup bash -c "./server.sh > ffead.log &"
	sleep 10
	echo "ffead-cpp with sql-orm support launched"
	curl -vvv http://localhost:9784/te-benchmark-um/json
	curl -vvv http://localhost:9784/te-benchmark-um/plaintext
	curl -vvv http://localhost:9784/te-benchmark-um/fortunes
	curl -vvv http://localhost:9784/te-benchmark-um/db
	curl -vvv http://localhost:9784/te-benchmark-um/queries?queries=1
	curl -vvv http://localhost:9784/te-benchmark-um/updates?queries=1
	curl -vvv http://localhost:9784/te-benchmark-um/cached-worlds?queries=1
fi
if [ "$1" == "g" ]; then
	gdb ffead-cpp
fi