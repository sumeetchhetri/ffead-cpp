FROM ubuntu:20.04

WORKDIR /h2o_app_src
COPY ./ ./

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get install -yqq autoconf bison cmake curl file flex g++ git libssl-dev libtool libz-dev make wget

### Install h2o

ARG H2O_VERSION=v2.2.6

ARG H2O_BUILD_DIR=h2o-build
ENV H2O_PREFIX /opt/h2o

RUN mkdir -p "${H2O_BUILD_DIR}/build" && \
    cd "$H2O_BUILD_DIR" && \
    wget -qO - "https://github.com/h2o/h2o/archive/${H2O_VERSION}.tar.gz" | \
    tar xz --strip-components=1 && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX="$H2O_PREFIX" -DCMAKE_C_FLAGS="-flto -march=native" \
          -DCMAKE_AR=/usr/bin/gcc-ar -DCMAKE_RANLIB=/usr/bin/gcc-ranlib .. && \
    make -j "$(nproc)" install && \
    cd ../.. && \
    rm -rf "$H2O_BUILD_DIR"

CMD ["./h2o.sh"]
