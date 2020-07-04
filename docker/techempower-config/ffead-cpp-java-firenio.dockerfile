FROM sumeetchhetri/ffead-cpp-v4.0-base:1.0

ENV IROOT=/installs

RUN /install_ffead-cpp-framework.sh && /install_ffead-cpp-httpd.sh && /install_ffead-cpp-nginx.sh && \
	cd ${IROOT}/ffead-cpp-src && make clean && rm -rf CMakeFiles

RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-4.0/lib/libte_benchmark_um.so /usr/local/lib/libte_benchmark_um.so && \
	ln -s ${IROOT}/ffead-cpp-4.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-4.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ln -s ${IROOT}/ffead-cpp-4.0/lib/libinter.so /usr/local/lib/libinter.so && \
	ln -s ${IROOT}/ffead-cpp-4.0/lib/libdinter.so /usr/local/lib/libdinter.so && \
	ldconfig

RUN apt update -yqq && apt install -y --no-install-recommends default-jre maven && rm -rf /var/lib/apt/lists/*
WORKDIR ${IROOT}/lang-server-backends/java/firenio
RUN mvn compile assembly:single -q && cp target/firenio-ffead-cpp-0.1-jar-with-dependencies.jar $IROOT/ && rm -rf ${IROOT}/lang-server-backends

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-4.0 java-firenio
