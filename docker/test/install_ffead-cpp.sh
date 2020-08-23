#!/bin/bash

#From https://github.com/TechEmpower/FrameworkBenchmarks/blob/master/frameworks/C%2B%2B/ulib/setup_json.sh
MAX_THREADS=$(( 3 * `nproc` / 2 ))
WRIT_THREADS=$(( $MAX_THREADS / 3 ))
SERV_THREADS=$(( $MAX_THREADS - $WRIT_THREADS ))

sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' /tmp/ffead-cpp-mongo-raw/resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' /tmp/ffead-cpp-mongo-raw/resources/server.prop

sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' /tmp/ffead-cpp-mongo-orm/resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' /tmp/ffead-cpp-mongo-orm/resources/server.prop

sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' /tmp/ffead-cpp-sql-raw/resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' /tmp/ffead-cpp-sql-raw/resources/server.prop

sed -i 's|THRD_PSIZ=6|THRD_PSIZ='${SERV_THREADS}'|g' /tmp/ffead-cpp-mongo-orm/resources/server.prop
sed -i 's|W_THRD_PSIZ=2|W_THRD_PSIZ='${WRIT_THREADS}'|g' /tmp/ffead-cpp-mongo-orm/resources/server.prop

chmod +x /tmp/ffead-cpp-mongo-raw/*.sh
chmod +x /tmp/ffead-cpp-mongo-orm/*.sh
chmod +x /tmp/ffead-cpp-sql-raw/*.sh
chmod +x /tmp/ffead-cpp-sql-orm/*.sh

cd /tmp/ffead-cpp-mongo-raw
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
echo "ffead-cpp with mongo-raw support launched"
echo "normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp

cd /tmp/ffead-cpp-mongo-orm
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
echo "ffead-cpp with mongo-orm support launched"
echo "normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp

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
echo "ffead-cpp with sql-raw-postgres support launched"
echo "normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp

cd /tmp/ffead-cpp-sql-orm
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
echo "ffead-cpp with sql-orm support launched"
echo "normal shutdown"
rm -f serv.ctrl
pkill ffead-cpp

exit 0