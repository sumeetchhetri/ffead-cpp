/*
	Copyright 2009-2012, Sumeet Chhetri

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
		int arrst = json.find("[");
		int objst = json.find("{");
		if(json.find("{")!=string::npos && json.find("}")!=string::npos && (objst<arrst || arrst==(int)string::npos))
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

void JSONUtil::readBalancedJSON(string& value, string& json, bool isarray, size_t obs)
{
	string typest = "{";
	string typeen = "}";
	if(isarray)
	{
		typest = "[";
		typeen = "]";
	}
	size_t eee = json.find(typeen, obs);
	if(eee==string::npos)
	{
		string ex = "invalid json - unbalanced " + typest + " found at " + CastUtil::lexical_cast<string>(obs);
		throw (ex.c_str());
	}
	string test = json.substr(obs, eee-obs+1);
	int stcnt = StringUtil::countOccurrences(test, typest);
	int encnt = StringUtil::countOccurrences(test, typeen);
	while(stcnt!=encnt)
	{
		eee = json.find(typeen, eee+1);
		if(eee==string::npos)
		{
			string ex = "invalid json - balancing " + typeen + " not found after " + CastUtil::lexical_cast<string>(eee);
			throw (ex.c_str());
		}
		test = json.substr(obs, eee-obs+1);
		stcnt = StringUtil::countOccurrences(test, typest);
		encnt = StringUtil::countOccurrences(test, typeen);
	}
	value = json.substr(obs, eee-obs+1);
	if(eee+1<json.length())
		json = json.substr(eee+1);
	else
		json = "";
}

void JSONUtil::readJSON(string& json,bool isarray,JSONElement *par)
{
	if(json=="")
		return;
	string name, value;
	if(!isarray)
	{
		size_t stn = json.find("\"");
		while(stn!=string::npos && stn>0 && json.at(stn-1)=='\\')
		{
			stn = json.find("\"", stn+1);
			if(stn==0)
			{
				stn = string::npos;
			}
		}
		if(stn==string::npos)
			throw ("invalid json - no start '\"' found for name parameter");
		size_t enn = json.find("\"", stn+1);
		while(enn!=string::npos && enn>0 && json.at(enn-1)=='\\')
		{
			enn = json.find("\"", enn+1);
			if(enn==0)
			{
				enn = string::npos;
			}
		}
		if(enn==string::npos)
			throw ("invalid json - no end '\"' found for name parameter");
		if(stn!=enn-1)
			name = json.substr(stn+1, (enn-stn-1));
		//StringUtil::trim(name);
		json = json.substr(enn+1);
		StringUtil::trim(json);
		size_t vst = json.find(":");
		if(vst==string::npos)
			throw ("invalid json - no ':' found");
		else if(vst!=0)
			throw ("invalid json - invalid json - invalid string before ':' found");
		json = json.substr(vst+1);
	}
	JSONElement* element = new JSONElement();
	element->setName(name);

	StringUtil::trim(json);
	size_t env = json.find(",");
	size_t obs = json.find("{");
	size_t ars = json.find("[");
	if(obs==0)
	{
		readBalancedJSON(value, json, false, obs);
		element->setType(JSONElement::JSON_OBJECT);
	}
	else if(ars==0)
	{
		readBalancedJSON(value, json, true, ars);
		element->setType(JSONElement::JSON_ARRAY);
	}
	else if(env==string::npos)
	{
		value = json;
		json = "";
		element->setType(JSONElement::JSON_STRING);
	}
	else
	{
		if(obs!=string::npos && env==0 && (obs<ars || ars==string::npos))
		{
			readBalancedJSON(value, json, false, obs);
			element->setType(JSONElement::JSON_OBJECT);
		}
		else if(ars!=string::npos && env==0 && (ars<obs || obs==string::npos))
		{
			readBalancedJSON(value, json, true, ars);
			element->setType(JSONElement::JSON_ARRAY);
		}
		else if(obs!=string::npos && obs<env && (obs<ars || ars==string::npos))
		{
			readBalancedJSON(value, json, false, obs);
			element->setType(JSONElement::JSON_OBJECT);
		}
		else if(ars!=string::npos && ars<env && (ars<obs || obs==string::npos))
		{
			readBalancedJSON(value, json, true, ars);
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
		while(stn!=string::npos && stn>0 && value.at(stn-1)=='\\')
		{
			stn = value.find("\"", stn+1);
			if(stn==0)
			{
				stn = string::npos;
			}
		}
		size_t enn = value.find("\"", stn+1);
		while(enn!=string::npos && enn>0 && value.at(enn-1)=='\\')
		{
			enn = value.find("\"", enn+1);
			if(enn==0)
			{
				enn = string::npos;
			}
		}
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
		else
		{
			value = "";
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
			CastUtil::lexical_cast<unsigned long long>(value);
		} catch (const char* ex) {
			string exp = "invalid json - invalid numeric value "+value+" found for name "+ element->getName();
			throw (exp.c_str());
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
	int arrst = json.find("[");
	int objst = json.find("{");
	if(json.find("{")!=string::npos && json.find("}")!=string::npos && (objst<arrst || arrst==(int)string::npos))
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
