#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))
WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' /tmp/ffead-cpp-sql-raw/resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' /tmp/ffead-cpp-sql-raw/resources/server.prop

chmod +x /tmp/ffead-cpp-sql-raw/*.sh

cp /tmp/ffead-cpp-sql-raw/server.sh /server_orig.sh

cd /tmp/ffead-cpp-sql-raw
nohup bash -c "./server.sh > ffead.log &"
echo "Waiting for ffead-cpp to launch on port 8080..."
COUNTER=0
while [ ! -f lib/libinter.so ]
do
  sleep 1
  COUNTER=$((COUNTER+1))
  if [ "$COUNTER" = 600 ]
  then
  	cat ffead.log
  	cat logs/jobs.log
    echo "exiting...."
    exit 1
  fi
done
COUNTER=0
while [ ! -f lib/libdinter.so ]
do
  sleep 1
  COUNTER=$((COUNTER+1))
  if [ "$COUNTER" = 120 ]
  then
  	cat ffead.log
  	cat logs/jobs.log
    echo "exiting....dlib"
    exit 1
  fi
done
rm -f serv.ctrl
pkill ffead-cpp

sed -i 's|EVH_SINGLE=false|EVH_SINGLE=true|g' resources/server.prop
nohup bash -c "./server.sh > ffead.log &"
sleep 20
echo "ffead-cpp with sql-raw support launched"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/j"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:8080/t4/plaint" -s /pipeline.lua -- 16
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 256 --timeout 8 -t 2 "http://localhost:8080/t4/fortu"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/d"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/quer?queries=20"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/quem?queries=20"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/que_?queries=20"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/updt?queries=20"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/updm?queries=20"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/upd_?queries=20"
wrk -H 'Host: localhost' -H 'Accept: application/json,text/html;q=0.9,application/xhtml+xml;q=0.9,application/xml;q=0.8,*/*;q=0.7' \
	-H 'Connection: keep-alive' --latency -d 15 -c 512 --timeout 8 -t 2 "http://localhost:8080/t4/cached-wld?count=100"
echo "normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp

exit 0
