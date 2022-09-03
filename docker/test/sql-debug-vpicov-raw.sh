if ! nc -z localhost 5432 ; then   
  /run.sh
fi

ulimit -c unlimited
export FFEAD_CPP_PATH=/tmp/ffead-cpp-sql-raw/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-sql-raw
cp -f web/t3/config/cachememory.xml web/t3/config/cache.xml
sed -i 's|"TeBkUmLpqRouter"|"TeBkUmLpqRouterPicoV"|g' web/t3/config/application.xml
sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop
if [ $# -eq 0 ]; then
	cd /tmp
	nohup bash -c "./main --server_dir=$FFEAD_CPP_PATH --server_port=9783 --is_async=false > ffead.log &"
	sleep 30
	echo "ffead-cpp with sql-raw-vpicov support launched"
	curl -vvv http://localhost:9783/t3/j
	curl -vvv http://localhost:9783/t3/plaint
	curl -vvv http://localhost:9783/t3/fortu
	curl -vvv http://localhost:9783/t3/d
	curl -vvv http://localhost:9783/t3/quer?queries=20
	curl -vvv http://localhost:9783/t3/que_?queries=20
	curl -vvv http://localhost:9783/t3/updt?queries=20
	curl -vvv http://localhost:9783/t3/upd_?queries=20
	curl -vvv http://localhost:9783/t3/cached-wld?count=100
	echo "ffead-cpp with sql-raw-vpicov verification complete"
	sleep 30
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/j" > j.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9783/t3/plaint" -s /pipeline.lua -- 16 > p.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9783/t3/fortu" > f.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/d" > d.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/quer?queries=20" > q.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/que_?queries=20" > q_.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/updt?queries=20" > u.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/upd_?queries=20" > u_.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9783/t3/cached-wld?count=100" > c.txt
	sleep 10
	JP=`grep "Requests/sec:" j.txt |sed -e 's|Requests/sec:\s*||g'`
	PP=`grep "Requests/sec:" p.txt |sed -e 's|Requests/sec:\s*||g'`
	FP=`grep "Requests/sec:" f.txt |sed -e 's|Requests/sec:\s*||g'`
	DP=`grep "Requests/sec:" d.txt |sed -e 's|Requests/sec:\s*||g'`
	QP=`grep "Requests/sec:" q.txt |sed -e 's|Requests/sec:\s*||g'`
	Q_P=`grep "Requests/sec:" q_.txt |sed -e 's|Requests/sec:\s*||g'`
	UP=`grep "Requests/sec:" u.txt |sed -e 's|Requests/sec:\s*||g'`
	U_P=`grep "Requests/sec:" u_.txt |sed -e 's|Requests/sec:\s*||g'`
	CP=`grep "Requests/sec:" c.txt |sed -e 's|Requests/sec:\s*||g'`
	echo "sql-raw-vpicov,${JP},${PP},${FP},${DP},${QP},${Q_P},${UP},${U_P},${CP}"
	pkill main
fi
if [ "$1" == "g" ]; then
	cd /tmp
	gdb main
fi