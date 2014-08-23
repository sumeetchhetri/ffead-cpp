#!/bin/sh


if [ "$#" -gt 1 -a -n "$2" ]
then
	FFEAD_CPPPTH=$2
	export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
	cd $FFEAD_CPP_PATH/rtdcf/autotools
elif [ "$#" -gt 0 -a -n "$1" ]
then
	FFEAD_CPPPTH=$1
	export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
	cd $FFEAD_CPP_PATH/rtdcf/autotools
fi

echo "autogen-noreconf requested..."
rm -f configure config.* aclocal* depcomp install-sh ltmain.sh libtool Makefile Makefile.in missing
rm -rf autom4te.cache
rm -rf $FFEAD_CPP_PATH/rtdcf/.deps

libtoolize --force
aclocal
autoconf -I m4
automake --add-missing
