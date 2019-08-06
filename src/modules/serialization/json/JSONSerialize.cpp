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
		throw std::runtime_error("Cannot load serialization shared library");
	}
	dlibinstantiated = true;
}

JSONSerialize::JSONSerialize(void* dlib) {
	if(dlib == NULL)
	{
		throw std::runtime_error("Cannot load serialization shared library");
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

std::string JSONSerialize::serializePrimitive(int serOpt, const std::string& className, void* t)
{
	switch(serOpt) {
		case 1: return "\""+*(std::string*)t+"\"";
		case 2: return "\""+(*(char*)t)[0]+"\"";
		case 3: return "\""+(*(unsigned char*)t)[0]+"\"";
		case 4: return "\""+CastUtil::lexical_cast<std::string>(*(int*)t)+"\"";
		case 5: return "\""+CastUtil::lexical_cast<std::string>(*(unsigned int*)t)+"\"";
		case 6: return "\""+CastUtil::lexical_cast<std::string>(*(short*)t)+"\"";
		case 7: return "\""+CastUtil::lexical_cast<std::string>(*(unsigned short*)t)+"\"";
		case 8: return "\""+CastUtil::lexical_cast<std::string>(*(long*)t)+"\"";
		case 9: return "\""+CastUtil::lexical_cast<std::string>(*(unsigned long*)t)+"\"";
		case 10: return "\""+CastUtil::lexical_cast<std::string>(*(long long*)t)+"\"";
		case 11: return "\""+CastUtil::lexical_cast<std::string>(*(unsigned long long*)t)+"\"";
		case 12: return "\""+CastUtil::lexical_cast<std::string>(*(float*)t)+"\"";
		case 13: return "\""+CastUtil::lexical_cast<std::string>(*(double*)t)+"\"";
		case 14: return "\""+CastUtil::lexical_cast<std::string>(*(long double*)t)+"\"";
		case 15: return "\""+CastUtil::lexical_cast<std::string>(*(bool*)t)+"\"";
		case 16: {
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return "\""+formt.format(*(Date*)t)+"\"";
		}
		case 17: return "\""+BinaryData::serilaize(*(BinaryData*)t)+"\"";
	}
	return "";
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

void JSONSerialize::addPrimitiveElementToContainer(void* _1, int serOpt, const int& counter, const std::string& className, void* cont, const std::string& container)
{
	JSONElement* root = (JSONElement*)_1;
	JSONElement* ele = (JSONElement*)&(root->getChildren().at(counter));
	switch(serOpt) {
		case 1:
		{
			std::string retVal = ele->getValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 2:
		{
			char retVal = ele->getValue().at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 3:
		{
			unsigned char retVal = ele->getValue().at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 4:
		{
			int retVal = CastUtil::lexical_cast<int>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 5:
		{
			unsigned int retVal = CastUtil::lexical_cast<unsigned int>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 6:
		{
			short retVal = CastUtil::lexical_cast<short>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 7:
		{
			unsigned short retVal = CastUtil::lexical_cast<unsigned short>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 8:
		{
			long retVal = CastUtil::lexical_cast<long>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 9:
		{
			unsigned long retVal = CastUtil::lexical_cast<unsigned long>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 10:
		{
			long long retVal = CastUtil::lexical_cast<long long>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 11:
		{
			unsigned long long retVal = CastUtil::lexical_cast<unsigned long long>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 12:
		{
			float retVal = CastUtil::lexical_cast<float>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 13:
		{
			double retVal = CastUtil::lexical_cast<double>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 14:
		{
			long double retVal = CastUtil::lexical_cast<long double>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 15:
		{
			bool retVal = CastUtil::lexical_cast<bool>(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			Date* _d = formt.parse(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, *_d, cont);
			else addValueToNestedContainer(container, *_d, cont);
			delete _d;
			break;
		}
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

void* JSONSerialize::getPrimitiveValue(void* _1, int serOpt, const std::string& className)
{
	std::string* root = (std::string*)_1;
	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = *root;
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = root->at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = root->at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = CastUtil::lexical_cast<int>(*root);
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::lexical_cast<unsigned int>(*root);
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::lexical_cast<short>(*root);
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::lexical_cast<unsigned short>(*root);
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::lexical_cast<long>(*root);
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::lexical_cast<long>(*root);
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::lexical_cast<long long>(*root);
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::lexical_cast<long>(*root);
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::lexical_cast<float>(*root);
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::lexical_cast<double>(*root);
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::lexical_cast<long double>(*root);
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::lexical_cast<bool>(*root);
			return vt;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(*root);
		}
		case 17:
		{
			return BinaryData::unSerilaize(*root);
		}
	}
	return NULL;
}

std::string JSONSerialize::serializeUnknown(void* t, int serOpt, const std::string& className, const std::string& appName)
{
	JSONSerialize serialize;
	return _handleAllSerialization(serOpt,className,t,appName, &serialize);
}

void* JSONSerialize::unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName)
{
	JSONSerialize serialize;
	return _handleAllUnSerialization(objXml,serOpt,className,appName,&serialize,true,NULL);
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

void JSONSerialize::addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t)
{
	std::string* object = (std::string*)_1;
	switch(serOpt) {
		case 1:
		{
			*object += "\"" + propName + "\" : \"" + *(std::string*)t + "\"";
			break;
		}
		case 2:
		{
			*object += "\"" + propName + "\" : \"" + (*(char*)t)[0] + "\"";
			break;
		}
		case 3:
		{
			*object += "\"" + propName + "\" : \"" + (*(unsigned char*)t)[0] + "\"";
			break;
		}
		case 4:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(int*)t);
			break;
		}
		case 5:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(unsigned int*)t);
			break;
		}
		case 6:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(short*)t);
			break;
		}
		case 7:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(unsigned short*)t);
			break;
		}
		case 8:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(long*)t);
			break;
		}
		case 9:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(unsigned long*)t);
			break;
		}
		case 10:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(long long*)t);
			break;
		}
		case 11:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(unsigned long long*)t);
			break;
		}
		case 12:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(float*)t);
			break;
		}
		case 13:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(double*)t);
			break;
		}
		case 14:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(long double*)t);
			break;
		}
		case 15:
		{
			*object += "\"" + propName + "\" : " + CastUtil::lexical_cast<std::string>(*(bool*)t);
			break;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			*object += "\"" + propName + "\" : \"" + formt.format((Date*)t) + "\"";
			break;
		}
		case 17:
		{
			*object += "\"" + propName + "\" : \"" + BinaryData::serilaize(*(BinaryData*)t) + "\"";
		}
	}
}

