FROM sumeetchhetri/ffead-cpp-v4.0-rust-base

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-4.0 rust-hyper
