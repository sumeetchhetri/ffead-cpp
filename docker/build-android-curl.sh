#!/bin/bash
#
# Copyright 2016 leenjewel
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# # read -n1 -p "Press any key to continue..."

set -u

export ANDROID_HOME=$1
export ANDROID_NDK_ROOT=$2

source ./build-android-common.sh $1 $2 $6

#init_log_color

TOOLS_ROOT=$(pwd)

SOURCE="$0"
while [ -h "$SOURCE" ]; do
    DIR="$(cd -P "$(dirname "$SOURCE")" && pwd)"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
pwd_path="$(cd -P "$(dirname "$SOURCE")" && pwd)"

echo pwd_path=${pwd_path}
echo TOOLS_ROOT=${TOOLS_ROOT}

LIB_VERSION="curl-7_84_0"
LIB_NAME="curl-7.84.0"
#LIB_DEST_DIR="${pwd_path}/../output/android/curl-universal"

echo "https://github.com/curl/curl/releases/download/${LIB_VERSION}/${LIB_NAME}.tar.gz"

# https://curl.haxx.se/download/${LIB_NAME}.tar.gz
# https://github.com/curl/curl/releases/download/curl-7_69_0/curl-7.69.0.tar.gz
# https://github.com/curl/curl/releases/download/curl-7_68_0/curl-7.68.0.tar.gz
DEVELOPER=
SDK_VERSION=
#rm -rf "${LIB_DEST_DIR}" "${LIB_NAME}"
[ -f "${LIB_NAME}.tar.gz" ] || curl -LO https://github.com/curl/curl/releases/download/${LIB_VERSION}/${LIB_NAME}.tar.gz >${LIB_NAME}.tar.gz

set_android_toolchain_bin

function configure_make() {

    ARCH=$1
    ABI=$2
    ABI_TRIPLE=$3

    log_info "configure $ABI start..."

    if [ -d "${LIB_NAME}" ]; then
        rm -fr "${LIB_NAME}"
    fi
    tar xfz "${LIB_NAME}.tar.gz"
    pushd .
    cd "${LIB_NAME}"

    PREFIX_DIR="${4}"
    #if [ -d "${PREFIX_DIR}" ]; then
    #    rm -fr "${PREFIX_DIR}"
    #fi
    #mkdir -p "${PREFIX_DIR}"

    OUTPUT_ROOT=${5}
    mkdir -p ${OUTPUT_ROOT}/log

    set_android_toolchain "curl" "${ARCH}" "${ANDROID_API}"
    #set_android_cpu_feature "curl" "${ARCH}" "${ANDROID_API}"

    export ANDROID_NDK_HOME=${ANDROID_NDK_ROOT}
    echo ANDROID_NDK_HOME=${ANDROID_NDK_HOME}

    export LDFLAGS="-L${4}/lib"
    # export LDFLAGS="-Wl,-rpath-link,-L${4}/lib $LDFLAGS "

    android_printf_global_params "$ARCH" "$ABI" "$ABI_TRIPLE" "$PREFIX_DIR" "$OUTPUT_ROOT"

    if [[ "${ARCH}" == "x86_64" ]]; then

        ./configure --host=$(android_get_build_host "${ARCH}") --prefix="${PREFIX_DIR}" --enable-ipv6 --with-ssl=${4} --with-nghttp2=${4} >"${OUTPUT_ROOT}/log/${ABI}.log" 2>&1

    elif [[ "${ARCH}" == "x86" ]]; then

        ./configure --host=$(android_get_build_host "${ARCH}") --prefix="${PREFIX_DIR}" --enable-ipv6 --with-ssl=${4} --with-nghttp2=${4} >"${OUTPUT_ROOT}/log/${ABI}.log" 2>&1

    elif [[ "${ARCH}" == "arm" ]]; then

        ./configure --host=$(android_get_build_host "${ARCH}") --prefix="${PREFIX_DIR}" --enable-ipv6 --with-ssl=${4} --with-nghttp2=${4} >"${OUTPUT_ROOT}/log/${ABI}.log" 2>&1

    elif [[ "${ARCH}" == "arm64" ]]; then

        # --enable-shared need nghttp2 cpp compile
        ./configure --host=$(android_get_build_host "${ARCH}") --prefix="${PREFIX_DIR}" --disable-shared --enable-ipv6 --with-ssl=${4} --with-nghttp2=${4} >"${OUTPUT_ROOT}/log/${ABI}.log" 2>&1

    else
        log_error "not support" && exit 1
    fi

    log_info "make $ABI start..."

    make clean >>"${OUTPUT_ROOT}/log/${ABI}.log"
    if make -j$(get_cpu_count) >>"${OUTPUT_ROOT}/log/${ABI}.log" 2>&1; then
        make install >>"${OUTPUT_ROOT}/log/${ABI}.log" 2>&1
    fi

    popd
}

log_info "${PLATFORM_TYPE} ${LIB_NAME} start..."

for ((i = 0; i < ${#ARCHS[@]}; i++)); do
    if [[ $# -eq 0 || "$3" == "${ARCHS[i]}" ]]; then
        configure_make "${ARCHS[i]}" "${ABIS[i]}" "${ABI_TRIPLES[i]}" "$4" "$5"
    fi
done

log_info "${PLATFORM_TYPE} ${LIB_NAME} end..."
