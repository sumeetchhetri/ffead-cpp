#!/bin/sh

echo =====================================================================================

cd `dirname "$0"`

if [ $# -eq 0 ] ; then
  echo Need a c file name, such as $0 mymodule.c
  echo
  exit 1
fi

echo "Your command is $0 $1 $2"
echo

if [ ! -f $1 ] ; then
  echo File $1 does not exist
  echo
  exit 1
fi

if [ ! -d $2 ] ; then
  echo Please specify ffead-cpp directory path
  echo
  exit 1
fi


if [ "x$LSIAPIDIR" = "x" ]; then
    #if not set the LSIAPIDIR, use the default location
    LSIAPIDIR=../../
fi

if [ ! -d "$LSIAPIDIR/include" ]; then
    echo "Directory $LSIAPIDIR/include missing"
    echo
    exit 1
fi

TARGET=`basename $1 .cpp`
echo Target=$TARGET
echo




SYS_NAME=`uname -s`
if [ "x$SYS_NAME" = "xDarwin" ] ; then
    UNDEFINED_FLAG="-undefined dynamic_lookup"
else
	UNDEFINED_FLAG=""
fi

if [ "$TARGET" = "imgresize" ] ; then
    if [ -e "/usr/local/lib/libgd.a" ] ; then
        GDLIB="-lgd"
    else
        echo "Lib gd is not installed.  Cannot use $TARGET without it."
        echo
        exit 1
    fi
else
    GDLIB=""
fi

FFEAD_CPP_DIR=$2

gcc -g -Wall -fPIC  -fvisibility=hidden -c -D_REENTRANT $(getconf LFS_CFLAGS)  $TARGET.cpp -I "$LSIAPIDIR/src" -I "$LSIAPIDIR/include" -I${FFEAD_CPP_DIR}/include -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -w -fpermissive -std=gnu++17
gcc -g -Wall -fPIC $UNDEFINED_FLAG  $(getconf LFS_CFLAGS) -lstdc++ -L/usr/local/lib -L${FFEAD_CPP_DIR}/lib -o $TARGET.so $TARGET.o -shared $GDLIB -lffead-modules -lffead-framework -lmongoc-1.0 -lbson-1.0

if [ -f $(pwd)/$TARGET.so ] ; then
	echo -e "\033[38;5;71m$TARGET.so created.\033[39m"
else
    echo -e "\033[38;5;203mError, $TARGET.so does not exist, failed.\033[39m"
fi

if [ -f $TARGET.o ] ; then
  rm $TARGET.o
fi


echo Done!
echo