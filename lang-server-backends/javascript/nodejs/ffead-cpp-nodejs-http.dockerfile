FROM sumeetchhetri/ffead-cpp-all-base:7.0

ENV NODE_MAJOR 20
WORKDIR ${IROOT}

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y && apt-get install -y ca-certificates curl gnupg && mkdir -p /etc/apt/keyrings
RUN curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg
RUN echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_MAJOR.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list
RUN apt-get update -y && apt-get install nodejs -y

WORKDIR ${IROOT}/lang-server-backends/javascript/nodejs
#COPY server.js package.json .
RUN npm install http ffi-napi ref-napi ref-struct-di ref-array-di

CMD node server.js 8080 /installs/ffead-cpp-7.0
