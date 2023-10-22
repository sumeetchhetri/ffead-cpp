#!/bin/sh

for i in $(seq 0 $(($(nproc --all)-1))); do
  taskset -c $i ./vweb --server_dir=/root/ffead-cpp-7.0 --server_port=8080 &
done

wait
