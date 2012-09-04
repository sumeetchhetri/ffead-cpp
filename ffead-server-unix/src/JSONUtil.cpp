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
 * JSONUtil.cpp
 *
 *  Created on: 06-Aug-2012
 *      Author: sumeetc
 */

#include "JSONUtil.h"

JSONUtil::JSONUtil() {
	// TODO Auto-generated constructor stub

}

JSONUtil::~JSONUtil() {
	// TODO Auto-generated destructor stub
}

void JSONUtil::array(string& json, JSONElement* element)
{
	while(json.find("[")!=string::npos && json.find("]")!=string::npos)
	{
		element->setType(JSONElement::JSON_ARRAY);
		StringUtil::replaceFirst(json, "[", "");
		StringUtil::replaceLast(json, "]", "");
		readJSON(json,true,element);
	}
}

void JSONUtil::object(string& json, JSONElement* element)
{
	while(json.find("{")!=string::npos && json.find("}")!=string::npos)
	{
		element->setType(JSONElement::JSON_OBJECT);
		StringUtil::replaceFirst(json, "{", "");
		StringUtil::replaceLast(json, "}", "");
		readJSON(json,false,element);
	}
}

void JSONUtil::arrayOrObject(string& json, JSONElement* element)
{
	while((json.find("{")!=string::npos && json.find("}")!=string::npos)
			|| (json.find("[")!=string::npos && json.find("]")!=string::npos))
	{
		if(json.find("{")!=string::npos && json.find("}")!=string::npos)
		{
			element->setType(JSONElement::JSON_OBJECT);
			StringUtil::replaceFirst(json, "{", "");
			StringUtil::replaceLast(json, "}", "");
			readJSON(json,false,element);
		}
		else if(json.find("[")!=string::npos && json.find("]")!=string::npos)
		{
			element->setType(JSONElement::JSON_ARRAY);
			StringUtil::replaceFirst(json, "[", "");
			StringUtil::replaceLast(json, "]", "");
			readJSON(json,true,element);
		}
	}
}

void JSONUtil::readJSON(string& json,bool isarray,JSONElement *par)
{
	if(json=="")
		return;
	string name, value;
	if(!isarray)
	{
		size_t stn = json.find("\"");
		if(stn==string::npos)
			throw ("invalid json - no start '\"' found for name parameter");
		size_t enn = json.find("\"", stn+1);
		if(enn==string::npos)
			throw ("invalid json - no end '\"' found for name parameter");
		if(stn!=enn-1)
			name = json.substr(stn+1, (enn-stn-1));
		//StringUtil::trim(name);
		json = json.substr(enn+1);
		StringUtil::trim(json);
		size_t vst = json.find(":");
		if(vst!=0)
			throw ("invalid json - invalid json - invalid string before ':' found");
		else if(vst==string::npos)
			throw ("invalid json - no ':' found");
		json = json.substr(vst+1);
	}
	JSONElement* element = new JSONElement();
	element->setName(name);

	StringUtil::trim(json);
	size_t env = json.find(",");
	size_t obs = json.find("{");
	size_t ars = json.find("[");
	if(env==string::npos)
	{
		value = json;
		json = "";
		element->setType(JSONElement::JSON_STRING);
	}
	else
	{
		if(obs!=string::npos && obs<env && obs<ars)
		{
			size_t obe = json.find("}");
			value = json.substr(obs, obe-obs+1);
			json = json.substr(obe+1);
			element->setType(JSONElement::JSON_OBJECT);
		}
		else if(ars!=string::npos && ars<env && ars<obs)
		{
			size_t are = json.find("]");
			value = json.substr(ars, are-ars+1);
			json = json.substr(are+1);
			element->setType(JSONElement::JSON_ARRAY);
		}
		else
		{
			value = json.substr(0, env);
			json = json.substr(env+1);
			element->setType(JSONElement::JSON_STRING);
		}
	}
	if(value=="")
	{
		string ex = "invalid json - no value object found for name "+ name;
		throw (ex.c_str());
	}
	if(element->getType()!=JSONElement::JSON_OBJECT && element->getType()!=JSONElement::JSON_ARRAY)
	{
		validateSetValue(element, value);
	}
	par->addChild(element);
	if(element->getType()==JSONElement::JSON_OBJECT)
		object(value, element);
	else if(element->getType()==JSONElement::JSON_ARRAY)
		array(value, element);
	readJSON(json,isarray,par);
}

