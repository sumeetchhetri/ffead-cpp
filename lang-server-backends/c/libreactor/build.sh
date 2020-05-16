apt-get install -y build-essential libjansson-dev wget
wget https://github.com/fredrikwidlund/libdynamic/releases/download/v1.3.0/libdynamic-1.3.0.tar.gz
tar fvxz libdynamic-1.3.0.tar.gz
cd libdynamic-1.3.0
./configure --prefix=/usr AR=gcc-ar NM=gcc-nm RANLIB=gcc-ranlib
make install
cd ..
wget https://github.com/fredrikwidlund/libreactor/releases/download/v1.0.1/libreactor-1.0.1.tar.gz
tar fvxz libreactor-1.0.1.tar.gz
cd libreactor-1.0.1
./configure --prefix=/usr AR=gcc-ar NM=gcc-nm RANLIB=gcc-ranlib
make install
cd ..
rm -rf libdynamic-1.3.0* libreactor-1.0.1*
make