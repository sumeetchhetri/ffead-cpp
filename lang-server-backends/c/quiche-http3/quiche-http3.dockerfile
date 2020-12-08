FROM sumeetchhetri/ffead-cpp-5.0-base:5.3

WORKDIR ${IROOT}
RUN mkdir quiche-http3

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -yqq libev-dev

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"

RUN git clone https://github.com/troydhanson/uthash
RUN cp ${IROOT}/uthash/src/*.h /usr/local/include/

RUN git clone --recursive https://github.com/cloudflare/quiche
RUN cd quiche && cargo build --release && cp target/release/libquiche.a target/release/libquiche.so /usr/local/lib \
	&& cp include/quiche.h /usr/local/include/
RUN rm -rf ${IROOT}/uthash ${IROOT}/quiche

RUN rustup self uninstall -y

RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-5.0/lib/libte_benchmark_um.so /usr/local/lib/libte_benchmark_um.so && \
	ln -s ${IROOT}/ffead-cpp-5.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-5.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ln -s ${IROOT}/ffead-cpp-5.0/lib/libinter.so /usr/local/lib/libinter.so && \
	ln -s ${IROOT}/ffead-cpp-5.0/lib/libdinter.so /usr/local/lib/libdinter.so && \
	ldconfig

COPY cert-big.crt cert.crt cert.key ffead-cpp.h http3-server.c ${IROOT}/quiche-http3/

WORKDIR ${IROOT}/quiche-http3
RUN gcc -I. -Wno-unused-result http3-server.c -flto -lquiche -lev -lffead-framework -O3 -o ffead-cpp-quiche-http3 \
	&& chmod +x ffead-cpp-quiche-http3

ENV FFEAD_CPP_PATH=${IROOT}/ffead-cpp-5.0
ENV LD_LIBRARY_PATH=${IROOT}/:${IROOT}/lib:${FFEAD_CPP_PATH}/lib:/usr/local/lib:$LD_LIBRARY_PATH

CMD ./ffead-cpp-quiche-http3 0.0.0.0 8080 ${IROOT}/ffead-cpp-5.0
