if ! nc -z localhost 5432 ; then   
  /run.sh
fi

ulimit -c unlimited
export FFEAD_CPP_PATH=/tmp/ffead-cpp-sql-raw-async-picoev/
export ODBCINI=$FFEAD_CPP_PATH/resources/odbc.ini 
export ODBCSYSINI=$FFEAD_CPP_PATH/resources
export LD_LIBRARY_PATH=$FFEAD_CPP_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$FFEAD_CPP_PATH/lib:$PATH
cd /tmp/ffead-cpp-sql-raw-async-picoev
cp -f web/t4/config/cachememory.xml web/t4/config/cache.xml
sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop
if [ $# -eq 0 ]; then
	cp /server_orig.sh server.sh
	nohup bash -c "./server.sh > ffead.log &"
	sleep 30
	echo "ffead-cpp with sql-raw-async-picoev support launched"
	curl -vvv http://localhost:9786/t4/j
	curl -vvv http://localhost:9786/t4/plaint
	curl -vvv http://localhost:9786/t4/fortu
	curl -vvv http://localhost:9786/t4/d
	curl -vvv http://localhost:9786/t4/quer?queries=20
	curl -vvv http://localhost:9786/t4/quem?queries=20
	curl -vvv http://localhost:9786/t4/updt?queries=20
	curl -vvv http://localhost:9786/t4/updm?queries=20
	curl -vvv http://localhost:9786/t4/upd_?queries=20
	curl -vvv http://localhost:9786/t4/cached-wld?count=100
	echo "ffead-cpp with sql-raw-async-picoev verification complete"
	sleep 30
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/j" > j.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9786/t4/plaint" -s /pipeline.lua -- 16 > p.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:9786/t4/fortu" > f.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/d" > d.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/quer?queries=20" > q.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/quem?queries=20" > qm.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/updt?queries=20" > u.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/updm?queries=20" > um.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/upd_?queries=20" > u_.txt
	wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:9786/t4/cached-wld?count=100" > c.txt 
	sleep 10
	JP=`grep "Requests/sec:" j.txt |sed -e 's|Requests/sec:\s*||g'`
	PP=`grep "Requests/sec:" p.txt |sed -e 's|Requests/sec:\s*||g'`
	FP=`grep "Requests/sec:" f.txt |sed -e 's|Requests/sec:\s*||g'`
	DP=`grep "Requests/sec:" d.txt |sed -e 's|Requests/sec:\s*||g'`
	QP=`grep "Requests/sec:" q.txt |sed -e 's|Requests/sec:\s*||g'`
	QMP=`grep "Requests/sec:" qm.txt |sed -e 's|Requests/sec:\s*||g'`
	UP=`grep "Requests/sec:" u.txt |sed -e 's|Requests/sec:\s*||g'`
	UMP=`grep "Requests/sec:" um.txt |sed -e 's|Requests/sec:\s*||g'`
	U_P=`grep "Requests/sec:" u_.txt |sed -e 's|Requests/sec:\s*||g'`
	CP=`grep "Requests/sec:" c.txt |sed -e 's|Requests/sec:\s*||g'`
	echo "sql-raw-async-picoev,${JP},${PP},${FP},${DP},${QP},${QMP},${UP},${UMP},${U_P},${CP}"
	pkill ffead-cpp
fi
if [ "$1" == "g" ]; then

	cp /server.sh .
	gdb ffead-cpp
fi