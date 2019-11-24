/*
 * XMLSerialize.cpp
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#include "XMLSerialize.h"

XMLSerialize XMLSerialize::_i;

XMLSerialize::XMLSerialize() {
}

XMLSerialize::XMLSerialize(void* dlib) {
}

XMLSerialize::~XMLSerialize() {
}

std::string XMLSerialize::serializePrimitive(int serOpt, const std::string& className, void* t)
{
	switch(serOpt) {
		case 1: return "<string>"+*(std::string*)t+"</string>";
		case 2: return "<string>"+std::string((char*)t)+"</string>";
		case 3: return "<string>"+std::string((char*)t)+"</string>";
		case 4: return "<int>"+CastUtil::fromNumber(*(int*)t)+"</int>";
		case 5: return "<uint>"+CastUtil::fromNumber(*(unsigned int*)t)+"</uint>";
		case 6: return "<short>"+CastUtil::fromNumber(*(short*)t)+"</short>";
		case 7: return "<ushort>"+CastUtil::fromNumber(*(unsigned short*)t)+"</ushort>";
		case 8: return "<long>"+CastUtil::fromNumber(*(long*)t)+"</long>";
		case 9: return "<ulong>"+CastUtil::fromNumber(*(unsigned long*)t)+"</ulong>";
		case 10: return "<llong>"+CastUtil::fromNumber(*(long long*)t)+"</llong>";
		case 11: return "<ullong>"+CastUtil::fromNumber(*(unsigned long long*)t)+"</ullong>";
		case 12: return "<float>"+CastUtil::fromFloat(*(float*)t)+"</float>";
		case 13: return "<double>"+CastUtil::fromDouble(*(double*)t)+"</double>";
		case 14: return "<ldouble>"+CastUtil::fromLongdouble(*(long double*)t)+"</ldouble>";
		case 15: return "<bool>"+CastUtil::fromBool(*(bool*)t)+"</bool>";
		case 16: {
			DateFormat formt;
			return "<Date>"+formt.format(*(Date*)t)+"</Date>";
		}
		case 17: return "<BinaryData>"+BinaryData::serilaize(*(BinaryData*)t)+"</BinaryData>";
	}
	return "";
}

void* XMLSerialize::getSerializableObject()
{
	return new std::string;
}

void XMLSerialize::cleanSerializableObject(void* _1)
{
	std::string* object = (std::string*)_1;
	delete object;
}

void XMLSerialize::startContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	std::string clsn = StringUtil::replaceFirstCopy(className, "std::", "");
	std::string cntn = StringUtil::replaceFirstCopy(container, "std::", "");
	std::string* object = (std::string*)_1;
	*object = "<"+cntn+"-"+clsn+">";
}

void XMLSerialize::endContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	std::string clsn = StringUtil::replaceFirstCopy(className, "std::", "");
	std::string cntn = StringUtil::replaceFirstCopy(container, "std::", "");
	std::string* object = (std::string*)_1;
	*object += "</"+cntn+"-"+clsn+">";
}

void XMLSerialize::afterAddContainerSerializableElement(void* _1, const int& counter, const int& size){}

void XMLSerialize::addContainerSerializableElement(void* _1, const std::string& tem)
{
	std::string* object = (std::string*)_1;
	*object += tem;
}

void XMLSerialize::addContainerSerializableElementMulti(void* _1, const std::string& tem)
{
	std::string* object = (std::string*)_1;
	*object += tem;
}

std::string XMLSerialize::fromSerializableObjectToString(void* _1)
{
	std::string* object = (std::string*)_1;
	return *object;
}

std::string XMLSerialize::elementToSerializedString(void* _1, const int& counter)
{
	Element* object = (Element*)_1;
	return object->getChildElements().at(counter).renderChildren();
}

std::string XMLSerialize::getConatinerElementClassName(void* _1, const std::string& className)
{
	Element* root = (Element*)_1;
	std::string stlclassName = root->getTagName();
	if(stlclassName.find("-")!=std::string::npos)
	{
		return stlclassName.substr(stlclassName.find_last_of("-")+1);
	}
	return className;
}

void* XMLSerialize::getContainerElement(void* _1, const int& counter, const int& counter1)
{
	Element* root = (Element*)_1;
	if((int)root->getChildElements().size()<counter)
		return NULL;
	Element* ele = (Element*)&(root->getChildElements().at(counter));
	if(counter1!=-1)
	{
		if((int)ele->getChildElements().size()<counter1)
			return NULL;
		ele = (Element*)&(ele->getChildElements().at(counter1));
	}
	return ele;
}

void XMLSerialize::addPrimitiveElementToContainer(void* _1, int serOpt, const int& counter, const std::string& className, void* cont, const std::string& container)
{
	Element* root = (Element*)_1;
	Element* ele = (Element*)&(root->getChildElements().at(counter));
	switch(serOpt) {
		case 1:
		{
			std::string retVal = ele->getText();
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 2:
		{
			char retVal = ele->getText().at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 3:
		{
			unsigned char retVal = ele->getText().at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 4:
		{
			int retVal = CastUtil::toInt(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 5:
		{
			unsigned int retVal = CastUtil::toUInt(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 6:
		{
			short retVal = CastUtil::toShort(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 7:
		{
			unsigned short retVal = CastUtil::toUShort(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 8:
		{
			long retVal = CastUtil::toLong(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 9:
		{
			unsigned long retVal = CastUtil::toULong(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 10:
		{
			long long retVal = CastUtil::toLonglong(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 11:
		{
			unsigned long long retVal = CastUtil::toULonglong(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 12:
		{
			float retVal = CastUtil::toFloat(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 13:
		{
			double retVal = CastUtil::toDouble(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 14:
		{
			long double retVal = CastUtil::toLongdouble(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 15:
		{
			bool retVal = CastUtil::toBool(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 16:
		{
			DateFormat formt;
			Date* _d = formt.parse(ele->getText());
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, *_d, cont);
			else addValueToNestedContainer(container, *_d, cont);
			delete _d;
			break;
		}
	}
}

void* XMLSerialize::getUnserializableObject(const std::string& _1)
{
	XmlParser parser("Parser");
	try
	{
		Document doc;
		parser.parse(_1, doc);
		Element* message = new Element;
		doc.getRootElement().copy(message);
		return message;
	} catch(const XmlParseException& str) {
		std::cout << str.getMessage() << std::endl;
	} catch(const std::exception& e) {
		std::cout << "XML Parse Error" << std::endl;
	}
	return NULL;
}

void XMLSerialize::cleanUnserializableObject(void* _1)
{
	Element* object = (Element*)_1;
	delete object;
}

void XMLSerialize::cleanValidUnserializableObject(void* _1)
{
	Element* object = (Element*)_1;
	delete object;
}

void* XMLSerialize::getValidUnserializableObject(const std::string& _1){return NULL;}

int XMLSerialize::getContainerSize(void* _1)
{
	Element* root = (Element*)_1;
	return root->getChildElements().size();
}

std::string XMLSerialize::getUnserializableClassName(void* _1, const std::string& className)
{
	Element* root = (Element*)_1;
	return root->getTagName();
}

void* XMLSerialize::getPrimitiveValue(void* _1, int serOpt, const std::string& className)
{
	Element* root = (Element*)_1;
	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = root->getText();
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = root->getText().at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getText().at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = CastUtil::toInt(root->getText());
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::toUInt(root->getText());
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::toShort(root->getText());
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::toUShort(root->getText());
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::toLong(root->getText());
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::toLong(root->getText());
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::toLonglong(root->getText());
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::toLong(root->getText());
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::toFloat(root->getText());
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::toDouble(root->getText());
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::toLongdouble(root->getText());
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::toBool(root->getText());
			return vt;
		}
		case 16:
		{
			DateFormat formt;
			return formt.parse(root->getText());
		}
		case 17:
		{
			return BinaryData::unSerilaize(root->getText());
		}
	}
	return NULL;
}

std::string XMLSerialize::serializeUnknown(void* t, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllSerialization(serOpt,className,t,appName, &_i, NULL, NULL, NULL);
}

void* XMLSerialize::unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(objXml,serOpt,className,appName,&_i,false,NULL);
}

bool XMLSerialize::isValidClassNamespace(void* _1, const std::string& className, const std::string& namespc, const bool& iscontainer)
{
	std::string tnmspc = namespc;
	StringUtil::replaceAll(tnmspc, "::", "_");
	/*StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);*/
	std::string cn = className;
	if(cn.find('-')!=std::string::npos)
	{
		std::string pre = cn.substr(0, cn.find_last_of("-")+1);
		cn = cn.substr(cn.find_last_of("-")+1);
		cn = pre + tnmspc + cn;
	}
	else
		cn = tnmspc + cn;
	Element* element = (Element*)_1;
	if(element->getTagName()!=cn || (iscontainer &&  element->getChildElements().size()==0))
		return false;
	return true;
}

