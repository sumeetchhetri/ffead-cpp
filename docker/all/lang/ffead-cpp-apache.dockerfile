FROM sumeetchhetri/ffead-cpp-all-base:7.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0 apache mongo
