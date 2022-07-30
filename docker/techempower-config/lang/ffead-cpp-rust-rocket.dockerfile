FROM sumeetchhetri/ffead-cpp-rust-rocket-base:6.1

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-6.0 rust-rocket
