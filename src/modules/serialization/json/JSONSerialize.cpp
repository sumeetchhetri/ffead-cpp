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
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#include "JSONSerialize.h"

JSONSerialize::JSONSerialize() {
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		throw "Cannot load serialization shared library";
	}
	dlibinstantiated = true;
}

JSONSerialize::JSONSerialize(void* dlib) {
	if(dlib == NULL)
	{
		throw "Cannot load serialization shared library";
	}
	this->dlib = dlib;
	dlibinstantiated = false;
}

JSONSerialize::~JSONSerialize() {
	if(dlibinstantiated)
	{
		dlclose(dlib);
	}
}

std::string JSONSerialize::serializePrimitive(const std::string& className, void* t)
{
	std::string objXml;
	if(className=="std::string" || className=="string")
	{
		std::string tem = *(std::string*)t;
		objXml = "\""+tem+"\"";
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		std::string temp;
		temp.push_back(tem);
		objXml = "\""+temp+"\"";
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(char*)t;
		std::string temp;
		temp.push_back(tem);
		objXml = "\""+temp+"\"";
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
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
	return objXml;
}

void* JSONSerialize::getSerializableObject()
{
	return new std::string;
}

void JSONSerialize::cleanSerializableObject(void* _1)
{
	std::string* object = (std::string*)_1;
	delete object;
}

void JSONSerialize::startContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	std::string* object = (std::string*)_1;
	*object = "[";
}

void JSONSerialize::endContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	std::string* object = (std::string*)_1;
	if(object->at(object->length()-1)==',')
		*object = object->substr(0, object->length()-1);
	*object += "]";
}

void JSONSerialize::afterAddContainerSerializableElement(void* _1, const int& counter, const int& size)
{
	std::string* object = (std::string*)_1;
	//if(counter!=size)
		*object += ",";
}

void JSONSerialize::addContainerSerializableElement(void* _1, const std::string& tem)
{
	std::string* object = (std::string*)_1;
	*object += tem;
}

void JSONSerialize::addContainerSerializableElementMulti(void* _1, const std::string& tem)
{
	std::string* object = (std::string*)_1;
	*object += tem;
}

std::string JSONSerialize::fromSerializableObjectToString(void* _1)
{
	std::string* object = (std::string*)_1;
	return *object;
}

std::string JSONSerialize::elementToSerializedString(void* _1, const int& counter)
{
	JSONElement* root = (JSONElement*)_1;
	if((int)root->getChildren().size()<counter)
		return NULL;
	return root->getChildren().at(counter).toString();
}

std::string JSONSerialize::getConatinerElementClassName(void* _1, const std::string& className)
{
	std::string stlclassName = className;
	if(stlclassName.find(">")!=std::string::npos)
	{
		stlclassName = stlclassName.substr(stlclassName.find("<")+1);
		return className.substr(0, className.find(">"));
	}
	else
	{
		stlclassName = stlclassName.substr(stlclassName.find("<")+1);
		if(stlclassName.find(",")!=std::string::npos)
		{
			return stlclassName.substr(0, stlclassName.find_last_of(','));
		}
	}
	return className;
}

void* JSONSerialize::getContainerElement(void* _1, const int& counter, const int& counter1)
{
	JSONElement* root = (JSONElement*)_1;
	if((int)root->getChildren().size()<counter)
		return NULL;
	return (void*)(&(root->getChildren().at(counter)));
}

