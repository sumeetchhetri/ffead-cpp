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
	echo "=== Building ttyd (${TARGET})..."
	rm -rf build && mkdir -p build && cd build
  cmake -E env CXXFLAGS="-I${STAGE_DIR}/include" cmake -DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}/cross-${TARGET}.cmake \
      -DCMAKE_INC_PATH=${STAGE_DIR} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on ..
  make -j4 install
}

build() {
	TARGET="$1"
	ALIAS="$2"
	STAGE_DIR="${STAGE_ROOT}/${TARGET}"
	BUILD_DIR="${BUILD_ROOT}/${TARGET}"

	echo "=== Building target ${ALIAS} (${TARGET})..."

	export PKG_CONFIG_PATH="${STAGE_DIR}/lib/pkgconfig"

	if [ "$2" = "mingw-w64" ]
	then
		install_cmake_mingw_w64_cross_file
	else
		install_cmake_musl_cross_file	
	fi
	
	build_ffeadcpp
}

case $1 in
  i386|x86_64|aarch64|mips|mipsel)
    build $1-linux-musl $1
    ;;
  arm)
    build arm-linux-musleabi $1
    ;;
  armhf)
    build arm-linux-musleabihf $1
    ;;
  mingw-w64)
    build x86_64-w64-mingw32 $1
    ;;
  *)
    echo "usage: $0 i386|x86_64|arm|armhf|aarch64|mips|mipsel|mingw-w64" && exit 1
esac