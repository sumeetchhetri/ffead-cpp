FROM sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base-2:6.1

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-6.0-sql emb postgresql-raw-async memory batch
