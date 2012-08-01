#!/bin/sh
rm -f src/*.d src/*.o CHS libweblib.so
gmake clean
gmake all
cd ../web/default/src/Debug
rm -f *.d *.o libdefault.so
gmake all
cp -f libdefault.so ../../../../Debug/ffead-server/lib
cd ../../../flexApp/src/Debug
rm -f *.d *.o libflexApp.so
gmake all
cp -f libflexApp.so ../../../../Debug/ffead-server/lib
cd ../../../oauthApp/src/Debug
rm -f *.d *.o liboauthApp.so
gmake all
cp -f liboauthApp.so ../../../../Debug/ffead-server/lib