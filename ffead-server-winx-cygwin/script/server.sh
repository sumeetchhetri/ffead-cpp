#!/bin/sh
export FEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
echo $FEAD_CPP_PATH
export PATH=$FEAD_CPP_PATH/lib:$PATH
echo $PATH
rm -f $FEAD_CPP_PATH/rtdcf/*.d $FEAD_CPP_PATH/rtdcf/*.o 
rm -f $FEAD_CPP_PATH/*.cntrl
rm -f $FEAD_CPP_PATH/tmp/*.sess
chmod 700 $FEAD_CPP_PATH/resources/run.sh
chmod 700 $FEAD_CPP_PATH/resources/rundyn.sh
#/usr/sbin/setenforce 0
./CHS.exe $FEAD_CPP_PATH > ffead.log