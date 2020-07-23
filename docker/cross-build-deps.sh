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

ANDROID_HOME="/usr/lib/android-sdk"
ANDROID_NDK_ROOT=$ANDROID_HOME/ndk-bundle
NDK=$ANDROID_NDK_ROOT
TOOLCHAIN="$NDK/toolchains/llvm/prebuilt/linux-x86_64"
ANDROID_API=21
ANDROID_ARCH=

build_zlib() {
	echo "=== Building zlib-${ZLIB_VERSION} (${TARGET})..."
	curl -sLo- https://zlib.net/zlib-${ZLIB_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/zlib-${ZLIB_VERSION}
		if [ "$1" = "mingw-w64" ]
    	then
    		env CHOST=${TARGET} ./configure --enable-shared --prefix=${STAGE_DIR}
    		make -j"$(nproc)"
    	elif [ "$1" = "android" ]
    	then
    		env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip CHOST=${TARGET} \
				./configure --enable-shared --prefix=${STAGE_DIR}
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
    curl -sLo ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}.zip "https://github.com/openssl/openssl/archive/OpenSSL_${OPENSSL_VERSION}.zip"
    unzip -qq ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}.zip -d ${BUILD_DIR}/
    pushd ${BUILD_DIR}/openssl-OpenSSL_${OPENSSL_VERSION}
    	if [ "$1" = "android" ]
    	then
    		wget https://wiki.openssl.org/images/7/70/Setenv-android.sh
    		chmod +x Setenv-android.sh
    		dos2unix Setenv-android.sh
    		sed -i'' -e's|_ANDROID_EABI="arm-linux-androideabi-4.8"|_ANDROID_EABI="${1}"|g' Setenv-android.sh
    		sed -i'' -e's|_ANDROID_ARCH=arch-arm|_ANDROID_ARCH=${2}|g' Setenv-android.sh
    		sed -i'' -e's|_ANDROID_API="android-18"|_ANDROID_API="${API}"|g' Setenv-android.sh
    		export ANDROID_NDK_ROOT=$NDK
    		./Setenv-android.sh
    		./config shared no-ssl2 no-ssl3 no-comp no-hw no-engine --prefix="${STAGE_DIR}" --openssldir=${STAGE_DIR}
			make depend
			make all
			make install_sw
		else
			./Configure linux-generic64 shared --cross-compile-prefix=${TARGET}- -fPIC --prefix="${STAGE_DIR}" --openssldir=${STAGE_DIR}
	        make -j"$(nproc)" > /dev/null
	        make install_sw
		fi
    popd
}

build_unixodbc() {
	echo "=== Building unixodbc-${UNIXODBC_VERSION} (${TARGET})..."
	curl -sLo- http://www.unixodbc.org/unixODBC-${UNIXODBC_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/unixODBC-${UNIXODBC_VERSION}
		if [ "$1" = "android" ]
    	then
    		env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip C_INCLUDE_PATH=${STAGE_DIR}/include \
				./configure --host="${TARGET}" --prefix=${STAGE_DIR}
    	else
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include \
				./configure --host="${TARGET}" --prefix=${STAGE_DIR}
		fi
		make -j"$(nproc)" > /dev/null
		make install
	popd
}

build_curl() {
	echo "=== Building curl-${CURL_VERSION} (${TARGET})..."
	curl -sLo- https://curl.haxx.se/download/curl-${CURL_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/curl-${CURL_VERSION}
		if [ "$1" = "android" ]
    	then
    		env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip LIBS="-lssl -lcrypto" \
				LDFLAGS="-L${STAGE_DIR}/lib" CPPFLAGS="-I${STAGE_DIR}/include" ./configure --build=`./config.guess` --target="${TARGET}" --host="${TARGET}" --prefix=${STAGE_DIR} \
				--with-ssl=${STAGE_DIR} --with-zlib --disable-ftp --disable-gopher --disable-file --disable-imap --disable-ldap --disable-ldaps --disable-pop3 --disable-proxy \
				--disable-rtsp --disable-smtp --disable-telnet --disable-tftp --without-gnutls --without-libidn --without-librtmp --disable-dict
    	else
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib \
				LDFLAGS="-L${STAGE_DIR}/lib" CPPFLAGS="-I${STAGE_DIR}/include" ./configure --build=`./config.guess` --target="${TARGET}" --host="${TARGET}" --prefix=${STAGE_DIR} \
				--with-ssl=${STAGE_DIR} --with-zlib --disable-ftp --disable-gopher --disable-file --disable-imap --disable-ldap --disable-ldaps --disable-pop3 --disable-proxy \
				--disable-rtsp --disable-smtp --disable-telnet --disable-tftp --without-gnutls --without-libidn --without-librtmp --disable-dict
		fi
		make -j"$(nproc)" > /dev/null
		make install
	popd
}

