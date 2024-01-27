FROM sumeetchhetri/ffead-cpp-all-base:7.0

ENV IROOT=/installs
ENV ODBCINI=/installs/odbc.ini
ENV ODBCSYSINI=/installs/

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-7.0-sql emb postgresql redis
