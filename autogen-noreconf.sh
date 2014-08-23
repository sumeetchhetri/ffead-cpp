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
find . -type d -name '.deps' -print -exec rm -rf {} \;

libtoolize --force
aclocal
autoconf -I m4
autoheader
automake --add-missing