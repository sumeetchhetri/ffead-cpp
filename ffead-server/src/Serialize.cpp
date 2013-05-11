/*
	Copyright 2009-2013, Sumeet Chhetri
  
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
 * Serialize.cpp
 *
 *  Created on: Apr 2, 2010
 *      Author: sumeet
 */

#include "Serialize.h"

string Serialize::demangle(const char *mangled)
{
	int status;	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	string s(demangled);
	delete demangled;
	return s;
}
string Serialize::getClassName(void* instance)
{
	const char *mangled = typeid(instance).name();
	return demangle(mangled);
}

string Serialize::_handleAllSerialization(string className,void *t, string appName)
{
	string objXml;
	AMEFEncoder enc;
	AMEFObject object;

	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
		//objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		object.addPacket(formt.format(*(Date*)t), className);
		objXml = enc.encodeB(&object, false);
		//objXml = formt.format(*(Date*)t);
	}
	else if(className=="BinaryData")
	{
		object.addPacket(BinaryData::serilaize(*(BinaryData*)t), className);
		objXml = enc.encodeB(&object, false);
		//objXml = BinaryData::serilaize(*(BinaryData*)t);
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
		return _servec(t,vtyp,appName);
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
		return _serlis(t,vtyp,appName);
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
		return _serset(t,vtyp,appName);
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
		return _sermultiset(t,vtyp,appName);
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
		return _serq(t,vtyp,appName);
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
		return _serdq(t,vtyp,appName);
	}
	else
	{
		return _ser(t,className,appName);
	}
	return objXml;
}

