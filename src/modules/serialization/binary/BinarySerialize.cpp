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
 * BinarySerialize.cpp
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#include "BinarySerialize.h"

BinarySerialize::BinarySerialize() {
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		throw std::runtime_error("Cannot load serialization shared library");
	}
	dlibinstantiated = true;
}

BinarySerialize::BinarySerialize(void* dlib) {
	if(dlib == NULL)
	{
		throw std::runtime_error("Cannot load serialization shared library");
	}
	this->dlib = dlib;
	dlibinstantiated = false;
}

BinarySerialize::~BinarySerialize() {
	if(dlibinstantiated)
	{
		dlclose(dlib);
	}
}

std::string BinarySerialize::serializePrimitive(int serOpt, const std::string& className, void* t)
{
	AMEFEncoder enc;
	AMEFObject object;
	switch(serOpt) {
		case 1: object.addPacket(*(std::string*)t, className);break;
		case 2: object.addPacket(*(char*)t, className);break;
		case 3: object.addPacket(*(unsigned char*)t, className);break;
		case 4: object.addPacket(*(int*)t, className);break;
		case 5: object.addPacket(*(unsigned int*)t, className);break;
		case 6: object.addPacket(*(short*)t, className);break;
		case 7: object.addPacket(*(unsigned short*)t, className);break;
		case 8: object.addPacket(*(long*)t, className);break;
		case 9: object.addPacket(*(unsigned long*)t, className);break;
		case 10: object.addPacket(*(long long*)t, className);break;
		case 11: object.addPacket(*(unsigned long long*)t, className);break;
		case 12: object.addPacket(*(float*)t, className);break;
		case 13: object.addPacket(*(double*)t, className);break;
		case 14: object.addPacket(*(long double*)t, className);break;
		case 15: object.addPacket(*(bool*)t, className);break;
		case 16: {
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			object.addPacket(formt.format(*(Date*)t), className);
			break;
		}
		case 17: object.addPacket(BinaryData::serilaize(*(BinaryData*)t), className);break;
	}
	return enc.encodeB(&object);
}

void* BinarySerialize::getSerializableObject()
{
	return new AMEFObject;
}

void BinarySerialize::cleanSerializableObject(void* _1)
{
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	delete object;
}

void BinarySerialize::startContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	AMEFEncoder enc;
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	object->setName(container+"-"+className);
}

void BinarySerialize::endContainerSerialization(void* _1, const std::string& className, const std::string& container){}

void BinarySerialize::afterAddContainerSerializableElement(void* _1, const int& counter, const int& size){}

void BinarySerialize::addContainerSerializableElement(void* _1, const std::string& tem)
{
	AMEFEncoder enc;
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	object->addPacket(tem);
}

void BinarySerialize::addContainerSerializableElementMulti(void* _1, const std::string& tem)
{
	//tem = tem.substr(4);
	AMEFEncoder enc;
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	object->addPacket(tem);
}

std::string BinarySerialize::fromSerializableObjectToString(void* _1)
{
	AMEFEncoder enc;
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	return enc.encodeB(object);
}

std::string BinarySerialize::elementToSerializedString(void* _1, const int& counter)
{
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	return object->getPackets().at(counter)->getValueStr();
}

std::string BinarySerialize::getConatinerElementClassName(void* _1, const std::string& className)
{
	//AMEFObject* root = static_cast<AMEFObject*>(_1);
	std::string stlclassName = className;
	if(stlclassName.find(">")!=std::string::npos)
	{
		stlclassName = stlclassName.substr(stlclassName.find("<")+1);
		return stlclassName.substr(0, stlclassName.find(">"));
	}
	else
	{
		stlclassName = stlclassName.substr(stlclassName.find("<")+1);
		if(stlclassName.find(",")!=std::string::npos)
		{
			return stlclassName.substr(0, stlclassName.find_last_of(','));
		}
	}
	/*if(stlclassName.find("-")!=std::string::npos)
	{
		className = stlclassName.substr(stlclassName.find("-")+1);
	}*/
	return className;
}

