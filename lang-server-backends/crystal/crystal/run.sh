#!/bin/bash

for i in $(seq 1 $(nproc --all)); do
  ./crystal-ffead-cpp.out --ffead-cpp-dir=/root/ffead-cpp-6.0 --to=8080 &
done

wait
