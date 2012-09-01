#the path of the ffead-cpp server folder inside apache web folder
export FFEAD_CPP_PATH=/home/user/ffead-server
#the path of the ffead-cpp lib folder inside apache web folder
export PATH=${PATH}:/usr/sbin/:/lib/:${FFEAD_CPP_PATH}/lib/
#set it in-case selinux security policy needs to be disabled 
make install