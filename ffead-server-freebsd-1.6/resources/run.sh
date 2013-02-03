#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf
rm -f  $FEAD_CPP_PATH/lib/libinter.so
gmake clean
gmake all
cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/default/public/