string Serialize::_servec(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className+"Vec";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string Serialize::_serlis(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className+"Lis";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_serset(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className+"Set";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_sermultiset(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className+"MulSet";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_serq(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className+"Q";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_serdq(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className+"Dq";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string Serialize::_ser(void* t,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binarySerialize"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string Serialize::_ser(Object t, string appName)
{
	return _ser(t.getVoidPointer(),t.getTypeName(),appName);
}


void* Serialize::_handleAllUnSerialization(string objXml,string className, string appName)
{
	AMEFDecoder dec;
	AMEFObject* root = dec.decodeB(objXml, true, false);
	if(root==NULL)
		return NULL;
	if(className=="std::string" || className=="string" || className=="int" || className=="short" ||
					className=="bool" || className=="long" || className=="float" || className=="double")
	{
		root = root->getPackets().at(0);
		if(className=="int" && className==root->getNameStr())
		{
			int *vt = new int;
			*vt = root->getIntValue();
			return vt;
		}
		else if(className=="short" && className==root->getNameStr())
		{
			short *vt = new short;
			*vt = root->getShortValue();
			return vt;
		}
		else if(className=="long" && className==root->getNameStr())
		{
			long *vt = new long;
			*vt = root->getLongValue();
			return vt;
		}
		else if(className=="Date" && className==root->getNameStr())
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(root->getValueStr());
		}
		else if(className=="BinaryData" && className==root->getNameStr())
		{
			return BinaryData::unSerilaize(root->getValueStr());
		}
		else if(className=="float" && className==root->getNameStr())
		{
			float *vt = new float;
			*vt = root->getFloatValue();
			return vt;
		}
		else if(className=="double" && className==root->getNameStr())
		{
			double *vt = new double;
			*vt = root->getDoubleValue();
			return vt;
		}
		else if(className=="bool" && className==root->getNameStr())
		{
			bool *vt = new bool;
			*vt = root->getBoolValue();
			return vt;
		}
		else if((className=="std::string" || className=="string") && className==root->getNameStr())
		{
			string *vt = new string;
			*vt = root->getValueStr();
			return vt;
		}
	}
	else if(root->getNameStr()!="")
	{
		if(root->getNameStr().find("vector<")==0)
		{
			return unserializevec(root, objXml,appName);
		}
		else if(root->getNameStr().find("set<")==0)
		{
			return unserializeset(root, objXml,appName);
		}
		else if(root->getNameStr().find("multiset<")==0)
		{
			return unserializemultiset(root, objXml,appName);
		}
		else if(root->getNameStr().find("list<")==0)
		{
			return unserializelist(root, objXml,appName);
		}
		else if(root->getNameStr().find("std::queue<")==0 || root->getNameStr().find("queue<")==0)
		{
			return unserializeq(root, objXml,appName);
		}
		else if(root->getNameStr().find("deque<")==0)
		{
			return unserializedq(root, objXml,appName);
		}
	}
	return _unser(objXml,className,appName);
}

void* Serialize::unserializeset(AMEFObject* root, string objXml, string appName)
{
	AMEFDecoder dec;
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(4,stlclassName.find(">")-4);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new set<string>();
	else if(className=="int")
		t = new set<int>();
	else if(className=="short")
		t = new set<short>();
	else if(className=="long")
		t = new set<long>();
	else if(className=="float")
		t = new set<float>();
	else if(className=="double")
		t = new set<double>();
	else if(className=="bool")
		t = new set<bool>();
	else
	{
		return _unserSet(objXml,className,appName);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
			if(className=="std::string" || className=="string")
				((set<string>*)t)->insert(root2->getPackets().at(0)->getValueStr());
			else if(className=="int")
				((set<int>*)t)->insert(root2->getPackets().at(0)->getIntValue());
			else if(className=="short")
				((set<short>*)t)->insert(root2->getPackets().at(0)->getShortValue());
			else if(className=="long")
				((set<long>*)t)->insert(root2->getPackets().at(0)->getNumericValue());
			else if(className=="float")
				((set<float>*)t)->insert(root2->getPackets().at(0)->getFloatValue());
			else if(className=="double")
				((set<double>*)t)->insert(root2->getPackets().at(0)->getDoubleValue());
			else if(className=="bool")
				((set<bool>*)t)->insert(root2->getPackets().at(0)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializelist(AMEFObject* root, string objXml, string appName)
{
	AMEFDecoder dec;
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(5,stlclassName.find(">")-5);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new list<string>();
	else if(className=="int")
		t = new list<int>();
	else if(className=="short")
		t = new list<short>();
	else if(className=="long")
		t = new list<long>();
	else if(className=="float")
		t = new list<float>();
	else if(className=="double")
		t = new list<double>();
	else if(className=="bool")
		t = new list<bool>();
	else
	{
		return _unserLis(objXml,className,appName);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
			if(className=="std::string" || className=="string")
				((list<string>*)t)->push_back(root2->getPackets().at(0)->getValueStr());
			else if(className=="int")
				((list<int>*)t)->push_back(root2->getPackets().at(0)->getIntValue());
			else if(className=="short")
				((list<short>*)t)->push_back(root2->getPackets().at(0)->getShortValue());
			else if(className=="long")
				((list<long>*)t)->push_back(root2->getPackets().at(0)->getNumericValue());
			else if(className=="float")
				((list<float>*)t)->push_back(root2->getPackets().at(0)->getFloatValue());
			else if(className=="double")
				((list<double>*)t)->push_back(root2->getPackets().at(0)->getDoubleValue());
			else if(className=="bool")
				((list<bool>*)t)->push_back(root2->getPackets().at(0)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializeq(AMEFObject* root, string objXml, string appName)
{
	AMEFDecoder dec;
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(6,stlclassName.find(">")-6);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::queue<string>();
	else if(className=="int")
		t = new std::queue<int>();
	else if(className=="short")
		t = new std::queue<short>();
	else if(className=="long")
		t = new std::queue<long>();
	else if(className=="float")
		t = new std::queue<float>();
	else if(className=="double")
		t = new std::queue<double>();
	else if(className=="bool")
		t = new std::queue<bool>();
	else
	{
		return _unserQ(objXml,className,appName);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
			if(className=="std::string" || className=="string")
				((std::queue<string>*)t)->push(root2->getPackets().at(0)->getValueStr());
			else if(className=="int")
				((std::queue<int>*)t)->push(root2->getPackets().at(0)->getIntValue());
			else if(className=="short")
				((std::queue<short>*)t)->push(root2->getPackets().at(0)->getShortValue());
			else if(className=="long")
				((std::queue<long>*)t)->push(root2->getPackets().at(0)->getNumericValue());
			else if(className=="float")
				((std::queue<float>*)t)->push(root2->getPackets().at(0)->getFloatValue());
			else if(className=="double")
				((std::queue<double>*)t)->push(root2->getPackets().at(0)->getDoubleValue());
			else if(className=="bool")
				((std::queue<bool>*)t)->push(root2->getPackets().at(0)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializevec(AMEFObject* root, string objXml, string appName)
{
	AMEFDecoder dec;
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(7,stlclassName.find(">")-7);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new vector<string>();
	else if(className=="int")
		t = new vector<int>();
	else if(className=="short")
		t = new vector<short>();
	else if(className=="long")
		t = new vector<long>();
	else if(className=="float")
		t = new vector<float>();
	else if(className=="double")
		t = new vector<double>();
	else if(className=="bool")
		t = new vector<bool>();
	else
	{
		return _unserVec(objXml,className,appName);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
			if(className=="std::string" || className=="string")
				((vector<string>*)t)->push_back(root2->getPackets().at(0)->getValueStr());
			else if(className=="int")
				((vector<int>*)t)->push_back(root2->getPackets().at(0)->getIntValue());
			else if(className=="short")
				((vector<short>*)t)->push_back(root2->getPackets().at(0)->getShortValue());
			else if(className=="long")
				((vector<long>*)t)->push_back(root2->getPackets().at(0)->getNumericValue());
			else if(className=="float")
				((vector<float>*)t)->push_back(root2->getPackets().at(0)->getFloatValue());
			else if(className=="double")
				((vector<double>*)t)->push_back(root2->getPackets().at(0)->getDoubleValue());
			else if(className=="bool")
				((vector<bool>*)t)->push_back(root2->getPackets().at(0)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializedq(AMEFObject* root, string objXml, string appName)
{
	AMEFDecoder dec;
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(6,stlclassName.find(">")-6);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new deque<string>();
	else if(className=="int")
		t = new deque<int>();
	else if(className=="short")
		t = new deque<short>();
	else if(className=="long")
		t = new deque<long>();
	else if(className=="float")
		t = new deque<float>();
	else if(className=="double")
		t = new deque<double>();
	else if(className=="bool")
		t = new deque<bool>();
	else
	{
		return _unserDq(objXml,className,appName);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
			if(className=="std::string" || className=="string")
				((deque<string>*)t)->push_back(root2->getPackets().at(0)->getValueStr());
			else if(className=="int")
				((deque<int>*)t)->push_back(root2->getPackets().at(0)->getIntValue());
			else if(className=="short")
				((deque<short>*)t)->push_back(root2->getPackets().at(0)->getShortValue());
			else if(className=="long")
				((deque<long>*)t)->push_back(root2->getPackets().at(0)->getNumericValue());
			else if(className=="float")
				((deque<float>*)t)->push_back(root2->getPackets().at(0)->getFloatValue());
			else if(className=="double")
				((deque<double>*)t)->push_back(root2->getPackets().at(0)->getDoubleValue());
			else if(className=="bool")
				((deque<bool>*)t)->push_back(root2->getPackets().at(0)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializemultiset(AMEFObject* root, string objXml, string appName)
{
	AMEFDecoder dec;
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(9,stlclassName.find(">")-9);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new multiset<string>();
	else if(className=="int")
		t = new multiset<int>();
	else if(className=="short")
		t = new multiset<short>();
	else if(className=="long")
		t = new multiset<long>();
	else if(className=="float")
		t = new multiset<float>();
	else if(className=="double")
		t = new multiset<double>();
	else if(className=="bool")
		t = new multiset<bool>();
	else
	{
		return _unserMulSet(objXml,className,appName);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
			if(className=="std::string" || className=="string")
				((multiset<string>*)t)->insert(root2->getPackets().at(0)->getValueStr());
			else if(className=="int")
				((multiset<int>*)t)->insert(root2->getPackets().at(0)->getIntValue());
			else if(className=="short")
				((multiset<short>*)t)->insert(root2->getPackets().at(0)->getShortValue());
			else if(className=="long")
				((multiset<long>*)t)->insert(root2->getPackets().at(0)->getNumericValue());
			else if(className=="float")
				((multiset<float>*)t)->insert(root2->getPackets().at(0)->getFloatValue());
			else if(className=="double")
				((multiset<double>*)t)->insert(root2->getPackets().at(0)->getDoubleValue());
			else if(className=="bool")
				((multiset<bool>*)t)->insert(root2->getPackets().at(0)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::_unserSet(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className+"Set";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserMulSet(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className+"MulSet";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserQ(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className+"Q";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserDq(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className+"Dq";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserLis(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className+"Lis";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserVec(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className+"Vec";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}


void* Serialize::_unser(string objXml,string className, string appName)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = appName + "binaryUnSerialize"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}
