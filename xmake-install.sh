#!/bin/sh

#xmake f --cxflags="-I/usr/local/include -w" --MOD_MEMCACHED=true --MOD_REDIS=true --MOD_SDORM_MONGO=true -v -D -c -m debug (debug mode)
#xmake f --cxflags="-I/usr/local/include -w" --MOD_MEMCACHED=true --MOD_REDIS=true --MOD_SDORM_MONGO=true -v -D -c (release mode)
#xmake
#xmake install

cp -rf resources public rtdcf web $1/
cp $1/bin/ffead-cpp $1/
mkdir $1/tests $1/logs
cp -f $2/tests/*.sh $2/tests/*.pem $2/tests/*.prop $2/tests/*.prop $2/tests/*.csv $1/bin/tests $1/tests/
cp -f $2/script/*.sh $2/script/*.pem $2/script/*.crt $2/script/*.key $1/
rm -rf $1/bin

if [ "$3" != "true" ]; then
	rm -rf $1/web/t2 || true
	rm -f $1/lib/libt2.* || true
fi
if [ "$4" != "true" ]; then
	rm -rf $1/web/t3 || true
	rm -rf $1/web/t4 || true
	rm -rf $1/web/t5 || true
	rm -rf $1/web/t6 || true
	rm -rf $1/web/t7 || true
	rm -f $1/lib/libt3.* || true
	rm -f $1/lib/libt4.* || true
	rm -f $1/lib/libt5.* || true
	rm -f $1/lib/libt6.* || true
	rm -f $1/lib/libt7.* || true
fi

for d in $1/web/* ; do
    rm -rf $d/src $d/CMakeLists.txt $d/Makefile $d/*.build $d/*.lua $d/lib*.so $d/lib*.dylib $d/.* 2> /dev/null
done

exit 0
