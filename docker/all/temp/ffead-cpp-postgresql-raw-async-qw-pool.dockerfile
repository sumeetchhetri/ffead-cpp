FROM sumeetchhetri/ffead-cpp-all-base:7.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0-sql emb postgresql-raw-async-qw memory nobatch pool
