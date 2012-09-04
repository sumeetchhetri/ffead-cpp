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
 * AppContext.cpp
 *
 *  Created on: Mar 30, 2010
 *      Author: sumeet
 */

#include "AppContext.h"

BeanException::BeanException(string message)
{
	this->setMessage(message);
}

BeanException::~BeanException() throw()
{

}

AppContext* AppContext::_instance =  NULL;

void AppContext::init()
{
	if(_instance==NULL)
	{
		_instance = new AppContext();
	}
}

AppContext::AppContext() {
	// TODO Auto-generated constructor stub

}

AppContext::~AppContext() {
	// TODO Auto-generated destructor stub
}

bool AppContext::registerComponent(string name)
{
	init();
	if(_instance->components.find(name)!=_instance->components.end())
	{
		return false;
	}
	else
	{
		_instance->components[name] = "";
		return true;
	}
}

bool AppContext::unregisterComponent(string name)
{
	init();
	map<string,string>::iterator it = _instance->components.find(name);
	if(it!=_instance->components.end())
	{
		_instance->components.erase(it);
		return true;
	}
	else
		return false;
}

void AppContext::lookup(string name)
{
	if(_instance==NULL)
		throw new Exception("Bean Container Not Running");
	if(_instance->components.find(name)==_instance->components.end())
	{
		throw new BeanException("Bean Not Deployed");
	}
}
