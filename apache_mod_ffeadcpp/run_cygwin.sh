USER_HOME=/home/`env | grep -e USER=|cut -d= -f2`
echo User Home Directory is : ${USER_HOME}
FFEAD_CPPPTH=${USER_HOME}/ffead-server
HTTPD_LIB=-lhttpd2core
if [ "$#" -gt 0 -a -n "$1" ]
then
	FFEAD_CPPPTH=$1
fi
#the path of the ffead-cpp server folder inside apache web folder
export FFEAD_CPP_PATH=${FFEAD_CPPPTH}
echo ffead-server Directory is : ${FFEAD_CPP_PATH}
#the path of the ffead-cpp lib folder inside apache web folder
export PATH=${PATH}:/usr/sbin/:/lib/:${FFEAD_CPP_PATH}/lib/
make all