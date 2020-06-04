FROM crystallang/crystal:0.26.1

WORKDIR /crystal
COPY run.sh run.sh
COPY crystal-ffead-cpp.cr crystal-ffead-cpp.cr

ENV GC_MARKERS 1

RUN crystal build --release --no-debug crystal-ffead-cpp.cr -o crystal-ffead-cpp.out

CMD bash run.sh
