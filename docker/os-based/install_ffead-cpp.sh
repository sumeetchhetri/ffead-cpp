#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))
WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' /tmp/ffead-cpp-7.0-bin/resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' /tmp/ffead-cpp-7.0-bin/resources/server.prop
#sed -i'' -e "s|<init>TeBkRestController.updateCache</init>||g" /tmp/ffead-cpp-7.0-bin/web/te-benchmark/config/cache.xml
#sed -i'' -e "s|<init>TeBkUmRouter.updateCache</init>||g" /tmp/ffead-cpp-7.0-bin/web/t1/config/cache.xml
#sed -i'' -e "s|<init>TeBkUmLpqRouter.updateCache</init>||g" /tmp/ffead-cpp-7.0-bin/web/t3/config/cache.xml
#sed -i'' -e "s|<init>TeBkUmMgrRouter.updateCache</init>||g" /tmp/ffead-cpp-7.0-bin/web/t2/config/cache.xml
#sed -i'' -e "s|<init>TeBkUmLpqAsync.updateCache</init>||g" /tmp/ffead-cpp-7.0-bin/web/t4/config/cache.xml

mv /tmp/ffead-cpp-7.0-bin /opt/ffead-cpp-7.0
chmod +x /opt/ffead-cpp-7.0/*.sh

cd /opt/ffead-cpp-7.0
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
    echo "ffead-cpp exiting due to failure...."
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
    echo "ffead-cpp exiting due to failure....dlib"
    exit 1
  fi
done
echo "ffead-cpp start successful"
sleep 30
cd tests && chmod +x *.sh && ./runTests.sh
cd -
echo "ffead-cpp normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp
