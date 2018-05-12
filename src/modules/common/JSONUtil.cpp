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

void JSONUtil::array(std::string& json, JSONElement* element)
{
	while(json.find("[")!=std::string::npos && json.find("]")!=std::string::npos)
	{
		element->setType(JSONElement::JSON_ARRAY);
		StringUtil::replaceFirst(json, "[", "");
		StringUtil::replaceLast(json, "]", "");
		readJSON(json,true,element);
	}
}

void JSONUtil::object(std::string& json, JSONElement* element)
{
	while(json.find("{")!=std::string::npos && json.find("}")!=std::string::npos)
	{
		element->setType(JSONElement::JSON_OBJECT);
		StringUtil::replaceFirst(json, "{", "");
		StringUtil::replaceLast(json, "}", "");
		readJSON(json,false,element);
	}
}

void JSONUtil::arrayOrObject(std::string& json, JSONElement* element)
{
	while((json.find("{")!=std::string::npos && json.find("}")!=std::string::npos)
			|| (json.find("[")!=std::string::npos && json.find("]")!=std::string::npos))
	{
		int arrst = json.find("[");
		int objst = json.find("{");
		if(json.find("{")!=std::string::npos && json.find("}")!=std::string::npos && (objst<arrst || arrst==(int)std::string::npos))
		{
			element->setType(JSONElement::JSON_OBJECT);
			StringUtil::replaceFirst(json, "{", "");
			StringUtil::replaceLast(json, "}", "");
			readJSON(json,false,element);
		}
		else if(json.find("[")!=std::string::npos && json.find("]")!=std::string::npos)
		{
			element->setType(JSONElement::JSON_ARRAY);
			StringUtil::replaceFirst(json, "[", "");
			StringUtil::replaceLast(json, "]", "");
			readJSON(json,true,element);
		}
	}
}

