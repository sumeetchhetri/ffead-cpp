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

/*string Serialize::_handleAllSerialization(string className, void *t, string appName)
{
	string objXml;
	AMEFEncoder enc;
	AMEFObject object;

	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(unsigned char*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		object.addPacket(formt.format(*(Date*)t), className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="BinaryData")
	{
		object.addPacket(BinaryData::serilaize(*(BinaryData*)t), className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className.find("std::vector<std::string,")!=string::npos || className.find("std::vector<string,")!=string::npos)
	{
		vector<string> *tt = (vector<string>*)t;
		objXml = serializevec<string>(*tt,appName);
	}
	else if(className.find("std::vector<char,")!=string::npos)
	{
		vector<char> *tt = (vector<char>*)t;
		objXml = serializevec<char>(*tt,appName);
	}
	else if(className.find("std::vector<unsigned char,")!=string::npos)
	{
		vector<unsigned char> *tt = (vector<unsigned char>*)t;
		objXml = serializevec<unsigned char>(*tt,appName);
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
	else if(className.find("std::vector<long long,")!=string::npos)
	{
		vector<long long> *tt = (vector<long long>*)t;
		objXml = serializevec<long long>(*tt,appName);
	}
	else if(className.find("std::vector<unsigned int,")!=string::npos)
	{
		vector<unsigned int> *tt = (vector<unsigned int>*)t;
		objXml = serializevec<unsigned int>(*tt,appName);
	}
	else if(className.find("std::vector<unsigned short,")!=string::npos)
	{
		vector<unsigned short> *tt = (vector<unsigned short>*)t;
		objXml = serializevec<unsigned short>(*tt,appName);
	}
	else if(className.find("std::vector<unsigned long,")!=string::npos)
	{
		vector<unsigned long> *tt = (vector<unsigned long>*)t;
		objXml = serializevec<unsigned long>(*tt,appName);
	}
	else if(className.find("std::vector<unsigned long long,")!=string::npos)
	{
		vector<unsigned long long> *tt = (vector<unsigned long long>*)t;
		objXml = serializevec<unsigned long long>(*tt,appName);
	}
	else if(className.find("std::vector<double,")!=string::npos)
	{
		vector<double> *tt = (vector<double>*)t;
		objXml = serializevec<double>(*tt,appName);
	}
	else if(className.find("std::vector<long double,")!=string::npos)
	{
		vector<long double> *tt = (vector<long double>*)t;
		objXml = serializevec<long double>(*tt,appName);
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
	}
	else if(className.find("std::list<std::string,")!=string::npos || className.find("std::list<string,")!=string::npos)
	{
		list<string> *tt = (list<string>*)t;
		objXml = serializelist<string>(*tt,appName);
	}
	else if(className.find("std::list<char,")!=string::npos)
	{
		list<char> *tt = (list<char>*)t;
		objXml = serializelist<char>(*tt,appName);
	}
	else if(className.find("std::list<unsigned char,")!=string::npos)
	{
		list<unsigned char> *tt = (list<unsigned char>*)t;
		objXml = serializelist<unsigned char>(*tt,appName);
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
	else if(className.find("std::list<long long,")!=string::npos)
	{
		list<long long> *tt = (list<long long>*)t;
		objXml = serializelist<long long>(*tt,appName);
	}
	else if(className.find("std::list<unsigned int,")!=string::npos)
	{
		list<unsigned int> *tt = (list<unsigned int>*)t;
		objXml = serializelist<unsigned int>(*tt,appName);
	}
	else if(className.find("std::list<unsigned long,")!=string::npos)
	{
		list<unsigned long> *tt = (list<unsigned long>*)t;
		objXml = serializelist<unsigned long>(*tt,appName);
	}
	else if(className.find("std::list<unsigned short,")!=string::npos)
	{
		list<unsigned short> *tt = (list<unsigned short>*)t;
		objXml = serializelist<unsigned short>(*tt,appName);
	}
	else if(className.find("std::list<unsigned long long,")!=string::npos)
	{
		list<unsigned long long> *tt = (list<unsigned long long>*)t;
		objXml = serializelist<unsigned long long>(*tt,appName);
	}
	else if(className.find("std::list<double,")!=string::npos)
	{
		list<double> *tt = (list<double>*)t;
		objXml = serializelist<double>(*tt,appName);
	}
	else if(className.find("std::list<long double,")!=string::npos)
	{
		list<long double> *tt = (list<long double>*)t;
		objXml = serializelist<long double>(*tt,appName);
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
		//return _serObjectSTLContainers(t,vtyp,appName,"Lis");
	}
	else if(className.find("std::set<std::string,")!=string::npos || className.find("std::set<string,")!=string::npos)
	{
		set<string> *tt = (set<string>*)t;
		objXml = serializeset<string>(*tt,appName);
	}
	else if(className.find("std::set<char,")!=string::npos)
	{
		set<char> *tt = (set<char>*)t;
		objXml = serializeset<char>(*tt,appName);
	}
	else if(className.find("std::set<unsigned char,")!=string::npos)
	{
		set<unsigned char> *tt = (set<unsigned char>*)t;
		objXml = serializeset<unsigned char>(*tt,appName);
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
	else if(className.find("std::set<long long,")!=string::npos)
	{
		set<long long> *tt = (set<long long>*)t;
		objXml = serializeset<long long>(*tt,appName);
	}
	else if(className.find("std::set<unsigned int,")!=string::npos)
	{
		set<unsigned int> *tt = (set<unsigned int>*)t;
		objXml = serializeset<unsigned int>(*tt,appName);
	}
	else if(className.find("std::set<unsigned short,")!=string::npos)
	{
		set<unsigned short> *tt = (set<unsigned short>*)t;
		objXml = serializeset<unsigned short>(*tt,appName);
	}
	else if(className.find("std::set<unsigned long,")!=string::npos)
	{
		set<unsigned long> *tt = (set<unsigned long>*)t;
		objXml = serializeset<unsigned long>(*tt,appName);
	}
	else if(className.find("std::set<unsigned long long,")!=string::npos)
	{
		set<unsigned long long> *tt = (set<unsigned long long>*)t;
		objXml = serializeset<unsigned long long>(*tt,appName);
	}
	else if(className.find("std::set<long double,")!=string::npos)
	{
		set<long double> *tt = (set<long double>*)t;
		objXml = serializeset<long double>(*tt,appName);
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
		//return _serObjectSTLContainers(t,vtyp,appName,"Set");
	}
	else if(className.find("std::multiset<std::string,")!=string::npos || className.find("std::multiset<string,")!=string::npos)
	{
		multiset<string> *tt = (multiset<string>*)t;
		objXml = serializemultiset<string>(*tt,appName);
	}
	else if(className.find("std::multiset<char,")!=string::npos)
	{
		multiset<char> *tt = (multiset<char>*)t;
		objXml = serializemultiset<char>(*tt,appName);
	}
	else if(className.find("std::multiset<unsigned char,")!=string::npos)
	{
		multiset<unsigned char> *tt = (multiset<unsigned char>*)t;
		objXml = serializemultiset<unsigned char>(*tt,appName);
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
	else if(className.find("std::multiset<long long,")!=string::npos)
	{
		multiset<long long> *tt = (multiset<long long>*)t;
		objXml = serializemultiset<long long>(*tt,appName);
	}
	else if(className.find("std::multiset<short,")!=string::npos)
	{
		multiset<short> *tt = (multiset<short>*)t;
		objXml = serializemultiset<short>(*tt,appName);
	}
	else if(className.find("std::multiset<unsigned int,")!=string::npos)
	{
		multiset<unsigned int> *tt = (multiset<unsigned int>*)t;
		objXml = serializemultiset<unsigned int>(*tt,appName);
	}
	else if(className.find("std::multiset<unsigned long,")!=string::npos)
	{
		multiset<unsigned long> *tt = (multiset<unsigned long>*)t;
		objXml = serializemultiset<unsigned long>(*tt,appName);
	}
	else if(className.find("std::multiset<long long,")!=string::npos)
	{
		multiset<unsigned long long> *tt = (multiset<unsigned long long>*)t;
		objXml = serializemultiset<unsigned long long>(*tt,appName);
	}
	else if(className.find("std::multiset<unsigned short,")!=string::npos)
	{
		multiset<unsigned short> *tt = (multiset<unsigned short>*)t;
		objXml = serializemultiset<unsigned short>(*tt,appName);
	}
	else if(className.find("std::multiset<double,")!=string::npos)
	{
		multiset<double> *tt = (multiset<double>*)t;
		objXml = serializemultiset<double>(*tt,appName);
	}
	else if(className.find("std::multiset<long double,")!=string::npos)
	{
		multiset<long double> *tt = (multiset<long double>*)t;
		objXml = serializemultiset<long double>(*tt,appName);
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
		//return _serObjectSTLContainers(t,vtyp,appName,"MulSet");
	}
	else if(className.find("std::queue<std::string,")!=string::npos || className.find("std::queue<string,")!=string::npos)
	{
		std::queue<string> *tt = (std::queue<string>*)t;
		objXml = serializeq<string>(*tt,appName);
	}
	else if(className.find("std::queue<char,")!=string::npos)
	{
		std::queue<char> *tt = (std::queue<char>*)t;
		objXml = serializeq<char>(*tt,appName);
	}
	else if(className.find("std::queue<unsigned char,")!=string::npos)
	{
		std::queue<unsigned char> *tt = (std::queue<unsigned char>*)t;
		objXml = serializeq<unsigned char>(*tt,appName);
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
	else if(className.find("std::std::queue<long long,")!=string::npos)
	{
		std::queue<long long> *tt = (std::queue<long long>*)t;
		objXml = serializeq<long long>(*tt,appName);
	}
	else if(className.find("std::std::queue<unsigned int,")!=string::npos)
	{
		std::queue<unsigned int> *tt = (std::queue<unsigned int>*)t;
		objXml = serializeq<unsigned int>(*tt,appName);
	}
	else if(className.find("std::std::queue<unsigned short,")!=string::npos)
	{
		std::queue<unsigned short> *tt = (std::queue<unsigned short>*)t;
		objXml = serializeq<unsigned short>(*tt,appName);
	}
	else if(className.find("std::std::queue<unsigned long,")!=string::npos)
	{
		std::queue<unsigned long> *tt = (std::queue<unsigned long>*)t;
		objXml = serializeq<unsigned long>(*tt,appName);
	}
	else if(className.find("std::std::queue<unsigned long long,")!=string::npos)
	{
		std::queue<unsigned long long> *tt = (std::queue<unsigned long long>*)t;
		objXml = serializeq<unsigned long long>(*tt,appName);
	}
	else if(className.find("std::std::queue<long double,")!=string::npos)
	{
		std::queue<long double> *tt = (std::queue<long double>*)t;
		objXml = serializeq<long double>(*tt,appName);
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
		//return _serObjectSTLContainers(t,vtyp,appName,"Q");
	}
	else if(className.find("std::deque<std::string,")!=string::npos || className.find("std::deque<string,")!=string::npos)
	{
		deque<string> *tt = (deque<string>*)t;
		objXml = serializedq<string>(*tt,appName);
	}
	else if(className.find("std::deque<char,")!=string::npos)
	{
		deque<char> *tt = (deque<char>*)t;
		objXml = serializedq<char>(*tt,appName);
	}
	else if(className.find("std::deque<unsigned char,")!=string::npos)
	{
		deque<unsigned char> *tt = (deque<unsigned char>*)t;
		objXml = serializedq<unsigned char>(*tt,appName);
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
	else if(className.find("std::deque<long long,")!=string::npos)
	{
		deque<long long> *tt = (deque<long long>*)t;
		objXml = serializedq<long long>(*tt,appName);
	}
	else if(className.find("std::deque<unsigned int,")!=string::npos)
	{
		deque<unsigned int> *tt = (deque<unsigned int>*)t;
		objXml = serializedq<unsigned int>(*tt,appName);
	}
	else if(className.find("std::deque<unsigned short,")!=string::npos)
	{
		deque<unsigned short> *tt = (deque<unsigned short>*)t;
		objXml = serializedq<unsigned short>(*tt,appName);
	}
	else if(className.find("std::deque<unsigned long,")!=string::npos)
	{
		deque<unsigned long> *tt = (deque<unsigned long>*)t;
		objXml = serializedq<unsigned long>(*tt,appName);
	}
	else if(className.find("std::deque<unsigned long long,")!=string::npos)
	{
		deque<unsigned long long> *tt = (deque<unsigned long long>*)t;
		objXml = serializedq<unsigned long long>(*tt,appName);
	}
	else if(className.find("std::deque<long double,")!=string::npos)
	{
		deque<long double> *tt = (deque<long double>*)t;
		objXml = serializedq<long double>(*tt,appName);
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
		//return _serObjectSTLContainers(t,vtyp,appName,"Dq");
	}
	else
	{
		//return _ser(t,className,appName);
	}
	return objXml;
}*/

