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
#RUN wget --no-check-certificate -q https://julialang-s3.julialang.org/bin/linux/x64/1.5/julia-1.5.2-linux-x86_64.tar.gz
#RUN tar -xzf julia-1.5.2-linux-x86_64.tar.gz
#RUN mv julia-1.5.2 /opt/
#RUN rm -f julia-1.5.2-linux-x86_64.tar.gz
#ENV PATH="/opt/julia-1.5.2/bin:${PATH}"
RUN curl -fsSL https://install.julialang.org | sh -s -- -y --default-channel release
ENV PATH="/root/.juliaup/bin:${PATH}"
RUN juliaup default release

RUN julia -e 'import Pkg; Pkg.update()' && \
    julia -e 'import Pkg; Pkg.add("HTTP")' && \
    julia -e 'import Pkg; Pkg.precompile()'

WORKDIR /

RUN rm -f /root/.julia/juliaup/julia-1.10.0+0.x64.linux.gnu/lib/julia/libcurl.*
RUN cp /usr/lib/x86_64-linux-gnu/libcurl.so* /root/.julia/juliaup/julia-1.10.0+0.x64.linux.gnu/lib/julia/
CMD ./run_ffead.sh ffead-cpp-7.0 julia-http
