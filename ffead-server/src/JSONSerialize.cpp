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
 * JSONSerialize.cpp
 *
 *  Created on: Jan 27, 2013
 *      Author: sumeet
 */

#include "JSONSerialize.h"

string JSONSerialize::demangle(const char *mangled)
{
	int status;	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	string s(demangled);
	delete demangled;
	return s;
}
string JSONSerialize::getClassName(void* instance)
{
	const char *mangled = typeid(instance).name();
	return demangle(mangled);
}

string JSONSerialize::_handleAllSerialization(string className,void *t, string appName)
{
	string objXml;
	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		objXml = "\""+CastUtil::lexical_cast<string>(tem)+"\"";
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		objXml = "\""+formt.format(*(Date*)t)+"\"";
	}
	else if(className=="BinaryData")
	{
		objXml = "\""+BinaryData::serilaize(*(BinaryData*)t)+"\"";
	}
	else if(className.find("std::vector<std::string,")!=string::npos || className.find("std::vector<string,")!=string::npos)
	{
		vector<string> *tt = (vector<string>*)t;
		objXml = serializevec<string>(*tt,appName);
	}
	else if(className.find("std::vector<int,")!=string::npos)
	{
		vector<int> *tt = (vector<int>*)t;
		objXml = serializevec<int>(*tt,appName);
	}
	else if(className.find("std::vector<short,")!=string::npos)
	{
		vector<short> *tt = (vector<short>*)t;
		objXml = serializevec<short>(*tt,appName);
	}
	else if(className.find("std::vector<long,")!=string::npos)
	{
		vector<long> *tt = (vector<long>*)t;
		objXml = serializevec<long>(*tt,appName);
	}
	else if(className.find("std::vector<double,")!=string::npos)
	{
		vector<double> *tt = (vector<double>*)t;
		objXml = serializevec<double>(*tt,appName);
	}
	else if(className.find("std::vector<float,")!=string::npos)
	{
		vector<float> *tt = (vector<float>*)t;
		objXml = serializevec<float>(*tt,appName);
	}
	else if(className.find("std::vector<bool,")!=string::npos)
	{
		vector<bool> *tt = (vector<bool>*)t;
		objXml = serializevec<bool>(*tt,appName);
	}
	else if(className.find("std::vector<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		string vtyp = className.substr(0,className.find(","));
		return _serContainers(t,vtyp,"Vec",appName);
	}
	else if(className.find("std::list<std::string,")!=string::npos || className.find("std::list<string,")!=string::npos)
	{
		list<string> *tt = (list<string>*)t;
		objXml = serializelist<string>(*tt,appName);
	}
	else if(className.find("std::list<int,")!=string::npos)
	{
		list<int> *tt = (list<int>*)t;
		objXml = serializelist<int>(*tt,appName);
	}
	else if(className.find("std::list<long,")!=string::npos)
	{
		list<long> *tt = (list<long>*)t;
		objXml = serializelist<long>(*tt,appName);
	}
	else if(className.find("std::list<short,")!=string::npos)
	{
		list<short> *tt = (list<short>*)t;
		objXml = serializelist<short>(*tt,appName);
	}
	else if(className.find("std::list<double,")!=string::npos)
	{
		list<double> *tt = (list<double>*)t;
		objXml = serializelist<double>(*tt,appName);
	}
	else if(className.find("std::list<float,")!=string::npos)
	{
		list<float> *tt = (list<float>*)t;
		objXml = serializelist<float>(*tt,appName);
	}
	else if(className.find("std::list<bool,")!=string::npos)
	{
		list<bool> *tt = (list<bool>*)t;
		objXml = serializelist<bool>(*tt,appName);
	}
	else if(className.find("std::list<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::list<","");
		string vtyp = className.substr(0,className.find(","));
		return _serContainers(t,vtyp,"Lis",appName);
	}
	else if(className.find("std::set<std::string,")!=string::npos || className.find("std::set<string,")!=string::npos)
	{
		set<string> *tt = (set<string>*)t;
		objXml = serializeset<string>(*tt,appName);
	}
	else if(className.find("std::set<int,")!=string::npos)
	{
		set<int> *tt = (set<int>*)t;
		objXml = serializeset<int>(*tt,appName);
	}
	else if(className.find("std::set<short,")!=string::npos)
	{
		set<short> *tt = (set<short>*)t;
		objXml = serializeset<short>(*tt,appName);
	}
	else if(className.find("std::set<long,")!=string::npos)
	{
		set<long> *tt = (set<long>*)t;
		objXml = serializeset<long>(*tt,appName);
	}
	else if(className.find("std::set<double,")!=string::npos)
	{
		set<double> *tt = (set<double>*)t;
		objXml = serializeset<double>(*tt,appName);
	}
	else if(className.find("std::set<float,")!=string::npos)
	{
		set<float> *tt = (set<float>*)&t;
		objXml = serializeset<float>(*tt,appName);
	}
	else if(className.find("std::set<bool,")!=string::npos)
	{
		set<bool> *tt = (set<bool>*)&t;
		objXml = serializeset<bool>(*tt,appName);
	}
	else if(className.find("std::set<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::set<","");
		string vtyp = className.substr(0,className.find(","));
		return _serContainers(t,vtyp,"Set",appName);
	}
	else if(className.find("std::multiset<std::string,")!=string::npos || className.find("std::multiset<string,")!=string::npos)
	{
		multiset<string> *tt = (multiset<string>*)t;
		objXml = serializemultiset<string>(*tt,appName);
	}
	else if(className.find("std::multiset<int,")!=string::npos)
	{
		multiset<int> *tt = (multiset<int>*)t;
		objXml = serializemultiset<int>(*tt,appName);
	}
	else if(className.find("std::multiset<long,")!=string::npos)
	{
		multiset<long> *tt = (multiset<long>*)t;
		objXml = serializemultiset<long>(*tt,appName);
	}
	else if(className.find("std::multiset<short,")!=string::npos)
	{
		multiset<short> *tt = (multiset<short>*)t;
		objXml = serializemultiset<short>(*tt,appName);
	}
	else if(className.find("std::multiset<double,")!=string::npos)
	{
		multiset<double> *tt = (multiset<double>*)t;
		objXml = serializemultiset<double>(*tt,appName);
	}
	else if(className.find("std::multiset<float,")!=string::npos)
	{
		multiset<float> *tt = (multiset<float>*)t;
		objXml = serializemultiset<float>(*tt,appName);
	}
	else if(className.find("std::multiset<bool,")!=string::npos)
	{
		multiset<bool> *tt = (multiset<bool>*)t;
		objXml = serializemultiset<bool>(*tt,appName);
	}
	else if(className.find("std::multiset<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::multiset<","");
		string vtyp = className.substr(0,className.find(","));
		return _serContainers(t,vtyp,"MulSet",appName);
	}
	else if(className.find("std::queue<std::string,")!=string::npos || className.find("std::queue<string,")!=string::npos)
	{
		std::queue<string> *tt = (std::queue<string>*)t;
		objXml = serializeq<string>(*tt,appName);
	}
	else if(className.find("std::queue<int,")!=string::npos)
	{
		std::queue<int> *tt = (std::queue<int>*)t;
		objXml = serializeq<int>(*tt,appName);
	}
	else if(className.find("std::queue<short,")!=string::npos)
	{
		std::queue<short> *tt = (std::queue<short>*)t;
		objXml = serializeq<short>(*tt,appName);
	}
	else if(className.find("std::queue<long,")!=string::npos)
	{
		std::queue<long> *tt = (std::queue<long>*)t;
		objXml = serializeq<long>(*tt,appName);
	}
	else if(className.find("std::queue<double,")!=string::npos)
	{
		std::queue<double> *tt = (std::queue<double>*)t;
		objXml = serializeq<double>(*tt,appName);
	}
	else if(className.find("std::queue<float,")!=string::npos)
	{
		std::queue<float> *tt = (std::queue<float>*)t;
		objXml = serializeq<float>(*tt,appName);
	}
	else if(className.find("std::queue<bool,")!=string::npos)
	{
		std::queue<bool> *tt = (std::queue<bool>*)t;
		objXml = serializeq<bool>(*tt,appName);
	}
	else if(className.find("std::queue<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::queue<","");
		string vtyp = className.substr(0,className.find(","));
		return _serContainers(t,vtyp,"Q",appName);
	}
	else if(className.find("std::deque<std::string,")!=string::npos || className.find("std::deque<string,")!=string::npos)
	{
		deque<string> *tt = (deque<string>*)t;
		objXml = serializedq<string>(*tt,appName);
	}
	else if(className.find("std::deque<int,")!=string::npos)
	{
		deque<int> *tt = (deque<int>*)t;
		objXml = serializedq<int>(*tt,appName);
	}
	else if(className.find("std::deque<long,")!=string::npos)
	{
		deque<long> *tt = (deque<long>*)t;
		objXml = serializedq<long>(*tt,appName);
	}
	else if(className.find("std::deque<short,")!=string::npos)
	{
		deque<short> *tt = (deque<short>*)t;
		objXml = serializedq<short>(*tt,appName);
	}
	else if(className.find("std::deque<double,")!=string::npos)
	{
		deque<double> *tt = (deque<double>*)t;
		objXml = serializedq<double>(*tt,appName);
	}
	else if(className.find("std::deque<float,")!=string::npos)
	{
		deque<float> *tt = (deque<float>*)t;
		objXml = serializedq<float>(*tt,appName);
	}
	else if(className.find("std::deque<bool,")!=string::npos)
	{
		deque<bool> *tt = (deque<bool>*)t;
		objXml = serializedq<bool>(*tt,appName);
	}
	else if(className.find("std::deque<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::deque<","");
		string vtyp = className.substr(0,className.find(","));
		return _serContainers(t,vtyp,"Dq",appName);
	}
	else
	{
		return _ser(t,className,appName);
	}
	return objXml;
}


