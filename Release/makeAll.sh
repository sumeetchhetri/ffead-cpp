#!/bin/sh
cd ../web/default/src/Release
rm -f *.d *.o libdefault.so
make all
cp -f libdefault.so ../../../../lib/
cd ../../../flexApp/src/Release
rm -f *.d *.o libflexApp.so
make all
cp -f libflexApp.so ../../../../lib/
cd ../../../oauthApp/src/Release
rm -f *.d *.o liboauthApp.so
make all
cp -f liboauthApp.so ../../../../lib/
cd ../../../../Release
rm -f src/*.d src/*.o CHS libweblib.so
make clean
make all