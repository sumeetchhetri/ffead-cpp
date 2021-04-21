rm -rf src web rtdcf CMakeLists.txt
cp -f ../../CMakeLists.txt .
cp -rf ../../src .
cp -rf ../../web .
cp -rf ../../rtdcf .
docker build -f DockerFile-UbuntuBionic-x64-ffead-cpp-forprofile -t ffeadcppprofile:1.0 .
rm -rf src web rtdcf CMakeLists.txt
