#!/bin/sh

for d in $1/web/* ; do
    rm -rf $d/src $d/CMakeLists.txt $d/Makefile $d/*.build $d/*.lua $d/lib*.so $d/lib*.dylib $d/.* 2> /dev/null
done

cp -f $2/tests/*.sh $2/tests/*.pem $2/tests/*.prop $2/tests/*.prop $2/tests/*.csv $1/tests/
cp -f $2/script/*.sh $2/script/*.pem $2/script/*.crt $2/script/*.key $1/

exit 0