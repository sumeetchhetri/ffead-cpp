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

#include "XMLSerialize.h"

string XMLSerialize::demangle(const char *mangled)
{
	int status;	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	//printf("\n---------Demanged --%s\n\n", demangled);
	string s(demangled);
	return s;
}
string XMLSerialize::getClassName(void* instance)
{
	const char *mangled = typeid(instance).name();
	return demangle(mangled);
}

string XMLSerialize::_handleAllSerialization(string className,void *t)
{
	string objXml;
	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		objXml = "<string>"+CastUtil::lexical_cast<string>(tem)+"</string>";
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		objXml = "<int>"+CastUtil::lexical_cast<string>(tem)+"</int>";
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		objXml = "<long>"+CastUtil::lexical_cast<string>(tem)+"</long>";
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		objXml = "<short>"+CastUtil::lexical_cast<string>(tem)+"</short>";
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		objXml = "<float>"+CastUtil::lexical_cast<string>(tem)+"</float>";
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		objXml = "<double>"+CastUtil::lexical_cast<string>(tem)+"</double>";
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		objXml = "<bool>"+CastUtil::lexical_cast<string>(tem)+"</bool>";
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		objXml = "<Date>"+formt.format(*(Date*)t)+"</Date>";
	}
	else if(className=="BinaryData")
	{
		objXml = BinaryData::serilaize(*(BinaryData*)t);
	}
	else if(className.find("std::vector<std::string,")!=string::npos || className.find("std::vector<string,")!=string::npos)
	{
		vector<string> *tt = (vector<string>*)t;
		objXml = serializevec<string>(*tt);
	}
	else if(className.find("std::vector<int,")!=string::npos)
	{
		vector<int> *tt = (vector<int>*)t;
		objXml = serializevec<int>(*tt);
	}
	else if(className.find("std::vector<short,")!=string::npos)
	{
		vector<short> *tt = (vector<short>*)t;
		objXml = serializevec<short>(*tt);
	}
	else if(className.find("std::vector<long,")!=string::npos)
	{
		vector<long> *tt = (vector<long>*)t;
		objXml = serializevec<long>(*tt);
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
	else if(className.find("std::vector<bool,")!=string::npos)
	{
		vector<bool> *tt = (vector<bool>*)t;
		objXml = serializevec<bool>(*tt);
	}
	else if(className.find("std::vector<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		string vtyp = className.substr(0,className.find(","));
		return _servec(t,vtyp);
	}
	else if(className.find("std::list<std::string,")!=string::npos || className.find("std::list<string,")!=string::npos)
	{
		list<string> *tt = (list<string>*)t;
		objXml = serializelist<string>(*tt);
	}
	else if(className.find("std::list<int,")!=string::npos)
	{
		list<int> *tt = (list<int>*)t;
		objXml = serializelist<int>(*tt);
	}
	else if(className.find("std::list<long,")!=string::npos)
	{
		list<long> *tt = (list<long>*)t;
		objXml = serializelist<long>(*tt);
	}
	else if(className.find("std::list<short,")!=string::npos)
	{
		list<short> *tt = (list<short>*)t;
		objXml = serializelist<short>(*tt);
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
	else if(className.find("std::list<bool,")!=string::npos)
	{
		list<bool> *tt = (list<bool>*)t;
		objXml = serializelist<bool>(*tt);
	}
	else if(className.find("std::list<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::list<","");
		string vtyp = className.substr(0,className.find(","));
		return _serlis(t,vtyp);
	}
	else if(className.find("std::set<std::string,")!=string::npos || className.find("std::set<string,")!=string::npos)
	{
		set<string> *tt = (set<string>*)t;
		objXml = serializeset<string>(*tt);
	}
	else if(className.find("std::set<int,")!=string::npos)
	{
		set<int> *tt = (set<int>*)t;
		objXml = serializeset<int>(*tt);
	}
	else if(className.find("std::set<short,")!=string::npos)
	{
		set<short> *tt = (set<short>*)t;
		objXml = serializeset<short>(*tt);
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
	else if(className.find("std::set<bool,")!=string::npos)
	{
		set<bool> *tt = (set<bool>*)&t;
		objXml = serializeset<bool>(*tt);
	}
	else if(className.find("std::set<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::set<","");
		string vtyp = className.substr(0,className.find(","));
		return _serset(t,vtyp);
	}
	else if(className.find("std::multiset<std::string,")!=string::npos || className.find("std::multiset<string,")!=string::npos)
	{
		multiset<string> *tt = (multiset<string>*)t;
		objXml = serializemultiset<string>(*tt);
	}
	else if(className.find("std::multiset<int,")!=string::npos)
	{
		multiset<int> *tt = (multiset<int>*)t;
		objXml = serializemultiset<int>(*tt);
	}
	else if(className.find("std::multiset<long,")!=string::npos)
	{
		multiset<long> *tt = (multiset<long>*)t;
		objXml = serializemultiset<long>(*tt);
	}
	else if(className.find("std::multiset<short,")!=string::npos)
	{
		multiset<short> *tt = (multiset<short>*)t;
		objXml = serializemultiset<short>(*tt);
	}
	else if(className.find("std::multiset<double,")!=string::npos)
	{
		multiset<double> *tt = (multiset<double>*)t;
		objXml = serializemultiset<double>(*tt);
	}
	else if(className.find("std::multiset<float,")!=string::npos)
	{
		multiset<float> *tt = (multiset<float>*)t;
		objXml = serializemultiset<float>(*tt);
	}
	else if(className.find("std::multiset<bool,")!=string::npos)
	{
		multiset<bool> *tt = (multiset<bool>*)t;
		objXml = serializemultiset<bool>(*tt);
	}
	else if(className.find("std::multiset<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::multiset<","");
		string vtyp = className.substr(0,className.find(","));
		return _sermultiset(t,vtyp);
	}
	else if(className.find("std::queue<std::string,")!=string::npos || className.find("std::queue<string,")!=string::npos)
	{
		std::queue<string> *tt = (std::queue<string>*)t;
		objXml = serializeq<string>(*tt);
	}
	else if(className.find("std::queue<int,")!=string::npos)
	{
		std::queue<int> *tt = (std::queue<int>*)t;
		objXml = serializeq<int>(*tt);
	}
	else if(className.find("std::queue<short,")!=string::npos)
	{
		std::queue<short> *tt = (std::queue<short>*)t;
		objXml = serializeq<short>(*tt);
	}
	else if(className.find("std::queue<long,")!=string::npos)
	{
		std::queue<long> *tt = (std::queue<long>*)t;
		objXml = serializeq<long>(*tt);
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
	else if(className.find("std::queue<bool,")!=string::npos)
	{
		std::queue<bool> *tt = (std::queue<bool>*)t;
		objXml = serializeq<bool>(*tt);
	}
	else if(className.find("std::queue<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::queue<","");
		string vtyp = className.substr(0,className.find(","));
		return _serq(t,vtyp);
	}
	else if(className.find("std::deque<std::string,")!=string::npos || className.find("std::deque<string,")!=string::npos)
	{
		deque<string> *tt = (deque<string>*)t;
		objXml = serializedq<string>(*tt);
	}
	else if(className.find("std::deque<int,")!=string::npos)
	{
		deque<int> *tt = (deque<int>*)t;
		objXml = serializedq<int>(*tt);
	}
	else if(className.find("std::deque<long,")!=string::npos)
	{
		deque<long> *tt = (deque<long>*)t;
		objXml = serializedq<long>(*tt);
	}
	else if(className.find("std::deque<short,")!=string::npos)
	{
		deque<short> *tt = (deque<short>*)t;
		objXml = serializedq<short>(*tt);
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
	else if(className.find("std::deque<bool,")!=string::npos)
	{
		deque<bool> *tt = (deque<bool>*)t;
		objXml = serializedq<bool>(*tt);
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

string XMLSerialize::_servec(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className+"Vec";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string XMLSerialize::_serlis(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className+"Lis";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string XMLSerialize::_serset(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className+"Set";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string XMLSerialize::_sermultiset(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className+"MulSet";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string XMLSerialize::_serq(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className+"Q";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}
string XMLSerialize::_serdq(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className+"Dq";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string XMLSerialize::_ser(void* t,string className)
{
	string objXml;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "serialize"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		objXml = f(t);
	return objXml;
}

string XMLSerialize::_ser(Object t)
{
	return _ser(t.getVoidPointer(),t.getTypeName());
}


void* XMLSerialize::_handleAllUnSerialization(string objXml,string className)
{
	if(className=="std::string" || className=="string" || className=="int" || className=="short" || className=="float" || className=="double"
			|| className=="long" || className=="bool")
	{
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		if(className=="int")
		{
			int *vt = new int;
			*vt = CastUtil::lexical_cast<int>(message.getText());
			return vt;
		}
		else if(className=="short")
		{
			short *vt = new short;
			*vt = CastUtil::lexical_cast<short>(message.getText());
			return vt;
		}
		else if(className=="long")
		{
			long *vt = new long;
			*vt = CastUtil::lexical_cast<long>(message.getText());
			return vt;
		}
		else if(className=="Date")
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(message.getText());
		}
		else if(className=="BinaryData")
		{
			return BinaryData::unSerilaize(message.getText());
		}
		else if(className=="float")
		{
			float *vt = new float;
			*vt = CastUtil::lexical_cast<float>(message.getText());
			return vt;
		}
		else if(className=="double")
		{
			double *vt = new double;
			*vt = CastUtil::lexical_cast<double>(message.getText());
			return vt;
		}
		else if(className=="bool")
		{
			bool *vt = new bool;
			*vt = CastUtil::lexical_cast<bool>(message.getText());
			return vt;
		}
		else if(className=="std::string" || className=="string")
		{
			string *vt = new string;
			*vt = CastUtil::lexical_cast<string>(message.getText());
			return vt;
		}
	}
	else if(objXml.find("<vector-")==0)
	{
		return unserializevec(objXml);
	}
	else if(objXml.find("<set-")==0)
	{
		return unserializeset(objXml);
	}
	else if(objXml.find("<multiset-")==0)
	{
		return unserializemultiset(objXml);
	}
	else if(objXml.find("<list-")==0)
	{
		return unserializelist(objXml);
	}
	else if(objXml.find("<queue-")==0)
	{
		return unserializeq(objXml);
	}
	else if(objXml.find("<deque-")==0)
	{
		return unserializedq(objXml);
	}
	return _unser(objXml,className);
}

void* XMLSerialize::unserializevec(string objXml)
{
	string cls = objXml.substr(8,objXml.find(">")-8);
	if(cls=="string" || cls=="std::string")
	{
		vector<string> *t = new vector<string>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="string" || ele.getTagName()=="std::string")
			{
				t->push_back(ele.getText());
			}
		}
		return t;
	}
	else if(cls=="double")
	{
		vector<double> *t = new vector<double>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="double")
			{
				t->push_back(CastUtil::lexical_cast<double>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="float")
	{
		vector<float> *t = new vector<float>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="float")
			{
				t->push_back(CastUtil::lexical_cast<float>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="int")
	{
		vector<int> *t = new vector<int>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="int")
			{
				t->push_back(CastUtil::lexical_cast<int>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="short")
	{
		vector<short> *t = new vector<short>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="short")
			{
				t->push_back(CastUtil::lexical_cast<short>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="long")
	{
		vector<long> *t = new vector<long>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="long")
			{
				t->push_back(CastUtil::lexical_cast<long>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="bool")
	{
		vector<bool> *t = new vector<bool>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="bool")
			{
				t->push_back(CastUtil::lexical_cast<bool>(ele.getText()));
			}
		}
		return t;
	}
	else
		return _unserVec(objXml,cls);

}

void* XMLSerialize::unserializelist(string objXml)
{
	string cls = objXml.substr(6,objXml.find(">")-6);
	if(cls=="string" || cls=="std::string")
	{
		list<string> *t = new list<string>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="string" || ele.getTagName()=="std::string")
			{
				t->push_back(ele.getText());
			}
		}
		return t;
	}
	else if(cls=="double")
	{
		list<double> *t = new list<double>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="double")
			{
				t->push_back(CastUtil::lexical_cast<double>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="float")
	{
		list<float> *t = new list<float>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="float")
			{
				t->push_back(CastUtil::lexical_cast<float>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="int")
	{
		list<int> *t = new list<int>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="int")
			{
				t->push_back(CastUtil::lexical_cast<int>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="short")
	{
		list<short> *t = new list<short>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="short")
			{
				t->push_back(CastUtil::lexical_cast<short>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="long")
	{
		list<long> *t = new list<long>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="long")
			{
				t->push_back(CastUtil::lexical_cast<long>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="bool")
	{
		list<bool> *t = new list<bool>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="bool")
			{
				t->push_back(CastUtil::lexical_cast<bool>(ele.getText()));
			}
		}
		return t;
	}
	else
		return _unserLis(objXml,cls);
}

void* XMLSerialize::unserializeset(string objXml)
{
	string cls = objXml.substr(5,objXml.find(">")-5);
	if(cls=="string" || cls=="std::string")
	{
		set<string> *t = new set<string>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="string" || ele.getTagName()=="std::string")
			{
				t->insert(ele.getText());
			}
		}
		return t;
	}
	else if(cls=="double")
	{
		set<double> *t = new set<double>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="double")
			{
				t->insert(CastUtil::lexical_cast<double>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="float")
	{
		set<float> *t = new set<float>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="float")
			{
				t->insert(CastUtil::lexical_cast<float>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="int")
	{
		set<int> *t = new set<int>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="int")
			{
				t->insert(CastUtil::lexical_cast<int>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="short")
	{
		set<short> *t = new set<short>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="short")
			{
				t->insert(CastUtil::lexical_cast<short>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="long")
	{
		set<long> *t = new set<long>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="long")
			{
				t->insert(CastUtil::lexical_cast<long>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="bool")
	{
		set<bool> *t = new set<bool>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="bool")
			{
				t->insert(CastUtil::lexical_cast<bool>(ele.getText()));
			}
		}
		return t;
	}
	else
		return _unserSet(objXml,cls);
}

void* XMLSerialize::unserializemultiset(string objXml)
{
	string cls = objXml.substr(10,objXml.find(">")-10);
	if(cls=="string" || cls=="std::string")
	{
		multiset<string> *t = new multiset<string>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="string" || ele.getTagName()=="std::string")
			{
				t->insert(ele.getText());
			}
		}
		return t;
	}
	else if(cls=="double")
	{
		multiset<double> *t = new multiset<double>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="double")
			{
				t->insert(CastUtil::lexical_cast<double>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="float")
	{
		multiset<float> *t = new multiset<float>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="float")
			{
				t->insert(CastUtil::lexical_cast<float>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="int")
	{
		multiset<int> *t = new multiset<int>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="int")
			{
				t->insert(CastUtil::lexical_cast<int>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="short")
	{
		multiset<short> *t = new multiset<short>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="short")
			{
				t->insert(CastUtil::lexical_cast<short>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="long")
	{
		multiset<long> *t = new multiset<long>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="long")
			{
				t->insert(CastUtil::lexical_cast<long>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="bool")
	{
		multiset<bool> *t = new multiset<bool>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="bool")
			{
				t->insert(CastUtil::lexical_cast<bool>(ele.getText()));
			}
		}
		return t;
	}
	else
		return _unserMulSet(objXml,cls);
}

void* XMLSerialize::unserializeq(string objXml)
{
	string cls = objXml.substr(7,objXml.find(">")-7);
	if(cls=="string" || cls=="std::string")
	{
		queue<string> *t = new queue<string>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="string" || ele.getTagName()=="std::string")
			{
				t->push(ele.getText());
			}
		}
		return t;
	}
	else if(cls=="double")
	{
		queue<double> *t = new queue<double>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="double")
			{
				t->push(CastUtil::lexical_cast<double>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="float")
	{
		queue<float> *t = new queue<float>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="float")
			{
				t->push(CastUtil::lexical_cast<float>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="int")
	{
		queue<int> *t = new queue<int>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="int")
			{
				t->push(CastUtil::lexical_cast<int>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="short")
	{
		queue<short> *t = new queue<short>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="short")
			{
				t->push(CastUtil::lexical_cast<short>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="long")
	{
		std::queue<long> *t = new queue<long>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="long")
			{
				t->push(CastUtil::lexical_cast<long>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="bool")
	{
		std::queue<bool> *t = new queue<bool>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="bool")
			{
				t->push(CastUtil::lexical_cast<bool>(ele.getText()));
			}
		}
		return t;
	}
	else
		return _unserQ(objXml,cls);
}

void* XMLSerialize::unserializedq(string objXml)
{
	string cls = objXml.substr(7,objXml.find(">")-7);
	if(cls=="string" || cls=="std::string")
	{
		deque<string> *t = new deque<string>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="string" || ele.getTagName()=="std::string")
			{
				t->push_back(ele.getText());
			}
		}
		return t;
	}
	else if(cls=="double")
	{
		deque<double> *t = new deque<double>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="double")
			{
				t->push_back(CastUtil::lexical_cast<double>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="float")
	{
		deque<float> *t = new deque<float>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="float")
			{
				t->push_back(CastUtil::lexical_cast<float>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="int")
	{
		deque<int> *t = new deque<int>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="int")
			{
				t->push_back(CastUtil::lexical_cast<int>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="short")
	{
		deque<short> *t = new deque<short>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="short")
			{
				t->push_back(CastUtil::lexical_cast<short>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="long")
	{
		deque<long> *t = new deque<long>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="long")
			{
				t->push_back(CastUtil::lexical_cast<long>(ele.getText()));
			}
		}
		return t;
	}
	else if(cls=="bool")
	{
		deque<bool> *t = new deque<bool>;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		for (int var = 0; var < (int)message.getChildElements().size(); var++)
		{
			Element ele = message.getChildElements().at(var);
			if(ele.getTagName()=="bool")
			{
				t->push_back(CastUtil::lexical_cast<bool>(ele.getText()));
			}
		}
		return t;
	}
	else
		return _unserDq(objXml,cls);
}

void* XMLSerialize::_unserSet(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className+"Set";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* XMLSerialize::_unserMulSet(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className+"MulSet";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* XMLSerialize::_unserQ(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className+"Q";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* XMLSerialize::_unserDq(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className+"Dq";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* XMLSerialize::_unserLis(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className+"Lis";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}

void* XMLSerialize::_unserVec(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className+"Vec";
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}


void* XMLSerialize::_unser(string objXml,string className)
{
	void* obj = NULL;
	string libName = Constants::INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = "unSerialize"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (string);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(objXml);
	}
	return obj;
}
