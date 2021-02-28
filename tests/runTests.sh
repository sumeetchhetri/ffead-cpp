#!/bin/bash

export LD_LIBRARY_PATH=../lib:/usr/local/lib:$LD_LIBRARY_PATH
export DYLD_FALLBACK_LIBRARY_PATH=$LD_LIBRARY_PATH
export PATH=../lib:$PATH
#/usr/sbin/setenforce 0
chmod 700 tests

./tests
