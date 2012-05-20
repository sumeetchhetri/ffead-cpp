#the path of the ffead-cpp server folder inside apache web folder
export FFEAD_CPP_PATH=/var/www/html/ffead-server
#the path of the ffead-cpp lib folder inside apache web folder
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/var/www/html/ffead-server/lib/
#set it in-case selinux security policy needs to be disabled 
/usr/sbin/setenforce 0
make reload