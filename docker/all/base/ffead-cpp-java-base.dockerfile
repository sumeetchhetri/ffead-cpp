FROM sumeetchhetri/ffead-cpp-base:7.0
LABEL maintainer="Sumeet Chhetri"
LABEL version="7.0"
LABEL description="Base java docker image with master code"

ENV IROOT=/installs

RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libinter.so /usr/local/lib/libinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libdinter.so /usr/local/lib/libdinter.so && \
	ldconfig
	
RUN apt update -yqq && apt install -y --no-install-recommends openjdk-11-jre maven gradle && rm -rf /var/lib/apt/lists/*
RUN cd ${IROOT}/lang-server-backends/java/firenio && mvn compile assembly:single -q && cp target/firenio-ffead-cpp-0.1-jar-with-dependencies.jar $IROOT/
RUN cd ${IROOT}/lang-server-backends/java/rapidoid && mvn compile assembly:single -q && cp target/rapidoid-ffead-cpp-1.0-jar-with-dependencies.jar $IROOT/
RUN cd ${IROOT}/lang-server-backends/java/wizzardo-http && gradle --refresh-dependencies clean fatJar -q && cp build/libs/wizzardo-ffead-cpp-all-1.0.jar $IROOT/
RUN rm -rf ${IROOT}/lang-server-backends
