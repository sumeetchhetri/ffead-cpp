docker rmi sumeetchhetri/ffead-cpp-5.0-base:2.0
docker build -f ffead-cpp-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-base:2.0 .
docker push sumeetchhetri/ffead-cpp-5.0-base:2.0

docker rmi sumeetchhetri/ffead-cpp-5.0-rust-base:2.0
docker build -f ffead-cpp-rust-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-rust-base:2.0 .
docker push sumeetchhetri/ffead-cpp-5.0-rust-base:2.0

docker rmi sumeetchhetri/ffead-cpp-5.0-v-base:2.0
docker build -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-v-base:2.0 .
docker push sumeetchhetri/ffead-cpp-5.0-v-base:2.0

docker rmi sumeetchhetri/ffead-cpp-5.0-java-base:2.0
docker build -f ffead-cpp-java-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-java-base:2.0 .
docker push sumeetchhetri/ffead-cpp-5.0-java-base:2.0

docker rmi sumeetchhetri/ffead-cpp-5.0-rust-rocket-base:2.0
docker build -f ffead-cpp-rust-rocket-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-rust-rocket-base:2.0 .
docker push sumeetchhetri/ffead-cpp-5.0-rust-rocket-base:2.0

