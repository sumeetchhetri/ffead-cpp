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

map<string, ClassInfo> Reflector::_ciMap;
ClassInfo Reflector::nullclass;

Reflector::Reflector()
{
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
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
	this->dlib = dlib;
	dlibinstantiated = false;
}

Reflector::~Reflector()
{
	if(dlibinstantiated)
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
				|| objectT.at(var)=="char")
			delete objects.at(var);
		else
		{
			destroy(objects.at(var),objectT.at(var));
		}
	}
}

const ClassInfo& Reflector::getClassInfo(const string& cs, const string& app)
{
	string className = cs;
	string appName = CommonUtils::getAppName(app);
	StringUtil::replaceAll(className, "::", "_");
	string ca = appName +"-"+ className;
	if(_ciMap.find(ca)!=_ciMap.end()) {
		return _ciMap[ca];
	}
	string methodname = appName + "_"+className;
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

