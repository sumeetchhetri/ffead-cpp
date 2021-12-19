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

cd techempower-config

if [ ! -z "$2" ] || [ "$2" = "deps" ]
then
	docker rmi -f sumeetchhetri/ffead-cpp-deps:${VERSION}
	docker build --progress plain -f ffead-cpp-deps.dockerfile -t sumeetchhetri/ffead-cpp-deps:${VERSION} .
	docker push sumeetchhetri/ffead-cpp-deps:${VERSION}
fi

find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-base:6.0|-base:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|-base-2:6.0|-base-2:${VERSION}|g"
find . -type f -name '*.dockerfile' | xargs sed -i'' -e "s|version=\"6.0\"|version=\"${VERSION}\"|g"

docker rmi -f sumeetchhetri/ffead-cpp-base:${VERSION}
docker build --progress plain -f ffead-cpp-base.dockerfile -t sumeetchhetri/ffead-cpp-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-java-base:${VERSION}
docker build --progress plain -f ffead-cpp-java-base.dockerfile -t sumeetchhetri/ffead-cpp-java-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-java-base:${VERSION}

####As some images use "-march=native -mtune=native" we need to build these below images locally and push to docker

docker rmi -f sumeetchhetri/ffead-cpp-rust-base:${VERSION}
docker build --progress plain -f ffead-cpp-rust-base.dockerfile -t sumeetchhetri/ffead-cpp-rust-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-rust-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-v-base:${VERSION}
docker build --progress plain -f ffead-cpp-v-base.dockerfile -t sumeetchhetri/ffead-cpp-v-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-v-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-v-picov-raw-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-v-picov-raw-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-v-picov-raw-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-v-picov-raw-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-v-picov-raw-clibpqb-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-v-picov-raw-clibpqb-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-v-picov-raw-clibpqb-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-v-picov-raw-clibpqb-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-clibpqb-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-clibpqb-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-clibpqb-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-clibpqb-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-async-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-pool-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-async-pool-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-pool-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-pool-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-async-clibpqb-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base-2:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-async-clibpqb-profiled-base-2.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base-2:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-profiled-base-2:${VERSION}

docker rmi -f sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-pool-profiled-base:${VERSION}
docker build --progress plain -f ffead-cpp-sql-raw-async-clibpqb-pool-profiled-base.dockerfile -t sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-pool-profiled-base:${VERSION} .
docker push sumeetchhetri/ffead-cpp-sql-raw-async-clibpqb-pool-profiled-base:${VERSION}
