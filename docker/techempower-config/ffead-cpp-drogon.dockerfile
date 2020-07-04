FROM sumeetchhetri/ffead-cpp-v4.0-base:1.0

ENV IROOT=/installs

RUN /install_ffead-cpp-framework.sh && /install_ffead-cpp-httpd.sh && /install_ffead-cpp-nginx.sh && \
	cd ${IROOT}/ffead-cpp-src && make clean && rm -rf CMakeFiles

WORKDIR /

CMD ./run_ffead.sh ffead-cpp-4.0 drogon