void JSONSerialize::addPrimitiveElementToContainer(void* _1, const int& counter, const std::string& className, void* cont, const std::string& container)
{
	JSONElement* root = (JSONElement*)_1;
	JSONElement* ele = (JSONElement*)&(root->getChildren().at(counter));
	if(className=="std::string" || className=="string")
	{
		std::string retVal = ele->getValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="int")
	{
		int retVal = CastUtil::lexical_cast<int>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="short")
	{
		short retVal = CastUtil::lexical_cast<short>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long")
	{
		long retVal = CastUtil::lexical_cast<long>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long long")
	{
		long long retVal = CastUtil::lexical_cast<long long>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long double")
	{
		long double retVal = CastUtil::lexical_cast<long double>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned int")
	{
		unsigned int retVal = CastUtil::lexical_cast<unsigned int>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned short")
	{
		unsigned short retVal = CastUtil::lexical_cast<unsigned short>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned long")
	{
		unsigned long retVal = CastUtil::lexical_cast<unsigned long>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long retVal = CastUtil::lexical_cast<unsigned long long>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="float")
	{
		float retVal = CastUtil::lexical_cast<float>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="double")
	{
		double retVal = CastUtil::lexical_cast<double>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="bool")
	{
		bool retVal = CastUtil::lexical_cast<bool>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="char")
	{
		char retVal = CastUtil::lexical_cast<char>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned char")
	{
		unsigned char retVal = CastUtil::lexical_cast<unsigned char>(ele->getValue());
		addValueToNestedContainer(container, retVal, cont);
	}
}

void* JSONSerialize::getUnserializableObject(const std::string& _1)
{
	std::string* tem = new std::string(_1);
	return tem;
}

void JSONSerialize::cleanUnserializableObject(void* _1)
{
	std::string* object = (std::string*)_1;
	delete object;
}

void JSONSerialize::cleanValidUnserializableObject(void* _1)
{
	JSONElement* object = (JSONElement*)_1;
	delete object;
}

void* JSONSerialize::getValidUnserializableObject(const std::string& _1)
{
	JSONElement* nroot = new JSONElement;
	JSONUtil::getDocument(_1, *nroot);
	return nroot;
}

int JSONSerialize::getContainerSize(void* _1)
{
	JSONElement* root = (JSONElement*)_1;
	return root->getChildren().size();
}

std::string JSONSerialize::getUnserializableClassName(void* _1, const std::string& className)
{
	return className;
}

void* JSONSerialize::getPrimitiveValue(void* _1, const std::string& className)
{
	std::string* root = (std::string*)_1;
	if((className=="signed" || className=="int" || className=="signed int"))
	{
		int *vt = new int;
		*vt = CastUtil::lexical_cast<int>(*root);
		return vt;
	}
	else if((className=="unsigned" || className=="unsigned int"))
	{
		unsigned int *vt = new unsigned int;
		*vt = CastUtil::lexical_cast<unsigned int>(*root);
		return vt;
	}
	else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int"))
	{
		short *vt = new short;
		*vt = CastUtil::lexical_cast<short>(*root);
		return vt;
	}
	else if((className=="unsigned short" || className=="unsigned short int"))
	{
		unsigned short *vt = new unsigned short;
		*vt = CastUtil::lexical_cast<unsigned short>(*root);
		return vt;
	}
	else if((className=="long int" || className=="signed long" || className=="signed long int" || className=="signed long long int"))
	{
		long *vt = new long;
		*vt = CastUtil::lexical_cast<long>(*root);
		return vt;
	}
	else if((className=="unsigned long long" || className=="unsigned long long int"))
	{
		unsigned long long *vt = new unsigned long long;
		*vt = CastUtil::lexical_cast<unsigned long long>(*root);
		return vt;
	}
	else if((className=="char" || className=="signed char"))
	{
		char *vt = new char;
		*vt = root->at(0);
		return vt;
	}
	else if(className=="unsigned char")
	{
		unsigned char *vt = new unsigned char;
		*vt = root->at(0);
		return vt;
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		return formt.parse(*root);
	}
	else if(className=="BinaryData")
	{
		return BinaryData::unSerilaize(*root);
	}
	else if(className=="float")
	{
		float *vt = new float;
		*vt = CastUtil::lexical_cast<float>(*root);
		return vt;
	}
	else if(className=="double")
	{
		double *vt = new double;
		*vt = CastUtil::lexical_cast<double>(*root);
		return vt;
	}
	else if(className=="long double")
	{
		long double *vt = new long double;
		*vt = CastUtil::lexical_cast<long double>(*root);
		return vt;
	}
	else if(className=="bool")
	{
		bool *vt = new bool;
		*vt = CastUtil::lexical_cast<bool>(*root);
		return vt;
	}
	else if(className=="std::string" || className=="string")
	{
		std::string *vt = new std::string;
		*vt = *root;
		return vt;
	}
	return NULL;
}

std::string JSONSerialize::serializeUnknown(void* t, const std::string& className, const std::string& appName)
{
	JSONSerialize serialize;
	return _handleAllSerialization(className,t,appName, &serialize);
}

void* JSONSerialize::unSerializeUnknown(const std::string& objXml, const std::string& className, const std::string& appName)
{
	JSONSerialize serialize;
	return _handleAllUnSerialization(objXml,className,appName,&serialize,true,NULL);
}

bool JSONSerialize::isValidClassNamespace(void* _1, const std::string& classname, const std::string& namespc, const bool& iscontainer)
{
	JSONElement* node = (JSONElement*)_1;
	if(iscontainer && node->getChildren().size()==0)
		return false;
	return true;
}

bool JSONSerialize::isValidObjectProperty(void* _1, const std::string& propname, const int& counter)
{
	JSONElement* node = (JSONElement*)_1;
	if((int)node->getChildren().size()>counter && node->getChildren().at(counter).getName()==propname)
		return true;
	return false;
}

void* JSONSerialize::getObjectProperty(void* _1, const int& counter)
{
	JSONElement* elel = (JSONElement*)_1;
	return (void*)&(elel->getChildren().at(counter));
}

void JSONSerialize::startObjectSerialization(void* _1, const std::string& className)
{
	std::string* object = (std::string*)_1;
	*object = "{";
}

void JSONSerialize::endObjectSerialization(void* _1, const std::string& className)
{
	std::string* object = (std::string*)_1;
	if(object->at(object->length()-1)==',')
		*object = object->substr(0, object->length()-1);
	*object += "}";
}

void JSONSerialize::afterAddObjectProperty(void* _1)
{
	std::string* object = (std::string*)_1;
	*object += ",";
}

void JSONSerialize::addObjectPrimitiveProperty(void* _1, const std::string& propName, const std::string& className, void* t)
{
	std::string* object = (std::string*)_1;

	std::string objXml;
	if(className=="std::string" || className=="string")
	{
		std::string tem = *(std::string*)t;
		objXml = "\""+tem+"\"";
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		std::string temp;
		temp.push_back(tem);
		objXml = "\""+temp+"\"";
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(char*)t;
		std::string temp;
		temp.push_back(tem);
		objXml = "\""+temp+"\"";
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		objXml = CastUtil::lexical_cast<std::string>(tem);
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
	*object += "\"" + propName + "\" : " + objXml;
}

void JSONSerialize::addObjectProperty(void* _1, const std::string& propName, std::string className, const std::string& t)
{
	std::string* object = (std::string*)_1;
	*object += "\"" + propName + "\" : " + t;
}

void* JSONSerialize::getObjectPrimitiveValue(void* _1, const std::string& className, const std::string& propName)
{
	JSONElement* root = (JSONElement*)_1;
	if((className=="signed" || className=="int" || className=="signed int"))
	{
		int *vt = new int;
		*vt = CastUtil::lexical_cast<int>(root->getValue());
		return vt;
	}
	else if((className=="unsigned" || className=="unsigned int"))
	{
		unsigned int *vt = new unsigned int;
		*vt = CastUtil::lexical_cast<unsigned int>(root->getValue());
		return vt;
	}
	else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int"))
	{
		short *vt = new short;
		*vt = CastUtil::lexical_cast<short>(root->getValue());
		return vt;
	}
	else if((className=="unsigned short" || className=="unsigned short int"))
	{
		unsigned short *vt = new unsigned short;
		*vt = CastUtil::lexical_cast<unsigned short>(root->getValue());
		return vt;
	}
	else if((className=="long" || className=="long int" || className=="signed long" || className=="signed long int"))
	{
		long *vt = new long;
		*vt = CastUtil::lexical_cast<long>(root->getValue());
		return vt;
	}
	else if((className=="unsigned long" || className=="unsigned long int"))
	{
		unsigned long *vt = new unsigned long;
		*vt = CastUtil::lexical_cast<unsigned long>(root->getValue());
		return vt;
	}
	else if((className=="long long" || className=="long long int" || className=="signed long long int"))
	{
		long long *vt = new long long;
		*vt = CastUtil::lexical_cast<long long>(root->getValue());
		return vt;
	}
	else if((className=="unsigned long long" || className=="unsigned long long int"))
	{
		unsigned long long *vt = new unsigned long long;
		*vt = CastUtil::lexical_cast<unsigned long long>(root->getValue());
		return vt;
	}
	else if((className=="char" || className=="signed char"))
	{
		char *vt = new char;
		*vt = root->getValue().at(0);
		return vt;
	}
	else if(className=="unsigned char")
	{
		unsigned char *vt = new unsigned char;
		*vt = root->getValue().at(0);
		return vt;
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		return formt.parse(root->getValue());
	}
	else if(className=="BinaryData")
	{
		return BinaryData::unSerilaize(root->getValue());
	}
	else if(className=="float")
	{
		float *vt = new float;
		*vt = CastUtil::lexical_cast<float>(root->getValue());
		return vt;
	}
	else if(className=="double")
	{
		double *vt = new double;
		*vt = CastUtil::lexical_cast<double>(root->getValue());
		return vt;
	}
	else if(className=="long double")
	{
		long double *vt = new long double;
		*vt = CastUtil::lexical_cast<long double>(root->getValue());
		return vt;
	}
	else if(className=="bool")
	{
		bool *vt = new bool;
		*vt = CastUtil::lexical_cast<bool>(root->getValue());
		return vt;
	}
	else if(className=="std::string" || className=="string")
	{
		std::string *vt = new std::string;
		*vt = root->getValue();
		return vt;
	}
	return NULL;
}

std::string JSONSerialize::serializeUnknownBase(void* t, const std::string& className, const std::string& appName)
{
	return _handleAllSerialization(className,t,appName, this);
}
void* JSONSerialize::unSerializeUnknownBase(void* unserObj, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization("",className,appName,this,true,unserObj);
}
void* JSONSerialize::unSerializeUnknownBase(const std::string& serVal, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(serVal,className,appName,this,true,NULL);
}
