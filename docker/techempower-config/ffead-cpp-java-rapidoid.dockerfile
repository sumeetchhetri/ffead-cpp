FROM sumeetchhetri/ffead-cpp-5.0-java-base:2.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-5.0 java-rapidoid
