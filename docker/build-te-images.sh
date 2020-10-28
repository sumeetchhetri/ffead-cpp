docker rmi -f sumeetchhetri/ffead-cpp-5.0-base:5.2
docker build -f ffead-cpp-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-base:5.2 .
docker push sumeetchhetri/ffead-cpp-5.0-base:5.2

docker rmi -f sumeetchhetri/ffead-cpp-5.0-rust-base:5.2
docker build -f ffead-cpp-rust-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-rust-base:5.2 .
docker push sumeetchhetri/ffead-cpp-5.0-rust-base:5.2

docker rmi -f sumeetchhetri/ffead-cpp-5.0-v-base:5.2
docker build -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-v-base:5.2 .
docker push sumeetchhetri/ffead-cpp-5.0-v-base:5.2

docker rmi -f sumeetchhetri/ffead-cpp-5.0-java-base:5.2
docker build -f ffead-cpp-java-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-java-base:5.2 .
docker push sumeetchhetri/ffead-cpp-5.0-java-base:5.2

docker rmi -f sumeetchhetri/ffead-cpp-5.0-rust-rocket-base:5.2
docker build -f ffead-cpp-rust-rocket-base.dockerfile -t sumeetchhetri/ffead-cpp-5.0-rust-rocket-base:5.2 .
docker push sumeetchhetri/ffead-cpp-5.0-rust-rocket-base:5.2
