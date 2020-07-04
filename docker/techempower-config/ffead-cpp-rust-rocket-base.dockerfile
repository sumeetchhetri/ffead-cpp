FROM sumeetchhetri/ffead-cpp-v4.0-rust-base:1.0
LABEL maintainer="Sumeet Chhetri"
LABEL version="1.0"
LABEL description="Base rust rocket docker image with ffead-cpp v4.0"

ENV IROOT=/installs

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
WORKDIR ${IROOT}/lang-server-backends/rust/rocket-ffead-cpp/
ENV PATH="/root/.cargo/bin:${PATH}"
RUN rustup default nightly && cargo update && cargo build --release && cp target/release/rocket-ffead-cpp $IROOT/ && rm -rf ${IROOT}/lang-server-backends
RUN rm -rf /root/.rustup /root/.cargo

FROM buildpack-deps:bionic
COPY --from=0 /installs/ffead-cpp-4.0 /installs/ffead-cpp-4.0
COPY --from=0 /installs/rocket-ffead-cpp /installs/
RUN mkdir /installs/snmalloc-0.4.2
COPY --from=0 /installs/snmalloc-0.4.2/build/libsnmallocshim-1mib.so /installs/snmalloc-0.4.2
COPY --from=0 /usr/lib/x86_64-linux-gnu/libcurl.so* /usr/lib/x86_64-linux-gnu/
COPY --from=0 /usr/lib/x86_64-linux-gnu/libuuid.so* /usr/lib/x86_64-linux-gnu/
COPY --from=0 /usr/lib/x86_64-linux-gnu/libhiredis.so* /usr/lib/x86_64-linux-gnu/
COPY --from=0 /usr/lib/x86_64-linux-gnu/libmemcached*.so* /usr/lib/x86_64-linux-gnu/
COPY --from=0 /usr/lib/x86_64-linux-gnu/libodbc.so* /usr/lib/x86_64-linux-gnu/
COPY --from=0 /usr/local/lib /usr/local/lib
COPY --from=0 /run_ffead.sh /