build_libcuckoo() {
	echo "=== Building libcuckoo-${LIBCUCKOO_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/efficient/libcuckoo/archive/v${LIBCUCKOO_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/libcuckoo-${LIBCUCKOO_VERSION}
		if [ "$1" = "android" ]
		then
			cmake -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake -DANDROID_NDK=${ANDROID_NDK_ROOT} \
				-DANDROID_ABI=${ANDROID_ARCH} -DANDROID_PLATFORM=android-${ANDROID_API} -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} .
		else
			cmake -DCMAKE_TOOLCHAIN_FILE=${BUILD_DIR}/cross-${TARGET}.cmake -DCMAKE_INSTALL_PREFIX=${STAGE_DIR} .
		fi
		make install
	popd
}

build_libhiredis() {
	echo "=== Building libhiredis-${LIBHIREDIS_VERSION} (${TARGET})..."
	curl -sLo- https://github.com/redis/hiredis/archive/v${LIBHIREDIS_VERSION}.tar.gz | tar xz -C ${BUILD_DIR}
	pushd ${BUILD_DIR}/hiredis-${LIBHIREDIS_VERSION}
		if [ "$1" = "android" ]
    	then
    		env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip C_INCLUDE_PATH=${STAGE_DIR}/include \
				make -j"$(nproc)" > /dev/null
			env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip C_INCLUDE_PATH=${STAGE_DIR}/include \
				PREFIX=${STAGE_DIR} make install
    	else
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include make -j"$(nproc)" > /dev/null
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include PREFIX=${STAGE_DIR} make install
		fi
	popd
}

build_uuid() {
	if [ "$1" = "android" ]
	then
		echo "=== Building e2fsprogs-uuid (${TARGET})..."
		curl -sLo- "https://mirrors.edge.kernel.org/pub/linux/kernel/people/tytso/e2fsprogs/v1.45.6/e2fsprogs-1.45.6.tar.gz" | tar xz -C ${BUILD_DIR}
		pushd ${BUILD_DIR}/e2fsprogs-1.45.6
			env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip C_INCLUDE_PATH=${STAGE_DIR}/include \
				./configure --enable-elf-shlibs --host="${TARGET}" --prefix=${STAGE_DIR}
		popd
		pushd ${BUILD_DIR}/e2fsprogs-1.45.6/lib/uuid
			CFLAGS="-I../../util" make install
	    popd
    else
		echo "=== Building ossp-uuid (${TARGET})..."
	    curl -sLo ${BUILD_DIR}/ossp-uuid.zip "https://github.com/sean-/ossp-uuid/archive/master.zip"
	    unzip -qq ${BUILD_DIR}/ossp-uuid.zip -d ${BUILD_DIR}/
	    pushd ${BUILD_DIR}/ossp-uuid-master
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
        		./configure --prefix="${STAGE_DIR}" --without-pgsql --without-perl --without-php --disable-static --host=x86_64 ac_cv_va_copy=C99 --enable-shared
	        make -j"$(nproc)" > /dev/null
	        make install
	    popd
	fi
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
    	elif [ "$1" = "android" ]
    	then
			env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip C_INCLUDE_PATH=${STAGE_DIR}/include \
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
				./configure --disable-tests --host="${TARGET}" --prefix=${STAGE_DIR} --disable-automatic-init-and-cleanup --disable-ssl --disable-sasl
		elif [ "$1" = "android" ]
    	then
			env AR=$TOOLCHAIN/bin/$TARGET-ar AS=$TOOLCHAIN/bin/$TARGET-as CC=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang CXX=$TOOLCHAIN/bin/$TARGET$ANDROID_API-clang++ \
				LD=$TOOLCHAIN/bin/$TARGET-ld RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib STRIP=$TOOLCHAIN/bin/$TARGET-strip C_INCLUDE_PATH=${STAGE_DIR}/include \
				LDFLAGS="-L${STAGE_DIR}/lib -lrt" ./configure --disable-tests --host="${TARGET}" --prefix=${STAGE_DIR} --disable-automatic-init-and-cleanup --disable-ssl --disable-sasl
    	else
			env NM=${TARGET}-nm AS=${TARGET}-as LD=${TARGET}-ld CC=${TARGET}-gcc AR=${TARGET}-ar RANLIB=${TARGET}-ranlib C_INCLUDE_PATH=${STAGE_DIR}/include LDFLAGS="-L${STAGE_DIR}/lib" CFLAGS="-I${STAGE_DIR}/include" \
				./configure --host="${TARGET}" --prefix=${STAGE_DIR} --disable-automatic-init-and-cleanup --disable-ssl --disable-sasl
		fi
		make -j"$(nproc)" > /dev/null
		make install
	popd
}

