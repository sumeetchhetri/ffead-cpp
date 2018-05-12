FROM buildpack-deps:xenial

ENV IROOT=/installs
ENV FFEAD_CPP_PATH=${IROOT}/ffead-cpp-2.0
ENV PATH=${FFEAD_CPP_PATH}:${PATH}
ENV FFEAD_TEST_TYPE=sql
ENV FFEAD_TEST_DB_NAME=mysql
ENV FFEAD_ENABLE_SDORM_SQL="yes"
ENV FFEAD_ENABLE_SDORM_MONGO="no"
ENV FFEAD_ENABLE_APACHEMOD="yes"
ENV FFEAD_ENABLE_NGINXMOD="no"
ENV FFEAD_ENABLE_REDIS="no"
ENV FFEAD_ENABLE_MEMCACHED="no"
ENV FFEAD_ENABLE_DEBUG="no"
ENV ADD_EXTRA_LIB=""

RUN mkdir /installs

WORKDIR /

COPY te-benchmark/ te-benchmark/
COPY *.sh ./
RUN chmod 755 *.sh

RUN ./ffead-cpp-dependencies.sh

WORKDIR /

RUN ./ffead-cpp-framework.sh

WORKDIR /

RUN ./ffead-cpp-httpd.sh

ENV PATH=${IROOT}/httpd/bin:${PATH}

CMD apachectl -D FOREGROUND
