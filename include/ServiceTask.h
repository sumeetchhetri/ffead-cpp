/*
	Copyright 2009-2012, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
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
#include "Task.h"
#include "SSLHandler.h"
#include "StringUtil.h"
#include "ConfigurationHandler.h"
#include "FormController.h"
#include "RestController.h"
#include "SecurityHandler.h"
#include "FilterHandler.h"
#include "AuthHandler.h"
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

#define MAXBUFLENM 32768
#define BUFSIZZ 1024

class ServiceTask : public Task
{
	Logger logger;
	SecurityHandler securityHandler;
	FilterHandler filterHandler;
	AuthHandler authHandler;
	ControllerHandler controllerHandler;
	FormHandler formHandler;
#ifdef INC_WEBSVC
	SoapHandler soapHandler;
#endif
#ifdef INC_SCRH
	ScriptHandler scriptHandler;
#endif
	FviewHandler fviewHandler;
	ExtHandler extHandler;
	int fd;
	string serverRootDirectory;
	map<string,string> *params;
	bool isSSLEnabled;
	SSL_CTX *ctx;
	void saveSessionDataToFile(string sessionId, string value);
	map<string,string> getSessionDataFromFile(string sessionId);
	void saveSessionDataToDistocache(string sessionId, map<string,string> sessAttrs);
	map<string,string> getSessionDataFromDistocache(string sessionId);
	void storeSessionAttributes(HttpResponse &res,HttpRequest* req, long sessionTimeout, bool sessatserv);
	void updateContent(HttpRequest* req, HttpResponse *res, string ext, int);
	unsigned int getFileSize(const char *fileName);
	string getFileContents(const char *fileName, int start = -1, int end = -1);
	bool checkSocketWaitForTimeout(int sock_fd, int writing, int seconds, int micros = 0);
	bool sendData(SSL* ssl, BIO* io, int fd, string h1);
	void closeSocket(SSL* ssl, BIO* io, int fd);
	bool readLine(SSL* ssl, BIO* io, int fd, string& line);
	bool readData(SSL* ssl, BIO* io, int fd, int cntlen, string& content);
public:
	ServiceTask(int fd,string serverRootDirectory);
	virtual ~ServiceTask();
	void run();
	HttpResponse apacheRun(HttpRequest* req);
	static string getFileExtension(string file);
};

#endif /* SERVICETASK_H_ */
