FROM sumeetchhetri/ffead-cpp-base:7.0-debug
LABEL maintainer="Sumeet Chhetri"
LABEL version="7.0"
LABEL description="Base rust docker image with ffead-cpp v4.0 - commit id - master"

ENV IROOT=/installs

RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libinter.so /usr/local/lib/libinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libdinter.so /usr/local/lib/libdinter.so && \
	ldconfig

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"

RUN cd ${IROOT}/lang-server-backends/rust/actix-ffead-cpp && RUSTFLAGS="-C target-cpu=native" cargo build && cp target/debug/actix-ffead-cpp $IROOT/ && rm -rf target && \
	cd ${IROOT}/lang-server-backends/rust/hyper-ffead-cpp && RUSTFLAGS="-C target-cpu=native" cargo build && cp target/debug/hyper-ffead-cpp $IROOT/ && rm -rf target && \
	cd ${IROOT}/lang-server-backends/rust/thruster-ffead-cpp && RUSTFLAGS="-C target-cpu=native" cargo build && cp target/debug/thruster-ffead-cpp $IROOT/ && rm -rf target && \
	rm -rf ${IROOT}/lang-server-backends && rm -rf /root/.rustup /root/.cargo
