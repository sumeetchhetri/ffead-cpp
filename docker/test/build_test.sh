rm -rf src web
cp -f ../../CMakeLists.txt .
cp -rf ../../src .
cp -rf ../../web .
docker build -f DockerFile-UbuntuBionic-x64-ffead-cpp-5.0-fortest -t ffeadcpptest:1.0 .
rm -rf src web