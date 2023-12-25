FROM sumeetchhetri/ffead-cpp-sql-raw-async-pool-profiled-base:7.0

RUN apt remove -yqq postgresql-${PG_VERSION} postgresql-contrib-${PG_VERSION} gnupg lsb-release && apt autoremove -yqq
RUN rm -rf /ssd/postgresql && rm -rf /tmp/postgresql && rm -rf /tmp/wrk /usr/local/bin/wrk

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0-io_uring emb postgresql-raw-async-qw memory nobatch pool
