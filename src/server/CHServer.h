/*
	Copyright 2009-2012, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 
  
        http://www.apache.org/licenses/LICENSE-2.0 
  
    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
*/
/*
 * CHServer.h
 *
 *  Created on: Aug 11, 2009
 *      Author: sumeet
 */

#ifndef CHSERVER_H_
#define CHSERVER_H_
#include "AppDefines.h"
#include "Compatibility.h"
#if !defined(OS_MINGW)
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#endif
#include <algorithm>
#include "Client.h"
#include "PropFileReader.h"
#include "AfcUtil.h"
#include "string"
#include "Controller.h"
#include "Server.h"
#include "PropFileReader.h"
#include "TemplateEngine.h"
#ifdef INC_DCP
#include "DCPGenerator.h"
#endif
#include "Reflection.h"
#include <cstdlib>
#include <dlfcn.h>

#include "WsUtil.h"
#include "sstream"

#include "ClassInfo.h"

#include "View.h"
#include "XmlParser.h"
#include "TemplateHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#ifdef INC_DVIEW
#include "DynamicView.h"
#endif
#include "HttpRequest.h"
#ifdef INC_APPFLOW
#include "ApplicationUtil.h"
#endif
#include <fcntl.h>
#include <sys/time.h>
#include <queue>


#ifdef INC_SDORM
#include "DataSourceManager.h"
#endif
#ifdef INC_COMP
#include "ComponentGen.h"
#include "ComponentHandler.h"
#endif
#ifdef INC_MSGH
#include "MessageHandler.h"
#endif
#ifdef INC_MI
#include "MethodInvoc.h"
#endif
#include "Reflector.h"
#ifdef INC_COMP
#include "AppContext.h"
#endif
#include "LoggerFactory.h"
#include "ThreadPool.h"
#include "FileAuthController.h"
#include <sys/stat.h>
#include "Timer.h"
#include "DateFormat.h"
#include "Filter.h"
#include "FormController.h"
#include "SecurityHandler.h"
#include "FilterHandler.h"
#include "ControllerHandler.h"
#include "FormHandler.h"
#include "SoapHandler.h"
#include "ScriptHandler.h"
#include "FviewHandler.h"
#include "ExtHandler.h"
#include "ServiceTask.h"
#include "ConfigurationHandler.h"
#include "SSLHandler.h"
#include "SSLClient.h"
#include "Client.h"
#include "ServiceTask.h"
#include "Thread.h"
#include "FFEADContext.h"
#include "SelEpolKqEvPrt.h"
#include <stdint.h>
#ifdef INC_DSTC
#include "DistoCacheHandler.h"
#endif
#ifdef WINDOWS
    #include <direct.h>
    #define pwd _getcwd
#else
    #include <unistd.h>
    #define pwd getcwd
 #endif
#if defined(OS_LINUX) || defined(OS_SOLARIS)
#include <execinfo.h>
#endif
#include "RequestReaderHandler.h"
#include "Http2Handler.h"
#include "Http11Handler.h"
#include "HttpServiceHandler.h"

//#define CA_LIST "root.pem"
#define HOST1	"localhost"
//#define RANDOM1  "random.pem"
#define PORT1	4433
#define BUFSIZZ 1024
//#define KEYFILE "server.pem"
//#define PASSWORD "password"
//#define DHFILE "dh1024.pem"

#define MAXEPOLLSIZE 100
#define BACKLOGM 500

typedef bool (*FunPtr1) (void *);
typedef ClassInfo (*FunPtr) ();
typedef void* (*toVoidP) (const std::string&);
typedef std::string (*DCPPtr) ();
typedef void (*ReceiveTask1)(const int&);

#if defined(OS_MINGW)
#define WNOHANG 1
static inline int waitpid(const pid_t& pid, int *status, const unsigned& options)
{
	if (const options& == 0)
		return _cwait(const status&, const pid&, const 0&);
	errno = EINVAL;
	return -1;
}
#endif



class CHServer {
	static int techunkSiz, connKeepAlive, maxReqHdrCnt, maxEntitySize;
	static Logger logger;
	static HttpServiceTask* httpServiceFactoryMethod();
public:
	static SocketInterface* createSocketInterface(SOCKET);
	static Logger& getLogger();
	static void* dynamic_page_monitor(void* arg);
	static void serve(std::string port, std::string ipaddr, int thrdpsiz, int wthrdpsiz, std::string serverRootDirectory, propMap sprops, int vhostNumber);
	static int entryPoint(int vhostNum, bool isMain, std::string serverRootDirectory, std::string port, std::string ipaddr, std::vector<std::string> servedAppNames);
};

#endif /* CHSERVER_H_ */
