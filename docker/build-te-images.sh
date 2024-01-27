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

cd te
rm -rf tekp
mkdir tekp
cp base/*.dockerfile scripts/*.sh *.sh *.dockerfile tekp/
#cp -rf postgresql tekp/
cd  tekp

find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-deps:6.0|-deps:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-base:6.0|-base:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-base-2:6.0|-base-2:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|version=\"6.0\"|version=\"${VERSION}\"|g"

docker rmi -f sumeetchhetri/ffead-cpp-base:${VERSION}
#DOCKER_BUILDKIT=0 
docker build --ulimit memlock=102400000:102400000 --progress plain -f ffead-cpp-base.dockerfile -t sumeetchhetri/ffead-cpp-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-v-base:${VERSION}
docker build --progress plain -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-v-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-v-base:${VERSION}

#docker rmi -f sumeetchhetri/ffead-cpp-base:${VERSION}-debug
#docker build --ulimit memlock=102400000:102400000 --progress plain -f ffead-cpp-base-debug.dockerfile -t sumeetchhetri/ffead-cpp-base:${VERSION}-debug .
#docker push sumeetchhetri/ffead-cpp-base:${VERSION}-debug

#docker rmi -f sumeetchhetri/ffead-cpp-v-base:${VERSION}-debug
#docker build --progress plain -f ffead-cpp-v-base-debug.dockerfile -t sumeetchhetri/ffead-cpp-v-base:${VERSION}-debug .
#docker push sumeetchhetri/ffead-cpp-v-base:${VERSION}-debug
