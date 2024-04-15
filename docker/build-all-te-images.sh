####As some images use "-march=native -mtune=native" we need to build these below images locally and push to docker
VERSION=""

if [ ! -z "$1" ]
then
	VERSION=$1
fi

if [ -z "$VERSION" ]
then
	echo 'Version cannot be blank'
	exit 0
fi

cd all
rm -rf temp
mkdir temp
cp base/*.dockerfile scripts/*.sh *.sh *.dockerfile temp/
#cp -rf postgresql temp/
cd  temp

find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-deps:6.0|-deps:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-base:6.0|-base:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-base-2:6.0|-base-2:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|version=\"6.0\"|version=\"${VERSION}\"|g"

#DOCKER_BUILDKIT=0 
docker rmi -f sumeetchhetri/ffead-cpp-deps:${VERSION}
docker build --ulimit memlock=102400000:102400000 --progress plain -f ffead-cpp-deps.dockerfile -t sumeetchhetri/ffead-cpp-deps:${VERSION} .
docker push sumeetchhetri/ffead-cpp-deps:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-base:${VERSION}
#docker buildx create --use --name insecbuild --buildkitd-flags '--allow-insecure-entitlement security.insecure'
#docker buildx build --ulimit memlock=102400000:102400000 --allow security.insecure --progress plain -f ffead-cpp-all-base.dockerfile -t sumeetchhetri/ffead-cpp-base:${VERSION} .
docker build --ulimit memlock=102400000:102400000 --progress plain -f ffead-cpp-all-base.dockerfile -t sumeetchhetri/ffead-cpp-base:${VERSION} .
#docker buildx rm --all-inactive --force
docker push sumeetchhetri/ffead-cpp-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-v-base:${VERSION}
docker build --progress plain -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-v-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-v-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-rust-base:${VERSION}
docker build --progress plain -f ffead-cpp-rust-base.dockerfile -t sumeetchhetri/ffead-cpp-rust-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-rust-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-java-base:${VERSION}
docker build --progress plain -f ffead-cpp-java-base.dockerfile -t sumeetchhetri/ffead-cpp-java-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-java-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-seastar-base:${VERSION}
docker build --progress plain -f ffead-cpp-seastar-base.dockerfile -t sumeetchhetri/ffead-cpp-seastar-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-seastar-base:${VERSION}