void JSONSerialize::addObjectProperty(void* _1, const std::string& propName, std::string className, const std::string& t)
{
	std::string* object = (std::string*)_1;
	*object += "\"" + propName + "\" : " + t;
}

void* JSONSerialize::getObjectPrimitiveValue(void* _1, int serOpt, const std::string& className, const std::string& propName)
{
	JSONElement* root = (JSONElement*)_1;
	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = root->getValue();
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = root->getValue().at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getValue().at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = CastUtil::lexical_cast<int>(root->getValue());
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::lexical_cast<unsigned int>(root->getValue());
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::lexical_cast<short>(root->getValue());
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::lexical_cast<unsigned short>(root->getValue());
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::lexical_cast<long>(root->getValue());
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::lexical_cast<long>(root->getValue());
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::lexical_cast<long long>(root->getValue());
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::lexical_cast<long>(root->getValue());
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::lexical_cast<float>(root->getValue());
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::lexical_cast<double>(root->getValue());
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::lexical_cast<long double>(root->getValue());
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::lexical_cast<bool>(root->getValue());
			return vt;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(root->getValue());
		}
		case 17:
		{
			return BinaryData::unSerilaize(root->getValue());
		}
	}
	return NULL;
}

std::string JSONSerialize::serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllSerialization(serOpt,className,t,appName, this);
}
void* JSONSerialize::unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization("",serOpt,className,appName,this,true,unserObj);
}
void* JSONSerialize::unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(serVal,serOpt,className,appName,this,true,NULL);
}