/*void* Serialize::_handleAllUnSerialization(string objXml,string className, string appName)
{
	AMEFDecoder dec;
	AMEFObject* root = dec.decodeB(objXml, true, false);
	if(root==NULL)
		return NULL;
	if(className=="short" || className=="short int" || className=="signed short" || className=="signed short int"
			|| className=="unsigned short" || className=="unsigned short int"
			|| className=="signed" || className=="int" || className=="signed int"
			|| className=="unsigned" || className=="unsigned int" || className=="long"
			|| className=="long int" || className=="signed long" || className=="signed long int"
			|| className=="unsigned long" || className=="unsigned long int"
			|| className=="long long" || className=="long long int" || className=="signed long long"
			|| className=="signed long long int" || className=="unsigned long long"
			|| className=="unsigned long long int" || className=="long double" || className=="bool"
			|| className=="float" || className=="double" || className=="string" || className=="std::string"
			|| className=="char" || className=="signed char" || className=="unsigned char"
			|| className=="BinaryData" || className=="Date")
	{
		root = root->getPackets().at(0);
		if((className=="signed" || className=="int" || className=="signed int") && className==root->getNameStr())
		{
			int *vt = new int;
			*vt = root->getIntValue();
			return vt;
		}
		else if((className=="unsigned" || className=="unsigned int") && className==root->getNameStr())
		{
			unsigned int *vt = new unsigned int;
			*vt = root->getUIntValue();
			return vt;
		}
		else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int") && className==root->getNameStr())
		{
			short *vt = new short;
			*vt = root->getShortValue();
			return vt;
		}
		else if((className=="unsigned short" || className=="unsigned short int") && className==root->getNameStr())
		{
			unsigned short *vt = new unsigned short;
			*vt = root->getUShortValue();
			return vt;
		}
		else if((className=="long int" || className=="signed long" || className=="signed long int" || className=="signed long long int") && className==root->getNameStr())
		{
			long *vt = new long;
			*vt = root->getLongLongValue();
			return vt;
		}
		else if((className=="unsigned long long" || className=="unsigned long long int") && className==root->getNameStr())
		{
			unsigned long long *vt = new unsigned long long;
			*vt = root->getULongLongValue();
			return vt;
		}
		else if((className=="char" || className=="signed char") && className==root->getNameStr())
		{
			char *vt = new char;
			*vt = root->getCharValue();
			return vt;
		}
		else if(className=="unsigned char" && className==root->getNameStr())
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getUCharValue();
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
		else if(className=="long double" && className==root->getNameStr())
		{
			long double *vt = new long double;
			*vt = root->getLongDoubleValue();
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
		Serialize serialize;
		int sizet;
		className = className.substr(0, className.find(",")+1);
		int level = StringUtil::countOccurrences(className, "<");
		if(level>1)
		{
			int size;
			cout << root->tostring() << endl;
			return handleMultiLevelUnSerialization(root, className, appName, size, &serialize);
		}
		else if(root->getNameStr().find("std::vector<")==0)
		{
			return unserializevec(root,appName,sizet, &serialize);
		}
		else if(root->getNameStr().find("std::set<")==0)
		{
			return unserializeset(root,appName,sizet, &serialize);
		}
		else if(root->getNameStr().find("std::multiset<")==0)
		{
			return unserializemultiset(root,appName,sizet, &serialize);
		}
		else if(root->getNameStr().find("std::list<")==0)
		{
			return unserializelist(root,appName,sizet, &serialize);
		}
		else if(root->getNameStr().find("std::queue<")==0)
		{
			return unserializeq(root,appName,sizet, &serialize);
		}
		else if(root->getNameStr().find("std::deque<")==0)
		{
			return unserializedq(root,appName,sizet, &serialize);
		}
	}
	return NULL;
}*/

