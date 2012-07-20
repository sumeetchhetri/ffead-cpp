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
	printf("\n---------Demanged --%s\n\n", demangled);
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
	if(className=="std::string" || className=="string")
	{
		objXml = serialize(*(string*)t);
	}
	else if(className=="int")
	{
		objXml = serialize(*(int*)t);
	}
	else if(className=="float")
	{
		objXml= serialize(*(float*)t);
	}
	else if(className=="double")
	{
		objXml = serialize(*(double*)t);
	}
	else if(className=="bool")
	{
		objXml = serialize(*(bool*)t);
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
	else if(className.find("std::vector<std::string,")!=string::npos)
	{
		vector<string> *tt = (vector<string>*)t;
		objXml = serializevecstring(tt);
	}
	else if(className.find("std::vector<int,")!=string::npos)
	{
		vector<int> *tt = (vector<int>*)t;
		objXml = serializevecint(tt);
	}
	else if(className.find("std::vector<double,")!=string::npos)
	{
		vector<double> *tt = (vector<double>*)t;
		objXml = serializevecdouble(tt);
	}
	else if(className.find("std::vector<float,")!=string::npos)
	{
		vector<float> *tt = (vector<float>*)t;
		objXml = serializevecfloat(tt);
	}
	else if(className.find("std::vector<")!=string::npos)
	{
		boost::replace_first(className,"std::vector<","");
		string vtyp = className.substr(0,className.find(","));
		return _servec(t,vtyp);
	}
	else if(className.find("std::list<std::string,")!=string::npos)
	{
		list<string> *tt = (list<string>*)t;
		objXml = serializeliststring(tt);
	}
	else if(className.find("std::list<int,")!=string::npos)
	{
		list<int> *tt = (list<int>*)t;
		objXml = serializelistint(tt);
	}
	else if(className.find("std::list<double,")!=string::npos)
	{
		list<double> *tt = (list<double>*)t;
		objXml = serializelistdouble(tt);
	}
	else if(className.find("std::list<float,")!=string::npos)
	{
		list<float> *tt = (list<float>*)t;
		objXml = serializelistfloat(tt);
	}
	else if(className.find("std::list<")!=string::npos)
	{
		boost::replace_first(className,"std::list<","");
		string vtyp = className.substr(0,className.find(","));
		return _serlis(t,vtyp);
	}
	else if(className.find("std::set<std::string,")!=string::npos)
	{
		set<string> *tt = (set<string>*)t;
		objXml = serializesetstring(tt);
	}
	else if(className.find("std::set<int,")!=string::npos)
	{
		set<int> *tt = (set<int>*)t;
		objXml = serializesetint(tt);
	}
	else if(className.find("std::set<double,")!=string::npos)
	{
		set<double> *tt = (set<double>*)t;
		objXml = serializesetdouble(tt);
	}
	else if(className.find("std::set<float,")!=string::npos)
	{
		set<float> *tt = (set<float>*)&t;
		objXml = serializesetfloat(tt);
	}
	else if(className.find("std::set<")!=string::npos)
	{
		boost::replace_first(className,"std::set<","");
		string vtyp = className.substr(0,className.find(","));
		return _serset(t,vtyp);
	}
	else if(className.find("std::multiset<std::string,")!=string::npos)
	{
		multiset<string> *tt = (multiset<string>*)t;
		objXml = serializemultisetstring(tt);
	}
	else if(className.find("std::multiset<int,")!=string::npos)
	{
		multiset<int> *tt = (multiset<int>*)t;
		objXml = serializemultisetint(tt);
	}
	else if(className.find("std::set<double,")!=string::npos)
	{
		multiset<double> *tt = (multiset<double>*)t;
		objXml = serializemultisetdouble(tt);
	}
	else if(className.find("std::set<float,")!=string::npos)
	{
		multiset<float> *tt = (multiset<float>*)t;
		objXml = serializemultisetfloat(tt);
	}
	else if(className.find("std::multiset<")!=string::npos)
	{
		boost::replace_first(className,"std::multiset<","");
		string vtyp = className.substr(0,className.find(","));
		return _sermultiset(t,vtyp);
	}
	else if(className.find("std::std::queue<std::string,")!=string::npos)
	{
		std::queue<string> *tt = (std::queue<string>*)t;
		objXml = serializeqstring(tt);
	}
	else if(className.find("std::std::queue<int,")!=string::npos)
	{
		std::queue<int> *tt = (std::queue<int>*)t;
		objXml = serializeqint(tt);
	}
	else if(className.find("std::std::queue<double,")!=string::npos)
	{
		std::queue<double> *tt = (std::queue<double>*)t;
		objXml = serializeqdouble(tt);
	}
	else if(className.find("std::std::queue<float,")!=string::npos)
	{
		std::queue<float> *tt = (std::queue<float>*)t;
		objXml = serializeqfloat(tt);
	}
	else if(className.find("std::std::queue<")!=string::npos)
	{
		boost::replace_first(className,"std::std::queue<","");
		string vtyp = className.substr(0,className.find(","));
		return _serq(t,vtyp);
	}
	else if(className.find("std::deque<std::string,")!=string::npos)
	{
		deque<string> *tt = (deque<string>*)t;
		objXml = serializedqstring(tt);
	}
	else if(className.find("std::deque<int,")!=string::npos)
	{
		deque<int> *tt = (deque<int>*)t;
		objXml = serializedqint(tt);
	}
	else if(className.find("std::deque<double,")!=string::npos)
	{
		deque<double> *tt = (deque<double>*)t;
		objXml = serializedqdouble(tt);
	}
	else if(className.find("std::deque<float,")!=string::npos)
	{
		deque<float> *tt = (deque<float>*)t;
		objXml = serializedqfloat(tt);
	}
	else if(className.find("std::deque<")!=string::npos)
	{
		boost::replace_first(className,"std::deque<","");
		string vtyp = className.substr(0,className.find(","));
		return _serdq(t,vtyp);
	}
	else
	{
		return _ser(t,className);
	}
	return objXml;
}



