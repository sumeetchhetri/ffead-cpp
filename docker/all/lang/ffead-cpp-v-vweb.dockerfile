FROM sumeetchhetri/ffead-cpp-v-base:7.0-all

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0 v-vweb
