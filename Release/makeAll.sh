#!/bin/sh
rm -f src/*.d src/*.o CHS libweblib.so
make clean
make all
cd ../web/default/src/Release
rm -f *.d *.o libdefault.so
make all
cp -f libdefault.so ../../../../Release/ffead-server/lib
cd ../../../flexApp/src/Release
rm -f *.d *.o libflexApp.so
make all
cp -f libflexApp.so ../../../../Release/ffead-server/lib
cd ../../../oauthApp/src/Release
rm -f *.d *.o liboauthApp.so
make all
cp -f liboauthApp.so ../../../../Release/ffead-server/lib