string Serialize::serialize(string t)
{
	return "<string>"+boost::lexical_cast<string>(t)+"</string>";
}

string Serialize::serialize(int t)
{
	return "<int>"+boost::lexical_cast<string>(t)+"</int>";
}

string Serialize::serialize(float t)
{
	return "<float>"+boost::lexical_cast<string>(t)+"</float>";
}

string Serialize::serialize(double t)
{
	return "<double>"+boost::lexical_cast<string>(t)+"</double>";
}

string Serialize::serialize(bool t)
{
	return "<bool>"+boost::lexical_cast<string>(t)+"</bool>";
}

string Serialize::serializesetstring(set<string> *t)
{
	set<string>::iterator it;
	string objXml = "<set-string>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</set-string>";
	return objXml;
}
string Serialize::serializesetint(set<int> *t)
{
	set<int>::iterator it;
	string objXml = "<set-int>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</set-int>";
	return objXml;
}
string Serialize::serializesetfloat(set<float> *t)
{
	set<float>::iterator it;
	string objXml = "<set-float>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</set-float>";
	return objXml;
}
string Serialize::serializesetdouble(set<double> *t)
{
	set<double>::iterator it;
	string objXml = "<set-double>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</set-double>";
	return objXml;
}

string Serialize::serializemultisetstring(multiset<string> *t)
{
	multiset<string>::iterator it;
	string objXml = "<multiset-string>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</multiset-string>";
	return objXml;
}
string Serialize::serializemultisetint(multiset<int> *t)
{
	multiset<int>::iterator it;
	string objXml = "<multiset-int>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</multiset-int>";
	return objXml;
}
string Serialize::serializemultisetfloat(multiset<float> *t)
{
	multiset<float>::iterator it;
	string objXml = "<multiset-float>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</multiset-float>";
	return objXml;
}
string Serialize::serializemultisetdouble(multiset<double> *t)
{
	multiset<double>::iterator it;
	string objXml = "<multiset-double>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</multiset-double>";
	return objXml;
}


