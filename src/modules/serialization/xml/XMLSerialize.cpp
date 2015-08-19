/*
 * XMLSerialize.cpp
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#include "XMLSerialize.h"

XMLSerialize::XMLSerialize() {
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		throw "Cannot load serialization shared library";
	}
	dlibinstantiated = true;
}

XMLSerialize::XMLSerialize(void* dlib) {
	if(dlib == NULL)
	{
		throw "Cannot load serialization shared library";
	}
	this->dlib = dlib;
	dlibinstantiated = false;
}

XMLSerialize::~XMLSerialize() {
	if(dlibinstantiated)
	{
		dlclose(dlib);
	}
}

string XMLSerialize::serializePrimitive(const string& className, void* t)
{
	string objXml;
	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		objXml = "<string>"+CastUtil::lexical_cast<string>(tem)+"</string>";
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		string temp;
		temp.push_back(tem);
		objXml = "<char>"+temp+"</char>";
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(char*)t;
		string temp;
		temp.push_back(tem);
		objXml = "<uchar>"+temp+"</uchar>";
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		objXml = "<int>"+CastUtil::lexical_cast<string>(tem)+"</int>";
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		objXml = "<uint>"+CastUtil::lexical_cast<string>(tem)+"</uint>";
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		objXml = "<short>"+CastUtil::lexical_cast<string>(tem)+"</short>";
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		objXml = "<ushort>"+CastUtil::lexical_cast<string>(tem)+"</ushort>";
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		objXml = "<long>"+CastUtil::lexical_cast<string>(tem)+"</long>";
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		objXml = "<ulong>"+CastUtil::lexical_cast<string>(tem)+"</ulong>";
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		objXml = "<llong>"+CastUtil::lexical_cast<string>(tem)+"</llong>";
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		objXml = "<ullong>"+CastUtil::lexical_cast<string>(tem)+"</ullong>";
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
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		objXml = "<ldouble>"+CastUtil::lexical_cast<string>(tem)+"</ldouble>";
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
	return objXml;
}

void* XMLSerialize::getSerializableObject()
{
	return new string;
}

void XMLSerialize::cleanSerializableObject(void* _1)
{
	string* object = (string*)_1;
	delete object;
}

void XMLSerialize::startContainerSerialization(void* _1, const string& className, const string& container)
{
	string* object = (string*)_1;
	*object = "<"+container+"-"+className+">";
}

void XMLSerialize::endContainerSerialization(void* _1, const string& className, const string& container)
{
	string* object = (string*)_1;
	*object += "</"+container+"-"+className+">";
}

void XMLSerialize::afterAddContainerSerializableElement(void* _1, const int& counter, const int& size){}

void XMLSerialize::addContainerSerializableElement(void* _1, const string& tem)
{
	string* object = (string*)_1;
	*object += tem;
}

void XMLSerialize::addContainerSerializableElementMulti(void* _1, const string& tem)
{
	string* object = (string*)_1;
	*object += tem;
}

string XMLSerialize::fromSerializableObjectToString(void* _1)
{
	string* object = (string*)_1;
	return *object;
}

string XMLSerialize::elementToSerializedString(void* _1, const int& counter)
{
	Element* object = (Element*)_1;
	return object->getChildElements().at(counter)->renderChildren();
}

string XMLSerialize::getConatinerElementClassName(void* _1, const string& className)
{
	Element* root = (Element*)_1;
	string stlclassName = root->getTagName();
	if(stlclassName.find("-")!=string::npos)
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
	Element* ele = root->getChildElements().at(counter);
	if(counter1!=-1)
	{
		if((int)ele->getChildElements().size()<counter1)
			return NULL;
		ele = ele->getChildElements().at(counter1);
	}
	return ele;
}

void XMLSerialize::addPrimitiveElementToContainer(void* _1, const int& counter, const string& className, void* cont, const string& container)
{
	Element* root = (Element*)_1;
	Element* ele = root->getChildElements().at(counter);
	if(className=="std::string" || className=="string")
	{
		string retVal = ele->getText();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="int")
	{
		int retVal = CastUtil::lexical_cast<int>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="short")
	{
		short retVal = CastUtil::lexical_cast<short>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long")
	{
		long retVal = CastUtil::lexical_cast<long>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long long")
	{
		long long retVal = CastUtil::lexical_cast<long long>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long double")
	{
		long double retVal = CastUtil::lexical_cast<long double>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned int")
	{
		unsigned int retVal = CastUtil::lexical_cast<unsigned int>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned short")
	{
		unsigned short retVal = CastUtil::lexical_cast<unsigned short>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned long")
	{
		unsigned long retVal = CastUtil::lexical_cast<unsigned long>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long retVal = CastUtil::lexical_cast<unsigned long long>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="float")
	{
		float retVal = CastUtil::lexical_cast<float>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="double")
	{
		double retVal = CastUtil::lexical_cast<double>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="bool")
	{
		bool retVal = CastUtil::lexical_cast<bool>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="char")
	{
		char retVal = CastUtil::lexical_cast<char>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned char")
	{
		unsigned char retVal = CastUtil::lexical_cast<unsigned char>(ele->getText());
		addValueToNestedContainer(container, retVal, cont);
	}
}

void* XMLSerialize::getUnserializableObject(const string& _1)
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
		cout << str.getMessage() << endl;
	} catch(...) {
		cout << "XML Parse Error" << endl;
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

void* XMLSerialize::getValidUnserializableObject(const string& _1){return NULL;}

int XMLSerialize::getContainerSize(void* _1)
{
	Element* root = (Element*)_1;
	return root->getChildElements().size();
}

string XMLSerialize::getUnserializableClassName(void* _1, const string& className)
{
	Element* root = (Element*)_1;
	return root->getTagName();
}

void* XMLSerialize::getPrimitiveValue(void* _1, const string& className)
{
	Element* root = (Element*)_1;
	if((className=="signed" || className=="int" || className=="signed int"))
	{
		int *vt = new int;
		*vt = CastUtil::lexical_cast<int>(root->getText());
		return vt;
	}
	else if((className=="unsigned" || className=="unsigned int"))
	{
		unsigned int *vt = new unsigned int;
		*vt = CastUtil::lexical_cast<unsigned int>(root->getText());
		return vt;
	}
	else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int"))
	{
		short *vt = new short;
		*vt = CastUtil::lexical_cast<short>(root->getText());
		return vt;
	}
	else if((className=="unsigned short" || className=="unsigned short int"))
	{
		unsigned short *vt = new unsigned short;
		*vt = CastUtil::lexical_cast<unsigned short>(root->getText());
		return vt;
	}
	else if((className=="long" || className=="long int" || className=="signed long" || className=="signed long int"))
	{
		long *vt = new long;
		*vt = CastUtil::lexical_cast<long>(root->getText());
		return vt;
	}
	else if((className=="unsigned long long" || className=="unsigned long long int"))
	{
		unsigned long long *vt = new unsigned long long;
		*vt = CastUtil::lexical_cast<unsigned long long>(root->getText());
		return vt;
	}
	else if((className=="unsigned long" || className=="unsigned long int"))
	{
		unsigned long *vt = new unsigned long;
		*vt = CastUtil::lexical_cast<unsigned long>(root->getText());
		return vt;
	}
	else if((className=="char" || className=="signed char"))
	{
		char *vt = new char;
		*vt = root->getText().at(0);
		return vt;
	}
	else if(className=="unsigned char")
	{
		unsigned char *vt = new unsigned char;
		*vt = root->getText().at(0);
		return vt;
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		return formt.parse(root->getText());
	}
	else if(className=="BinaryData")
	{
		return BinaryData::unSerilaize(root->getText());
	}
	else if(className=="float")
	{
		float *vt = new float;
		*vt = CastUtil::lexical_cast<float>(root->getText());
		return vt;
	}
	else if(className=="double")
	{
		double *vt = new double;
		*vt = CastUtil::lexical_cast<double>(root->getText());
		return vt;
	}
	else if(className=="long double")
	{
		long double *vt = new long double;
		*vt = CastUtil::lexical_cast<long double>(root->getText());
		return vt;
	}
	else if(className=="bool")
	{
		bool *vt = new bool;
		*vt = CastUtil::lexical_cast<bool>(root->getText());
		return vt;
	}
	else if((className=="std::string" || className=="string"))
	{
		string *vt = new string;
		*vt = root->getText();
		return vt;
	}
	return NULL;
}

string XMLSerialize::serializeUnknown(void* t, const string& className, const string& appName)
{
	XMLSerialize serialize;
	return _handleAllSerialization(className,t,appName, &serialize);
}

void* XMLSerialize::unSerializeUnknown(const string& objXml, const string& className, const string& appName)
{
	XMLSerialize serialize;
	return _handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
}

bool XMLSerialize::isValidClassNamespace(void* _1, const string& className, const string& namespc, const bool& iscontainer)
{
	string tnmspc = namespc;
	StringUtil::replaceAll(tnmspc, "::", "_");
	/*StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);*/
	string cn = className;
	if(cn.find('-')!=string::npos)
	{
		string pre = cn.substr(0, cn.find_last_of("-")+1);
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

bool XMLSerialize::isValidObjectProperty(void* _1, const string& propname, const int& counter)
{
	Element* element = (Element*)_1;
	if((int)element->getChildElements().size()>counter && element->getChildElements().at(counter)->getTagName()==propname)
		return true;
	return false;
}

void* XMLSerialize::getObjectProperty(void* _1, const int& counter)
{
	Element* elel = (Element*)_1;
	return elel->getChildElements().at(counter);
}

void XMLSerialize::startObjectSerialization(void* _1, const string& className)
{
	string* object = (string*)_1;
	*object = "<"+className+">";
}

void XMLSerialize::endObjectSerialization(void* _1, const string& className)
{
	string* object = (string*)_1;
	*object += "</"+className+">";
}

void XMLSerialize::afterAddObjectProperty(void* _1){}

void XMLSerialize::addObjectPrimitiveProperty(void* _1, const string& propName, const string& className, void* t)
{
	string* object = (string*)_1;
	string objXml;
	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		string temp;
		temp.push_back(tem);
		objXml = temp;
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(char*)t;
		string temp;
		temp.push_back(tem);
		objXml = temp;
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		objXml = CastUtil::lexical_cast<string>(tem);
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		objXml = formt.format(*(Date*)t);
	}
	else if(className=="BinaryData")
	{
		objXml = BinaryData::serilaize(*(BinaryData*)t);
	}

	/*StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);*/
	//TODO - any side effects??
	//*object += "<" + propName + " type=\"" + className + "\">" + objXml + "</" + propName + ">";
	*object += "<" + propName + ">" + objXml + "</" + propName + ">";
}