bool XMLSerialize::isValidObjectProperty(void* _1, const std::string& propname, const int& counter)
{
	Element* element = (Element*)_1;
	if((int)element->getChildElements().size()>counter && element->getChildElements().at(counter).getTagName()==propname)
		return true;
	return false;
}

void* XMLSerialize::getObjectProperty(void* _1, const int& counter)
{
	Element* elel = (Element*)_1;
	return (void*)&(elel->getChildElements().at(counter));
}

void XMLSerialize::startObjectSerialization(void* _1, const std::string& className)
{
	std::string* object = (std::string*)_1;
	*object = "<"+className+">";
}

void XMLSerialize::endObjectSerialization(void* _1, const std::string& className)
{
	std::string* object = (std::string*)_1;
	*object += "</"+className+">";
}

void XMLSerialize::afterAddObjectProperty(void* _1){}

void XMLSerialize::addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t)
{
	std::string* object = (std::string*)_1;
	std::string objXml;
	switch(serOpt) {
		case 1:
		{
			*object += "<" + propName + ">" + *(std::string*)t + "</" + propName + ">";
			break;
		}
		case 2:
		{
			*object += "<" + propName + ">" + std::string((char*)t) + "</" + propName + ">";
			break;
		}
		case 3:
		{
			*object += "<" + propName + ">" + std::string((char*)t) + "</" + propName + ">";
			break;
		}
		case 4:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(int*)t) + "</" + propName + ">";
			break;
		}
		case 5:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(unsigned int*)t) + "</" + propName + ">";
			break;
		}
		case 6:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(short*)t) + "</" + propName + ">";
			break;
		}
		case 7:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(unsigned short*)t) + "</" + propName + ">";
			break;
		}
		case 8:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(long*)t) + "</" + propName + ">";
			break;
		}
		case 9:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(unsigned long*)t) + "</" + propName + ">";
			break;
		}
		case 10:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(long long*)t) + "</" + propName + ">";
			break;
		}
		case 11:
		{
			*object += "<" + propName + ">" + CastUtil::fromNumber(*(unsigned long long*)t) + "</" + propName + ">";
			break;
		}
		case 12:
		{
			*object += "<" + propName + ">" + CastUtil::fromFloat(*(float*)t) + "</" + propName + ">";
			break;
		}
		case 13:
		{
			*object += "<" + propName + ">" + CastUtil::fromDouble(*(double*)t) + "</" + propName + ">";
			break;
		}
		case 14:
		{
			*object += "<" + propName + ">" + CastUtil::fromLongdouble(*(long double*)t) + "</" + propName + ">";
			break;
		}
		case 15:
		{
			*object += "<" + propName + ">" + CastUtil::fromBool(*(bool*)t) + "</" + propName + ">";
			break;
		}
		case 16:
		{
			DateFormat formt;
			*object += "<" + propName + ">" + formt.format((Date*)t) + "</" + propName + ">";
			break;
		}
		case 17:
		{
			*object += "<" + propName + ">" + BinaryData::serilaize(*(BinaryData*)t) + "</" + propName + ">";
		}
	}
}

