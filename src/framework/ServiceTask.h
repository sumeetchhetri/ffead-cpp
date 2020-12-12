/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * ServiceTask.h
 *
 *  Created on: 20-Jun-2012
 *      Author: sumeetc
 */

#ifndef SERVICETASK_H_
#define SERVICETASK_H_
#include "AppDefines.h"
#include <sys/stat.h>
#include <math.h>
#include "DateFormat.h"
#include "HttpServiceHandler.h"
#ifdef HAVE_SSLINC
#include "SSLHandler.h"
#endif
#include "StringUtil.h"
#include "ConfigurationHandler.h"
#include "FormController.h"
#include "SecurityHandler.h"
#include "FilterHandler.h"
#include "ControllerHandler.h"
#include "FormHandler.h"
#include "Thread.h"
#ifdef INC_WEBSVC
#include "SoapHandler.h"
#endif
#include "ScriptHandler.h"
#include "FviewHandler.h"
#include "ExtHandler.h"
#include "CORSHandler.h"
#include "LoggerFactory.h"
#include "Timer.h"
#include <fcntl.h>
#ifdef INC_DSTC
#include "DistGlobalCache.h"
#endif
#include <stdio.h>
#include "Http11WebSocketDataFrame.h"
#include "CommonUtils.h"
#include "SocketInterface.h"
#include "Router.h"
#include "WebSockHandler.h"
#include "string_view"

class ServiceTask : public HttpServiceTask
{
	Logger logger;
	void saveSessionDataToFile(const std::string& sessionId, const std::string& value);
	std::map<std::string,std::string> getSessionDataFromFile(const std::string& sessionId);
	void saveSessionDataToDistocache(const std::string& sessionId, std::map<std::string,std::string>& sessAttrs);
	std::map<std::string,std::string> getSessionDataFromDistocache(const std::string& sessionId);
	void storeSessionAttributes(HttpResponse* res, HttpRequest* req, const long& sessionTimeout, const bool& sessatserv);
	void updateContent(HttpRequest* req, HttpResponse *res, const std::string& ext, const int&);
	unsigned int getFileSize(const char *fileName);
	std::string getFileContents(const char *fileName, const int& start= -1, const int& end= -1);
	bool checkSocketWaitForTimeout(const int& sock_fd, const int& writing, const int& seconds, const int& micros= 0);
	void handleWebSocket(HttpRequest* req, void* dlib, void* ddlib, SocketInterface* sockUtil);
public:
	ServiceTask();
	virtual ~ServiceTask();
	std::string getCntEncoding();
	WebSockHandler* handleWebsockOpen(WebSocketData* request, WebSocketRespponseData* response, SocketInterface* sif, HttpRequest* hreq);
	void handle(HttpRequest* req, HttpResponse* res);
	bool handle(HttpRequest* req, HttpResponse* res, SocketInterface* sif);
};

#endif /* SERVICETASK_H_ */
