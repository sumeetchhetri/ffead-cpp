USER_HOME=$(eval echo ~${SUDO_USER})
echo User Home Directory is : ${USER_HOME}
FFEAD_CPPPTH=${USER_HOME}/ffead-server
HTTPD_LIB=
if [ "$#" -gt 0 -a -n "$1" ]
then
	FFEAD_CPPPTH=$1
fi
#the path of the ffead-cpp server folder inside apache web folder
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
#the path of the ffead-cpp lib folder inside apache web folder
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/var/www/html/ffead-server/lib/
#set it in-case selinux security policy needs to be disabled 
#/usr/sbin/setenforce 0
make all