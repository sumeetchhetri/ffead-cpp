#!/bin/bash
#
# This script should be run inside the tsl0922/musl-cross docker image
# Example:
#         docker run --rm -v $(pwd):/ffead-cpp -w /ffead-cpp tsl0922/musl-cross ./scripts/cross-build.sh mips
#
set -eo pipefail

BUILD_ROOT="${BUILD_ROOT:-/opt/build}"

ZLIB_VERSION="${ZLIB_VERSION:-1.2.11}"
OPENSSL_VERSION="${OPENSSL_VERSION:-1_1_1q}"
UNIXODBC_VERSION="${UNIXODBC_VERSION:-2.3.11}"
CURL_VERSION="${CURL_VERSION:-7.84.0}"
LIBCUCKOO_VERSION="${LIBCUCKOO_VERSION:-0.3.1}"
LIBHIREDIS_VERSION="${LIBHIREDIS_VERSION:-1.0.0}"
MONGOCDRIVER_VERSION="${MONGOCDRIVER_VERSION:-1.4.2}"

build_zlib() {
	echo "=== Building zlib-${ZLIB_VERSION} (${TARGET})..."
	curl -sLo- https://zlib.net/fossils/zlib-${ZLIB_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/zlib-${ZLIB_VERSION}
		env CHOST=${TARGET} ./configure --enable-shared --archs="-fPIC" --prefix=${STAGE_DIR}
		make -j"$(nproc)"
    	make install
	popd
}

build_openssl() {
    echo "=== Building openssl-${OPENSSL_VERSION} (${TARGET})..."
    curl -sLo ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}.zip "https://github.com/openssl/openssl/archive/OpenSSL_${OPENSSL_VERSION}.zip"
    unzip -qq ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}.zip -d ${BUILD_DIR}/
    pushd ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}
		./Configure linux-armv4 shared --cross-compile-prefix= -fPIC --prefix="${STAGE_DIR}" --openssldir=${STAGE_DIR}
        make -j"$(nproc)" > /dev/null
        make install_sw
    popd
}

build_unixodbc() {
	echo "=== Building unixodbc-${UNIXODBC_VERSION} (${TARGET})..."
	curl -sLo- https://ftp.osuosl.org/pub/blfs/conglomeration/unixODBC/unixODBC-${UNIXODBC_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/unixODBC-${UNIXODBC_VERSION}
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include \
			./configure --host="${TARGET}" --prefix=${STAGE_DIR}
		make -j"$(nproc)" > /dev/null
		make install
	popd
}

build_curl() {
	echo "=== Building curl-${CURL_VERSION} (${TARGET})..."
	#curl -sLo- https://curl.haxx.se/download/curl-${CURL_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	git clone https://github.com/curl/curl && mv curl ${BUILD_DIR}/curl-${CURL_VERSION}
	pushd ${BUILD_DIR}/curl-${CURL_VERSION}
		cmake -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} .
		make install
	popd
}

build_libcuckoo() {
	echo "=== Building libcuckoo-${LIBCUCKOO_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/efficient/libcuckoo/archive/v${LIBCUCKOO_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/libcuckoo-${LIBCUCKOO_VERSION}
		cmake -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} .
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
    curl -sLo ${BUILD_DIR}/ossp-uuid.zip "https://github.com/sean-/ossp-uuid/archive/master.zip"
    unzip -qq ${BUILD_DIR}/ossp-uuid.zip -d ${BUILD_DIR}/
    pushd ${BUILD_DIR}/ossp-uuid-master
    	mv /usr/bin/strip /tmp/strip
    	ln -s ${CROSS_ROOT}/bin/${TARGET}-strip /usr/bin/strip
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar STRIP=${TARGET}-strip RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
    		./configure --host="${TARGET}" --prefix="${STAGE_DIR}" --without-pgsql --without-perl --without-php --disable-static ac_cv_va_copy=C99 --enable-shared
        make -j"$(nproc)" > /dev/null
        make install
        ${TARGET}-gcc -c -fPIC -o uuid.po uuid.c
        ${TARGET}-gcc -c -fPIC -o uuid_cli.po uuid_cli.c
        ${TARGET}-gcc -c -fPIC -o uuid_dce.po uuid_dce.c
        ${TARGET}-gcc -c -fPIC -o uuid_mac.po uuid_mac.c
        ${TARGET}-gcc -c -fPIC -o uuid_md5.po uuid_md5.c
        ${TARGET}-gcc -c -fPIC -o uuid_prng.po uuid_prng.c
        ${TARGET}-gcc -c -fPIC -o uuid_sha1.po uuid_sha1.c
        ${TARGET}-gcc -c -fPIC -o uuid_str.po uuid_str.c
        ${TARGET}-gcc -c -fPIC -o uuid_time.po uuid_time.c
        ${TARGET}-gcc -c -fPIC -o uuid_ui128.po uuid_ui128.c
        ${TARGET}-gcc -c -fPIC -o uuid_ui64.po uuid_ui64.c
        ${TARGET}-gcc -shared -fPIC -Wl,-soname,libossp-uuid.so -o libossp-uuid.so *.po
        cp libossp-uuid.so ${STAGE_DIR}/lib/
        rm -f /usr/bin/strip
        mv /tmp/strip /usr/bin/strip
    popd
}

build_libmemcached() {
	#https://gitea.de/marco/aports/commit/6d7e800e8f8d0b033d99c5cc1f00dbc7fa1592ee
	echo "=== Building libmemcached (${TARGET})..."
    curl -sLo- https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-1.0.18.tar.gz | tar xz -C ${BUILD_DIR}
    pushd ${BUILD_DIR}/libmemcached-1.0.18
    	patch -p1 -i /ffead-cpp/musl-fixes.patch
    	patch -s -p0 < /ffead-cpp/libmemcached-build.patch
    	echo -e "#include <stdarg.h>\n$(cat libtest/exception.hpp)" > libtest/exception.hpp
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
    		./configure --host="${TARGET}" --prefix="${STAGE_DIR}"
        make -j"$(nproc)" > /dev/null
        make install
    popd
}

build_mongocdriver() {
	echo "=== Building mongo-c-driver-${MONGOCDRIVER_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/mongodb/mongo-c-driver/releases/download/${MONGOCDRIVER_VERSION}/mongo-c-driver-${MONGOCDRIVER_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/mongo-c-driver-${MONGOCDRIVER_VERSION}
		env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
			./configure --host="${TARGET}" --prefix=${STAGE_DIR} --disable-automatic-init-and-cleanup --disable-ssl --disable-sasl
		make -j"$(nproc)" > /dev/null
		make install
	popd
}


init() {
	TARGET="$1"
	ALIAS="$1"
	STAGE_DIR="${CROSS_ROOT}"
	BUILD_DIR="${BUILD_ROOT}/${TARGET}"
	export PATH=${CROSS_ROOT}/bin:$PATH

	echo "=== Building target ${ALIAS} (${TARGET})..."

    rm -rf ${BUILD_DIR}
	mkdir -p ${BUILD_DIR}
	export PKG_CONFIG_PATH="/usr/lib/${CROSS_ROOT}/pkgconfig/"
}

build() {
	build_zlib
	build_libmemcached
	build_mongocdriver
	build_openssl
	build_unixodbc
	build_libcuckoo
	build_libhiredis
	build_uuid
	build_curl
}

init $1
build
