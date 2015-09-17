#!/bin/bash

FAPP_LIBS=""
libraries () {
	TAL=0
	FAPP_LIBS=""
	echo 1>&2 "Enter Library Dependencies: "
	read -a APP_LIBS
	RE="^[a-zA-Z0-9_][a-zA-Z0-9_]*$"
	for var in "${APP_LIBS[@]}"
	do
	    if ! [[ $var =~ $RE ]]; then
			FAPP_LIBS="0"
			return
	    fi
	    if [[ $var == "-l*" ]]; then
	    	FAPP_LIBS+=" $var"
	    else
	    	FAPP_LIBS+=" -l$var"
	    fi
	done
}

IS_OS_DARWIN=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"darwin") != 0 {print "darwin"}'`
if [ "$IS_OS_DARWIN" != "" ]; then
	export FFEAD_CPP_PATH=`cd "$(dirname server.sh)" && ABSPATH=$(pwd) && cd -`
else
	export FFEAD_CPP_PATH=`echo $(dirname $(readlink -f $0))`
fi

echo $FFEAD_CPP_PATH
cat ffead_gen.help

echo 1>&2 "Enter Application Name: "
read APP_NAME
APP_NAME_REGEX="[a-zA-Z0-9-_\s]*"
while [[ "$APP_NAME" =~ $APP_NAME_REGEX ]]; do 
	echo "Application name can contain only alphanumeric characters, '-' and '_'"
	echo 1>&2 "Enter Application Name: "
	read APP_NAME
done

libraries
echo $FAPP_LIBS
while [[ "$FAPP_LIBS" = "0" ]]; do
	echo "Application library dependency name can contain only alphanumeric characters, '-' and '_'"
	libraries
done

echo 1>&2 "The generator will now create application directories and makefile, 
 configure.ac will also be modified, Do you want to proceed (y/n): "
read CONFIRM
RE="^[y|n|Y|N]$"
if [[ "$CONFIRM" =~ $RE ]]; then
	echo "Application Name: $APP_NAME"
	echo "Library Dependencies: $FAPP_LIBS"
	echo "Generator will now generate a new application directory $FFEAD_CPP_PATH/web/$APP_NAME"
	mkdir $FFEAD_CPP_PATH/web/$APP_NAME
	mkdir $FFEAD_CPP_PATH/web/$APP_NAME/config
	mkdir $FFEAD_CPP_PATH/web/$APP_NAME/include
	mkdir $FFEAD_CPP_PATH/web/$APP_NAME/src
	mkdir $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools
	cp -f $FFEAD_CPP_PATH/resources/Makefile.am $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools
	cp -Rf $FFEAD_CPP_PATH/public $FFEAD_CPP_PATH/web/$APP_NAME/
	sed -Ei "s/APP_NAME/$APP_NAME/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am
	FAPP_LIBS=`echo $FAPP_LIBS|sed 's/ /\\ /g'`
	sed -Ei "s/FAPP_LIBS/$FAPP_LIBS/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am
	WEB_APPS=`find $FFEAD_CPP_PATH/web -maxdepth 1 -mindepth 1 -type d -not -path '*/\.*' -printf "%p\n"| rev |awk -F"./" '{print $0}'| cut -d '/' -f1 | rev| xargs -I {} printf '\tweb\/'{}"\/src\/autotools\/Makefile"`
	cp -f configure.ac.orig configure.ac.template
	sed -Ei "s/WEB_APPS/$WEB_APPS/g" configure.ac.template
	cp -f configure.ac.template configure.ac
	test -d "$FFEAD_CPP_PATH/web/$APP_NAME" && echo "Application directory created Successfully" || echo "Unbale to create Application directory"
	test -d "$FFEAD_CPP_PATH/web/$APP_NAME/config" && echo "Application config directory created Successfully" || echo "Unbale to create Application directory"
	test -d "$FFEAD_CPP_PATH/web/$APP_NAME/include" && echo "Application include directory created Successfully" || echo "Unbale to create Application directory"
	test -d "$FFEAD_CPP_PATH/web/$APP_NAME/src" && echo "Application src directory created Successfully" || echo "Unbale to create Application directory"
	test -d "$FFEAD_CPP_PATH/web/$APP_NAME/src/autotools" && echo "Application src/autotools directory created Successfully" || echo "Unbale to create Application directory"
	test "$FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am" && echo "Application Makefile created Successfully" || echo "Unbale to create Application directory"
else
	echo "Will not generate code"
fi
