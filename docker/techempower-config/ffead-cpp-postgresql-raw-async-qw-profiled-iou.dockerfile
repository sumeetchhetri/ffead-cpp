FROM sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:6.1

RUN apt remove -yqq postgresql-13 postgresql-contrib-13 gnupg lsb-release && apt autoremove -yqq
RUN rm -rf /ssd/postgresql && rm -rf /tmp/postgresql && rm -rf /tmp/wrk /usr/local/bin/wrk

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-6.0-io_uring emb postgresql-raw-async-qw memory
