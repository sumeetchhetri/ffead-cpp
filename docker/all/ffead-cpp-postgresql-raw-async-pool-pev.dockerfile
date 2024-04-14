FROM sumeetchhetri/ffead-cpp-base:7.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0-picoev emb postgresql-raw-async memory nobatch pool
