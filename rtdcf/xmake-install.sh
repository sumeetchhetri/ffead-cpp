#!/bin/sh

find lib/ -name "$1.*" -exec cp "{}" ../lib/  \;
