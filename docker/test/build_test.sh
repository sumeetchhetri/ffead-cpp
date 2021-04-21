rm -rf src web rtdcf script CMakeLists.txt
cp -f ../../CMakeLists.txt .
cp -rf ../../src .
cp -rf ../../web .
cp -rf ../../rtdcf .
cp -rf ../../script .
docker build -f DockerFile-UbuntuBionic-x64-ffead-cpp-fortest -t ffeadcpptest:1.0 .
rm -rf src web rtdcf script CMakeLists.txt
