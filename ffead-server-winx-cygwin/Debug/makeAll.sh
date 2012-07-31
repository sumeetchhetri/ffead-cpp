#!/bin/sh
rm -f src/*.d src/*.o CHS libweblib.a
make clean
make all
cd ../web/default/src/Debug
rm -f *.d *.o libdefault.a
make all
cp -f libdefault.a ../../../../Debug/ffead-server/lib
cd ../../../flexApp/src/Debug
rm -f *.d *.o libflexApp.a
make all
cp -f libflexApp.a ../../../../Debug/ffead-server/lib
cd ../../../oauthApp/src/Debug
rm -f *.d *.o liboauthApp.a
make all
cp -f liboauthApp.a ../../../../Debug/ffead-server/lib