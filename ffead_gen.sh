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

FARTIFACT_TYPE=""
artifact () {
	echo 1>&2 "Enter Artifact Type to create (c:Controller f:Filter r:RestAPI e:Continue): "
	read -a FARTIFACT_TYPE
	while [[ "$FARTIFACT_TYPE" = "0" ]]; do
		echo "Application library class names can contain only alphanumeric characters, '-' and '_'"
		artifact
		return
	done
    case "$FARTIFACT_TYPE" in
    	c)
    		controllers
			echo $FAPP_CONTROLLERS
			while [[ "$FAPP_CONTROLLERS" = "0" ]]; do
				echo "Controller class names can contain only alphanumeric characters, '-' and '_'"
				controllers
			done
    	;;
    	f)
    		filters
			echo $FAPP_FILTERS
			while [[ "$FAPP_FILTERS" = "0" ]]; do
				echo "Filter class names can contain only alphanumeric characters, '-' and '_'"
				filters
			done
    	;;
    	r)
    		restapis
			echo $FAPP_RESTAPIS
			while [[ "$FAPP_RESTAPIS" = "0" ]]; do
				echo "Rest API class names can contain only alphanumeric characters, '-' and '_'"
				restapis
			done
    	;;
    	e)
    		return
    	;;
    	*)
    		FARTIFACT_TYPE=0
    	;;
    esac
	artifact
}

CONTOLLER_CONF=""
APP_CONTROLLERS_CLASSES=""
APP_CLASSES=""
FAPP_CONTROLLERS=""
controllers () {
	TAL=0
	FAPP_CONTROLLERS=""
	echo 1>&2 "Enter Class names: "
	read -a APP_CONTROLLERS_CLASSES
	RE="^[a-zA-Z0-9_][a-zA-Z0-9_]*$"
	for var in "${APP_CONTROLLERS_CLASSES[@]}"
	do
	    if ! [[ $var =~ $RE ]]; then
			FAPP_CONTROLLERS="0"
			return
	    fi
	done
}
controllers_create () {
	for var in "${APP_CONTROLLERS_CLASSES[@]}"
	do
		uvar=`echo $var |tr '[:lower:]' '[:upper:]'`
	    cp -f $FFEAD_CPP_PATH/resources/ffead-gen/$FCNF_TYPE/TControllerHeader.t $FFEAD_CPP_PATH/web/$APP_NAME/include/${var}.h
		sed -Ei "s/@TCLASS@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/include/${var}.h
		sed -Ei "s/@TCLASSU@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/include/${uvar}.h
		cp -f $FFEAD_CPP_PATH/resources/ffead-gen/xml/TControllerDef.t $FFEAD_CPP_PATH/web/$APP_NAME/src/${var}.cpp
		sed -Ei "s/@TCLASS@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/${var}.cpp
		APP_CLASSES+="\.\.\/${var}\.cpp "
		CONTOLLER_CONF+="\\n\\t\\t<controller class=\"${var}\" path=\"*\" \/>"
	done
}

FILTER_CONF=""
APP_FILTERS_CLASSES=""
FAPP_FILTERS=""
filters () {
	TAL=0
	FAPP_FILTERS=""
	echo 1>&2 "Enter Class names: "
	read -a APP_FILTERS_CLASSES
	RE="^[a-zA-Z0-9_][a-zA-Z0-9_]*$"
	for var in "${APP_FILTERS_CLASSES[@]}"
	do
	    if ! [[ $var =~ $RE ]]; then
			FAPP_FILTERS="0"
			return
	    fi
	done
}
filters_create () {
	for var in "${APP_FILTERS_CLASSES[@]}"
	do
		uvar=`echo $var |tr '[:lower:]' '[:upper:]'`
	    cp -f $FFEAD_CPP_PATH/resources/ffead-gen/$FCNF_TYPE/TFilterHeader.t $FFEAD_CPP_PATH/web/$APP_NAME/include/${var}.h
		sed -Ei "s/@TCLASS@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/include/${var}.h
		sed -Ei "s/@TCLASSU@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/include/${uvar}.h
		cp -f $FFEAD_CPP_PATH/resources/ffead-gen/xml/TFilterDef.t $FFEAD_CPP_PATH/web/$APP_NAME/src/${var}.cpp
		sed -Ei "s/@TCLASS@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/${var}.cpp
		APP_CLASSES+="\.\.\/${var}\.cpp "
		FILTER_CONF+="\\n\\t\\t<filter class=\"${var}\" type=\"in\" \/>\\n"
		FILTER_CONF+="\\t\\t<filter class=\"${var}\" type=\"out\" \/>\\n"
		FILTER_CONF+="\\t\\t<filter class=\"${var}\" type=\"handle\" \/>"
	done
}

