/*
	Copyright 2010, Sumeet Chhetri

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
#include "Task.h"
#include "SSLHandler.h"
#include <boost/algorithm/string.hpp>
#include "ConfigurationHandler.h"
#include "FormController.h"
#include "RestController.h"
#include "SecurityHandler.h"
#include "FilterHandler.h"
#include "AuthHandler.h"
#include "ControllerHandler.h"
#include "FormHandler.h"
#include "SoapHandler.h"
#include "ScriptHandler.h"
#include "FviewHandler.h"
#include "ExtHandler.h"
#include "Logger.h"
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
	SoapHandler soapHandler;
	ScriptHandler scriptHandler;
	FviewHandler fviewHandler;
	ExtHandler extHandler;
	int fd;
	string serverRootDirectory;
	map<string,string> *params;
	bool isSSLEnabled;
	SSL_CTX *ctx;
	SSLHandler sslHandler;
	ConfigurationData configData;
	void* dlib;
	void writeToSharedMemeory(string sessionId, string value,bool napp);
	map<string,string> readFromSharedMemeory(string sessionId);
	void createResponse(HttpResponse &res,bool flag,map<string,string> vals,string prevcookid, long sessionTimeout, bool sessatserv);
	string getContentStr(string url,string locale,string ext);
public:
	ServiceTask(int fd,string serverRootDirectory,map<string,string> *params,
			bool isSSLEnabled, SSL_CTX *ctx, SSLHandler sslHandler, ConfigurationData configData, void* dlib);
	virtual ~ServiceTask();
	void run();
	HttpResponse apacheRun(HttpRequest* req);
	static string getFileExtension(string file);
};

#endif /* SERVICETASK_H_ */
