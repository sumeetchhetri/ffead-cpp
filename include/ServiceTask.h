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
#define MAXBUFLENM 32768
#define BUFSIZZ 1024

class ServiceTask : public Task
{
private:
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
	void writeToSharedMemeory(string sessionId, string value,bool napp);
	map<string,string> readFromSharedMemeory(string sessionId);
	void createResponse(HttpResponse &res,bool flag,map<string,string> vals,string prevcookid, long sessionTimeout, bool sessatserv);
	string getContentStr(string url,string locale,string ext);
public:
	ServiceTask(int fd,string serverRootDirectory,map<string,string> *params);
	virtual ~ServiceTask();
	void run(bool isSSLEnabled, SSL_CTX *ctx, SSLHandler sslHandler, ConfigurationData configData, void* dlib);
	static string getFileExtension(const string& file);
};

#endif /* SERVICETASK_H_ */
