FROM sumeetchhetri/ffead-cpp-4.0-base:2.0

ENV IROOT=/installs

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-4.0-sql emb mysql
