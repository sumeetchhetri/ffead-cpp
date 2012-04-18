#!/bin/sh
export FEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
export PATH=$FEAD_CPP_PATH/../lib:$PATH
cd $FEAD_CPP_PATH/../rtdcf
rm -f $FEAD_CPP_PATH/../*.cntrl
make all
cp -Rf $FEAD_CPP_PATH/../public/* $FEAD_CPP_PATH/../web/default/public/