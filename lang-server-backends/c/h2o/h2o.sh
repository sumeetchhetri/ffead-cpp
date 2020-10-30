#!/bin/bash

set -e

export FFEAD_CPP_PATH=$1
export LD_LIBRARY_PATH=$2
PORT=$3

CPU_COUNT=$(nproc)
H2O_APP_PROFILE_PORT=54321
H2O_APP_PROFILE_URL="http://127.0.0.1:$H2O_APP_PROFILE_PORT"
SCRIPT_PATH=$(realpath "$0")
H2O_APP_SRC_ROOT=$(dirname "$SCRIPT_PATH")
H2O_APP_BUILD_DIR="${H2O_APP_SRC_ROOT}/build"

if [[ -z "$DBHOST" ]]; then
	DBHOST=tfb-database
fi

if [[ -z "$H2O_APP_PREFIX" ]]; then
	H2O_APP_PREFIX=/opt/h2o_app
fi

if [[ -z "$H2O_PREFIX" ]]; then
	H2O_PREFIX=/usr
fi

if [[ -z "$MUSTACHE_C_PREFIX" ]]; then
	MUSTACHE_C_PREFIX=/opt/mustache-c
fi

# A hacky way to detect whether we are running in the physical hardware or the cloud environment.
if [[ "$CPU_COUNT" -gt 16 ]]; then
	#echo "Running h2o_app in the physical hardware environment."
	DB_CONN=5
else
	#echo "Running h2o_app in the cloud environment."
	DB_CONN=5
fi

build_h2o_app()
{
	cmake -DCMAKE_INSTALL_PREFIX="$H2O_APP_PREFIX" -DCMAKE_BUILD_TYPE=Release \
	      -DCMAKE_PREFIX_PATH="${H2O_PREFIX}" \
	      -DCMAKE_C_FLAGS="-march=native $1" "$H2O_APP_SRC_ROOT"
	make -j "$CPU_COUNT"
}

run_curl()
{
	for ((i = 0; i < 10; i++)); do
		curl "${H2O_APP_PROFILE_URL}/$1" > /dev/null 2>&1
	done
}

run_h2o_app()
{
	for i in $(seq 0 $(($(nproc --all)-1))); do
		taskset -c "$1" "$2/h2o_app" -a20 $4 -u ${FFEAD_CPP_PATH} -t1 &
	done
}

generate_profile_data()
{
	run_h2o_app 0 . "$H2O_APP_SRC_ROOT" "-p$H2O_APP_PROFILE_PORT" -t1
	local -r H2O_APP_PROFILE_PID=$!
	while ! curl "$H2O_APP_PROFILE_URL" > /dev/null 2>&1; do sleep 1; done
	run_curl json
	run_curl plaintext
	kill -s SIGTERM "$H2O_APP_PROFILE_PID"
	wait "$H2O_APP_PROFILE_PID"
}

install -d "$H2O_APP_BUILD_DIR"
pushd "$H2O_APP_BUILD_DIR"
build_h2o_app "-fprofile-generate"
#generate_profile_data
make clean
rm -f CMakeCache.txt
build_h2o_app "-fprofile-use"
make -j "$CPU_COUNT" install
popd
rm -rf "$H2O_APP_BUILD_DIR"
#echo "Maximum database connections per thread: $DB_CONN"
run_h2o_app 0 "${H2O_APP_PREFIX}/bin" "${H2O_APP_PREFIX}/share/h2o_app" ${PORT}
