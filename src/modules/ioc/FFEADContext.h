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
 * FFEADContext.h
 *
 *  Created on: Oct 17, 2010
 *      Author: root
 */

#ifndef FFEADCONTEXT_H_
#define FFEADCONTEXT_H_
#include "SimpleXmlParser.h"
#include "Reflector.h"
#include "CastUtil.h"
#include "StringUtil.h"
#include "LoggerFactory.h"
#ifdef HAVE_UUIDINC
#include <uuid/uuid.h>
#endif
#if defined(HAVE_BSDUUIDINC) || defined(HAVE_OSSPUUIDINC)
#include <uuid.h>
#endif
#include "Timer.h"
#include "map"

class Bean
{
	friend class FFEADContext;
	std::string name,inbuilt,value,clas,bean,intfType,injectAs,scope;
	bool realbean;
	bool isController;
	std::vector<std::string> injs,names,types;
	std::string appName;
public:
	Bean();
	Bean(const std::string& name, const std::string& value, const std::string& clas, const std::string& scope, const bool& isInbuilt, bool isController, const std::string& appName= "default");
	~Bean();
};
typedef std::map<std::string,Bean> beanMap;
class FFEADContext {
	Logger logger;
	beanMap beans,injbns;
	std::map<std::string, void*> objects;
	std::map<std::string, ClassInfo*> contInsMap;
	bool cleared;
	Reflector* reflector;
	friend class ControllerHandler;
	friend class ExtHandler;
	friend class FormHandler;
	friend class SecurityHandler;
	friend class FilterHandler;
	friend class ServiceTask;
	friend class ConfigurationData;
public:

	FFEADContext(const std::string&, const std::string& appName= "default");
	FFEADContext();
	virtual ~FFEADContext();
	void* getBean(const std::string&, const std::string& appName= "default");
	void* getBean(const std::string&, std::string_view);
	void* getBean(const Bean&);
	void clear(const std::string& appName= "default");
	void addBean(Bean& bean);
	void initializeAllSingletonBeans(const std::map<std::string, bool, std::less<> >& servingContexts, Reflector* reflector);
	void clearAllSingletonBeans(const std::map<std::string, bool, std::less<> >& servingContexts);
	Reflector* getReflector();
	void release(void* instance, const std::string& beanName, const std::string& appName);
	//TODO optimize for string_view later
	void release(void* instance, const std::string& beanName, std::string_view appName);
};

#endif /* FFEADCONTEXT_H_ */
