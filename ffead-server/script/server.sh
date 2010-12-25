#!/bin/sh
export FEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
echo $FEAD_CPP_PATH
export LD_LIBRARY_PATH=$FEAD_CPP_PATH/lib:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
rm -f $FEAD_CPP_PATH/rtdcf/*.d $FEAD_CPP_PATH/rtdcf/*.o $FEAD_CPP_PATH/lib/libinter.so 
rm -f $FEAD_CPP_PATH/*.cntrl
rm -f $FEAD_CPP_PATH/tmp/*.sess
chmod 700 $FEAD_CPP_PATH/resources/run.sh
chmod 700 $FEAD_CPP_PATH/resources/rundyn.sh
#/usr/sbin/setenforce 0
./CHS $FEAD_CPP_PATH > ffead.log