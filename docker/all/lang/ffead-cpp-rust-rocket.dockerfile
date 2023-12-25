FROM sumeetchhetri/ffead-cpp-rust-rocket-base:7.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0 rust-rocket
