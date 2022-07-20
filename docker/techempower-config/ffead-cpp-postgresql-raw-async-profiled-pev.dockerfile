FROM sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:6.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-6.0-picoev emb postgresql-raw-async memory
