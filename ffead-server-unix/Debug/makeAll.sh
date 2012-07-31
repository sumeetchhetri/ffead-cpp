#!/bin/sh
cd ../web/default/src/Debug
rm -f *.d *.o libdefault.so
make all
cp -f libdefault.so ../../../../lib/
cd ../../../flexApp/src/Debug
rm -f *.d *.o libflexApp.so
make all
cp -f libflexApp.so ../../../../lib/
cd ../../../oauthApp/src/Debug
rm -f *.d *.o liboauthApp.so
make all
cp -f liboauthApp.so ../../../../lib/
cd ../../../../Debug
rm -f src/*.d src/*.o CHS libweblib.so
make clean
make all