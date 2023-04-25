#!/bin/sh

TYPE="cmake"
if [ "$2" != "" ]
then
	TYPE="$2"
fi

FFEAD_CPPPTH=$1
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${FFEAD_CPPPTH}/lib:/usr/local/lib

IS_OS_MINGW=`uname|tr '[A-Z]' '[a-z]'|awk 'index($0,"mingw") != 0 {print "mingw"}'`

NINJA_EXISTS=0
if [ -f "/usr/bin/ninja" ] || [ -f "/usr/local/bin/ninja" ] || [ -f "/mingw64/bin/ninja" ]
then
	NINJA_EXISTS=1
fi

cd $FFEAD_CPP_PATH/rtdcf/

if [ "$TYPE" = "cmake" ]
then
	if [ "$NINJA_EXISTS" = "1" ]
	then
		rm -rf CMakeFiles CMakeCache.txt
		if [ "$IS_OS_MINGW" != "" ]; then
			cmake -G "MinGW Makefiles" -GNinja .
		else
			cmake -GNinja .
		fi
		ninja clean
		ninja
	else
		rm -rf CMakeFiles CMakeCache.txt
		cmake .
		if [ "$IS_OS_MINGW" != "" ]; then
			mingw32-make clean
			mingw32-make -j 4
		else
			make clean
			make -j 4
		fi
	fi
elif [ "$TYPE" = "xmake" ]
then
	rm -rf .xmake
	export XMAKE_ROOT=y
	xmake f --cxflags="-I/usr/local/include -w" -v -D -c
	xmake && xmake install
	rm -rf lib include
elif [ "$TYPE" = "meson" ]
then
	rm -rf build_meson || true
	meson setup build_meson && cd build_meson
	ninja install
elif [ "$TYPE" = "scons" ]
then
	 rm -rf .scons_temp
	 scons
elif [ "$TYPE" = "bazel" ]
then
	chmod +x ./shellb
	./shellb inter-shellb-bazel
elif [ "$TYPE" = "buck2" ]
then
	chmod +x ./shellb
	touch .buckroot
	./shellb inter-shellb-buck2
elif [ "$TYPE" = "shellb" ]
then
	rm -rf .bin || true
	chmod +x ./shellb
	./shellb inter-shellb
else
	echo "Invalid Build Type specified, only cmake, xmake, meson, scons, bazel and shellb supported..."
fi

cp -f *inter* $FFEAD_CPP_PATH/lib/
