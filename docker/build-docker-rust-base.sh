docker rmi -f sumeetchhetri/ffead-cpp-v4.0-rust-base:1.0
docker build -f ffead-cpp-rust-base.dockerfile -t sumeetchhetri/ffead-cpp-v4.0-rust-base:1.0 .
docker push sumeetchhetri/ffead-cpp-v4.0-rust-base:1.0

docker rmi -f sumeetchhetri/ffead-cpp-v4.0-v-base:1.0
docker build -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-v4.0-v-base:1.0 .
docker push sumeetchhetri/ffead-cpp-v4.0-v-base:1.0