void JSONUtil::readBalancedJSON(std::string& value, std::string& json, const bool& isarray, const size_t& obs)
{
	std::string typest = "{";
	std::string typeen = "}";
	if(isarray)
	{
		typest = "[";
		typeen = "]";
	}
	size_t eee = json.find(typeen, obs);
	if(eee==std::string::npos)
	{
		std::string ex = "invalid json - unbalanced " + typest + " found at " + CastUtil::lexical_cast<std::string>(obs);
		throw ex;
	}
	std::string test = json.substr(obs, eee-obs+1);
	int stcnt = StringUtil::countOccurrences(test, typest);
	int encnt = StringUtil::countOccurrences(test, typeen);
	while(stcnt!=encnt)
	{
		eee = json.find(typeen, eee+1);
		if(eee==std::string::npos)
		{
			std::string ex = "invalid json - balancing " + typeen + " not found after " + CastUtil::lexical_cast<std::string>(eee);
			throw ex;
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

void JSONUtil::readJSON(std::string& json, const bool& isarray, JSONElement *par)
{
	if(json=="")
		return;
	std::string name, value;
	if(!isarray)
	{
		size_t stn = json.find("\"");
		while(stn!=std::string::npos && stn>0 && json.at(stn-1)=='\\')
		{
			stn = json.find("\"", stn+1);
			if(stn==0)
			{
				stn = std::string::npos;
			}
		}
		if(stn==std::string::npos)
			throw ("invalid json - no start '\"' found for name parameter");
		size_t enn = json.find("\"", stn+1);
		while(enn!=std::string::npos && enn>0 && json.at(enn-1)=='\\')
		{
			enn = json.find("\"", enn+1);
			if(enn==0)
			{
				enn = std::string::npos;
			}
		}
		if(enn==std::string::npos)
			throw ("invalid json - no end '\"' found for name parameter");
		if(stn!=enn-1)
			name = json.substr(stn+1, (enn-stn-1));
		//StringUtil::trim(name);
		json = json.substr(enn+1);
		StringUtil::trim(json);
		size_t vst = json.find(":");
		if(vst==std::string::npos)
			throw ("invalid json - no ':' found");
		else if(vst!=0)
			throw ("invalid json - invalid json - invalid std::string before ':' found");
		json = json.substr(vst+1);
	}
	JSONElement element;
	element.setName(name);

	StringUtil::trim(json);
	size_t env = json.find(",");
	size_t obs = json.find("{");
	size_t ars = json.find("[");
	if(obs==0)
	{
		readBalancedJSON(value, json, false, obs);
		element.setType(JSONElement::JSON_OBJECT);
	}
	else if(ars==0)
	{
		readBalancedJSON(value, json, true, ars);
		element.setType(JSONElement::JSON_ARRAY);
	}
	else if(env==std::string::npos)
	{
		value = json;
		json = "";
		element.setType(JSONElement::JSON_STRING);
	}
	else
	{
		if(obs!=std::string::npos && env==0 && (obs<ars || ars==std::string::npos))
		{
			readBalancedJSON(value, json, false, obs);
			element.setType(JSONElement::JSON_OBJECT);
		}
		else if(ars!=std::string::npos && env==0 && (ars<obs || obs==std::string::npos))
		{
			readBalancedJSON(value, json, true, ars);
			element.setType(JSONElement::JSON_ARRAY);
		}
		else if(obs!=std::string::npos && obs<env && (obs<ars || ars==std::string::npos))
		{
			readBalancedJSON(value, json, false, obs);
			element.setType(JSONElement::JSON_OBJECT);
		}
		else if(ars!=std::string::npos && ars<env && (ars<obs || obs==std::string::npos))
		{
			readBalancedJSON(value, json, true, ars);
			element.setType(JSONElement::JSON_ARRAY);
		}
		else
		{
			value = json.substr(0, env);
			json = json.substr(env+1);
			element.setType(JSONElement::JSON_STRING);
		}
	}
	if(value=="")
	{
		std::string ex = "invalid json - no value object found for name "+ name;
		throw ex;
	}
	if(element.getType()!=JSONElement::JSON_OBJECT && element.getType()!=JSONElement::JSON_ARRAY)
	{
		validateSetValue(&element, value);
	}
	par->addChild(element);
	JSONElement* ele = (JSONElement*)&(par->getChildren().back());
	if(ele->getType()==JSONElement::JSON_OBJECT)
		object(value, ele);
	else if(ele->getType()==JSONElement::JSON_ARRAY)
		array(value, ele);
	readJSON(json,isarray,par);
}

void JSONUtil::validateSetValue(JSONElement* element, const std::string& v)
{
	std::string value = v;
	StringUtil::trim(value);
	size_t stn = value.find("\"");
	if(stn!=std::string::npos)
	{
		while(stn!=std::string::npos && stn>0 && value.at(stn-1)=='\\')
		{
			stn = value.find("\"", stn+1);
			if(stn==0)
			{
				stn = std::string::npos;
			}
		}
		size_t enn = value.find("\"", stn+1);
		while(enn!=std::string::npos && enn>0 && value.at(enn-1)=='\\')
		{
			enn = value.find("\"", enn+1);
			if(enn==0)
			{
				enn = std::string::npos;
			}
		}
		if(enn==std::string::npos)
		{
			std::string ex = "invalid json - invalid std::string object '"+value+"' found for name "+ element->getName();
			throw ex;
		}
		else if(enn!=value.length()-1)
		{
			std::string ex = "invalid json - invalid literal found after std::string object '"+value+"' for name "+ element->getName();
			throw ex;
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
	else if(value.find(".")!=std::string::npos)
	{
		try
		{
			CastUtil::lexical_cast<double>(value);
		} catch (const char* ex) {
			std::string exp = "invalid json - invalid double value "+value+" found for name "+ element->getName();
			throw exp;
		}
		element->setType(JSONElement::JSON_FLOAT);
	}
	else
	{
		try
		{
			CastUtil::lexical_cast<unsigned long long>(value);
		} catch (const char* ex) {
			std::string exp = "invalid json - invalid numeric value "+value+" found for name "+ element->getName();
			throw exp;
		}
		element->setType(JSONElement::JSON_NUMBER);
	}
	element->setValue(value);
}

void JSONUtil::getDocument(const std::string& jsonTxt, JSONElement& root)
{
	std::string json(jsonTxt);
	root.setType(JSONElement::JSON_OBJECT);
	root.setName("_JSON_ROOT");
	int arrst = json.find("[");
	int objst = json.find("{");
	if(json.find("{")!=std::string::npos && json.find("}")!=std::string::npos && (objst<arrst || arrst==(int)std::string::npos))
	{
		root.setType(JSONElement::JSON_OBJECT);
		StringUtil::replaceFirst(json, "{", "");
		StringUtil::replaceLast(json, "}", "");
		readJSON(json,false,&root);
	}
	else if(json.find("[")!=std::string::npos && json.find("]")!=std::string::npos)
	{
		root.setType(JSONElement::JSON_ARRAY);
		StringUtil::replaceFirst(json, "[", "");
		StringUtil::replaceLast(json, "]", "");
		readJSON(json,true,&root);
	}
}

std::string JSONUtil::getDocumentStr(const JSONElement& doc)
{
	std::string jsonText;
	if(doc.getType()==JSONElement::JSON_OBJECT)
		jsonText += "{";
	else
		jsonText += "[";
	if(doc.hasChildren())
	{
		for (int var = 0; var < (int)doc.getChildren().size(); ++var) {
			const JSONElement* child = &(doc.getChildren().at(var));
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
