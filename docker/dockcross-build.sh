#!/bin/bash
#
# This script should be run inside the tsl0922/musl-cross docker image
# Example:
#         docker run --rm -v $(pwd):/ffead-cpp -w /ffead-cpp tsl0922/musl-cross ./scripts/cross-build.sh mips
#
set -eo pipefail

#Most of the script has been shamelessly lifted from https://github.com/tsl0922/ttyd/blob/master/scripts/cross-build.sh and modified for ffead-cpp

BUILD_ROOT="${BUILD_ROOT:-/opt/build}"

install_cmake_musl_cross_file() {
	cat << EOF > ${CMAKE_TOOLCHAIN_FILE}
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
	cat << EOF > ${CMAKE_TOOLCHAIN_FILE}
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

build_ffeadcpp_cmake() {
	echo "=== Building ffead-cpp with cmake for (${TARGET})..."
	rm -rf build && mkdir -p build
	pushd build
		cmake -E env CXXFLAGS="-I${STAGE_DIR}/include" cmake -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} \
  			-DCMAKE_INC_PATH=${STAGE_DIR} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} -DCMAKE_CXX_LINK_FLAGS="-latomic" -DSRV_EMB=on -DMOD_MEMCACHED=on -DMOD_REDIS=on -DMOD_SDORM_MONGO=on ..
		make -j4 install
  	popd
}

build_ffeadcpp_autoconf() {
	echo "=== Building ffead-cpp with autoconf with (${TARGET})..."
	chmod +x autogen.sh
	sed -i'' -e "s|m4_include|#m4_include|g" configure.ac
	sed -i'' -e "s|AX_CXX_COMPILE_STDCXX|#AX_CXX_COMPILE_STDCXX|g" configure.ac
	./autogen.sh
	env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc CXX=${TARGET}-g++ AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include \
		CFLAGS="-I${STAGE_DIR}/include" CXXFLAGS="-I${STAGE_DIR}/include -std=c++17" LDFLAGS="-L${STAGE_DIR}/lib -latomic" ./configure --host="${TARGET}" --enable-srv_emb=yes --enable-mod_sdormmongo=yes \
		--enable-mod_sdormsql=yes --enable-mod_rediscache=yes --with-top_inc_dir=${STAGE_DIR}/include
	make install
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
	TARGET="$1"
	ALIAS="$1"
	STAGE_DIR="${CROSS_ROOT}"
	BUILD_DIR="${BUILD_ROOT}/${TARGET}"

	echo "=== Building target ${ALIAS} (${TARGET})..."
	export PKG_CONFIG_PATH="/usr/lib/${CROSS_ROOT}/pkgconfig/"
}

init $1
build_ffeadcpp_autoconf musl
build_ffeadcpp_cmake musl