void XMLSerialize::addObjectProperty(void* _1, const string& propName, string className, const string& t)
{
	string* object = (string*)_1;

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

void* XMLSerialize::getObjectPrimitiveValue(void* _1, const string& className, const string& propName)
{
	Element* root = (Element*)_1;
	if(root->getTagName()!=propName)
		return NULL;
	if((className=="signed" || className=="int" || className=="signed int"))
	{
		int *vt = new int;
		*vt = CastUtil::lexical_cast<int>(root->getText());
		return vt;
	}
	else if((className=="unsigned" || className=="unsigned int"))
	{
		unsigned int *vt = new unsigned int;
		*vt = CastUtil::lexical_cast<unsigned int>(root->getText());
		return vt;
	}
	else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int"))
	{
		short *vt = new short;
		*vt = CastUtil::lexical_cast<short>(root->getText());
		return vt;
	}
	else if((className=="unsigned short" || className=="unsigned short int"))
	{
		unsigned short *vt = new unsigned short;
		*vt = CastUtil::lexical_cast<unsigned short>(root->getText());
		return vt;
	}
	else if((className=="long" || className=="long int" || className=="signed long" || className=="signed long int"))
	{
		long *vt = new long;
		*vt = CastUtil::lexical_cast<long>(root->getText());
		return vt;
	}
	else if((className=="long long" || className=="long long int" || className=="signed long long int"))
	{
		long long *vt = new long long;
		*vt = CastUtil::lexical_cast<long long>(root->getText());
		return vt;
	}
	else if((className=="unsigned long" || className=="unsigned long int"))
	{
		unsigned long *vt = new unsigned long;
		*vt = CastUtil::lexical_cast<unsigned long>(root->getText());
		return vt;
	}
	else if((className=="unsigned long long" || className=="unsigned long long int"))
	{
		unsigned long long *vt = new unsigned long long;
		*vt = CastUtil::lexical_cast<unsigned long long>(root->getText());
		return vt;
	}
	else if((className=="char" || className=="signed char"))
	{
		char *vt = new char;
		*vt = root->getText().at(0);
		return vt;
	}
	else if(className=="unsigned char")
	{
		unsigned char *vt = new unsigned char;
		*vt = root->getText().at(0);
		return vt;
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		return formt.parse(root->getText());
	}
	else if(className=="BinaryData")
	{
		return BinaryData::unSerilaize(root->getText());
	}
	else if(className=="float")
	{
		float *vt = new float;
		*vt = CastUtil::lexical_cast<float>(root->getText());
		return vt;
	}
	else if(className=="double")
	{
		double *vt = new double;
		*vt = CastUtil::lexical_cast<double>(root->getText());
		return vt;
	}
	else if(className=="long double")
	{
		long double *vt = new long double;
		*vt = CastUtil::lexical_cast<long double>(root->getText());
		return vt;
	}
	else if(className=="bool")
	{
		bool *vt = new bool;
		*vt = CastUtil::lexical_cast<bool>(root->getText());
		return vt;
	}
	else if((className=="std::string" || className=="string"))
	{
		string *vt = new string;
		*vt = root->getText();
		return vt;
	}
	return NULL;
}

string XMLSerialize::serializeUnknownBase(void* t, const string& className, const string& appName)
{
	return _handleAllSerialization(className,t,appName, this);
}
void* XMLSerialize::unSerializeUnknownBase(void* unserObj, const string& className, const string& appName)
{
	return _handleAllUnSerialization("",className,appName,this,false,unserObj);
}
void* XMLSerialize::unSerializeUnknownBase(const string& serVal, const string& className, const string& appName)
{
	return _handleAllUnSerialization(serVal,className,appName,this,false,NULL);
}
