#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf
make all
cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/default/public/
