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
 * JSONElement.cpp
 *
 *  Created on: 06-Aug-2012
 *      Author: sumeetc
 */

#include "JSONElement.h"

JSONElement JSONElement::nullele;

JSONElement::JSONElement() {
	type = -1;
}

JSONElement::JSONElement(int type) {
	this->type = type;
}

JSONElement::JSONElement(int type, std::string k, std::string v) {
	this->type = type;
	this->name = k;
	this->value = v;
}

JSONElement::~JSONElement() {
	for (int var = 0; var < (int)children.size(); ++var) {
		//delete children.at(var);
	}
}

JSONElement& JSONElement::addChild(JSONElement ele) {
	this->children.push_back(ele);
	JSONElement* child = &(this->children.back());
	if(child->name!="" && this->type!=JSON_ARRAY)
	{
		this->allnodes[child->name] = child;
	}
	return *this;
}

int JSONElement::getType() const {
	return type;
}

JSONElement& JSONElement::setType(const int& type) {
	this->type = type;
	return *this;
}

const std::string& JSONElement::getValue() const {
	return value;
}

bool JSONElement::hasChildren() const {
	return this->children.size()>0;
}

JSONElement& JSONElement::setValue(const std::string& value) {
	this->value = value;
	return *this;
}

const std::string& JSONElement::getName() const {
	return name;
}

const std::vector<JSONElement>& JSONElement::getChildren() const {
	return this->children;
}

JSONElement& JSONElement::setName(const std::string& name) {
	this->name = name;
	return *this;
}

/*const JSONElement& JSONElement::getNode(const std::string& name) {
	if(allnodes.find(name)!=allnodes.end())
		return allnodes[name];
	return nullele;
}*/

JSONElement* JSONElement::getNodeP(const std::string& name) {
	if(allnodes.find(name)!=allnodes.end())
		return allnodes[name];
	return NULL;
}

std::string JSONElement::toString() const
{
	std::string jsonText;
	if(type==JSONElement::JSON_OBJECT)
		jsonText += "{";
	else
		jsonText += "[";
	if(hasChildren())
	{
		for (int var = 0; var < (int)children.size(); ++var) {
			const JSONElement* child = &(children.at(var));
			if(type==JSONElement::JSON_OBJECT)
				jsonText += "\"" + child->getName() + "\":";
			if(child->getType()==JSONElement::JSON_OBJECT || child->getType()==JSONElement::JSON_ARRAY)
			{
				jsonText += child->toString();
			}
			else
			{
				if(child->getType()==JSONElement::JSON_STRING)
					jsonText += "\"" + child->getValue() + "\"";
				else
					jsonText += child->getValue();
			}
			if(var!=(int)children.size()-1)
			{
				jsonText += ", ";
			}
		}
	}
	if(type==JSONElement::JSON_OBJECT)
		jsonText += "}";
	else
		jsonText += "]";
	return jsonText;
}

JSONElement JSONElement::object() {
	return JSONElement(JSON_OBJECT);
}

JSONElement JSONElement::array() {
	return JSONElement(JSON_ARRAY);
}

JSONElement& JSONElement::add(JSONElement el) {
	if(this->type == JSON_OBJECT || this->type == JSON_ARRAY) {
		addChild(el);
	}
	return *this;
}

JSONElement& JSONElement::add(std::string k, std::string v) {
	if(this->type == JSON_OBJECT || this->type == JSON_ARRAY) {
		addChild(JSONElement(JSON_STRING, k, v));
	}
	return *this;
}

JSONElement& JSONElement::add(std::string k, unsigned long long v) {
	if(this->type == JSON_OBJECT || this->type == JSON_ARRAY) {
		addChild(JSONElement(JSON_NUMBER, k, CastUtil::lexical_cast<std::string>(v)));
	}
	return *this;
}

JSONElement& JSONElement::add(std::string k, bool v) {
	if(this->type == JSON_OBJECT || this->type == JSON_ARRAY) {
		addChild(JSONElement(JSON_BOOL, k, CastUtil::lexical_cast<std::string>(v)));
	}
	return *this;
}

JSONElement& JSONElement::add(std::string k, long double v) {
	if(this->type == JSON_OBJECT) {
		addChild(JSONElement(JSON_FLOAT, k, CastUtil::lexical_cast<std::string>(v)));
	}
	return *this;
}
