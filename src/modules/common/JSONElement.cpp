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

JSONElement::~JSONElement() {
	for (int var = 0; var < (int)children.size(); ++var) {
		//delete children.at(var);
	}
}

void JSONElement::addChild(JSONElement ele) {
	this->children.push_back(ele);
	JSONElement* child = &(this->children.back());
	if(child->name!="" && this->type!=JSON_ARRAY)
	{
		this->allnodes[child->name] = child;
	}
}

int JSONElement::getType() const {
	return type;
}

void JSONElement::setType(const int& type) {
	this->type = type;
}

const std::string& JSONElement::getValue() const {
	return value;
}

bool JSONElement::hasChildren() const {
	return this->children.size()>0;
}

void JSONElement::setValue(const std::string& value) {
	this->value = value;
}

const std::string& JSONElement::getName() const {
	return name;
}

const std::vector<JSONElement>& JSONElement::getChildren() const {
	return this->children;
}

void JSONElement::setName(const std::string& name) {
	this->name = name;
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
	std::string txt = "[name=" + name;
	txt += ", value=" + value;
	txt += ", children={";
	for (int var = 0; var < (int)children.size(); ++var) {
		txt += children.at(var).toString();
	}
	txt += "}]";
	return txt;
}

