FROM archlinux/base

RUN pacman -Sy --noconfirm autoconf-archive make clang unixodbc curl hiredis libmemcached mongo-c-driver cmake openssh libutil-linux wget gnu-netcat tar gzip zlib unzip gawk procps-ng

#Install libcuckoo headers
WORKDIR /tmp
RUN wget -q https://github.com/efficient/libcuckoo/archive/master.zip
RUN unzip master.zip
RUN rm -f master.zip
WORKDIR /tmp/libcuckoo-master
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ .
RUN make install
WORKDIR /tmp
RUN rm -rf /tmp/libcuckoo-master

#Install ffead-cpp
WORKDIR /tmp
RUN wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
RUN unzip master.zip
RUN rm -f master.zip
RUN mv ffead-cpp-master ffead-cpp-src
WORKDIR /tmp/ffead-cpp-src
RUN cmake .
RUN make install -j4
RUN mv /tmp/ffead-cpp-src/ffead-cpp-3.0-bin /tmp/
WORKDIR /tmp
RUN rm -rf /tmp/ffead-cpp-src

WORKDIR /opt

COPY *.sh  ./

RUN chmod +x install_ffead-cpp-3.0.sh
RUN ./install_ffead-cpp-3.0.sh

WORKDIR /opt/ffead-cpp-3.0

RUN chmod +x server.sh
CMD ./server.sh
