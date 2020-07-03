FROM sumeetchhetri/ffead-cpp-v4.0-rust-base:1.0

ENV IROOT=/installs

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
WORKDIR ${IROOT}/lang-server-backends/rust/rocket-ffead-cpp/
ENV PATH="/root/.cargo/bin:${PATH}"
RUN rustup default nightly && cargo update && cargo build --release && cp target/release/rocket-ffead-cpp $IROOT/ && rm -rf ${IROOT}/lang-server-backends

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-4.0 rust-rocket
