#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf/autotools
make libdinter.la
cp -f .libs/*dinter* $FEAD_CPP_PATH/lib/