string JSONSerialize::_serContainers(void* t,string className, string type, string appName)
{
	string json;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "from"+className+type+"VPToJSON";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		json = f(t);
	return json;
}

string JSONSerialize::_ser(void* t,string className, string appName)
{
	string json;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "from"+className+"VPToJSON";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		json = f(t);
	return json;
}

string JSONSerialize::_ser(Object t, string appName)
{
	return _ser(t.getVoidPointer(),t.getTypeName(), appName);
}


void* JSONSerialize::_handleAllUnSerialization(string json,string className, string appName)
{
	if(className=="std::string" || className=="string" || className=="int" || className=="short" || className=="float" || className=="double"
			|| className=="long" || className=="bool")
	{
		if(className=="int")
		{
			int *vt = new int;
			*vt = CastUtil::lexical_cast<int>(json);
			return vt;
		}
		else if(className=="long")
		{
			long *vt = new long;
			*vt = CastUtil::lexical_cast<long>(json);
			return vt;
		}
		else if(className=="short")
		{
			short *vt = new short;
			*vt = CastUtil::lexical_cast<short>(json);
			return vt;
		}
		else if(className=="Date")
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			if(json.length()>2)
				return formt.parse(json.substr(1, json.length()-1));
			else
			{
				return new Date;
			}
		}
		else if(className=="BinaryData")
		{
			if(json.length()>2)
				return BinaryData::unSerilaize(json.substr(1, json.length()-1));
			else
				return new BinaryData;
		}
		else if(className=="float")
		{
			float *vt = new float;
			*vt = CastUtil::lexical_cast<float>(json);
			return vt;
		}
		else if(className=="double")
		{
			double *vt = new double;
			*vt = CastUtil::lexical_cast<double>(json);
			return vt;
		}
		else if(className=="bool")
		{
			bool *vt = new bool;
			*vt = CastUtil::lexical_cast<bool>(json);
			return vt;
		}
		else if(className=="std::string" || className=="string")
		{
			string *vt = new string;
			if(json.length()>2)
				*vt = CastUtil::lexical_cast<string>(json.substr(1, json.length()-1));
			return vt;
		}
	}
	else if(className.find("std::vector<")==0)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,">","");
		return unserializeConatiner(json, className, "Vec", appName);
	}
	else if(className.find("std::list<")==0)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,">","");
		return unserializeConatiner(json, className,"Lis", appName);
	}
	else if(className.find("std::set<")==0)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,">","");
		return unserializeConatiner(json, className,"Set", appName);
	}
	else if(className.find("std::multiset<")==0)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,">","");
		return unserializeConatiner(json, className, "MulSet", appName);
	}
	else if(className.find("std::queue<")==0)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,">","");
		return unserializeConatiner(json, className,"Q", appName);
	}
	else if(className.find("std::deque<")==0)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,">","");
		return unserializeConatiner(json, className, "Dq", appName);
	}
	return _unser(json,className, appName);
}

