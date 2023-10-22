FROM sumeetchhetri/ffead-cpp-base:7.0

WORKDIR ${IROOT}

ENV DEBIAN_FRONTEND noninteractive

RUN curl -sL https://deb.nodesource.com/setup_14.x | bash -
RUN apt-get -y install nodejs curl gnupg

WORKDIR ${IROOT}/lang-server-backends/javascript/nodejs-http
#COPY server.js package.json .
RUN npm install http ffi-napi ref-napi ref-struct-di ref-array-di

CMD node server.js 8080 /installs/ffead-cpp-7.0
