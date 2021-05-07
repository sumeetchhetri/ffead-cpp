/*
	Copyright 2009-2020, Sumeet Chhetri

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

JSONSerialize JSONSerialize::_i;

JSONSerialize::JSONSerialize() {
}

JSONSerialize::JSONSerialize(void* dlib) {
}

JSONSerialize::~JSONSerialize() {
}

std::string JSONSerialize::serializePrimitive(int serOpt, const std::string& className, void* t, void* serobject)
{
#ifdef RAPID_JSON
	RapiJsonState* str = (RapiJsonState*)serobject;
#else
	std::string* str = (std::string*)serobject;
#endif
	switch(serOpt) {
		case 1: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				std::string* tt = *(std::string*)t;
				str->writer.String(tt->data(), tt->size());
#else
				str->append("\""+*(std::string*)t+"\"");
#endif
				return CommonUtils::BLANK;
			}
			return "\""+*(std::string*)t+"\"";
		}
		case 2: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Uint(((unsigned char*)t)[0]);
#else
				str->push_back(((unsigned char*)t)[0]);
#endif
				return CommonUtils::BLANK;
			}
			std::string s;
			s.push_back(((unsigned char*)t)[0]);
			return "\""+s+"\"";
		}
		case 3: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Uint(((char*)t)[0]);
#else
				str->push_back(((char*)t)[0]);
#endif
				return CommonUtils::BLANK;
			}
			std::string s;
			s.push_back(((char*)t)[0]);
			return "\""+s+"\"";
		}
		case 4: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Int(*(int*)t);
#else
				CastUtil::fromNumber(*(int*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(int*)t);
		}
		case 5: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Uint(*(unsigned int*)t);
#else
				CastUtil::fromNumber(*(unsigned int*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(unsigned int*)t);
		}
		case 6: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Int(*(short*)t);
#else
				CastUtil::fromNumber(*(short*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(short*)t);
		}
		case 7: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Int(*(unsigned short*)t);
#else
				CastUtil::fromNumber(*(unsigned short*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(unsigned short*)t);
		}
		case 8: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Int(*(long*)t);
#else
				CastUtil::fromNumber(*(long*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(long*)t);
		}
		case 9: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Uint(*(unsigned long*)t);
#else
				CastUtil::fromNumber(*(unsigned long*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(unsigned long*)t);
		}
		case 10: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Int64(*(long long*)t);
#else
				CastUtil::fromNumber(*(long long*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(long long*)t);
		}
		case 11: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Uint64(*(unsigned long long*)t);
#else
				CastUtil::fromNumber(*(unsigned long long*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromNumber(*(unsigned long long*)t);
		}
		case 12: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Double(*(float*)t);
#else
				CastUtil::fromFloat(*(float*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromFloat(*(float*)t);
		}
		case 13: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Double(*(double*)t);
#else
				CastUtil::fromDouble(*(double*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromDouble(*(double*)t);
		}
		case 14: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Double(*(long double*)t);
#else
				CastUtil::fromLongdouble(*(long double*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromLongdouble(*(long double*)t);
		}
		case 15: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				str->writer.Bool(*(bool*)t);
#else
				CastUtil::fromBool(*(bool*)t, str);
#endif
				return CommonUtils::BLANK;
			}
			return CastUtil::fromBool(*(bool*)t);
		}
		case 16: {
			DateFormat formt;
			if(str!=NULL) {
#ifdef RAPID_JSON
				std::string tt = formt.format(*(Date*)t);
				str->writer.String(tt.data(), tt.size());
#else
				str->append("\""+formt.format(*(Date*)t)+"\"");
#endif
				return CommonUtils::BLANK;
			}
			return "\""+formt.format(*(Date*)t)+"\"";
		}
		case 17: {
			if(str!=NULL) {
#ifdef RAPID_JSON
				std::string tt = BinaryData::serilaize(*(BinaryData*)t);
				str->writer.String(tt.data(), tt.size());
#else
				str->append("\""+BinaryData::serilaize(*(BinaryData*)t)+"\"");
#endif
				return CommonUtils::BLANK;
			}
			return "\""+BinaryData::serilaize(*(BinaryData*)t)+"\"";
		}
	}
	return CommonUtils::BLANK;
}

void* JSONSerialize::getSerializableObject()
{
#ifdef RAPID_JSON
	return new RapiJsonState();
#else
	return new std::string;
#endif
}

void JSONSerialize::cleanSerializableObject(void* _1)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	delete object;
#else
	std::string* object = (std::string*)_1;
	delete object;
#endif
}

void JSONSerialize::startContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.StartArray();
#else
	std::string* object = (std::string*)_1;
	*object += "[";
#endif
}

void JSONSerialize::endContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.EndArray();
#else
	std::string* object = (std::string*)_1;
	if(object->at(object->length()-1)==',')
		*object = object->substr(0, object->length()-1);
	*object += "]";
#endif
}

void JSONSerialize::afterAddContainerSerializableElement(void* _1, const int& counter, const int& size)
{
#ifndef RAPID_JSON
	std::string* object = (std::string*)_1;
	//if(counter!=size)
		*object += ",";
#endif
}

void JSONSerialize::addContainerSerializableElement(void* _1, const std::string& tem)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.RawValue(tem.data(), tem.size(), rapidjson::Type::kArrayType);
#else
	std::string* object = (std::string*)_1;
	*object += tem;
#endif
}

void JSONSerialize::addContainerSerializableElementMulti(void* _1, const std::string& tem)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.RawValue(tem.data(), tem.size(), rapidjson::Type::kArrayType);
#else
	std::string* object = (std::string*)_1;
	*object += tem;
#endif
}

std::string JSONSerialize::fromSerializableObjectToString(void* _1)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	return std::string(object->s.GetString(), object->s.GetSize());
#else
	std::string* object = (std::string*)_1;
	return *object;
#endif
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
			int retVal = CastUtil::toInt(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 5:
		{
			unsigned int retVal = CastUtil::toUInt(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 6:
		{
			short retVal = CastUtil::toShort(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 7:
		{
			unsigned short retVal = CastUtil::toUShort(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 8:
		{
			long retVal = CastUtil::toLong(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 9:
		{
			unsigned long retVal = CastUtil::toULong(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 10:
		{
			long long retVal = CastUtil::toLonglong(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 11:
		{
			unsigned long long retVal = CastUtil::toULonglong(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 12:
		{
			float retVal = CastUtil::toFloat(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 13:
		{
			double retVal = CastUtil::toDouble(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 14:
		{
			long double retVal = CastUtil::toLongdouble(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 15:
		{
			bool retVal = CastUtil::toBool(ele->getValue());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 16:
		{
			DateFormat formt;
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
			*vt = CastUtil::toInt(*root);
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::toUInt(*root);
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::toShort(*root);
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::toUShort(*root);
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::toLong(*root);
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::toLong(*root);
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::toLonglong(*root);
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::toLong(*root);
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::toFloat(*root);
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::toDouble(*root);
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::toLongdouble(*root);
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::toBool(*root);
			return vt;
		}
		case 16:
		{
			DateFormat formt;
			return formt.parse(*root);
		}
		case 17:
		{
			return BinaryData::unSerilaize(*root);
		}
	}
	return NULL;
}

std::string JSONSerialize::serializeUnknown(void* t, int serOpt, const std::string& className, Ser f1, SerCont f2, SerCont f3, void* serobject, const std::string& appName)
{
	return _handleAllSerialization(serOpt,className,t,appName,&_i, f1, f2, f3, serobject);
}

std::string JSONSerialize::serializeUnknown(void* t, int serOpt, const std::string& className, void* serobject, const std::string& appName)
{
	return _handleAllSerialization(serOpt,className,t,appName,&_i, NULL, NULL, NULL, serobject);
}

void* JSONSerialize::unSerializeUnknown(const std::string& objJson, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(objJson,serOpt,className,appName,&_i,true,NULL);
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
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.StartObject();
#else
	std::string* object = (std::string*)_1;
	*object += "{";
#endif
}

void JSONSerialize::endObjectSerialization(void* _1, const std::string& className)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.EndObject();
#else
	std::string* object = (std::string*)_1;
	if(object->at(object->length()-1)==',')
		*object = object->substr(0, object->length()-1);
	*object += "}";
#endif
}

void JSONSerialize::afterAddObjectProperty(void* _1, const std::string& propName)
{
#ifndef RAPID_JSON
	std::string* object = (std::string*)_1;
	*object += ",";
#endif
}

void JSONSerialize::addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
#else
	std::string* object = (std::string*)_1;
#endif
	switch(serOpt) {
		case 1:
		{
#ifdef RAPID_JSON
			std::string& tt = *(std::string*)t;
			object->writer.Key(propName.data(), propName.size());
			object->writer.String(tt.data(), tt.size());
#else
			*object += "\"" + propName + "\":\"" + *(std::string*)t + "\"";
#endif
			break;
		}
		case 2:
		{
			std::string s;
			s.push_back(((char*)t)[0]);
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.String(s.data(), s.size());
#else
			*object += "\"" + propName + "\":\"" + s + "\"";
#endif
			break;
		}
		case 3:
		{
			std::string s;
			s.push_back(((unsigned char*)t)[0]);
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.String(s.data(), s.size());
#else
			*object += "\"" + propName + "\":\"" + s + "\"";
#endif
			break;
		}
		case 4:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Int(*(int*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(int*)t);
#endif
			break;
		}
		case 5:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Uint(*(unsigned int*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(unsigned int*)t);
#endif
			break;
		}
		case 6:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Int(*(short*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(short*)t);
#endif
			break;
		}
		case 7:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Int(*(unsigned short*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(unsigned short*)t);
#endif
			break;
		}
		case 8:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Int(*(long*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(long*)t);
#endif
			break;
		}
		case 9:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Uint(*(unsigned long*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(unsigned long*)t);
#endif
			break;
		}
		case 10:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Int64(*(long long*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(long long*)t);
#endif
			break;
		}
		case 11:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Uint64(*(unsigned long long*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromNumber(*(unsigned long long*)t);
#endif
			break;
		}
		case 12:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Double(*(float*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromFloat(*(float*)t);
#endif
			break;
		}
		case 13:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Double(*(double*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromDouble(*(double*)t);
#endif
			break;
		}
		case 14:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Double(*(long double*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromLongdouble(*(long double*)t);
#endif
			break;
		}
		case 15:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			object->writer.Bool(*(bool*)t);
#else
			*object += "\"" + propName + "\":" + CastUtil::fromBool(*(bool*)t);
#endif
			break;
		}
		case 16:
		{
			DateFormat formt;
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			std::string s = formt.format((Date*)t);
			object->writer.String(s.data(), s.size());
#else
			*object += "\"" + propName + "\":\"" + formt.format((Date*)t) + "\"";
#endif
			break;
		}
		case 17:
		{
#ifdef RAPID_JSON
			object->writer.Key(propName.data(), propName.size());
			std::string s = BinaryData::serilaize(*(BinaryData*)t);
			object->writer.String(s.data(), s.size());
#else
			*object += "\"" + propName + "\":\"" + BinaryData::serilaize(*(BinaryData*)t) + "\"";
#endif
		}
	}
}

void JSONSerialize::addObjectProperty(void* _1, const std::string& propName, std::string className)
{
#ifdef RAPID_JSON
	RapiJsonState* object = (RapiJsonState*)_1;
	object->writer.Key(propName.data(), propName.size());
#else
	std::string* object = (std::string*)_1;
	*object += "\"" + propName + "\":";
#endif
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
			*vt = CastUtil::toInt(root->getValue());
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::toUInt(root->getValue());
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::toShort(root->getValue());
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::toUShort(root->getValue());
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::toLong(root->getValue());
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::toLong(root->getValue());
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::toLonglong(root->getValue());
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::toLong(root->getValue());
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::toFloat(root->getValue());
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::toDouble(root->getValue());
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::toLongdouble(root->getValue());
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::toBool(root->getValue());
			return vt;
		}
		case 16:
		{
			DateFormat formt;
			return formt.parse(root->getValue());
		}
		case 17:
		{
			return BinaryData::unSerilaize(root->getValue());
		}
	}
	return NULL;
}

std::string JSONSerialize::serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& appName, void* serobject)
{
	return _handleAllSerialization(serOpt,className,t,appName, this, NULL, NULL, NULL, serobject);
}
void* JSONSerialize::unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(CommonUtils::BLANK,serOpt,className,appName,this,true,unserObj);
}
void* JSONSerialize::unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(serVal,serOpt,className,appName,this,true,NULL);
}

std::string Serializer::toJson(const std::string &appName, std::string className, void *object, void *serOutput) {
	return SerializeBase::_ser(object, className, appName, &JSONSerialize::_i, serOutput);
}

std::string Serializer::toJson(const std::string &appName, std::string className, const std::string &container, void *object, void *serOutput) {
	return SerializeBase::_serContainer(object, className, appName, container, &JSONSerialize::_i, serOutput);
}

void* Serializer::fromJson(const std::string &appName, std::string className, void *serObject) {
	return SerializeBase::_unser(serObject, className, appName, &JSONSerialize::_i);
}

void* Serializer::fromJson(const std::string &appName, std::string className, const std::string &container, void *serObject) {
	return SerializeBase::_unserContainer(serObject, className, appName, container, &JSONSerialize::_i);
}

std::string JSONSerialize::serializeObject(void* t, Ser f, void* serobject) {
	std::string serVal;
	if(f!=NULL)
		serVal = f(t, &JSONSerialize::_i, serobject);
	return serVal;
}

std::string JSONSerialize::serializeObjectCont(void* t, SerCont f, const std::string &container, void* serobject) {
	std::string serVal;
	if(f!=NULL)
		serVal = f(t, &JSONSerialize::_i, container, serobject);
	return serVal;
}
