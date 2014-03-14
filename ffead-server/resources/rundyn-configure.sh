#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf/autotools
rm -f $FEAD_CPP_PATH/lib/*inter.*
if [ -f configure ];
then
echo "configure already generated, proceeding to make"
else
./autogen.sh
./configure
fi