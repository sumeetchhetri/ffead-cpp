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
#include "Reflector.h"

std::map<std::string, ClassInfo> Reflector::_ciMap;
ClassInfo Reflector::nullclass;

Reflector::Reflector()
{
	closedlib = true;
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		throw "Cannot load reflection shared library";
	}
	dlibinstantiated = true;
}

Reflector::Reflector(void* dlib)
{
	if(dlib == NULL)
	{
		throw "Cannot load reflection shared library";
	}
	closedlib = false;
	this->dlib = dlib;
	dlibinstantiated = false;
}

Reflector::~Reflector()
{
	if(dlibinstantiated && closedlib)
	{
		dlclose(dlib);
	}
}
void Reflector::cleanUp()
{
	for (int var=0;var<(int)objectT.size();var++)
	{
		if(objectT.at(var)=="string" || objectT.at(var)=="int" || objectT.at(var)=="long"
			|| objectT.at(var)=="double" || objectT.at(var)=="float" || objectT.at(var)=="bool"
				|| objectT.at(var)=="char" || objectT.at(var)=="long long")
			delete objects.at(var);
		else
		{
			destroy(objects.at(var),objectT.at(var));
		}
	}
}

const ClassInfo& Reflector::getClassInfo(const std::string& cs, const std::string& app)
{
	std::string className = cs;
	std::string appName = CommonUtils::getAppName(app);
	StringUtil::replaceAll(className, "::", "_");
	std::string ca = appName +"-"+ className;
	if(_ciMap.find(ca)!=_ciMap.end()) {
		return _ciMap[ca];
	}
	std::string methodname = appName + "_"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef ClassInfo (*RfPtr) ();
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		_ciMap[ca] = f();
		return _ciMap[ca];
	}
	else
		return nullclass;
}

