#!/bin/sh

for i in $(seq 1 $(nproc --all)); do
  ./h2o-evloop-ffead-cpp.out &
done

wait
