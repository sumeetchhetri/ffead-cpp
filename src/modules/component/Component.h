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
#include "DataSourceManager.h"

class ComponentService {
private:
	std::string classn;
	std::string methName;
	std::string retType;
	std::string address;
	std::vector<std::string> allwugList;
	std::vector<std::string> blkdugList;
	std::vector<std::string> allwList;
	std::vector<std::string> blkdList;
public:
	ComponentService(){}
	virtual ~ComponentService(){}
	friend class ComponentGen;
};
class Component {
	void* invokeServiceInternal(const std::string& serviceName, std::vector<void*> args);
	std::string name;
	std::string desc;
	bool ajaxAvail;
	bool websAvail;
	bool methinvAvail;
	bool autoTranx;
	bool thrdPerReq;
	int thrdPoolSize;
	int dbconpoolSize;
	std::string authFrom;
	ConnectionSettings authDets;
	bool authMode;
	std::vector<std::string> protocols;
	std::vector<std::string> allwList;
	std::vector<std::string> blkdList;
	bool session;
	std::map<std::string,ComponentService> services;
	DataSourceInterface *chib;
public:
	Component();
	virtual ~Component();
	void init();
	void destroy();
	bool isAvaibleForMI(const bool&);//is available for method invocation
	bool isWebService(const bool&);//is exposed as a web service
	bool isAJAXAvailable(const bool&);//is avaiable to javascript as a service
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
	DataSourceInterface* getDataSource();

	//Security, Authentication and Authorization functions
	bool setMode(const std::string& mode);//ldap|database|filesystem
	void setDetails();//Details about the file/db or ldap connecn and parameters ConnectionSettings
	bool isAuthorizedTo();//Task
	bool allowAccess();
	bool isAccesAllowed();

   	//Messaging services
	void setMode(const bool&);//true for asynchronous and false for synchronous
	bool sendMessage(const Message&);
	Message readMessage();
	void registerRemoteEventHandler();
	void unRegisterRemoteEventHandler();

	bool calledFromHTTPAJAX();
	bool calledFromHTTPREQU();
	bool calledFromHTTPSOAP();
	bool calledFromMETHINVK();
	bool calledFromPROGCNTX();

	void* invokeService(const std::string& serviceName, std::vector<void*> args);

	std::string getName() const
	{
		return name;
	}

	void setName(const std::string& name)
	{
		this->name = name;
	}

	std::string getDesc() const
	{
		return desc;
	}

	void setDesc(const std::string& desc)
	{
		this->desc = desc;
	}

	bool getAjaxAvail() const
	{
		return ajaxAvail;
	}

	void setAjaxAvail(const bool& ajaxAvail)
	{
		this->ajaxAvail = ajaxAvail;
	}

	bool getWebsAvail() const
	{
		return websAvail;
	}

	void setWebsAvail(const bool& websAvail)
	{
		this->websAvail = websAvail;
	}

	bool getMethinvAvail() const
	{
		return methinvAvail;
	}

	void setMethinvAvail(const bool& methinvAvail)
	{
		this->methinvAvail = methinvAvail;
	}

	bool getAutoTranx() const
	{
		return autoTranx;
	}

	void setAutoTranx(const bool& autoTranx)
	{
		this->autoTranx = autoTranx;
	}

	bool getThrdPerReq() const
	{
		return thrdPerReq;
	}

	void setThrdPerReq(const bool& thrdPerReq)
	{
		this->thrdPerReq = thrdPerReq;
	}

	int getThrdPoolSize() const
	{
		return thrdPoolSize;
	}

	void setThrdPoolSize(const int& thrdPoolSize)
	{
		this->thrdPoolSize = thrdPoolSize;
	}

	int getDbconpoolSize() const
	{
		return dbconpoolSize;
	}

	void setDbconpoolSize(const int& dbconpoolSize)
	{
		this->dbconpoolSize = dbconpoolSize;
	}

	std::string getAuthFrom() const
	{
		return authFrom;
	}

	void setAuthFrom(const std::string& authFrom)
	{
		this->authFrom = authFrom;
	}

	ConnectionSettings getAuthDets() const
	{
		return authDets;
	}

	void setAuthDets(const ConnectionSettings& authDets)
	{
		this->authDets = authDets;
	}

	bool getAuthMode() const
	{
		return authMode;
	}

	void setAuthMode(const bool& authMode)
	{
		this->authMode = authMode;
	}

	std::vector<std::string> getProtocols() const
	{
		return protocols;
	}

	void setProtocols(const std::vector<std::string>& protocols)
	{
		this->protocols = protocols;
	}

	std::vector<std::string> getAllwList() const
	{
		return allwList;
	}

	void setAllwList(const std::vector<std::string>& allwList)
	{
		this->allwList = allwList;
	}

	std::vector<std::string> getBlkdList() const
	{
		return blkdList;
	}

	void setBlkdList(const std::vector<std::string>& blkdList)
	{
		this->blkdList = blkdList;
	}

	bool getSession() const
	{
		return session;
	}

	void setSession(const bool& session)
	{
		this->session = session;
	}

	std::map<std::string,ComponentService> getServices() const
	{
		return services;
	}

	void setServices(const std::map<std::string,ComponentService>& services)
	{
		this->services = services;
	}
};

#endif /* COMPONENT_H_ */
