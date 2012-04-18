#!/bin/sh
export FEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
cd $FEAD_CPP_PATH/../rtdcf
export PATH=$FEAD_CPP_PATH/../lib:$PATH
echo $PATH
rm -f  $FEAD_CPP_PATH/../lib/libinter.a
make clean
make all
cp -Rf $FEAD_CPP_PATH/../public/* $FEAD_CPP_PATH/../web/default/public/
