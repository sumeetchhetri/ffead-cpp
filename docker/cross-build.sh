#!/bin/bash
#
# This script should be run inside the tsl0922/musl-cross docker image
# Example:
#         docker run --rm -v $(pwd):/ffead-cpp -w /ffead-cpp tsl0922/musl-cross ./scripts/cross-build.sh mips
#
set -eo pipefail

#Most of the script has been shamelessly lifted from https://github.com/tsl0922/ttyd/blob/master/scripts/cross-build.sh and modified for ffead-cpp

CROSS_ROOT="${CROSS_ROOT:-/opt/cross}"
STAGE_ROOT="${STAGE_ROOT:-/opt/stage}"
BUILD_ROOT="${BUILD_ROOT:-/opt/build}"

install_cmake_musl_cross_file() {
	cat << EOF > ${BUILD_DIR}/cross-${TARGET}.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER "${TARGET}-gcc")
set(CMAKE_CXX_COMPILER "${TARGET}-g++")
set(CMAKE_FIND_ROOT_PATH "${STAGE_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
}

install_cmake_mingw_w64_cross_file() {
	cat << EOF > ${BUILD_DIR}/cross-${TARGET}.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH "${STAGE_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
}

build_ffeadcpp() {
	echo "=== Building ffead-cpp (${TARGET})..."
	rm -rf build && mkdir -p build
	pushd build
		if [ "$1" = "android" ]
		then
			cmake -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake \
				-DANDROID_NDK=${ANDROID_NDK_ROOT} -DANDROID_ABI=${ANDROID_ARCH} -DANDROID_PLATFORM=android-${ANDROID_API} \
				-DCMAKE_INC_PATH=${STAGE_DIR} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} -DSRV_EMB=on -DMOD_REDIS=on ..
		else
			cmake -E env CXXFLAGS="-I${STAGE_DIR}/include" cmake -DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}/cross-${TARGET}.cmake \
      			-DCMAKE_INC_PATH=${STAGE_DIR} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on ..
		fi
		make -j4 install
  	popd
}

install_cmake_musl_cross_file() {
	cat << EOF > ${BUILD_DIR}/cross-${TARGET}.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER "${TARGET}-gcc")
set(CMAKE_CXX_COMPILER "${TARGET}-g++")
set(CMAKE_FIND_ROOT_PATH "${STAGE_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
}

install_cmake_mingw_w64_cross_file() {
	cat << EOF > ${BUILD_DIR}/cross-${TARGET}.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH "${STAGE_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
}

init() {
	TARGET="$2"
	ALIAS="$3"
	STAGE_DIR="${STAGE_ROOT}/${TARGET}"
	BUILD_DIR="${BUILD_ROOT}/${TARGET}"

	echo "=== Building target ${ALIAS} (${TARGET})..."
	
	export PKG_CONFIG_PATH="${STAGE_DIR}/lib/pkgconfig"
}

if [ "$1" = "musl" ]
then
	case $2 in
	  i386|x86_64|aarch64|mips|mipsel|mips64|mips64el)
	  	init musl $2-linux-musl $2
	  	install_cmake_musl_cross_file
	    build_ffeadcpp musl
	    ;;
	  arm)
	    init musl arm-linux-musleabi $2
	  	install_cmake_musl_cross_file
	    build_ffeadcpp musl
	    ;;
	  armhf)
	    init musl arm-linux-musleabihf $2
	  	install_cmake_musl_cross_file
	    build_ffeadcpp musl
	    ;;
	  *)
	    echo "usage: $0 musl i386|x86_64|arm|armhf|aarch64|mips|mipse|mips64|mips64ell" && exit 1
	esac
fi

if [ "$1" = "mingw-w64" ]
then
	ZLIB_VERSION=1.2.5
	STAGE_ROOT=${MINGW}
	init mingw-w64 x86_64-w64-mingw32
	install_cmake_mingw_w64_cross_file
    build_ffeadcpp mingw-w64
fi

if [ "$1" = "android" ]
then
	ANDROID_HOME=$2
	ANDROID_NDK_ROOT=$3
	NDK=$ANDROID_NDK_ROOT
	TOOLCHAIN="$NDK/toolchains/llvm/prebuilt/linux-x86_64"
	ANDROID_API=$5
	ANDROID_ARCH=$4
	case $4 in
	  armeabi-v7a)
	  	cp ${TOOLCHAIN}/bin/arm-linux-androideabi-ar ${TOOLCHAIN}/bin/armv7a-linux-androideabi-ar
	    cp ${TOOLCHAIN}/bin/arm-linux-androideabi-as ${TOOLCHAIN}/bin/armv7a-linux-androideabi-as
	    cp ${TOOLCHAIN}/bin/arm-linux-androideabi-ld ${TOOLCHAIN}/bin/armv7a-linux-androideabi-ld
	    cp ${TOOLCHAIN}/bin/arm-linux-androideabi-ranlib ${TOOLCHAIN}/bin/armv7a-linux-androideabi-ranlib
	    cp ${TOOLCHAIN}/bin/arm-linux-androideabi-strip ${TOOLCHAIN}/bin/armv7a-linux-androideabi-strip
	    init android armv7a-linux-androideabi x86_32
	    build_ffeadcpp android
	    ;;
	  arm64-v8a)
	    init android aarch64-linux-android arm64
	    build_ffeadcpp android
	    ;;
	  x86)
	    init android i686-linux-android x86
	    build_ffeadcpp android
	    ;;
	  x86-64)
	    init android x86_64-linux-android x86-64
	    build_ffeadcpp android
	    ;;
	  *)
	    echo "usage: $0 android ANDROID_HOME ANDROID_NDK_ROOT armeabi-v7a|arm64-v8a|x86|x86-64 API" && exit 1
	esac
fi
