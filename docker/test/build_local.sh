rm -rf src web rtdcf script CMakeLists.txt
cp -f ../../CMakeLists.txt .
cp -rf ../../src .
cp -rf ../../web .
cp -rf ../../rtdcf .
cp -rf ../../script .
docker build -f DockerFile-UbuntuBionic-x64-ffead-cpp-fortest_localhost -t ffeadcpptest_localhost:1.0 --progress plain .
rm -rf src web rtdcf script CMakeLists.txt
