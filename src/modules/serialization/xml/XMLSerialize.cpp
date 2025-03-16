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

std::string XMLSerialize::serializePrimitive(int serOpt, const std::string& className, void* t, void* serobject)
{
	XmlWriter* str = (XmlWriter*)serobject;
	switch(serOpt) {
		case 1: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(*(std::string*)t);
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+*(std::string*)t+"</element>";
		}
		case 2: {
			if(str!=NULL) {
				str->startElement("element");
				char t[2] = {((char*)t)[0], '\0'};
				str->content((char*)t);
				str->closeElement();
				return CommonUtils::BLANK;
			}
			std::string s;
			s.push_back(((char*)t)[0]);
			return "<element>"+s+"</element>";
		}
		case 3: {
			if(str!=NULL) {
				str->startElement("element");
				unsigned char t[2] = {((unsigned  char*)t)[0], '\0'};
				str->content((char*)t);
				str->closeElement();
				return CommonUtils::BLANK;
			}
			std::string s;
			s.push_back(((unsigned char*)t)[0]);
			return "<element>"+s+"</element>";
		}
		case 4: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(int*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(int*)t)+"</element>";
		}
		case 5: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(unsigned int*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(unsigned int*)t)+"</element>";
		}
		case 6: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(short*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(short*)t)+"</element>";
		}
		case 7: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(unsigned short*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(unsigned short*)t)+"</element>";
		}
		case 8: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(long*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(long*)t)+"</element>";
		}
		case 9: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(unsigned long*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(unsigned long*)t)+"</element>";
		}
		case 10: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(long long*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(long long*)t)+"</element>";
		}
		case 11: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromNumber(*(unsigned long long*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromNumber(*(unsigned long long*)t)+"</element>";
		}
		case 12: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromFloat(*(float*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromFloat(*(float*)t)+"</element>";
		}
		case 13: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromDouble(*(double*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromDouble(*(double*)t)+"</element>";
		}
		case 14: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromLongdouble(*(long double*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromLongdouble(*(long double*)t)+"</element>";
		}
		case 15: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(CastUtil::fromBool(*(bool*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+CastUtil::fromBool(*(bool*)t)+"</element>";
		}
		case 16: {
			DateFormat formt;
			if(str!=NULL) {
				str->startElement("element");
				str->content(formt.format(*(Date*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+formt.format(*(Date*)t)+"</element>";
		}
		case 17: {
			if(str!=NULL) {
				str->startElement("element");
				str->content(BinaryData::serilaize(*(BinaryData*)t));
				str->closeElement();
				return CommonUtils::BLANK;
			}
			return "<element>"+BinaryData::serilaize(*(BinaryData*)t)+"</element>";
		}
	}
	return CommonUtils::BLANK;
}

void* XMLSerialize::getSerializableObject(void* exobj)
{
	return exobj!=NULL?exobj:new SimpleStringXmlWriter;
}

void XMLSerialize::cleanSerializableObject(void* _1)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->closeAll();
	if(xw->writerType()=="stream") {
		delete (SimpleStreamXmlWriter*)_1;
	} else {
		delete (SimpleStringXmlWriter*)_1;
	}
}

void XMLSerialize::startContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->startElement("collection");
	xw->attribute("ctype", container);
	xw->attribute("etype", className);
}

void XMLSerialize::endContainerSerialization(void* _1, const std::string& className, const std::string& container)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->closeElement();
}

void XMLSerialize::afterAddContainerSerializableElement(void* _1, const int& counter, const int& size){}

void XMLSerialize::addContainerSerializableElement(void* _1, const std::string& tem){}

void XMLSerialize::addContainerSerializableElementMulti(void* _1, const std::string& tem){}

std::string XMLSerialize::fromSerializableObjectToString(void* _1)
{
	XmlWriter* xw = (XmlWriter*)_1;
	return xw->toString();
}

std::string XMLSerialize::elementToSerializedString(void* _1, const int& counter)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	int cnt = 0;
	for (pugi::xml_node n: doc.children()) {
		if(cnt++==counter) {
			xml_string_writer xw;
			n.print(xw);
			return xw.result;
		}
	}
	return CommonUtils::BLANK;
#else
	Element* object = (Element*)_1;
	return object->getChildElements().at(counter).renderChildren();
#endif
}

std::string XMLSerialize::getConatinerElementClassName(void* _1, const std::string& className)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	if(strcmp(doc.name(), "collection")==0) {
		return std::string(doc.attribute("etype").value());
	}
#else
	Element* root = (Element*)_1;
	if(root->getTagName()=="collection") {
		return root->getAttribute("etype");
	}
#endif
	return className;
}

void* XMLSerialize::getContainerElement(void* _1, const int& counter, const int& counter1)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	int cnt = 0, cnt1 = 0;
	for (pugi::xml_node c: doc.children()) {
		if(cnt++==counter && strcmp(c.name(), "element")==0) {
			return c.internal_object();
		}
	}
	return NULL;
#else
	Element* root = (Element*)_1;
	if((int)root->getChildElements().size()<counter)
		return NULL;
	Element* ele = (Element*)&(root->getChildElements().at(counter));
	if(ele->getTagName()=="element")
	{
		return ele;
	}
	return NULL;
#endif
}

void XMLSerialize::addPrimitiveElementToContainer(void* _1, int serOpt, const int& counter, const std::string& className, void* cont, const std::string& container)
{
#ifdef HAVE_PUGI_XML
	std::string tval;
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	int cnt = 0;
	for (pugi::xml_node n: doc.children()) {
		if(cnt++==counter) {
			tval.append(n.text().get());
			break;
		}
	}
#else
	Element* root = (Element*)_1;
	Element* ele = (Element*)&(root->getChildElements().at(counter));
	std::string tval = ele->getText();
#endif
	switch(serOpt) {
		case 1:
		{
			std::string retVal = tval;
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 2:
		{
			char retVal = tval.at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 3:
		{
			unsigned char retVal = tval.at(0);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 4:
		{
			int retVal = CastUtil::toInt(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 5:
		{
			unsigned int retVal = CastUtil::toUInt(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 6:
		{
			short retVal = CastUtil::toShort(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 7:
		{
			unsigned short retVal = CastUtil::toUShort(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 8:
		{
			long retVal = CastUtil::toLong(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 9:
		{
			unsigned long retVal = CastUtil::toULong(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 10:
		{
			long long retVal = CastUtil::toLonglong(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 11:
		{
			unsigned long long retVal = CastUtil::toULonglong(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 12:
		{
			float retVal = CastUtil::toFloat(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 13:
		{
			double retVal = CastUtil::toDouble(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 14:
		{
			long double retVal = CastUtil::toLongdouble(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 15:
		{
			bool retVal = CastUtil::toBool(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, retVal, cont);
			else addValueToNestedContainer(container, retVal, cont);
			break;
		}
		case 16:
		{
			DateFormat formt;
			Date* _d = formt.parse(tval);
			if(container=="std::set" || container=="std::multiset") addValueToNestedContainerSV(container, *_d, cont);
			else addValueToNestedContainer(container, *_d, cont);
			delete _d;
			break;
		}
	}
}

void* XMLSerialize::getUnserializableObject(const std::string& _1)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_document* doc = new pugi::xml_document;
	pugi::xml_parse_result result = doc->load_buffer(_1.c_str(), _1.length());
	if (!result) return NULL;
	pugiDoc.set(doc);
	return doc->document_element().internal_object();
#else
	SimpleXmlParser parser("Parser");
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
#endif
}

void XMLSerialize::cleanUnserializableObject(void* _1)
{
#ifdef HAVE_PUGI_XML
	if(pugiDoc.get()!=NULL) {
		pugi::xml_document* doc = (pugi::xml_document*)pugiDoc.get();
		pugiDoc.reset(NULL);
		delete doc;
	}
#else
	Element* object = (Element*)_1;
	delete object;
#endif
}

void XMLSerialize::cleanValidUnserializableObject(void* _1)
{
	//no op
}

void* XMLSerialize::getValidUnserializableObject(const std::string& _1){return NULL;}

int XMLSerialize::getContainerSize(void* _1)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	int counter = 0;
	for (pugi::xml_node tool: doc.children()) counter++;
	return counter;
#else
	Element* root = (Element*)_1;
	return root->getChildElements().size();
#endif
}

std::string XMLSerialize::getUnserializableClassName(void* _1, const std::string& className)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	return std::string(doc.name());
#else
	Element* root = (Element*)_1;
	return root->getTagName();
#endif
}

void* XMLSerialize::getPrimitiveValue(void* _1, int serOpt, const std::string& className)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	std::string tval(doc.text().get());
#else
	Element* root = (Element*)_1;
	std::string tval = root->getText();
#endif
	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = tval;
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = tval.at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = tval.at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = CastUtil::toInt(tval);
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::toUInt(tval);
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::toShort(tval);
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::toUShort(tval);
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::toLong(tval);
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::toLong(tval);
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::toLonglong(tval);
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::toLong(tval);
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::toFloat(tval);
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::toDouble(tval);
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::toLongdouble(tval);
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::toBool(tval);
			return vt;
		}
		case 16:
		{
			DateFormat formt;
			return formt.parse(tval);
		}
		case 17:
		{
			return BinaryData::unSerilaize(tval);
		}
	}
	return NULL;
}

std::string XMLSerialize::serializeUnknown(void* t, int serOpt, const std::string& className, void* serobject, const std::string& appName)
{
	bool soe = serobject!=NULL;
	if(soe) {
		serobject = new SimpleStringXmlWriter((std::string*)serobject);
		_handleAllSerialization(serOpt,className,t,appName, &_i, NULL, NULL, NULL, serobject);
		XMLSerialize::_i.cleanSerializableObject(serobject);
		return CommonUtils::BLANK;
	}
	return _handleAllSerialization(serOpt,className,t,appName, &_i, NULL, NULL, NULL, serobject);
}

void* XMLSerialize::unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(objXml,serOpt,className,appName,&_i,false,NULL);
}

bool XMLSerialize::isValidClassNamespace(void* _1, const std::string& className, const std::string& namespc, const bool& iscontainer)
{
	std::string cn = namespc+className;
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	if(!iscontainer && strcmp(doc.name(), "element")==0 && strcmp(doc.attribute("type").value(), cn.c_str())==0) {
		return true;
	}
	if(iscontainer && strcmp(doc.name(), "collection")==0 && strcmp(doc.attribute("etype").value(), cn.c_str())==0) {
		return true;
	}
#else
	Element* element = (Element*)_1;
	if(!iscontainer && element->getTagName()=="element" && element->getAttribute("type")==cn) {
		return true;
	}
	if(iscontainer && element->getTagName()=="collection" && element->getAttribute("etype")==cn) {
		return true;
	}
#endif
	return false;
}

bool XMLSerialize::isValidObjectProperty(void* _1, const std::string& propname, const int& counter)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	for (pugi::xml_node n: doc.children()) {
		if(strcmp(n.name(), "property")==0 && strcmp(n.attribute("name").value(), propname.c_str())==0) {
			return true;
		}
	}
#else
	Element* elel = (Element*)_1;
	for(auto& el: elel->getChildElements()) {
		if(el.getTagName()=="property" && el.getAttribute("name")==propname) {
			return true;
		}
	}
#endif
	return false;
}

void* XMLSerialize::getObjectProperty(void* _1, const int& counter, const std::string& propname)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	for (pugi::xml_node n: doc.children()) {
		if(strcmp(n.name(), "property")==0 && strcmp(n.attribute("name").value(), propname.c_str())==0) {
			if(n.first_child().type()==pugi::xml_node_type::node_element) return n.first_child().internal_object();
			return n.internal_object();
		}
	}
#else
	Element* elel = (Element*)_1;
	for(auto& el: elel->getChildElements()) {
		if(el.getTagName()=="property" && el.getAttribute("name")==propname) {
			if(el.getChildElements().size()>0) {
				return (Element*)&(el.getChildElements().at(0));
			}
			return (Element*)&el;
		}
	}
#endif
	return NULL;
}

void XMLSerialize::startObjectSerialization(void* _1, const std::string& className)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->startElement("element");
	xw->attribute("type", className);
}

void XMLSerialize::endObjectSerialization(void* _1, const std::string& className)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->closeElement();
}

void XMLSerialize::afterAddObjectProperty(void* _1, const std::string& propName) {
	XmlWriter* xw = (XmlWriter*)_1;
	xw->closeElement();
}

void XMLSerialize::addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->startElement("property");
	xw->attribute("name", propName);
	switch(serOpt) {
		case 1:
		{
			xw->content(*(std::string*)t);
			break;
		}
		case 2:
		{
			std::string s;
			s.push_back(((char*)t)[0]);
			xw->content(s);
			break;
		}
		case 3:
		{
			std::string s;
			s.push_back(((unsigned char*)t)[0]);
			xw->content(s);
			break;
		}
		case 4:
		{
			xw->content(CastUtil::fromNumber(*(int*)t));
			break;
		}
		case 5:
		{
			xw->content(CastUtil::fromNumber(*(unsigned int*)t));
			break;
		}
		case 6:
		{
			xw->content(CastUtil::fromNumber(*(short*)t));
			break;
		}
		case 7:
		{
			xw->content(CastUtil::fromNumber(*(unsigned short*)t));
			break;
		}
		case 8:
		{
			xw->content(CastUtil::fromNumber(*(long*)t));
			break;
		}
		case 9:
		{
			xw->content(CastUtil::fromNumber(*(unsigned long*)t));
			break;
		}
		case 10:
		{
			xw->content(CastUtil::fromNumber(*(long long*)t));
			break;
		}
		case 11:
		{
			xw->content(CastUtil::fromNumber(*(unsigned long long*)t));
			break;
		}
		case 12:
		{
			xw->content(CastUtil::fromFloat(*(float*)t));
			break;
		}
		case 13:
		{
			xw->content(CastUtil::fromDouble(*(double*)t));
			break;
		}
		case 14:
		{
			xw->content(CastUtil::fromLongdouble(*(long double*)t));
			break;
		}
		case 15:
		{
			xw->content(CastUtil::fromBool(*(bool*)t));
			break;
		}
		case 16:
		{
			DateFormat formt;
			xw->content(formt.format((Date*)t));
			break;
		}
		case 17:
		{
			xw->content(BinaryData::serilaize(*(BinaryData*)t));
			break;
		}
	}
}

void XMLSerialize::addObjectProperty(void* _1, const std::string& propName, std::string className)
{
	XmlWriter* xw = (XmlWriter*)_1;
	xw->startElement("property");
	xw->attribute("name", propName);
}

void* XMLSerialize::getObjectPrimitiveValue(void* _1, int serOpt, const std::string& className, const std::string& propName)
{
#ifdef HAVE_PUGI_XML
	pugi::xml_node doc((pugi::xml_node_struct*)_1);
	if(strcmp(doc.name(), "property")!=0 || strcmp(doc.attribute("name").value(), propName.c_str())!=0)
		return NULL;
	std::string tval(doc.text().get());
#else
	Element* root = (Element*)_1;
	std::string tval = root->getText();
	std::string tname = root->getTagName();
	if(tname!="property" || root->getAttribute("name")!=propName)
		return NULL;
#endif

	switch(serOpt) {
		case 1:
		{
			std::string *vt = new std::string;
			*vt = tval;
			return vt;
		}
		case 2:
		{
			char *vt = new char;
			*vt = tval.at(0);
			return vt;
		}
		case 3:
		{
			unsigned char *vt = new unsigned char;
			*vt = tval.at(0);
			return vt;
		}
		case 4:
		{
			int *vt = new int;
			*vt = CastUtil::toInt(tval);
			return vt;
		}
		case 5:
		{
			unsigned int *vt = new unsigned int;
			*vt = CastUtil::toUInt(tval);
			return vt;
		}
		case 6:
		{
			short *vt = new short;
			*vt = CastUtil::toShort(tval);
			return vt;
		}
		case 7:
		{
			unsigned short *vt = new unsigned short;
			*vt = CastUtil::toUShort(tval);
			return vt;
		}
		case 8:
		{
			long *vt = new long;
			*vt = CastUtil::toLong(tval);
			return vt;
		}
		case 9:
		{
			unsigned long *vt = new unsigned long;
			*vt = CastUtil::toLong(tval);
			return vt;
		}
		case 10:
		{
			long long *vt = new long long;
			*vt = CastUtil::toLonglong(tval);
			return vt;
		}
		case 11:
		{
			unsigned long long *vt = new unsigned long long;
			*vt = CastUtil::toLong(tval);
			return vt;
		}
		case 12:
		{
			float *vt = new float;
			*vt = CastUtil::toFloat(tval);
			return vt;
		}
		case 13:
		{
			double *vt = new double;
			*vt = CastUtil::toDouble(tval);
			return vt;
		}
		case 14:
		{
			long double *vt = new long double;
			*vt = CastUtil::toLongdouble(tval);
			return vt;
		}
		case 15:
		{
			bool *vt = new bool;
			*vt = CastUtil::toBool(tval);
			return vt;
		}
		case 16:
		{
			DateFormat formt;
			return formt.parse(tval);
		}
		case 17:
		{
			return BinaryData::unSerilaize(tval);
		}
	}
	return NULL;
}

std::string XMLSerialize::serializeUnknownBaseInt(void* t, int serOpt, const std::string& className, const std::string& appName, void* serobject)
{
	return _handleAllSerialization(serOpt, className, t, appName, this, NULL, NULL, NULL, serobject);
}

std::string XMLSerialize::serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& appName, void* serobject)
{
	bool soe = serobject!=NULL;
	if(soe) {
		serobject = new SimpleStringXmlWriter((std::string*)serobject);
		_handleAllSerialization(serOpt, className, t, appName, this, NULL, NULL, NULL, serobject);
		cleanSerializableObject(serobject);
		return CommonUtils::BLANK;
	}
	return _handleAllSerialization(serOpt, className, t, appName, this, NULL, NULL, NULL, serobject);
}
void* XMLSerialize::unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization("",serOpt,className,appName,this,false,unserObj);
}
void* XMLSerialize::unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& appName)
{
	return _handleAllUnSerialization(serVal,serOpt,className,appName,this,false,NULL);
}

std::string Serializer::toXml(const std::string &appName, std::string className, void *object, void *serobject) {
	bool soe = serobject!=NULL;
	if(soe) {
		serobject = new SimpleStringXmlWriter((std::string*)serobject);
		SerializeBase::_ser(object, className, appName, &XMLSerialize::_i, serobject);
		XMLSerialize::_i.cleanSerializableObject(serobject);
		return CommonUtils::BLANK;
	}
	return SerializeBase::_ser(object, className, appName, &XMLSerialize::_i, serobject);
}

std::string Serializer::toXml(const std::string &appName, std::string className, const std::string &container, void *object, void *serobject) {
	bool soe = serobject!=NULL;
	if(soe) {
		serobject = new SimpleStringXmlWriter((std::string*)serobject);
		 SerializeBase::_serContainer(object, className, appName, container, &XMLSerialize::_i, serobject);
		 XMLSerialize::_i.cleanSerializableObject(serobject);
		return CommonUtils::BLANK;
	}
	return SerializeBase::_serContainer(object, className, appName, container, &XMLSerialize::_i, serobject);
}

void* Serializer::fromXml(const std::string &appName, std::string className, void *serObject) {
	return SerializeBase::_unser(serObject, className, appName, &XMLSerialize::_i);
}

void* Serializer::fromXml(const std::string &appName, std::string className, const std::string &container, void *serObject) {
	return SerializeBase::_unserContainer(serObject, className, appName, container, &XMLSerialize::_i);
}
