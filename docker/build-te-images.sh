docker rmi -f sumeetchhetri/ffead-cpp-deps:6.0
docker build --progress plain -f ffead-cpp-deps.dockerfile -t sumeetchhetri/ffead-cpp-deps:6.0 .
docker push sumeetchhetri/ffead-cpp-deps:6.0

docker rmi -f sumeetchhetri/ffead-cpp-base:6.0
docker build --progress plain -f ffead-cpp-base.dockerfile -t sumeetchhetri/ffead-cpp-base:6.0 .
docker push sumeetchhetri/ffead-cpp-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-java-base:6.0
docker build --progress plain -f ffead-cpp-java-base.dockerfile -t sumeetchhetri/ffead-cpp-java-base:6.0 .
docker push sumeetchhetri/ffead-cpp-java-base:6.0

####As some images use -march=native -mtune=native we need to build these below images locally and push to docker

docker rmi -f sumeetchhetri/ffead-cpp-rust-base:6.0
docker build --progress plain -f ffead-cpp-rust-base.dockerfile -t sumeetchhetri/ffead-cpp-rust-base:6.0 .
docker push sumeetchhetri/ffead-cpp-rust-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-v-base:6.0
docker build --progress plain -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-v-base:6.0 .
docker push sumeetchhetri/ffead-cpp-v-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-rust-rocket-base:6.0
docker build --progress plain -f ffead-cpp-rust-rocket-base.dockerfile -t sumeetchhetri/ffead-cpp-rust-rocket-base:6.0 .
docker push sumeetchhetri/ffead-cpp-rust-rocket-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-profiled-base:6.0
docker build --progress plain -f ffead-cpp-sql-raw-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-profiled-base:6.0 .
docker push sumeetchhetri/ffead-cpp-sql-raw-profiled-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-clibpqb-profiled-base:6.0
docker build --progress plain -f ffead-cpp-sql-raw-clibpqb-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-clibpqb-profiled-base:6.0 .
docker push sumeetchhetri/ffead-cpp-sql-raw-clibpqb-profiled-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:6.0
docker build --progress plain -f ffead-cpp-sql-raw-async-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:6.0 .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:6.0

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base:6.0
docker build --progress plain -f ffead-cpp-sql-raw-async-clibpqb-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base:6.0 .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base:6.0