void* BinarySerialize::getContainerElement(void* _1, const int& counter, const int& counter1)
{
	AMEFDecoder dec;
	AMEFObject* root = static_cast<AMEFObject*>(_1);
	AMEFObject* root2 = dec.decodeB(root->getPackets().at(counter)->getValue(), true);
	return root2;
}

void BinarySerialize::addPrimitiveElementToContainer(void* _1, int serOpt, const int& counter, const std::string& className, void* cont, const std::string& container)
{
	AMEFDecoder dec;
	AMEFObject* root = static_cast<AMEFObject*>(_1);
	AMEFObject* ele = dec.decodeB(root->getPackets().at(counter)->getValue(), true);
	switch(serOpt) {
		case 1:
		{
			std::string retVal = ele->getValueStr();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 2:
		{
			char retVal = ele->getValueStr().at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 3:
		{
			unsigned char retVal = ele->getValueStr().at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 4:
		{
			int retVal = ele->getIntValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 5:
		{
			unsigned int retVal = ele->getUIntValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 6:
		{
			short retVal = ele->getShortValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 7:
		{
			unsigned short retVal = ele->getUShortValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 8:
		{
			long retVal = ele->getLongValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 9:
		{
			unsigned long retVal = ele->getULongValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 10:
		{
			long long retVal = ele->getLongLongValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 11:
		{
			unsigned long long retVal = ele->getULongLongValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 12:
		{
			float retVal = ele->getFloatValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 13:
		{
			double retVal = ele->getDoubleValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 14:
		{
			long double retVal = ele->getLongDoubleValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 15:
		{
			bool retVal = ele->getBoolValue();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			Date* _d = formt.parse(ele->getValueStr());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, *_d, cont);
			else addValueToNestedContainer(container, *_d, cont);
			delete _d;
			break;
		}
	}
}

void* BinarySerialize::getUnserializableObject(const std::string& _1)
{
	AMEFDecoder dec;
	AMEFObject* root = dec.decodeB(_1, true);
	return root;
}

void BinarySerialize::cleanUnserializableObject(void* _1)
{
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	delete object;
}

void BinarySerialize::cleanValidUnserializableObject(void* _1)
{
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	delete object;
}

void* BinarySerialize::getValidUnserializableObject(const std::string& _1){return NULL;}

int BinarySerialize::getContainerSize(void* _1)
{
	AMEFObject* root = static_cast<AMEFObject*>(_1);
	return root->getPackets().size();
}

std::string BinarySerialize::getUnserializableClassName(void* _1, const std::string& className)
{
	AMEFObject* root = static_cast<AMEFObject*>(_1);
	return root->getNameStr();
}

void* BinarySerialize::getPrimitiveValue(void* _1, int serOpt, const std::string& className)
{
	AMEFObject* root = static_cast<AMEFObject*>(_1);
	root = root->getPackets().at(0);

	if(className!=root->getNameStr())
		return NULL;

	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = root->getValueStr();
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = root->getValueStr().at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getValueStr().at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = root->getIntValue();
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = root->getUIntValue();
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = root->getShortValue();
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = root->getUShortValue();
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = root->getLongValue();
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = root->getULongValue();
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = root->getLongLongValue();
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = root->getULongLongValue();
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = root->getFloatValue();
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = root->getDoubleValue();
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = root->getLongDoubleValue();
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = root->getBoolValue();
			return vt;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(root->getValueStr());
		}
		case 17:
		{
			return BinaryData::unSerilaize(root->getValueStr());
		}
	}
	return NULL;
}

std::string BinarySerialize::serializeUnknown(void* t, int serOpt, const std::string& className, const std::string& appName)
{
	BinarySerialize serialize;
	return _handleAllSerialization(serOpt,className,t,appName, &serialize);
}

void* BinarySerialize::unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName)
{
	BinarySerialize serialize;
	return _handleAllUnSerialization(objXml,serOpt,className,appName,&serialize,false,NULL);
}

bool BinarySerialize::isValidClassNamespace(void* _1, const std::string& cn, const std::string& namespc, const bool& iscontainer)
{
	/*StringUtil::replaceAll(namespc, "::", "_");
	StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);*/
	std::string className = cn;
	if(className.find('-')!=std::string::npos)
	{
		std::string pre = className.substr(0, className.find_last_of("-")+1);
		className = className.substr(className.find_last_of("-")+1);
		className = pre + namespc + className;
	}
	else
		className = namespc + className;
	AMEFObject* element = static_cast<AMEFObject*>(_1);
	if(element->getNameStr()!=className || (iscontainer &&  element->getPackets().size()==0))
		return false;
	return true;
}

bool BinarySerialize::isValidObjectProperty(void* _1, const std::string& propname, const int& counter)
{
	AMEFObject* element = static_cast<AMEFObject*>(_1);
	if((int)element->getPackets().size()>counter && element->getPackets().at(counter)->getNameStr()==propname)
		return true;
	return false;
}

void* BinarySerialize::getObjectProperty(void* _1, const int& counter)
{
	AMEFObject* element = static_cast<AMEFObject*>(_1);
	return element->getPackets().at(counter);
}

void BinarySerialize::startObjectSerialization(void* _1, const std::string& className)
{
	AMEFEncoder enc;
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	object->setName(className);
}

void BinarySerialize::endObjectSerialization(void* _1, const std::string& className){}

void BinarySerialize::afterAddObjectProperty(void* _1){}

void BinarySerialize::addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t)
{
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	switch(serOpt) {
		case 1:
		{
			object->addPacket(*(std::string*)t, propName);
			break;
		}
		case 2:
		{
			object->addPacket(*(char*)t, propName);
			break;
		}
		case 3:
		{
			object->addPacket(*(unsigned char*)t, propName);
			break;
		}
		case 4:
		{
			object->addPacket(*(int*)t, propName);
			break;
		}
		case 5:
		{
			object->addPacket(*(unsigned int*)t, propName);
			break;
		}
		case 6:
		{
			object->addPacket(*(short*)t, propName);
			break;
		}
		case 7:
		{
			object->addPacket(*(unsigned short*)t, propName);
			break;
		}
		case 8:
		{
			object->addPacket(*(long*)t, propName);
			break;
		}
		case 9:
		{
			object->addPacket(*(unsigned long*)t, propName);
			break;
		}
		case 10:
		{
			object->addPacket(*(long long*)t, propName);
			break;
		}
		case 11:
		{
			object->addPacket(*(unsigned long long*)t, propName);
			break;
		}
		case 12:
		{
			object->addPacket(*(float*)t, propName);
			break;
		}
		case 13:
		{
			object->addPacket(*(double*)t, propName);
			break;
		}
		case 14:
		{
			object->addPacket(*(long double*)t, propName);
			break;
		}
		case 15:
		{
			object->addPacket(*(bool*)t, propName);
			break;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			object->addPacket(formt.format((Date*)t), propName);
			break;
		}
	}
}

void BinarySerialize::addObjectProperty(void* _1, const std::string& propName, std::string className, const std::string& t)
{
	AMEFObject* object = static_cast<AMEFObject*>(_1);
	object->addPacket(t, propName);
}

void* BinarySerialize::getObjectPrimitiveValue(void* _1, int serOpt, const std::string& className, const std::string& propName)
{
	AMEFObject* root = static_cast<AMEFObject*>(_1);
	root = root->getPackets().at(0);

	if(propName!=root->getNameStr())
		return NULL;

	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = root->getValueStr();
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = root->getValueStr().at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getValueStr().at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = root->getIntValue();
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = root->getUIntValue();
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = root->getShortValue();
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = root->getUShortValue();
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = root->getLongValue();
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = root->getULongValue();
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = root->getLongLongValue();
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = root->getULongLongValue();
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = root->getFloatValue();
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = root->getDoubleValue();
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = root->getLongDoubleValue();
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = root->getBoolValue();
			return vt;
		}
		case 16:
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(root->getValueStr());
		}
		case 17:
		{
			return BinaryData::unSerilaize(root->getValueStr());
		}
	}
	return NULL;
}

std::string BinarySerialize::serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllSerialization(serOpt,className,t,appName, this);
}
void* BinarySerialize::unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization("",serOpt,className,appName,this,false,unserObj);
}
void* BinarySerialize::unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(serVal,serOpt,className,appName,this,false,NULL);
}