string Serialize::serializeliststring(list<string> *t)
{
	list<string>::iterator it;
	string objXml = "<list-string>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</list-string>";
	return objXml;
}
string Serialize::serializelistint(list<int> *t)
{
	list<int>::iterator it;
	string objXml = "<list-int>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</list-int>";
	return objXml;
}
string Serialize::serializelistfloat(list<float> *t)
{
	list<float>::iterator it;
	string objXml = "<list-float>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</list-float>";
	return objXml;
}
string Serialize::serializelistdouble(list<double> *t)
{
	list<double>::iterator it;
	string objXml = "<list-double>";
	for(it=t->begin();it!=t->end();++it)
	{
		objXml += serialize(*it);
	}
	objXml += "</list-double>";
	return objXml;
}

string Serialize::serializeqstring(std::queue<string> *t)
{
	std::queue<string> *tt = new std::queue<string>;
	*tt = *t;
	string objXml = "<std::queue-string>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(tt->front());
			tt->pop();
		}
	}
	objXml += "</std::queue-string>";
	return objXml;

}
string Serialize::serializeqint(std::queue<int> *t)
{
	std::queue<int> *tt = new std::queue<int>;
	*tt = *t;
	string objXml = "<std::queue-int>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(tt->front());
			tt->pop();
		}
	}
	objXml += "</std::queue-int>";
	return objXml;

}
string Serialize::serializeqfloat(std::queue<float> *t)
{
	std::queue<float> *tt = new std::queue<float>;
	*tt = *t;
	string objXml = "<std::queue-float>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(tt->front());
			tt->pop();
		}
	}
	objXml += "</std::queue-float>";
	return objXml;

}
string Serialize::serializeqdouble(std::queue<double> *t)
{
	std::queue<double> *tt = new std::queue<double>;
	*tt = *t;
	string objXml = "<std::queue-double>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(tt->front());
			tt->pop();
		}
	}
	objXml += "</std::queue-double>";
	return objXml;

}

string Serialize::serializedqstring(deque<string> *t)
{

	string objXml = "<deque-string>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</deque-string>";
	return objXml;

}
string Serialize::serializedqint(deque<int> *t)
{

	string objXml = "<deque-int>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</deque-int>";
	return objXml;

}
string Serialize::serializedqfloat(deque<float> *t)
{

	string objXml = "<deque-float>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</deque-float>";
	return objXml;

}
string Serialize::serializedqdouble(deque<double> *t)
{

	string objXml = "<deque-double>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</deque-double>";
	return objXml;

}

string Serialize::serializevecstring(vector<string> *t)
{
	string objXml = "<vector-string>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</vector-string>";
	return objXml;
}

string Serialize::serializevecint(vector<int> *t)
{
	string objXml = "<vector-int>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</vector-int>";
	return objXml;
}

string Serialize::serializevecfloat(vector<float> *t)
{
	string objXml = "<vector-float>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</vector-float>";
	return objXml;
}

