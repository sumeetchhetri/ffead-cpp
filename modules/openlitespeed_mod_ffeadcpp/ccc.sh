#!/bin/sh

LSIAPIDIR=/root/openlitespeed
FFEAD_CPP_DIR=/root/ffead-cpp-3.0

if [ ! -f $2 ] ; then
  LSIAPIDIR=$2
  echo
  exit 1
fi

if [ ! -f $3 ] ; then
  FFEAD_CPP_DIR=$3
  echo
  exit 1
fi

gcc -g -Wall -fPIC  -fvisibility=hidden -c -D_REENTRANT $(getconf LFS_CFLAGS)  mod_ffeadcpp.cpp -I "$LSIAPIDIR/src" -I "$LSIAPIDIR/include" -I${FFEAD_CPP_DIR}/include -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -w -fpermissive -std=gnu++17
gcc -g -Wall -fPIC $UNDEFINED_FLAG  $(getconf LFS_CFLAGS) -lstdc++ -L/usr/local/lib -L${FFEAD_CPP_DIR}/lib -o mod_ffeadcpp.so mod_ffeadcpp.o -shared -lffead-framework

if [ -f $(pwd)/mod_ffeadcpp.so ] ; then
	echo -e "\033[38;5;71mmod_ffeadcpp.so created.\033[39m"
else
    echo -e "\033[38;5;203mError, mod_ffeadcpp.so does not exist, failed.\033[39m"
fi

if [ -fmod_ffeadcpp.o ] ; then
  rm mod_ffeadcpp.o
fi

echo Done!
echo