FROM sumeetchhetri/ffead-cpp-base:6.0

WORKDIR ${IROOT}

ENV DEBIAN_FRONTEND noninteractive

RUN git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install latest && ./emsdk activate latest

ENV PATH=$PATH:/installs/emsdk:/installs/emsdk/node/14.18.2_64bit/bin:/installs/emsdk/upstream/emscripten
ENV EMSDK=/installs/emsdk
ENV EM_CONFIG=/installs/emsdk/.emscripten
ENV EM_CACHE=/installs/emsdk/upstream/emscripten/cache
ENV EMSDK_NODE=/installs/emsdk/node/14.18.2_64bit/bin/node

#RUN git clone https://github.com/madler/zlib && cd zlib && emcmake cmake -DBUILD_SHARED_LIBS=off . && emmake make -j4 install
RUN git clone https://github.com/sean-/ossp-uuid && cd ossp-uuid && emconfigure ./configure --disable-shared && emmake make -j4 && cp uuid.h /usr/local/include/ \
	&& cp .libs/libuuid.a /usr/local/lib
RUN git clone https://github.com/kkos/oniguruma && cd oniguruma && emcmake cmake -DBUILD_SHARED_LIBS=off -DENABLE_BINARY_COMPATIBLE_POSIX_API=on . && emmake make -j4 install
RUN git clone https://github.com/efficient/libcuckoo.git && cd libcuckoo && emcmake cmake . && emmake make install

RUN cd ${IROOT}/ffead-cpp-src/ && rm -rf CMakeFiles CMakeCache.txt build && mkdir build && cd build && emcmake cmake -DBUILD_SHARED_LIBS=off -DMOD_SDORM_SQL=off .. \
	&& emmake make -j4 install && rm -rf ${IROOT}/ffead-cpp-6.0 && mv ../ffead-cpp-6.0-bin ${IROOT}/ffead-cpp-6.0

WORKDIR ${IROOT}/lang-server-backends/javascript/emscripten

RUN emcc ffead-cpp-glue.cpp ${IROOT}/ffead-cpp-6.0/lib/libffead-framework.a ${IROOT}/ffead-cpp-6.0/lib/libffead-modules.a -s USE_PTHREADS=1 \
	-s USE_ZLIB=1 -s USE_PTHREADS=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -std=c++11 -o ffead-cpp-glue.js --bind
