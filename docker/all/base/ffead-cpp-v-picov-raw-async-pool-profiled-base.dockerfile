FROM sumeetchhetri/ffead-cpp-sql-raw-async-pool-profiled-base:7.0
LABEL maintainer="Sumeet Chhetri"
LABEL version="7.0"
LABEL description="vpicov SQL Raw Async Pool Profiled Base ffead-cpp docker image with commit id - master"

WORKDIR ${IROOT}

RUN apt update -yqq && apt install -y git make && rm -rf /var/lib/apt/lists/*
#For Latest vlang, uncomment the below mentioned line, due to lot of new enhancements and unsafe block handling, vlang has slowed down tremendously
#RUN git clone https://github.com/vlang/v && cd v && make && ./v symlink

#For the fastest vlang performance, use 0.1.29, where the unsafe changes were only restricted to pointer arithmetic
RUN wget -q https://github.com/vlang/v/releases/download/0.1.29/v_linux.zip && unzip -q v_linux.zip && cp ${IROOT}/lang-server-backends/v/pico.v/picoev.v v/vlib/picoev/picoev.v && cd v && chmod +x v && ./v symlink && cd .. && rm -f v_linux.zip

RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0-sql/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-7.0-sql/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ldconfig

COPY sql-v-picov-profiled-install.sh install_ffead-cpp-sql-raw-v-picov-profiled.sh ${IROOT}/
RUN chmod 755 ${IROOT}/sql-v-picov-profiled-install.sh ${IROOT}/install_ffead-cpp-sql-raw-v-picov-profiled.sh
RUN ./sql-v-picov-profiled-install.sh async-pool
