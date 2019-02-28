#!/bin/sh

#/usr/sbin/setenforce 0
./ffead-cpp "$@" > ffead."$5".log 2>&1 &