build_android_openssl_curl() {
	cp build-android*.sh ${BUILD_ROOT}/
	cp build-common*.sh ${BUILD_ROOT}/
	pushd ${BUILD_ROOT}
		chmod +x *.sh
		./build-android-openssl.sh $1 $2 $3 ${STAGE_DIR}/ ${BUILD_ROOT}/
		./build-android-nghttp2.sh $1 $2 $3 ${STAGE_DIR}/ ${BUILD_ROOT}/
		./build-android-curl.sh $1 $2 $3 ${STAGE_DIR}/ ${BUILD_ROOT}/
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

    rm -rf ${STAGE_DIR} ${BUILD_DIR}
	mkdir -p ${STAGE_DIR} ${BUILD_DIR} ${BUILD_ROOT}/${ANDROID_ARCH}
	export PKG_CONFIG_PATH="${STAGE_DIR}/lib/pkgconfig"
}

build() {
	if [ "$1" = "android" ]
	then
		build_unixodbc $1
		build_libhiredis $1
		build_uuid $1
		build_libcuckoo $1
	else
		build_zlib $1
		if [ "$1" == "musl" ]
		then
			build_libmemcached $1
			build_mongocdriver $1
		else
			build_mongocdriver $1
		fi
		build_openssl $1
		build_unixodbc $1
		build_libcuckoo $1
		build_libhiredis $1
		build_uuid $1
		build_curl $1
	fi
}

if [ "$1" = "musl" ]
then
	install_cmake_musl_cross_file
	case $2 in
	  i386|x86_64|aarch64|mips|mipsel)
	  	init musl $1-linux-musl $2
	    build musl $1-linux-musl $2
	    ;;
	  arm)
	    init musl arm-linux-musleabi $2
	    build musl arm-linux-musleabi $2
	    ;;
	  armhf)
	    init musl arm-linux-musleabihf $2
	    build musl arm-linux-musleabihf $2
	    ;;
	  *)
	    echo "usage: $0 musl i386|x86_64|arm|armhf|aarch64|mips|mipsel" && exit 1
	esac
fi

if [ "$1" = "mingw-w64" ]
then
	install_cmake_mingw_w64_cross_file
	ZLIB_VERSION=1.2.5
	STAGE_ROOT=${MINGW}
	init mingw-w64 x86_64-w64-mingw32
    build mingw-w64 x86_64-w64-mingw32
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
	    build_android_openssl_curl $2 $3 arm
	    build android armv7a-linux-androideabi x86_32
	    ;;
	  arm64-v8a)
	    init android aarch64-linux-android arm64
	    build_android_openssl_curl $2 $3 arm64
	    build android aarch64-linux-android arm64
	    ;;
	  x86)
	    init android i686-linux-android x86
	    build_android_openssl_curl $2 $3 x86
	    build android i686-linux-android x86
	    ;;
	  x86-64)
	    init android x86_64-linux-android x86-64
	    build_android_openssl_curl $2 $3 x86-64
	    build android x86_64-linux-android x86-64
	    ;;
	  *)
	    echo "usage: $0 android ANDROID_HOME ANDROID_NDK_ROOT armeabi-v7a|arm64-v8a|x86|x86-64 API" && exit 1
	esac
fi
