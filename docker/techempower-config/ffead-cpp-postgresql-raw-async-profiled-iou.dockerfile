FROM sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:6.1

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-6.0-io_uring emb postgresql-raw-async memory