void* JSONSerialize::unserializeConatiner(string json, string className, string type, string appName)
{
	if(className=="string" || className=="std::string")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<string> *t = JSONUtil::toVectorP<string>(root);
		return unserContainer<string>(*t, type, appName);
	}
	else if(className=="double")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<double> *t = JSONUtil::toVectorP<double>(root);
		return unserContainer<double>(*t, type, appName);
	}
	else if(className=="float")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<float> *t = JSONUtil::toVectorP<float>(root);
		return unserContainer<float>(*t, type, appName);
	}
	else if(className=="int")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<int> *t = JSONUtil::toVectorP<int>(root);
		return unserContainer<int>(*t, type, appName);
	}
	else if(className=="short")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<short> *t = JSONUtil::toVectorP<short>(root);
		return unserContainer<short>(*t, type, appName);
	}
	else if(className=="long")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<long> *t = JSONUtil::toVectorP<long>(root);
		return unserContainer<long>(*t, type, appName);
	}
	else if(className=="bool")
	{
		JSONElement root = JSONUtil::getDocument(json);
		vector<bool> *t = JSONUtil::toVectorP<bool>(root);
		return unserContainer<bool>(*t, type, appName);
	}
	else
		return _unserCont(json,className, type, appName);

}

void* JSONSerialize::_unserCont(string objXml,string className,string type, string appName)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "to"+className+type+"VP";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}


void* JSONSerialize::_unser(string objXml,string className, string appName)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "toVoidP"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}
