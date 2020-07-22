#!/bin/bash
#
# This script should be run inside the tsl0922/musl-cross docker image
# Example:
#         docker run --rm -v $(pwd):/ffead-cpp -w /ffead-cpp tsl0922/musl-cross ./scripts/cross-build.sh mips
#
set -eo pipefail

CROSS_ROOT="${CROSS_ROOT:-/opt/cross}"
STAGE_ROOT="${STAGE_ROOT:-/opt/stage}"
BUILD_ROOT="${BUILD_ROOT:-/opt/build}"

ZLIB_VERSION="${ZLIB_VERSION:-1.2.11}"
OPENSSL_VERSION="${OPENSSL_VERSION:-1_1_1-stable}"
UNIXODBC_VERSION="${UNIXODBC_VERSION:-2.3.7}"
CURL_VERSION="${CURL_VERSION:-7.71.1}"
LIBCUCKOO_VERSION="${LIBCUCKOO_VERSION:-0.3}"
LIBHIREDIS_VERSION="${LIBHIREDIS_VERSION:-0.13.3}"
MONGOCDRIVER_VERSION="${MONGOCDRIVER_VERSION:-1.4.2}"

build_zlib() {
	echo "=== Building zlib-${ZLIB_VERSION} (${TARGET})..."
	curl -sLo- https://zlib.net/zlib-${ZLIB_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/zlib-${ZLIB_VERSION}
		if [ "$1" = "mingw-w64" ]
    	then
    		env CHOST=${TARGET} ./configure --enable-shared --prefix=${STAGE_DIR}
    		make -j"$(nproc)"
    	else
    		env CHOST=${TARGET} ./configure --enable-shared --archs="-fPIC" --prefix=${STAGE_DIR}
			make -j"$(nproc)"
    	fi
    	make install
	popd
}

build_openssl() {
    echo "=== Building openssl-${OPENSSL_VERSION} (${TARGET})..."
    curl -sLo openssl-OpenSSL_${OPENSSL_VERSION}.zip "https://github.com/openssl/openssl/archive/OpenSSL_${OPENSSL_VERSION}.zip"
    unzip openssl-OpenSSL_${OPENSSL_VERSION}.zip -d ${BUILD_DIR}/
    pushd ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}
        ./Configure linux-generic64 shared --cross-compile-prefix=${TARGET}- -fPIC --prefix="${STAGE_DIR}" --openssldir=${STAGE_DIR}
        make -j"$(nproc)" > /dev/null
        make install_sw
    popd
}

build_unixodbc() {
	echo "=== Building unixodbc-${UNIXODBC_VERSION} (${TARGET})..."
	curl -sLo- http://www.unixodbc.org/unixODBC-${UNIXODBC_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/unixODBC-${UNIXODBC_VERSION}
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include \
			./configure --host="${TARGET}" --prefix=${STAGE_DIR}
		make -j"$(nproc)" > /dev/null
		make install
	popd
}

build_curl() {
	echo "=== Building curl-${CURL_VERSION} (${TARGET})..."
	curl -sLo- https://curl.haxx.se/download/curl-${CURL_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/curl-${CURL_VERSION}
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib \
			LDFLAGS="-L${STAGE_DIR}/lib" CPPFLAGS="-I${STAGE_DIR}/include" ./configure --build=`./config.guess` --target="${TARGET}" --host="${TARGET}" --prefix=${STAGE_DIR} \
			--with-ssl=${STAGE_DIR} --with-zlib --disable-ftp --disable-gopher --disable-file --disable-imap --disable-ldap --disable-ldaps --disable-pop3 --disable-proxy \
			--disable-rtsp --disable-smtp --disable-telnet --disable-tftp --without-gnutls --without-libidn --without-librtmp --disable-dict
		make -j"$(nproc)" > /dev/null
		make install
	popd
}

build_libcuckoo() {
	echo "=== Building libcuckoo-${LIBCUCKOO_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/efficient/libcuckoo/archive/v${LIBCUCKOO_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/libcuckoo-${LIBCUCKOO_VERSION}
		cmake -DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}/cross-${TARGET}.cmake -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} .
		make install
	popd
}

