FROM buildpack-deps:bionic

ENV IROOT=/installs

COPY te-benchmark-um/ te-benchmark-um/
COPY *.sh ./

RUN mkdir /installs && chmod 755 *.sh && /install_ffead-cpp-dependencies.sh
