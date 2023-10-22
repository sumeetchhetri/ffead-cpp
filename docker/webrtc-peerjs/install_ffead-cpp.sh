#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))
WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

mv /tmp/ffead-cpp-7.0-bin /opt/ffead-cpp-7.0
chmod +x /opt/ffead-cpp-7.0/*.sh

cd /opt/ffead-cpp-7.0
sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' resources/server.prop

nohup bash -c "./server.sh > ffead.log &"
while ! nc -z localhost 8080; do
  echo "Waiting for ffead-cpp to launch on port 8080..." 
  sleep 5
done
echo "ffead-cpp launched"
rm -f serv.ctrl
pkill ffead-cpp
