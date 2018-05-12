FROM buildpack-deps:xenial

ENV IROOT=/installs
ENV FFEAD_CPP_PATH=${IROOT}/ffead-cpp-2.0
ENV PATH=${FFEAD_CPP_PATH}:${PATH}
ENV FFEAD_TEST_TYPE=mongo
ENV FFEAD_TEST_DB_NAME=mongo
ENV FFEAD_ENABLE_SDORM_SQL="no"
ENV FFEAD_ENABLE_SDORM_MONGO="yes"
ENV FFEAD_ENABLE_APACHEMOD="no"
ENV FFEAD_ENABLE_NGINXMOD="yes"
ENV FFEAD_ENABLE_REDIS="yes"
ENV FFEAD_ENABLE_MEMCACHED="no"
ENV FFEAD_ENABLE_DEBUG="no"
ENV ADD_EXTRA_LIB="-lhiredis"

RUN mkdir /installs

WORKDIR /

COPY te-benchmark/ te-benchmark/
COPY *.sh ./
RUN chmod 755 *.sh

RUN ./ffead-cpp-dependencies.sh

WORKDIR /

RUN ./ffead-cpp-redis.sh

WORKDIR /

RUN ./ffead-cpp-framework.sh

WORKDIR /

RUN ./ffead-cpp-nginx.sh

RUN cp -f ${IROOT}/ffead-cpp-2.0/web/te-benchmark/config/cacheredis.xml ${IROOT}/ffead-cpp-2.0/web/te-benchmark/config/cache.xml

ENV PATH=${IROOT}/nginxfc/sbin:${PATH}
ENV LD_LIBRARY_PATH=${IROOT}/:${IROOT}/lib:${FFEAD_CPP_PATH}/lib:$LD_LIBRARY_PATH
ENV ODBCINI=${IROOT}/odbc.ini
ENV ODBCSYSINI=${IROOT}

CMD nginx -g 'daemon off;'
