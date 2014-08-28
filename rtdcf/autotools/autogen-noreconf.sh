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

IS_OS_DARWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"darwin") != 0 {print "darwin"}'`
if [ "$IS_OS_DARWIN" == "" ]; then
	libtoolize --force
else
	glibtoolize --force
fi
aclocal
autoconf -I m4
automake --add-missing