/*void* Serialize::handleMultiLevelUnSerialization(AMEFObject* root, string className, string appName, int& size)
{
	size = 0;
	int level = StringUtil::countOccurrences(className, "<") - 1 ;
	AMEFDecoder dec;
	void* tv = NULL;
	if(level>0)
	{
		string container;
		if(className.find("std::vector<")==0)
		{
			container = "std::vector<";
		}
		else if(className.find("std::deque<")==0)
		{
			container = "std::deque<";
		}
		else if(className.find("std::queue<")==0)
		{
			container = "std::queue<";
		}
		else if(className.find("std::list<")==0)
		{
			container = "std::list<";
		}
		else if(className.find("std::set<")==0)
		{
			container = "std::set<";
		}
		else if(className.find("std::multiset<")==0)
		{
			container = "std::multiset<";
		}
		vector<int> elesizes;
		tv = getNestedContainer(className);
		for (int var = 0; var < (int)root->getPackets().size(); var++)
		{
			AMEFObject* roott = dec.decodeB(root->getPackets().at(var)->getValue(), false, false);
			size++;
			cout << roott->tostring() << endl;
			int lsiz = 0;
			if(container=="std::list<")
				addToNestedContainer(roott, className, appName, lsiz, container, tv, var, &handleMultiLevelUnSerialization);
			else
				addToNestedContainer(roott, className, appName, lsiz, container, tv, var, &handleMultiLevelUnSerialization);
			elesizes.push_back(lsiz);
		}
		if(container.find("std::vector<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			vector<DummyList>* vdl = (vector<DummyList>*)tv;
			vector<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
			}
		}
		else if(container.find("std::deque<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			deque<DummyList>* vdl = (deque<DummyList>*)tv;
			deque<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
			}
		}
		else if(container.find("std::set<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			set<DummyList>* vdl = (set<DummyList>*)tv;
			set<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
			}
		}
		else if(container.find("std::multiset<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			multiset<DummyList>* vdl = (multiset<DummyList>*)tv;
			multiset<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
			}
		}
		else if(container.find("std::queue<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			queue<DummyList>* vdl = (deque<DummyList>*)tv;
			queue<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
			}
		}
	}
	else
	{
		if(className.find("std::vector<")==0)
		{
			tv = unserializevec(root, appName, size);
		}
		else if(className.find("std::list<")==0)
		{
			tv = unserializelist(root, appName, size);
		}
		else if(className.find("std::deque<")==0)
		{
			tv = unserializedq(root, appName, size);
		}
		else if(className.find("std::set<")==0)
		{
			tv = unserializeset(root, appName, size);
		}
		else if(className.find("std::multiset<")==0)
		{
			tv = unserializemultiset(root, appName, size);
		}
		else if(className.find("std::queue<")==0)
		{
			tv = unserializeq(root, appName, size);
		}
	}
	return tv;
}


void* Serialize::unserializeset(AMEFObject* root, string appName, int &size)
{
	AMEFDecoder dec;
	string className, stlclassName;
	if(root!=NULL)
	{
		stlclassName = root->getNameStr();
	}
	if(stlclassName.find(">")!=string::npos)
	{
		if(stlclassName.find("std::set<")!=string::npos)
		{
			className = stlclassName.substr(9,stlclassName.find(">")-9);
		}
		else if(stlclassName.find("set<")!=string::npos)
		{
			className = stlclassName.substr(4,stlclassName.find(">")-4);
		}
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.at(className.length()-1)==',')
		{
			className = className.substr(0, className.length()-1);
		}
	}
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new set<string>();
	else if(className=="int")
		t = new set<int>();
	else if(className=="short")
		t = new set<short>();
	else if(className=="long")
		t = new set<long>();
	else if(className=="long long")
		t = new set<long long>();
	else if(className=="long double")
		t = new set<long double>();
	else if(className=="unsigned int")
		t = new set<unsigned int>();
	else if(className=="unsigned short")
		t = new set<unsigned short>();
	else if(className=="unsigned long")
		t = new set<unsigned long>();
	else if(className=="unsigned long long")
		t = new set<unsigned long long>();
	else if(className=="float")
		t = new set<float>();
	else if(className=="double")
		t = new set<double>();
	else if(className=="bool")
		t = new set<bool>();
	else if(className=="char")
		t = new set<char>();
	else if(className=="unsigned char")
		t = new set<unsigned char>();
	else
	{
		//return _unserSet(objXml,className,appName);
	}
	if(t!=NULL)
	{
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				size++;
				AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
				if(className=="std::string" || className=="string")
					((set<string>*)t)->insert(root2->getPackets().at(0)->getValueStr());
				else if(className=="int")
					((set<int>*)t)->insert(root2->getPackets().at(0)->getIntValue());
				else if(className=="short")
					((set<short>*)t)->insert(root2->getPackets().at(0)->getShortValue());
				else if(className=="long")
					((set<long>*)t)->insert(root2->getPackets().at(0)->getLongValue());
				else if(className=="long long")
					((set<long long>*)t)->insert(root2->getPackets().at(0)->getLongLongValue());
				else if(className=="long double")
					((set<long double>*)t)->insert(root2->getPackets().at(0)->getLongDoubleValue());
				else if(className=="unsigned int")
					((set<unsigned int>*)t)->insert(root2->getPackets().at(0)->getUIntValue());
				else if(className=="unsigned short")
					((set<unsigned short>*)t)->insert(root2->getPackets().at(0)->getUShortValue());
				else if(className=="unsigned long")
					((set<unsigned long>*)t)->insert(root2->getPackets().at(0)->getULongValue());
				else if(className=="unsigned long long")
					((set<unsigned long long>*)t)->insert(root2->getPackets().at(0)->getULongLongValue());
				else if(className=="float")
					((set<float>*)t)->insert(root2->getPackets().at(0)->getFloatValue());
				else if(className=="double")
					((set<double>*)t)->insert(root2->getPackets().at(0)->getDoubleValue());
				else if(className=="bool")
					((set<bool>*)t)->insert(root2->getPackets().at(0)->getBoolValue());
				else if(className=="char")
					((set<char>*)t)->insert(root2->getPackets().at(0)->getCharValue());
				else if(className=="unsigned char")
					((set<unsigned char>*)t)->insert(root2->getPackets().at(0)->getUCharValue());
			}
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializelist(AMEFObject* root, string appName, int &size)
{
	AMEFDecoder dec;
	string className, stlclassName;
	if(root!=NULL)
	{
		stlclassName = root->getNameStr();
	}
	if(stlclassName.find(">")!=string::npos)
	{
		if(stlclassName.find("std::list<")!=string::npos)
		{
			className = stlclassName.substr(10,stlclassName.find(">")-10);
		}
		else if(stlclassName.find("list<")!=string::npos)
		{
			className = stlclassName.substr(5,stlclassName.find(">")-5);
		}
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.at(className.length()-1)==',')
		{
			className = className.substr(0, className.length()-1);
		}
	}
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new list<string>();
	else if(className=="int")
		t = new list<int>();
	else if(className=="short")
		t = new list<short>();
	else if(className=="long")
		t = new list<long>();
	else if(className=="long long")
		t = new list<long long>();
	else if(className=="long double")
		t = new list<long double>();
	else if(className=="unsigned int")
		t = new list<unsigned int>();
	else if(className=="unsigned short")
		t = new list<unsigned short>();
	else if(className=="unsigned long")
		t = new list<unsigned long>();
	else if(className=="unsigned long long")
		t = new list<unsigned long long>();
	else if(className=="float")
		t = new list<float>();
	else if(className=="double")
		t = new list<double>();
	else if(className=="bool")
		t = new list<bool>();
	else if(className=="char")
		t = new list<char>();
	else if(className=="unsigned char")
		t = new list<unsigned char>();
	else
	{
		//return _unserLis(objXml,className,appName);
	}
	if(t!=NULL)
	{
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				size++;
				AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
				if(className=="std::string" || className=="string")
					((list<string>*)t)->push_back(root2->getPackets().at(0)->getValueStr());
				else if(className=="int")
					((list<int>*)t)->push_back(root2->getPackets().at(0)->getIntValue());
				else if(className=="short")
					((list<short>*)t)->push_back(root2->getPackets().at(0)->getShortValue());
				else if(className=="long")
					((list<long>*)t)->push_back(root2->getPackets().at(0)->getLongValue());
				else if(className=="long long")
					((list<long long>*)t)->push_back(root2->getPackets().at(0)->getLongLongValue());
				else if(className=="long double")
					((list<long double>*)t)->push_back(root2->getPackets().at(0)->getLongDoubleValue());
				else if(className=="unsigned int")
					((list<unsigned int>*)t)->push_back(root2->getPackets().at(0)->getUIntValue());
				else if(className=="unsigned short")
					((list<unsigned short>*)t)->push_back(root2->getPackets().at(0)->getUShortValue());
				else if(className=="unsigned long")
					((list<unsigned long>*)t)->push_back(root2->getPackets().at(0)->getULongValue());
				else if(className=="unsigned long long")
					((list<unsigned long long>*)t)->push_back(root2->getPackets().at(0)->getULongLongValue());
				else if(className=="float")
					((list<float>*)t)->push_back(root2->getPackets().at(0)->getFloatValue());
				else if(className=="double")
					((list<double>*)t)->push_back(root2->getPackets().at(0)->getDoubleValue());
				else if(className=="bool")
					((list<bool>*)t)->push_back(root2->getPackets().at(0)->getBoolValue());
				else if(className=="char")
					((list<char>*)t)->push_back(root2->getPackets().at(0)->getCharValue());
				else if(className=="unsigned char")
					((list<unsigned char>*)t)->push_back(root2->getPackets().at(0)->getUCharValue());
			}
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializeq(AMEFObject* root, string appName, int &size)
{
	AMEFDecoder dec;
	string className, stlclassName;
	if(root!=NULL)
	{
		stlclassName = root->getNameStr();
	}
	if(stlclassName.find(">")!=string::npos)
	{
		if(stlclassName.find("std::queue<")!=string::npos)
		{
			className = stlclassName.substr(11,stlclassName.find(">")-11);
		}
		else if(stlclassName.find("queue<")!=string::npos)
		{
			className = stlclassName.substr(6,stlclassName.find(">")-6);
		}
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.at(className.length()-1)==',')
		{
			className = className.substr(0, className.length()-1);
		}
	}
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::queue<string>();
	else if(className=="int")
		t = new std::queue<int>();
	else if(className=="short")
		t = new std::queue<short>();
	else if(className=="long")
		t = new std::queue<long>();
	else if(className=="long long")
		t = new std::queue<long long>();
	else if(className=="long double")
		t = new std::queue<long double>();
	else if(className=="unsigned int")
		t = new std::queue<unsigned int>();
	else if(className=="unsigned short")
		t = new std::queue<unsigned short>();
	else if(className=="unsigned long")
		t = new std::queue<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::queue<unsigned long long>();
	else if(className=="float")
		t = new std::queue<float>();
	else if(className=="double")
		t = new std::queue<double>();
	else if(className=="bool")
		t = new std::queue<bool>();
	else if(className=="char")
		t = new std::queue<char>();
	else if(className=="unsigned char")
		t = new std::queue<unsigned char>();
	else
	{
		//return _unserQ(objXml,className,appName);
	}
	if(t!=NULL)
	{
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				size++;
				AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
				if(className=="std::string" || className=="string")
					((std::queue<string>*)t)->push(root2->getPackets().at(0)->getValueStr());
				else if(className=="int")
					((std::queue<int>*)t)->push(root2->getPackets().at(0)->getIntValue());
				else if(className=="short")
					((std::queue<short>*)t)->push(root2->getPackets().at(0)->getShortValue());
				else if(className=="long")
					((std::queue<long>*)t)->push(root2->getPackets().at(0)->getLongValue());
				else if(className=="long long")
					((std::queue<long long>*)t)->push(root2->getPackets().at(0)->getLongLongValue());
				else if(className=="long double")
					((std::queue<long double>*)t)->push(root2->getPackets().at(0)->getLongDoubleValue());
				else if(className=="unsigned int")
					((std::queue<unsigned int>*)t)->push(root2->getPackets().at(0)->getUIntValue());
				else if(className=="unsigned short")
					((std::queue<unsigned short>*)t)->push(root2->getPackets().at(0)->getUShortValue());
				else if(className=="unsigned long")
					((std::queue<unsigned long>*)t)->push(root2->getPackets().at(0)->getULongValue());
				else if(className=="unsigned long long")
					((std::queue<unsigned long long>*)t)->push(root2->getPackets().at(0)->getULongLongValue());
				else if(className=="float")
					((std::queue<float>*)t)->push(root2->getPackets().at(0)->getFloatValue());
				else if(className=="double")
					((std::queue<double>*)t)->push(root2->getPackets().at(0)->getDoubleValue());
				else if(className=="bool")
					((std::queue<bool>*)t)->push(root2->getPackets().at(0)->getBoolValue());
				else if(className=="char")
					((queue<char>*)t)->push(root2->getPackets().at(0)->getCharValue());
				else if(className=="unsigned char")
					((queue<unsigned char>*)t)->push(root2->getPackets().at(0)->getUCharValue());
			}
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializevec(AMEFObject* root, string appName, int &size)
{
	AMEFDecoder dec;
	string className, stlclassName;
	if(root!=NULL)
	{
		stlclassName = root->getNameStr();
	}
	if(stlclassName.find(">")!=string::npos)
	{
		if(stlclassName.find("std::vector<")!=string::npos)
		{
			className = stlclassName.substr(12,stlclassName.find(">")-12);
		}
		else if(stlclassName.find("vector<")!=string::npos)
		{
			className = stlclassName.substr(7,stlclassName.find(">")-7);
		}
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.at(className.length()-1)==',')
		{
			className = className.substr(0, className.length()-1);
		}
	}
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new vector<string>();
	else if(className=="int")
		t = new vector<int>();
	else if(className=="short")
		t = new vector<short>();
	else if(className=="long")
		t = new vector<long>();
	else if(className=="long long")
		t = new vector<long long>();
	else if(className=="long double")
		t = new vector<long double>();
	else if(className=="unsigned int")
		t = new vector<unsigned int>();
	else if(className=="unsigned short")
		t = new vector<unsigned short>();
	else if(className=="unsigned long")
		t = new vector<unsigned long>();
	else if(className=="unsigned long long")
		t = new vector<unsigned long long>();
	else if(className=="float")
		t = new vector<float>();
	else if(className=="double")
		t = new vector<double>();
	else if(className=="bool")
		t = new vector<bool>();
	else if(className=="char")
		t = new vector<char>();
	else if(className=="unsigned char")
		t = new vector<unsigned char>();
	else
	{
		//return _unserVec(objXml,className,appName);
	}
	if(t!=NULL)
	{
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				size++;
				AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
				if(className=="std::string" || className=="string")
					((vector<string>*)t)->push_back(root2->getPackets().at(0)->getValueStr());
				else if(className=="int")
					((vector<int>*)t)->push_back(root2->getPackets().at(0)->getIntValue());
				else if(className=="short")
					((vector<short>*)t)->push_back(root2->getPackets().at(0)->getShortValue());
				else if(className=="long")
					((vector<long>*)t)->push_back(root2->getPackets().at(0)->getLongValue());
				else if(className=="long long")
					((vector<long long>*)t)->push_back(root2->getPackets().at(0)->getLongLongValue());
				else if(className=="long double")
					((vector<long double>*)t)->push_back(root2->getPackets().at(0)->getLongDoubleValue());
				else if(className=="unsigned int")
					((vector<unsigned int>*)t)->push_back(root2->getPackets().at(0)->getUIntValue());
				else if(className=="unsigned short")
					((vector<unsigned short>*)t)->push_back(root2->getPackets().at(0)->getUShortValue());
				else if(className=="unsigned long")
					((vector<unsigned long>*)t)->push_back(root2->getPackets().at(0)->getULongValue());
				else if(className=="unsigned long long")
					((vector<unsigned long long>*)t)->push_back(root2->getPackets().at(0)->getULongLongValue());
				else if(className=="float")
					((vector<float>*)t)->push_back(root2->getPackets().at(0)->getFloatValue());
				else if(className=="double")
					((vector<double>*)t)->push_back(root2->getPackets().at(0)->getDoubleValue());
				else if(className=="bool")
					((vector<bool>*)t)->push_back(root2->getPackets().at(0)->getBoolValue());
				else if(className=="char")
					((vector<char>*)t)->push_back(root2->getPackets().at(0)->getCharValue());
				else if(className=="unsigned char")
					((vector<unsigned char>*)t)->push_back(root2->getPackets().at(0)->getUCharValue());
			}
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializedq(AMEFObject* root, string appName, int &size)
{
	AMEFDecoder dec;
	string className, stlclassName;
	if(root!=NULL)
	{
		stlclassName = root->getNameStr();
	}
	if(stlclassName.find(">")!=string::npos)
	{
		if(stlclassName.find("std::deque<")!=string::npos)
		{
			className = stlclassName.substr(11,stlclassName.find(">")-11);
		}
		else if(stlclassName.find("deque<")!=string::npos)
		{
			className = stlclassName.substr(6,stlclassName.find(">")-6);
		}
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.at(className.length()-1)==',')
		{
			className = className.substr(0, className.length()-1);
		}
	}
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new deque<string>();
	else if(className=="int")
		t = new deque<int>();
	else if(className=="short")
		t = new deque<short>();
	else if(className=="long")
		t = new deque<long>();
	else if(className=="long long")
		t = new deque<long long>();
	else if(className=="long double")
		t = new deque<long double>();
	else if(className=="unsigned int")
		t = new deque<unsigned int>();
	else if(className=="unsigned short")
		t = new deque<unsigned short>();
	else if(className=="unsigned long")
		t = new deque<unsigned long>();
	else if(className=="unsigned long long")
		t = new deque<unsigned long long>();
	else if(className=="float")
		t = new deque<float>();
	else if(className=="double")
		t = new deque<double>();
	else if(className=="bool")
		t = new deque<bool>();
	else if(className=="char")
		t = new deque<char>();
	else if(className=="unsigned char")
		t = new deque<unsigned char>();
	else
	{
		//return _unserDq(objXml,className,appName);
	}
	if(t!=NULL)
	{
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				size++;
				AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
				if(className=="std::string" || className=="string")
					((deque<string>*)t)->push_back(root2->getPackets().at(0)->getValueStr());
				else if(className=="int")
					((deque<int>*)t)->push_back(root2->getPackets().at(0)->getIntValue());
				else if(className=="short")
					((deque<short>*)t)->push_back(root2->getPackets().at(0)->getShortValue());
				else if(className=="long")
					((deque<long>*)t)->push_back(root2->getPackets().at(0)->getLongValue());
				else if(className=="long long")
					((deque<long long>*)t)->push_back(root2->getPackets().at(0)->getLongLongValue());
				else if(className=="long double")
					((deque<long double>*)t)->push_back(root2->getPackets().at(0)->getLongDoubleValue());
				else if(className=="unsigned int")
					((deque<unsigned int>*)t)->push_back(root2->getPackets().at(0)->getUIntValue());
				else if(className=="unsigned short")
					((deque<unsigned short>*)t)->push_back(root2->getPackets().at(0)->getUShortValue());
				else if(className=="unsigned long")
					((deque<unsigned long>*)t)->push_back(root2->getPackets().at(0)->getULongValue());
				else if(className=="unsigned long long")
					((deque<unsigned long long>*)t)->push_back(root2->getPackets().at(0)->getULongLongValue());
				else if(className=="float")
					((deque<float>*)t)->push_back(root2->getPackets().at(0)->getFloatValue());
				else if(className=="double")
					((deque<double>*)t)->push_back(root2->getPackets().at(0)->getDoubleValue());
				else if(className=="bool")
					((deque<bool>*)t)->push_back(root2->getPackets().at(0)->getBoolValue());
				else if(className=="char")
					((deque<char>*)t)->push_back(root2->getPackets().at(0)->getCharValue());
				else if(className=="unsigned char")
					((deque<unsigned char>*)t)->push_back(root2->getPackets().at(0)->getUCharValue());
			}
		}
		return t;
	}
	return NULL;
}

void* Serialize::unserializemultiset(AMEFObject* root, string appName, int &size)
{
	AMEFDecoder dec;
	string className, stlclassName;
	if(root!=NULL)
	{
		stlclassName = root->getNameStr();
	}
	if(stlclassName.find(">")!=string::npos)
	{
		if(stlclassName.find("std::multiset<")!=string::npos)
		{
			className = stlclassName.substr(14,stlclassName.find(">")-14);
		}
		else if(stlclassName.find("multiset<")!=string::npos)
		{
			className = stlclassName.substr(8,stlclassName.find(">")-8);
		}
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.at(className.length()-1)==',')
		{
			className = className.substr(0, className.length()-1);
		}
	}
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new multiset<string>();
	else if(className=="int")
		t = new multiset<int>();
	else if(className=="short")
		t = new multiset<short>();
	else if(className=="long")
		t = new multiset<long>();
	else if(className=="long long")
		t = new multiset<long long>();
	else if(className=="long double")
		t = new multiset<long double>();
	else if(className=="unsigned int")
		t = new multiset<unsigned int>();
	else if(className=="unsigned short")
		t = new multiset<unsigned short>();
	else if(className=="unsigned long")
		t = new multiset<unsigned long>();
	else if(className=="unsigned long long")
		t = new multiset<unsigned long long>();
	else if(className=="float")
		t = new multiset<float>();
	else if(className=="double")
		t = new multiset<double>();
	else if(className=="bool")
		t = new multiset<bool>();
	else if(className=="char")
		t = new multiset<char>();
	else if(className=="unsigned char")
		t = new multiset<unsigned char>();
	else
	{
		//return _unserMulSet(objXml,className,appName);
	}
	if(t!=NULL)
	{
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				size++;
				AMEFObject* root2 = dec.decodeB(root->getPackets().at(var)->getValue(), true, false);
				if(className=="std::string" || className=="string")
					((multiset<string>*)t)->insert(root2->getPackets().at(0)->getValueStr());
				else if(className=="int")
					((multiset<int>*)t)->insert(root2->getPackets().at(0)->getIntValue());
				else if(className=="short")
					((multiset<short>*)t)->insert(root2->getPackets().at(0)->getShortValue());
				else if(className=="long")
					((multiset<long>*)t)->insert(root2->getPackets().at(0)->getLongValue());
				else if(className=="long long")
					((multiset<long long>*)t)->insert(root2->getPackets().at(0)->getLongLongValue());
				else if(className=="long double")
					((multiset<long double>*)t)->insert(root2->getPackets().at(0)->getLongDoubleValue());
				else if(className=="unsigned int")
					((multiset<unsigned int>*)t)->insert(root2->getPackets().at(0)->getUIntValue());
				else if(className=="unsigned short")
					((multiset<unsigned short>*)t)->insert(root2->getPackets().at(0)->getUShortValue());
				else if(className=="unsigned long")
					((multiset<unsigned long>*)t)->insert(root2->getPackets().at(0)->getULongValue());
				else if(className=="unsigned long long")
					((multiset<unsigned long long>*)t)->insert(root2->getPackets().at(0)->getULongLongValue());
				else if(className=="float")
					((multiset<float>*)t)->insert(root2->getPackets().at(0)->getFloatValue());
				else if(className=="double")
					((multiset<double>*)t)->insert(root2->getPackets().at(0)->getDoubleValue());
				else if(className=="bool")
					((multiset<bool>*)t)->insert(root2->getPackets().at(0)->getBoolValue());
				else if(className=="char")
					((multiset<char>*)t)->insert(root2->getPackets().at(0)->getCharValue());
				else if(className=="unsigned char")
					((multiset<unsigned char>*)t)->insert(root2->getPackets().at(0)->getUCharValue());
			}
		}
		return t;
	}
	return NULL;
}*/