void XMLSerialize::addObjectProperty(void* _1, const std::string& propName, std::string className, const std::string& t)
{
	std::string* object = (std::string*)_1;

	StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);
	//TODO - any side effects??
	//*object += "<" + propName + " type=\"" + className + "\">" + t + "</" + propName + ">";
	*object += "<" + propName + ">" + t + "</" + propName + ">";
}

void* XMLSerialize::getObjectPrimitiveValue(void* _1, int serOpt, const std::string& className, const std::string& propName)
{
	Element* root = (Element*)_1;
	if(root->getTagName()!=propName)
		return NULL;

	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = root->getText();
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = root->getText().at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getText().at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = CastUtil::toInt(root->getText());
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::toUInt(root->getText());
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::toShort(root->getText());
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::toUShort(root->getText());
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::toLong(root->getText());
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::toLong(root->getText());
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::toLonglong(root->getText());
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::toLong(root->getText());
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::toFloat(root->getText());
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::toDouble(root->getText());
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::toLongdouble(root->getText());
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::toBool(root->getText());
			return vt;
		}
		case 16:
		{
			DateFormat formt;
			return formt.parse(root->getText());
		}
		case 17:
		{
			return BinaryData::unSerilaize(root->getText());
		}
	}
	return NULL;
}

std::string XMLSerialize::serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllSerialization(serOpt,className,t,appName, this, NULL, NULL, NULL);
}
void* XMLSerialize::unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization("",serOpt,className,appName,this,false,unserObj);
}
void* XMLSerialize::unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(serVal,serOpt,className,appName,this,false,NULL);
}
