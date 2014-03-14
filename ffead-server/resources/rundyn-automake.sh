#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf/autotools
make clean
make all
cp .libs/*inter* $FEAD_CPP_PATH/lib/