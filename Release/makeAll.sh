#!/bin/sh
rm -f src/*.d src/*.o CHS libweblib.a
make clean
make all
cd ../web/default/src/Release
rm -f *.d *.o libdefault.a
make all
cp -f libdefault.a ../../../../Release/ffead-server/lib
cd ../../../flexApp/src/Release
rm -f *.d *.o libflexApp.a
make all
cp -f libflexApp.a ../../../../Release/ffead-server/lib
cd ../../../oauthApp/src/Release
rm -f *.d *.o liboauthApp.a
make all
cp -f liboauthApp.a ../../../../Release/ffead-server/lib