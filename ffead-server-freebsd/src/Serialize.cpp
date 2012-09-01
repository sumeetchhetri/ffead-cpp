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
	//printf("\n---------Demanged --%s\n\n", demangled);
	string s(demangled);
	return s;
}
string Serialize::getClassName(void* instance)
{
	const char *mangled = typeid(instance).name();
	return demangle(mangled);
}

string Serialize::_hanldeAllSerialization(string className,void *t)
{
	string objXml;
	AMEFEncoder enc;
	AMEFObject object;

	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		object.setName(className);
		object.addPacket(tem);
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
	else if(className.find("std::vector<std::string,")!=string::npos)
	{
		vector<string> *tt = (vector<string>*)t;
		objXml = serializevec<string>(*tt);
	}
	else if(className.find("std::vector<int,")!=string::npos)
	{
		vector<int> *tt = (vector<int>*)t;
		objXml = serializevec<int>(*tt);
	}
	else if(className.find("std::vector<double,")!=string::npos)
	{
		vector<double> *tt = (vector<double>*)t;
		objXml = serializevec<double>(*tt);
	}
	else if(className.find("std::vector<float,")!=string::npos)
	{
		vector<float> *tt = (vector<float>*)t;
		objXml = serializevec<float>(*tt);
	}
	else if(className.find("std::vector<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		string vtyp = className.substr(0,className.find(","));
		return _servec(t,vtyp);
	}
	else if(className.find("std::list<std::string,")!=string::npos)
	{
		list<string> *tt = (list<string>*)t;
		objXml = serializelist<string>(*tt);
	}
	else if(className.find("std::list<int,")!=string::npos)
	{
		list<int> *tt = (list<int>*)t;
		objXml = serializelist<int>(*tt);
	}
	else if(className.find("std::list<double,")!=string::npos)
	{
		list<double> *tt = (list<double>*)t;
		objXml = serializelist<double>(*tt);
	}
	else if(className.find("std::list<float,")!=string::npos)
	{
		list<float> *tt = (list<float>*)t;
		objXml = serializelist<float>(*tt);
	}
	else if(className.find("std::list<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::list<","");
		string vtyp = className.substr(0,className.find(","));
		return _serlis(t,vtyp);
	}
	else if(className.find("std::set<std::string,")!=string::npos)
	{
		set<string> *tt = (set<string>*)t;
		objXml = serializeset<string>(*tt);
	}
	else if(className.find("std::set<int,")!=string::npos)
	{
		set<int> *tt = (set<int>*)t;
		objXml = serializeset<int>(*tt);
	}
	else if(className.find("std::set<long,")!=string::npos)
	{
		set<long> *tt = (set<long>*)t;
		objXml = serializeset<long>(*tt);
	}
	else if(className.find("std::set<double,")!=string::npos)
	{
		set<double> *tt = (set<double>*)t;
		objXml = serializeset<double>(*tt);
	}
	else if(className.find("std::set<float,")!=string::npos)
	{
		set<float> *tt = (set<float>*)&t;
		objXml = serializeset<float>(*tt);
	}
	else if(className.find("std::set<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::set<","");
		string vtyp = className.substr(0,className.find(","));
		return _serset(t,vtyp);
	}
	else if(className.find("std::multiset<std::string,")!=string::npos)
	{
		multiset<string> *tt = (multiset<string>*)t;
		objXml = serializemultiset<string>(*tt);
	}
	else if(className.find("std::multiset<int,")!=string::npos)
	{
		multiset<int> *tt = (multiset<int>*)t;
		objXml = serializemultiset<int>(*tt);
	}
	else if(className.find("std::set<double,")!=string::npos)
	{
		multiset<double> *tt = (multiset<double>*)t;
		objXml = serializemultiset<double>(*tt);
	}
	else if(className.find("std::set<float,")!=string::npos)
	{
		multiset<float> *tt = (multiset<float>*)t;
		objXml = serializemultiset<float>(*tt);
	}
	else if(className.find("std::multiset<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::multiset<","");
		string vtyp = className.substr(0,className.find(","));
		return _sermultiset(t,vtyp);
	}
	else if(className.find("std::queue<std::string,")!=string::npos)
	{
		std::queue<string> *tt = (std::queue<string>*)t;
		objXml = serializeq<string>(*tt);
	}
	else if(className.find("std::queue<int,")!=string::npos)
	{
		std::queue<int> *tt = (std::queue<int>*)t;
		objXml = serializeq<int>(*tt);
	}
	else if(className.find("std::queue<double,")!=string::npos)
	{
		std::queue<double> *tt = (std::queue<double>*)t;
		objXml = serializeq<double>(*tt);
	}
	else if(className.find("std::queue<float,")!=string::npos)
	{
		std::queue<float> *tt = (std::queue<float>*)t;
		objXml = serializeq<float>(*tt);
	}
	else if(className.find("std::queue<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::queue<","");
		string vtyp = className.substr(0,className.find(","));
		return _serq(t,vtyp);
	}
	else if(className.find("std::deque<std::string,")!=string::npos)
	{
		deque<string> *tt = (deque<string>*)t;
		objXml = serializedq<string>(*tt);
	}
	else if(className.find("std::deque<int,")!=string::npos)
	{
		deque<int> *tt = (deque<int>*)t;
		objXml = serializedq<int>(*tt);
	}
	else if(className.find("std::deque<double,")!=string::npos)
	{
		deque<double> *tt = (deque<double>*)t;
		objXml = serializedq<double>(*tt);
	}
	else if(className.find("std::deque<float,")!=string::npos)
	{
		deque<float> *tt = (deque<float>*)t;
		objXml = serializedq<float>(*tt);
	}
	else if(className.find("std::deque<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::deque<","");
		string vtyp = className.substr(0,className.find(","));
		return _serdq(t,vtyp);
	}
	else
	{
		return _ser(t,className);
	}
	return objXml;
}

string Serialize::_servec(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className+"Vec";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string Serialize::_serlis(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className+"Lis";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_serset(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className+"Set";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_sermultiset(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className+"MulSet";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_serq(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className+"Q";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string Serialize::_serdq(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className+"Dq";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string Serialize::_ser(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binarySerialize"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string Serialize::_ser(Object t)
{
	return _ser(t.getVoidPointer(),t.getTypeName());
}


void* Serialize::_hanldeAllUnSerialization(string objXml,string className)
{
	AMEFDecoder dec;
	AMEFObject* root = dec.decodeB(objXml, true, false);
	if(root==NULL || root->getNameStr()=="")
		return NULL;
	if(root->getNameStr()==className &&
			(className=="std::string" || className=="string" || className=="int" || className=="long" || className=="float" || className=="double"))
	{
		if(className=="int")
		{
			int *vt = new int;
			*vt = root->getIntValue();
			return vt;
		}
		else if(className=="Date")
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(root->getValueStr());
		}
		else if(className=="BinaryData")
		{
			return BinaryData::unSerilaize(root->getValueStr());
		}
		else if(className=="float")
		{
			float *vt = new float;
			*vt = root->getFloatValue();
			return vt;
		}
		else if(className=="double")
		{
			double *vt = new double;
			*vt = root->getDoubleValue();
			return vt;
		}
		else if(className=="bool")
		{
			bool *vt = new bool;
			*vt = root->getBoolValue();
			return vt;
		}
		else if(className=="std::string" || className=="string")
		{
			string *vt = new string;
			*vt = root->getValueStr();
			return vt;
		}
	}
	if(root->getNameStr().find("vector<")==0)
	{
		return unserializevec(root, objXml);
	}
	else if(root->getNameStr().find("set<")==0)
	{
		return unserializeset(root, objXml);
	}
	else if(root->getNameStr().find("multiset<")==0)
	{
		return unserializemultiset(root, objXml);
	}
	else if(root->getNameStr().find("list<")==0)
	{
		return unserializelist(root, objXml);
	}
	else if(root->getNameStr().find("std::queue<")==0)
	{
		return unserializeq(root, objXml);
	}
	else if(root->getNameStr().find("deque<")==0)
	{
		return unserializedq(root, objXml);
	}
	return _unser(objXml,className);
}

void* Serialize::unserializeset(AMEFObject* root, string objXml)
{
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(4,stlclassName.find(">")-4);
	void* t = NULL;
	if(className=="std::string")
		t = new set<string>();
	else if(className=="int")
		t = new set<int>();
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
		return _unserSet(objXml,className);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			if(className=="std::string")
				((set<string>*)t)->insert(root->getPackets().at(var)->getValueStr());
			else if(className=="int")
				((set<int>*)t)->insert(root->getPackets().at(var)->getIntValue());
			else if(className=="long")
				((set<long>*)t)->insert(root->getPackets().at(var)->getLongValue());
			else if(className=="float")
				((set<float>*)t)->insert(root->getPackets().at(var)->getFloatValue());
			else if(className=="double")
				((set<double>*)t)->insert(root->getPackets().at(var)->getDoubleValue());
			else if(className=="bool")
				((set<bool>*)t)->insert(root->getPackets().at(var)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializelist(AMEFObject* root, string objXml)
{
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(5,stlclassName.find(">")-5);
	void* t = NULL;
	if(className=="std::string")
		t = new list<string>();
	else if(className=="int")
		t = new list<int>();
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
		return _unserLis(objXml,className);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			if(className=="std::string")
				((list<string>*)t)->push_back(root->getPackets().at(var)->getValueStr());
			else if(className=="int")
				((list<int>*)t)->push_back(root->getPackets().at(var)->getIntValue());
			else if(className=="long")
				((list<long>*)t)->push_back(root->getPackets().at(var)->getLongValue());
			else if(className=="float")
				((list<float>*)t)->push_back(root->getPackets().at(var)->getFloatValue());
			else if(className=="double")
				((list<double>*)t)->push_back(root->getPackets().at(var)->getDoubleValue());
			else if(className=="bool")
				((list<bool>*)t)->push_back(root->getPackets().at(var)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializeq(AMEFObject* root, string objXml)
{
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(6,stlclassName.find(">")-6);
	void* t = NULL;
	if(className=="std::string")
		t = new std::queue<string>();
	else if(className=="int")
		t = new std::queue<int>();
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
		return _unserQ(objXml,className);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			if(className=="std::string")
				((std::queue<string>*)t)->push(root->getPackets().at(var)->getValueStr());
			else if(className=="int")
				((std::queue<int>*)t)->push(root->getPackets().at(var)->getIntValue());
			else if(className=="long")
				((std::queue<long>*)t)->push(root->getPackets().at(var)->getLongValue());
			else if(className=="float")
				((std::queue<float>*)t)->push(root->getPackets().at(var)->getFloatValue());
			else if(className=="double")
				((std::queue<double>*)t)->push(root->getPackets().at(var)->getDoubleValue());
			else if(className=="bool")
				((std::queue<bool>*)t)->push(root->getPackets().at(var)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializevec(AMEFObject* root, string objXml)
{
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(7,stlclassName.find(">")-7);
	void* t = NULL;
	if(className=="std::string")
		t = new vector<string>();
	else if(className=="int")
		t = new vector<int>();
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
		return _unserVec(objXml,className);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			if(className=="std::string")
				((vector<string>*)t)->push_back(root->getPackets().at(var)->getValueStr());
			else if(className=="int")
				((vector<int>*)t)->push_back(root->getPackets().at(var)->getIntValue());
			else if(className=="long")
				((vector<long>*)t)->push_back(root->getPackets().at(var)->getLongValue());
			else if(className=="float")
				((vector<float>*)t)->push_back(root->getPackets().at(var)->getFloatValue());
			else if(className=="double")
				((vector<double>*)t)->push_back(root->getPackets().at(var)->getDoubleValue());
			else if(className=="bool")
				((vector<bool>*)t)->push_back(root->getPackets().at(var)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializedq(AMEFObject* root, string objXml)
{
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(6,stlclassName.find(">")-6);
	void* t = NULL;
	if(className=="std::string")
		t = new deque<string>();
	else if(className=="int")
		t = new deque<int>();
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
		return _unserDq(objXml,className);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			if(className=="std::string")
				((deque<string>*)t)->push_back(root->getPackets().at(var)->getValueStr());
			else if(className=="int")
				((deque<int>*)t)->push_back(root->getPackets().at(var)->getIntValue());
			else if(className=="long")
				((deque<long>*)t)->push_back(root->getPackets().at(var)->getLongValue());
			else if(className=="float")
				((deque<float>*)t)->push_back(root->getPackets().at(var)->getFloatValue());
			else if(className=="double")
				((deque<double>*)t)->push_back(root->getPackets().at(var)->getDoubleValue());
			else if(className=="bool")
				((deque<bool>*)t)->push_back(root->getPackets().at(var)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializemultiset(AMEFObject* root, string objXml)
{
	string stlclassName = root->getNameStr();
	string className = stlclassName.substr(9,stlclassName.find(">")-9);
	void* t = NULL;
	if(className=="std::string")
		t = new multiset<string>();
	else if(className=="int")
		t = new multiset<int>();
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
		return _unserMulSet(objXml,className);
	}
	if(t!=NULL)
	{
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			if(className=="std::string")
				((multiset<string>*)t)->insert(root->getPackets().at(var)->getValueStr());
			else if(className=="int")
				((multiset<int>*)t)->insert(root->getPackets().at(var)->getIntValue());
			else if(className=="long")
				((multiset<long>*)t)->insert(root->getPackets().at(var)->getLongValue());
			else if(className=="float")
				((multiset<float>*)t)->insert(root->getPackets().at(var)->getFloatValue());
			else if(className=="double")
				((multiset<double>*)t)->insert(root->getPackets().at(var)->getDoubleValue());
			else if(className=="bool")
				((multiset<bool>*)t)->insert(root->getPackets().at(var)->getBoolValue());
		}
		return t;
	}
	return NULL;
}

void* Serialize::_unserSet(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className+"Set";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserMulSet(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className+"MulSet";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserQ(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className+"Q";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserDq(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className+"Dq";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserLis(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className+"Lis";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* Serialize::_unserVec(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className+"Vec";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}


void* Serialize::_unser(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "binaryUnSerialize"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}
