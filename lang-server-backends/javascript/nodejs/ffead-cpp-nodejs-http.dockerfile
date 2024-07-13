FROM sumeetchhetri/ffead-cpp-base:7.0

#ENV NODE_MAJOR 16
#ffi-napi install fails for latest node versions -- not failing anymore
WORKDIR ${IROOT}

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y && apt-get install nodejs npm -y

RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libinter.so /usr/local/lib/libinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libdinter.so /usr/local/lib/libdinter.so && \
	ldconfig

WORKDIR ${IROOT}/lang-server-backends/javascript/nodejs
RUN npm install http ffi-napi ref-napi ref-struct-di ref-array-di

CMD node server.js 8080 /installs/ffead-cpp-7.0
