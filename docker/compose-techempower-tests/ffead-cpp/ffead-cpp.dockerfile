FROM buildpack-deps:latest

ENV IROOT=/installs
ENV FFEAD_CPP_PATH=${IROOT}/ffead-cpp-5.0
ENV PATH=${FFEAD_CPP_PATH}:${PATH}
ENV TROOT=/

ENV DEBIAN_FRONTEND noninteractive
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

RUN mkdir /installs

WORKDIR /

COPY *.sh ./
RUN chmod 755 *.sh

RUN ./install_ffead-cpp-dependencies.sh

WORKDIR /

RUN ./install_ffead-cpp-framework.sh

WORKDIR /

RUN ./install_ffead-cpp-httpd.sh

WORKDIR /

RUN ./install_ffead-cpp-nginx.sh

WORKDIR /

CMD ./run_ffead.sh emb mongo
