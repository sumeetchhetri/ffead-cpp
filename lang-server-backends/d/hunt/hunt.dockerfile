FROM dlangchina/dlang-ldc:latest

ADD ./ /hunt
WORKDIR /hunt

RUN apt-get update -y 
RUN apt-get install -y --no-install-recommends make
RUN apt-get install -y --no-install-recommends git
RUN apt-get install -yqq libpq-dev libsqlite3-dev default-libmysqlclient-dev zlib1g-dev
RUN rm -rf /var/lib/apt/lists/* && rm -rf /var/cache/apt/*

RUN dub upgrade --verbose
RUN dub build --build=release --arch=x86_64 --compiler=ldc2 -c=minihttp -f

CMD ["./hunt-minihttp"]

#source ~/dlang/ldc-1.23.0/activate
#source ~/dlang/dmd-2.094.0/activate