build_libhiredis() {
	echo "=== Building libhiredis-${LIBHIREDIS_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/redis/hiredis/archive/v${LIBHIREDIS_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/hiredis-${LIBHIREDIS_VERSION}
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include make -j"$(nproc)" > /dev/null
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include PREFIX=${STAGE_DIR} make install
	popd
}

build_uuid() {
	echo "=== Building ossp-uuid (${TARGET})..."
    curl -sLo ossp-uuid.zip "https://github.com/sean-/ossp-uuid/archive/master.zip"
    unzip ossp-uuid.zip -d ${BUILD_DIR}/
    pushd ${BUILD_DIR}/ossp-uuid-master
        env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
        	./configure --prefix="${STAGE_DIR}" --without-pgsql --without-perl --without-php --disable-static --host=x86_64 ac_cv_va_copy=C99 --enable-shared
        make -j"$(nproc)" > /dev/null
        make install
    popd
}

build_libmemcached() {
	echo "=== Building libmemcached (${TARGET})..."
    curl -sLo- https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-1.0.18.tar.gz | tar xz -C ${BUILD_DIR}
    pushd ${BUILD_DIR}/libmemcached-1.0.18
    	patch -p1 -i /ffead-cpp/musl-fixes.patch
    	patch -s -p0 < /ffead-cpp/libmemcached-build.patch
    	if [ "$1" = "mingw-w64" ]
    	then
    		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CXX=${TARGET}-g++ CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LIB="-lstdc++" LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
        		./configure --host="${TARGET}" --prefix="${STAGE_DIR}"
    	else
    		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
        		./configure --host="${TARGET}" --prefix="${STAGE_DIR}"
    	fi
        make -j"$(nproc)" > /dev/null
        make install
    popd
}

build_mongocdriver() {
	echo "=== Building mongo-c-driver-${MONGOCDRIVER_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/mongodb/mongo-c-driver/releases/download/${MONGOCDRIVER_VERSION}/mongo-c-driver-${MONGOCDRIVER_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/mongo-c-driver-${MONGOCDRIVER_VERSION}
		if [ "$1" = "mingw-w64" ]
		then
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-D__USE_MINGW_ANSI_STDIO=1 -I${STAGE_DIR}/include" \
				--host="${TARGET}" ./configure --disable-tests --host="${TARGET}" --prefix=${STAGE_DIR} --disable-automatic-init-and-cleanup --disable-ssl --disable-sasl
		else
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
				--host="${TARGET}" ./configure --host="${TARGET}" --prefix=${STAGE_DIR} --disable-automatic-init-and-cleanup --disable-ssl --disable-sasl
		fi
		make -j"$(nproc)" > /dev/null
		make install
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

build() {
	TARGET="$1"
	ALIAS="$2"
	STAGE_DIR="${STAGE_ROOT}/${TARGET}"
	BUILD_DIR="${BUILD_ROOT}/${TARGET}"

	echo "=== Building target ${ALIAS} (${TARGET})..."

    rm -rf ${STAGE_DIR} ${BUILD_DIR}
	mkdir -p ${STAGE_DIR} ${BUILD_DIR}
	export PKG_CONFIG_PATH="${STAGE_DIR}/lib/pkgconfig"

	if [ "$2" = "mingw-w64" ]
	then
		install_cmake_mingw_w64_cross_file
	else
		install_cmake_musl_cross_file	
	fi

	build_zlib $2
	build_mongocdriver $2
	if [ "$2" != "mingw-w64" ]
	then
		build_libmemcached $2
	fi
	build_openssl
	build_unixodbc
	build_curl
	build_libcuckoo
	build_libhiredis
	build_uuid
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
  	ZLIB_VERSION=1.2.5
	STAGE_ROOT=${MINGW}
    build x86_64-w64-mingw32 $1
    ;;
  *)
    echo "usage: $0 i386|x86_64|arm|armhf|aarch64|mips|mipsel|mingw-w64" && exit 1
esac