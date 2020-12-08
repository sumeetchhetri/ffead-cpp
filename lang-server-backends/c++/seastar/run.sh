#!/bin/sh

cd ${IROOT}/lang-server-backends/c++/seastar
#./ffead-cpp-seastar --port=8080 --address=0.0.0.0 --fcpdir=${FFEAD_CPP_PATH} -c$(nproc) --network-stack native
./ffead-cpp-seastar --port=8080 --address=0.0.0.0 --fcpdir=${FFEAD_CPP_PATH} -c$(nproc)