void JSONUtil::validateSetValue(JSONElement* element, string value)
{
	StringUtil::trim(value);
	size_t stn = value.find("\"");
	if(stn!=string::npos)
	{
		size_t enn = value.find("\"", stn+1);
		if(enn==string::npos)
		{
			string ex = "invalid json - invalid string object '"+value+"' found for name "+ element->getName();
			throw (ex.c_str());
		}
		else if(enn!=value.length()-1)
		{
			string ex = "invalid json - invalid literal found after string object '"+value+"' for name "+ element->getName();
			throw (ex.c_str());
		}
		if(stn!=enn-1)
		{
			value = value.substr(stn+1, (enn-stn-1));
		}
		element->setType(JSONElement::JSON_STRING);
	}
	else if(StringUtil::toLowerCopy(value)=="true" || StringUtil::toLowerCopy(value)=="false")
	{
		value = StringUtil::toLowerCopy(value);
		element->setType(JSONElement::JSON_BOOL);
	}
	else if(value.find(".")!=string::npos)
	{
		try
		{
			CastUtil::lexical_cast<double>(value);
		} catch (const char* ex) {
			string exp = "invalid json - invalid double value "+value+" found for name "+ element->getName();
			throw (exp.c_str());
		}
		element->setType(JSONElement::JSON_FLOAT);
	}
	else
	{
		try
		{
			CastUtil::lexical_cast<int>(value);
		} catch (const char* ex) {
			try
			{
				CastUtil::lexical_cast<long>(value);
			} catch (const char* ex) {
				try
				{
					CastUtil::lexical_cast<long long>(value);
				} catch (const char* ex) {
					string exp = "invalid json - invalid numeric value "+value+" found for name "+ element->getName();
					throw (exp.c_str());
				}
			}
		}
		element->setType(JSONElement::JSON_NUMBER);
	}
	element->setValue(value);
}

JSONElement JSONUtil::getDocument(const string& jsonTxt)
{
	string json(jsonTxt);
	JSONElement root;
	root.setType(JSONElement::JSON_OBJECT);
	root.setName("_JSON_ROOT");
	if(json.find("{")!=string::npos && json.find("}")!=string::npos)
	{
		root.setType(JSONElement::JSON_OBJECT);
		StringUtil::replaceFirst(json, "{", "");
		StringUtil::replaceLast(json, "}", "");
		readJSON(json,false,&root);
	}
	else if(json.find("[")!=string::npos && json.find("]")!=string::npos)
	{
		root.setType(JSONElement::JSON_ARRAY);
		StringUtil::replaceFirst(json, "[", "");
		StringUtil::replaceLast(json, "]", "");
		readJSON(json,true,&root);
	}
	return root;
}

string JSONUtil::getDocumentStr(JSONElement doc)
{
	string jsonText;
	if(doc.getType()==JSONElement::JSON_OBJECT)
		jsonText += "{";
	else
		jsonText += "[";
	if(doc.hasChildren())
	{
		for (int var = 0; var < (int)doc.getChildren().size(); ++var) {
			JSONElement* child = doc.getChildren().at(var);
			if(doc.getType()==JSONElement::JSON_OBJECT)
				jsonText += "\"" + child->getName() + "\":";
			if(child->getType()==JSONElement::JSON_OBJECT || child->getType()==JSONElement::JSON_ARRAY)
			{
				jsonText += getDocumentStr(*child);
			}
			else
			{
				if(child->getType()==JSONElement::JSON_STRING)
					jsonText += "\"" + child->getValue() + "\"";
				else
					jsonText += child->getValue();
			}
			if(var!=(int)doc.getChildren().size()-1)
			{
				jsonText += ", ";
			}
		}
	}
	if(doc.getType()==JSONElement::JSON_OBJECT)
		jsonText += "}";
	else
		jsonText += "]";
	return jsonText;
}