string Serialize::serializevecdouble(vector<double> *t)
{
	string objXml = "<vector-double>";
	if(!t->empty())
	{
		for(int var=0;var<(int)t->size();var++)
		{
			objXml += serialize(t->at(var));
		}
	}
	objXml += "</vector-double>";
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
	string methodname = "serialize"+className+"Vec";
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
	string methodname = "serialize"+className+"Lis";
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
	string methodname = "serialize"+className+"Set";
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
	string methodname = "serialize"+className+"MulSet";
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
	string methodname = "serialize"+className+"Q";
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
	string methodname = "serialize"+className+"Dq";
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
	string methodname = "serialize"+className;
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
	if(className=="std::string" || className=="string" || className=="int" || className=="float" || className=="double")
	{
		XmlParser parser("Parser");
		Document doc = parser.getDocument(objXml);
		Element message = doc.getRootElement();
		if(className=="int")
		{
			int *vt = new int;
			*vt = boost::lexical_cast<int>(message.getText());
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
			*vt = boost::lexical_cast<float>(message.getText());
			return vt;
		}
		else if(className=="double")
		{
			double *vt = new double;
			*vt = boost::lexical_cast<double>(message.getText());
			return vt;
		}
		else if(className=="bool")
		{
			bool *vt = new bool;
			*vt = boost::lexical_cast<bool>(message.getText());
			return vt;
		}
		else if(className=="std::string" || className=="string")
		{
			string *vt = new string;
			*vt = boost::lexical_cast<string>(message.getText());
			return vt;
		}
	}
	else if(objXml.find("<vector-")==0)
	{
		string cls = objXml.substr(8,objXml.find(">")-8);
		if(cls=="string")
		{
			vector<string> *t = new vector<string>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="string")
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
					t->push_back(boost::lexical_cast<double>(ele.getText()));
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
					t->push_back(boost::lexical_cast<float>(ele.getText()));
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
					t->push_back(boost::lexical_cast<int>(ele.getText()));
				}
			}
			return t;
		}
		else
			return _unserVec(objXml,cls);
	}
	else if(objXml.find("<set-")==0)
	{
		string cls = objXml.substr(8,objXml.find(">")-8);
		if(cls=="string")
		{
			set<string> *t = new set<string>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="string")
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
					t->insert(boost::lexical_cast<double>(ele.getText()));
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
					t->insert(boost::lexical_cast<float>(ele.getText()));
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
					t->insert(boost::lexical_cast<int>(ele.getText()));
				}
			}
			return t;
		}
		else
			return _unserSet(objXml,cls);
	}
	else if(objXml.find("<multiset-")==0)
	{
		string cls = objXml.substr(8,objXml.find(">")-8);
		if(cls=="string")
		{
			multiset<string> *t = new multiset<string>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="string")
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
					t->insert(boost::lexical_cast<double>(ele.getText()));
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
					t->insert(boost::lexical_cast<float>(ele.getText()));
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
					t->insert(boost::lexical_cast<int>(ele.getText()));
				}
			}
			return t;
		}
		else
			return _unserMulSet(objXml,cls);
	}
	else if(objXml.find("<list-")==0)
	{
		string cls = objXml.substr(8,objXml.find(">")-8);
		if(cls=="string")
		{
			list<string> *t = new list<string>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="string")
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
					t->push_back(boost::lexical_cast<double>(ele.getText()));
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
					t->push_back(boost::lexical_cast<float>(ele.getText()));
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
					t->push_back(boost::lexical_cast<int>(ele.getText()));
				}
			}
			return t;
		}
		else
			return _unserLis(objXml,cls);
	}
	else if(objXml.find("<std::queue-")==0)
	{
		string cls = objXml.substr(8,objXml.find(">")-8);
		if(cls=="string")
		{
			std::queue<string> *t = new std::queue<string>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="string")
				{
					t->push(ele.getText());
				}
			}
			return t;
		}
		else if(cls=="double")
		{
			std::queue<double> *t = new std::queue<double>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="double")
				{
					t->push(boost::lexical_cast<double>(ele.getText()));
				}
			}
			return t;
		}
		else if(cls=="float")
		{
			std::queue<float> *t = new std::queue<float>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="float")
				{
					t->push(boost::lexical_cast<float>(ele.getText()));
				}
			}
			return t;
		}
		else if(cls=="int")
		{
			std::queue<int> *t = new std::queue<int>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="int")
				{
					t->push(boost::lexical_cast<int>(ele.getText()));
				}
			}
			return t;
		}
		else
			return _unserQ(objXml,cls);
	}
	else if(objXml.find("<deque-")==0)
	{
		string cls = objXml.substr(8,objXml.find(">")-8);
		if(cls=="string")
		{
			deque<string> *t = new deque<string>;
			XmlParser parser("Parser");
			Document doc = parser.getDocument(objXml);
			Element message = doc.getRootElement();
			for (int var = 0; var < (int)message.getChildElements().size(); var++)
			{
				Element ele = message.getChildElements().at(var);
				if(ele.getTagName()=="string")
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
					t->push_back(boost::lexical_cast<double>(ele.getText()));
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
					t->push_back(boost::lexical_cast<float>(ele.getText()));
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
					t->push_back(boost::lexical_cast<int>(ele.getText()));
				}
			}
			return t;
		}
		else
			return _unserDq(objXml,cls);
	}
	return _unser(objXml,className);
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
