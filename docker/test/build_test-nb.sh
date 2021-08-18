rm -rf src web rtdcf script CMakeLists.txt
cp -f ../../CMakeLists.txt .
cp -rf ../../src .
cp -rf ../../web .
cp -rf ../../rtdcf .
cp -rf ../../script .
cp -rf ../../lang-server-backends/v/pico.v .
docker build -f DockerFile-UbuntuBionic-x64-ffead-cpp-fortest-nb -t ffeadcpptestnb:1.0 --progress plain .
rm -rf src web rtdcf script CMakeLists.txt
