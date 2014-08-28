#!/bin/sh


echo "autogen-noreconf requested..."
rm -f configure config.* aclocal* depcomp install-sh ltmain.sh libtool Makefile Makefile.in missing
rm -f Makefile
rm -rf autom4te.cache
rm -rf src/.deps
rm -rf tests/.deps
rm -rf apache_mod_ffeadcpp/.deps
rm -rf web/default/src/.deps
rm -rf web/flexApp/src/.deps
rm -rf web/oauthApp/src/.deps

IS_OS_DARWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"darwin") != 0 {print "darwin"}'`
if [ "$IS_OS_DARWIN" == "" ]; then
	libtoolize --force
else
	glibtoolize --force
fi
aclocal
autoconf -I m4
autoheader
automake --add-missing