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
 * FFEADContext.h
 *
 *  Created on: Oct 17, 2010
 *      Author: root
 */

#ifndef FFEADCONTEXT_H_
#define FFEADCONTEXT_H_
#include "XmlParser.h"
#include "Reflector.h"
#include "CastUtil.h"
#include "StringUtil.h"
#include "LoggerFactory.h"
#ifdef HAVE_LIBUUID
#include <uuid/uuid.h>
#endif
#ifdef HAVE_BSDUUIDINC
#include <uuid.h>
#endif
#include "Timer.h"

class Bean
{
	friend class FFEADContext;
	std::string name,inbuilt,value,clas,bean,intfType,injectAs,scope;
	bool realbean;
	std::vector<std::string> injs,names,types;
	std::string appName;
public:
	Bean();
	Bean(const std::string& name, const std::string& value, const std::string& clas, const std::string& scope, const bool& isInbuilt, const std::string& appName= "default");
	~Bean();
};
typedef std::map<std::string,Bean> beanMap;
class FFEADContext {
	Logger logger;
	beanMap beans,injbns;
	std::map<std::string, std::map<std::string, void*> > objects;
	bool cleared;
	Reflector* reflector;
	std::map<std::string, std::map<std::string, ClassInfo> > classInfoMap;
	friend class ControllerHandler;
	friend class ExtHandler;
	friend class FormHandler;
	friend class SecurityHandler;
	friend class FilterHandler;
	friend class ServiceTask;
public:
	FFEADContext(const std::string&, const std::string& appName= "default");
	FFEADContext();
	virtual ~FFEADContext();
	void* getBean(const std::string&, const std::string& appName= "default");
	void* getBean(const Bean&);
	void clear(const std::string& appName= "default");
	void addBean(Bean& bean);
	void initializeAllSingletonBeans(const std::map<std::string, bool>& servingContexts);
	void clearAllSingletonBeans(const std::map<std::string, bool>& servingContexts);
	Reflector& getReflector();
	void release(void* instance, const std::string& beanName, const std::string& appName);
};

#endif /* FFEADCONTEXT_H_ */
