FROM sumeetchhetri/ffead-cpp-all-base:7.0

ENV IROOT=/installs

ENV DEBIAN_FRONTEND noninteractive
RUN rm -f /usr/local/lib/libffead-* /usr/local/lib/libte_benc* /usr/local/lib/libinter.so /usr/local/lib/libdinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-modules.so /usr/local/lib/libffead-modules.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libffead-framework.so /usr/local/lib/libffead-framework.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libinter.so /usr/local/lib/libinter.so && \
	ln -s ${IROOT}/ffead-cpp-7.0/lib/libdinter.so /usr/local/lib/libdinter.so && \
	ldconfig

WORKDIR ${IROOT}
RUN curl -fsSL https://install.julialang.org | sh -s -- -y --default-channel release
ENV PATH="/root/.juliaup/bin:${PATH}"
RUN juliaup default release

RUN julia -e 'import Pkg; Pkg.update()' && \
    julia -e 'import Pkg; Pkg.add("HTTP")' && \
    julia -e 'import Pkg; Pkg.precompile()'

COPY julia_curl_fix.sh /

WORKDIR /

RUN rm -f /root/.julia/juliaup/julia-1.10.0+0.x64.linux.gnu/lib/julia/libcurl.*
RUN chmod +x julia_curl_fix.sh && ./julia_curl_fix.sh
CMD ./run_ffead.sh ffead-cpp-7.0 julia-http