RESTAPI_CONF=""
APP_RESTAPIS_CLASSES=""
FAPP_RESTAPIS=""
restapis () {
	TAL=0
	FAPP_RESTAPIS=""
	echo 1>&2 "Enter Class names: "
	read -a APP_RESTAPIS_CLASSES
	RE="^[a-zA-Z0-9_][a-zA-Z0-9_]*$"
	for var in "${APP_RESTAPIS_CLASSES[@]}"
	do
	    if ! [[ $var =~ $RE ]]; then
			FAPP_RESTAPIS="0"
			return
	    fi
	done
}
restapis_create () {
	for var in "${APP_RESTAPIS_CLASSES[@]}"
	do
		uvar=`echo $var |tr '[:lower:]' '[:upper:]'`
	    cp -f $FFEAD_CPP_PATH/resources/ffead-gen/$FCNF_TYPE/TRestApiHeader.t $FFEAD_CPP_PATH/web/$APP_NAME/include/${var}.h
		sed -Ei "s/@TCLASS@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/include/${var}.h
		sed -Ei "s/@TCLASSU@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/include/${uvar}.h
		cp -f $FFEAD_CPP_PATH/resources/ffead-gen/xml/TRestApiDef.t $FFEAD_CPP_PATH/web/$APP_NAME/src/${var}.cpp
		sed -Ei "s/@TCLASS@/${var}/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/${var}.cpp
		APP_CLASSES+="\.\.\/${var}\.cpp "
		lvar=`echo $var |tr '[:upper:]' '[:lower:]'`
		RESTAPI_CONF+="\\n\\t\\t<restcontroller class=\"${var}\" path=\"\/$lvar\">\\n"
		RESTAPI_CONF+="\\t\\t\\t<restfunction name=\"serve\" path=\"\/serve\" meth=\"GET\" statusCode=\"200\"\/>\\n"
		RESTAPI_CONF+="\\t\\t<\/restcontroller>"
	done
}

FCNF_TYPE=""
FCONTROLLERS=""
config_setup () {
	FCNF_TYPE=""
	echo 1>&2 "Enter Configuration type (xml/markers): "
	read -a FCNF_TYPE
	RE="xml|markers"
    if ! [[ $FCNF_TYPE =~ $RE ]]; then
		FCNF_TYPE="0"
		return
    fi
}

main () {
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
		controllers_create
		filters_create
		restapis_create
		if [[ "$FCNF_TYPE" = "xml" ]]; then
			cp -f $FFEAD_CPP_PATH/resources/ffead-gen/xml/application.xml $FFEAD_CPP_PATH/web/$APP_NAME/config
			sed -Ei $"s/@CONTOLLER_CONF@/$CONTOLLER_CONF/g" $FFEAD_CPP_PATH/web/$APP_NAME/config/application.xml
			sed -Ei $"s/@FILTER_CONF@/$FILTER_CONF/g" $FFEAD_CPP_PATH/web/$APP_NAME/config/application.xml
			sed -Ei $"s/@RESTAPI_CONF@/$RESTAPI_CONF/g" $FFEAD_CPP_PATH/web/$APP_NAME/config/application.xml
		else
			cp -f $FFEAD_CPP_PATH/resources/ffead-gen/markers/application.xml $FFEAD_CPP_PATH/web/$APP_NAME/config
		fi
		cp -f $FFEAD_CPP_PATH/resources/ffead-gen/Makefile.am $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools
		cp -Rf $FFEAD_CPP_PATH/public $FFEAD_CPP_PATH/web/$APP_NAME/
		sed -Ei "s/@APP_NAME@/$APP_NAME/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am
		sed -Ei "s/@APP_CLASSES@/$APP_CLASSES/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am
		FAPP_LIBS=`echo $FAPP_LIBS|sed 's/ /\\ /g'`
		sed -Ei "s/@FAPP_LIBS@/$FAPP_LIBS/g" $FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am
		WEB_APPS=`find $FFEAD_CPP_PATH/web -maxdepth 1 -mindepth 1 -type d -not -path '*/\.*' -printf "%p\n"| rev |awk -F"./" '{print $0}'| cut -d '/' -f1 | rev| xargs -I {} printf '\tweb\/'{}"\/src\/autotools\/Makefile"`
		cp -f $FFEAD_CPP_PATH/resources/ffead-gen/configure.ac.orig $FFEAD_CPP_PATH/configure.ac.template
		sed -Ei "s/WEB_APPS/$WEB_APPS/g" $FFEAD_CPP_PATH/configure.ac.template
		cp -f $FFEAD_CPP_PATH/configure.ac $FFEAD_CPP_PATH/configure.ac.old
		cp -f $FFEAD_CPP_PATH/configure.ac.template $FFEAD_CPP_PATH/configure.ac
		rm -f $FFEAD_CPP_PATH/configure.ac.template
		test -d "$FFEAD_CPP_PATH/web/$APP_NAME" && echo "Application directory created Successfully" || echo "Unable to create Application directory"
		test -d "$FFEAD_CPP_PATH/web/$APP_NAME/config" && echo "Application config directory created Successfully" || echo "Unable to create Application directory"
		test -d "$FFEAD_CPP_PATH/web/$APP_NAME/include" && echo "Application include directory created Successfully" || echo "Unable to create Application directory"
		test -d "$FFEAD_CPP_PATH/web/$APP_NAME/src" && echo "Application src directory created Successfully" || echo "Unable to create Application directory"
		test -d "$FFEAD_CPP_PATH/web/$APP_NAME/src/autotools" && echo "Application src/autotools directory created Successfully" || echo "Unable to create Application directory"
		test "$FFEAD_CPP_PATH/web/$APP_NAME/src/autotools/Makefile.am" && echo "Application Makefile created Successfully" || echo "Unable to create Application directory"
	else
		echo "Will not generate code"
	fi
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

config_setup
echo $FCNF_TYPE
while [[ "$FCNF_TYPE" = "0" ]]; do
	echo "Application configuration type can be xml or markers"
	config_setup
done

artifact
if [[ "$FARTIFACT_TYPE" = "e" ]]; then
	main
fi