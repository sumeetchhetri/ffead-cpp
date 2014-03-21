#!/bin/sh

ENVVARS_FILE=$FEAD_CPP_PATH/rtdcf/envvars.mk

IS_OS_CYGWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"cygwin") != 0 {print "cygwin"}'`
IS_OS_BSD=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"bsd") != 0 {print "bsd"}'`
IS_OS_SOLARIS=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"sunos") != 0 {print "sunos"}'`
IS_OS_LINUX=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"linux") != 0 {print "linux"}'`

MAKE_EXE=make
RES_LIB_EXT=so

if [ "$IS_OS_LINUX" != "" ]; then
cat <<EOM> $ENVVARS_FILE
MAKE_EXE := make
PTHREADS :=
EX_LIBS := 
RES_LIB_EXT := so
EOM
elif [ "$IS_OS_BSD" != "" ]; then
MAKE_EXE=gmake
cat <<EOM> $ENVVARS_FILE
MAKE_EXE := gmake
PTHREADS :=
EX_LIBS :=
RES_LIB_EXT := so
EOM
elif [ "$IS_OS_SOLARIS" != "" ]; then
MAKE_EXE=gmake
cat <<EOM> $ENVVARS_FILE
MAKE_EXE := gmake
PTHREADS := -pthreads
EX_LIBS := -lsocket -lnsl
RES_LIB_EXT := so
EOM
elif [ "$IS_OS_CYGWIN" != "" ]; then
RES_LIB_EXT=a
cat <<EOM> $ENVVARS_FILE
MAKE_EXE := make
PTHREADS :=
EX_LIBS :=
RES_LIB_EXT := a
EOM
fi

cd $FEAD_CPP_PATH/rtdcf
rm -f $FEAD_CPP_PATH/*.cntrl
$MAKE_EXE all
cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/default/public/
cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/default/public/
cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/flexApp/public/
cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/oauthApp/public/

