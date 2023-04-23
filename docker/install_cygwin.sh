cd /tmp
wget -q https://github.com/efficient/libcuckoo/archive/master.zip
unzip -qq master.zip
rm -f master.zip
cd /tmp/libcuckoo-master
cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ .
make install
cd /tmp
rm -rf libcuckoo-master

wget -q https://ftp.osuosl.org/pub/blfs/conglomeration/unixODBC/unixODBC-2.3.7.tar.gz
tar zxf unixODBC-2.3.7.tar.gz
cd unixODBC-2.3.7
./configure
make
make install
cd /tmp
rm -rf unixODBC-2.3.7

wget -q https://github.com/redis/hiredis/archive/v0.13.3.tar.gz
tar zxf v0.13.3.tar.gz
rm -f v0.13.3.tar.gz
cd hiredis-0.13.3/ && rm -f net.c && wget https://raw.githubusercontent.com/sumeetchhetri/ffead-cpp/master/docker/files/net.c && make && PREFIX=/usr make install
cd /tmp
rm -rf hiredis-0.13.3

wget -q https://github.com/sumeetchhetri/ffead-cpp/archive/master.zip
unzip -qq master.zip
mv ffead-cpp-master ffead-cpp-src
rm -f master.zip
cd /tmp/ffead-cpp-src
mkdir build
cd build
cmake -DSRV_EMB=on -DMOD_REDIS=on ..
make install -j4
mv /tmp/ffead-cpp-src/ffead-cpp-6.0-bin /tmp/
cd /tmp/ffead-cpp-6.0-bin && chmod +x *.sh
export PATH=/tmp/ffead-cpp-6.0-bin/lib:$PATH
#nohup bash -c "./server.sh > ffead.log &"
#echo "Waiting for ffead-cpp to launch on port 8080..."
#COUNTER=0
#while [ ! -f lib/cyginter.dll ]
#do
#  sleep 1
#  COUNTER=$((COUNTER+1))
#  if [ "$COUNTER" = 600 ]
#  then
#  	cat ffead.log
#  	cat logs/jobs.log
#    echo "ffead-cpp exiting due to failure...."
#    exit 1
#  fi
#done
#COUNTER=0
#while [ ! -f lib/cygdinter.dll ]
#do
#  sleep 1
#  COUNTER=$((COUNTER+1))
#  if [ "$COUNTER" = 120 ]
#  then
#  	cat ffead.log
#  	cat logs/jobs.log
#    echo "ffead-cpp exiting due to failure....dlib"
#    exit 1
#  fi
#done
#echo "ffead-cpp start successful"
#sleep 5
#cd tests && chmod +x *.sh && ./runTests.sh
#cd -
#echo "ffead-cpp normal shutdown"
#rm -f serv.ctrl
#pkill ffead-cpp
#cd /tmp/ffead-cpp-src
#chmod +x autogen.sh
#sed -i'' -e "s|m4_include|#m4_include|g" configure.ac
#sed -i'' -e "s|AX_CXX_COMPILE_STDCXX|#AX_CXX_COMPILE_STDCXX|g" configure.ac
#sed -i'' -e "s|AC_CHECK_LIB(regex|#AC_CHECK_LIB(regex|g" configure.ac
#./autogen.sh
#CXXFLAGS="-std=c++17" lt_cv_deplibs_check_method=pass_all ./configure --enable-srv_emb=yes --enable-mod_rediscache=yes
#make install -j4
#mv /tmp/ffead-cpp-src/ffead-cpp-6.0-bin /tmp/ffead-cpp-6.0-bin_ac
#cd /tmp
#rm -rf /tmp/ffead-cpp-src
