#!/bin/sh
cd $FEAD_CPP_PATH/rtdcf/autotools
rm -f $FEAD_CPP_PATH/lib/*inter.*
rm -f Makefile
if [ "$#" -eq 1 ] && [ "$1" = "reconf" ]; then
	echo "Reconfiguer requested..."
	rm -f configure
fi
if [ -f configure ]; then
	echo 'Configure file already exists'
else
	./autogen.sh
fi
./configure
