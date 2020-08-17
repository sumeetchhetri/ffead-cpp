FROM sumeetchhetri/ffead-cpp-4.0-rust-rocket-base:2.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-4.0 rust-rocket
