#!/bin/sh
rm -f src/*.d src/*.o CHS libweblib.so
gmake clean
gmake all
cd ../web/default/src/Release
rm -f *.d *.o libdefault.so
gmake all
cp -f libdefault.so ../../../../Release/ffead-server/lib
cd ../../../flexApp/src/Release
rm -f *.d *.o libflexApp.so
gmake all
cp -f libflexApp.so ../../../../Release/ffead-server/lib
cd ../../../oauthApp/src/Release
rm -f *.d *.o liboauthApp.so
gmake all
cp -f liboauthApp.so ../../../../Release/ffead-server/lib