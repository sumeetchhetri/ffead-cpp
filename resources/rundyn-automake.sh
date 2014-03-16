#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf/autotools
make clean
make all
cp -f .libs/*inter* $FEAD_CPP_PATH/lib/