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
 * Component.h
 *
 *  Created on: Jan 29, 2010
 *      Author: sumeet
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_
#include "string"
#include "Message.h"
#include "ConnectionSettings.h"
#include "Cibernate.h"
using namespace std;
class ComponentService {
private:
	string classn;
	string methName;
	string retType;
	string address;
	vector<string> allwugList;
	vector<string> blkdugList;
	vector<string> allwList;
	vector<string> blkdList;
public:
	ComponentService(){}
	virtual ~ComponentService(){}
	friend class ComponentGen;
};
class Component {
	void* invokeServiceInternal(string serviceName,vector<void*> args);
	string name;
	string desc;
	bool ajaxAvail;
	bool websAvail;
	bool methinvAvail;
	bool autoTranx;
	bool thrdPerReq;
	int thrdPoolSize;
	int dbconpoolSize;
	string authFrom;
	ConnectionSettings authDets;
	bool authMode;
	vector<string> protocols;
	vector<string> allwList;
	vector<string> blkdList;
	bool session;
	map<string,ComponentService> services;
	Cibernate *chib;
public:
	Component(string name,string desc,bool ajav,bool wsav,bool miav,bool autotx,bool thrdpr,int thrdpsiz,int dbcps,string authfr,ConnectionSettings authdet,string authmod,
			vector<string> protocol,vector<string> alwls,vector<string> blkls,bool sess,map<string,ComponentService> service){}
	Component();
	virtual ~Component();
	void init();
	void destroy();
	bool isAvaibleForMI(bool);//is available for method invocation
	bool isWebService(bool);//is exposed as a web service
	bool isAJAXAvailable(bool);//is avaiable to javascript as a service
	//Synchronize all required methods for concurrency control

	//Database and Trnsaction related functions
	Connection getReadConnection();
	Connection getWriteConnection();
	void closeConnection();
	void startTransaction();
	void endTransaction();
	void commit();
	void rollback();
	void execute();
	Cibernate* getCibernate();

	//Security, Authentication and Authorization functions
	bool setMode(string mode);//ldap|database|filesystem
	void setDetails();//Details about the file/db or ldap connecn and parameters ConnectionSettings
	bool isAuthorizedTo();//Task
	bool allowAccess();
	bool isAccesAllowed();

   	//Messaging services
	void setMode(bool);//true for asynchronous and false for synchronous
	bool sendMessage(Message);
	Message readMessage();
	void registerRemoteEventHandler();
	void unRegisterRemoteEventHandler();

	bool calledFromHTTPAJAX();
	bool calledFromHTTPREQU();
	bool calledFromHTTPSOAP();
	bool calledFromMETHINVK();
	bool calledFromPROGCNTX();

	void* invokeService(string serviceName,vector<void*> args);

	string getName() const
	{
		return name;
	}

	void setName(string name)
	{
		this->name = name;
	}

	string getDesc() const
	{
		return desc;
	}

	void setDesc(string desc)
	{
		this->desc = desc;
	}

	bool getAjaxAvail() const
	{
		return ajaxAvail;
	}

	void setAjaxAvail(bool ajaxAvail)
	{
		this->ajaxAvail = ajaxAvail;
	}

	bool getWebsAvail() const
	{
		return websAvail;
	}

	void setWebsAvail(bool websAvail)
	{
		this->websAvail = websAvail;
	}

	bool getMethinvAvail() const
	{
		return methinvAvail;
	}

	void setMethinvAvail(bool methinvAvail)
	{
		this->methinvAvail = methinvAvail;
	}

	bool getAutoTranx() const
	{
		return autoTranx;
	}

	void setAutoTranx(bool autoTranx)
	{
		this->autoTranx = autoTranx;
	}

	bool getThrdPerReq() const
	{
		return thrdPerReq;
	}

	void setThrdPerReq(bool thrdPerReq)
	{
		this->thrdPerReq = thrdPerReq;
	}

	int getThrdPoolSize() const
	{
		return thrdPoolSize;
	}

	void setThrdPoolSize(int thrdPoolSize)
	{
		this->thrdPoolSize = thrdPoolSize;
	}

	int getDbconpoolSize() const
	{
		return dbconpoolSize;
	}

	void setDbconpoolSize(int dbconpoolSize)
	{
		this->dbconpoolSize = dbconpoolSize;
	}

	string getAuthFrom() const
	{
		return authFrom;
	}

	void setAuthFrom(string authFrom)
	{
		this->authFrom = authFrom;
	}

	ConnectionSettings getAuthDets() const
	{
		return authDets;
	}

	void setAuthDets(ConnectionSettings authDets)
	{
		this->authDets = authDets;
	}

	bool getAuthMode() const
	{
		return authMode;
	}

	void setAuthMode(bool authMode)
	{
		this->authMode = authMode;
	}

	vector<string> getProtocols() const
	{
		return protocols;
	}

	void setProtocols(vector<string> protocols)
	{
		this->protocols = protocols;
	}

	vector<string> getAllwList() const
	{
		return allwList;
	}

	void setAllwList(vector<string> allwList)
	{
		this->allwList = allwList;
	}

	vector<string> getBlkdList() const
	{
		return blkdList;
	}

	void setBlkdList(vector<string> blkdList)
	{
		this->blkdList = blkdList;
	}

	bool getSession() const
	{
		return session;
	}

	void setSession(bool session)
	{
		this->session = session;
	}

	map<string,ComponentService> getServices() const
	{
		return services;
	}

	void setServices(map<string,ComponentService> services)
	{
		this->services = services;
	}
};

#endif /* COMPONENT_H_ */
