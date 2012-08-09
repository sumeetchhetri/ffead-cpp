/*
 * JSONElement.cpp
 *
 *  Created on: 06-Aug-2012
 *      Author: sumeetc
 */

#include "JSONElement.h"

JSONElement::JSONElement() {
	// TODO Auto-generated constructor stub

}

JSONElement::~JSONElement() {
	// TODO Auto-generated destructor stub
}

void JSONElement::addChild(JSONElement* child) {
	this->children.push_back(child);
	if(child->name!="" && this->type!=JSON_ARRAY)
	{
		this->allnodes[child->name] = child;
	}
}

int JSONElement::getType() const {
	return type;
}

void JSONElement::setType(int type) {
	this->type = type;
}

string JSONElement::getValue() const {
	return value;
}

bool JSONElement::hasChildren() {
	return this->children.size()>0;
}

void JSONElement::setValue(string value) {
	this->value = value;
}

string JSONElement::getName() const {
	return name;
}

vector<JSONElement*> JSONElement::getChildren() {
	return this->children;
}

void JSONElement::setName(string name) {
	this->name = name;
}

JSONElement* JSONElement::getNode(const string& name) {
	if(allnodes.find(name)!=allnodes.end())
		return allnodes[name];
	return NULL;
}

string JSONElement::toString()
{
	string txt = "[name=" + name;
	txt += ", value=" + value;
	txt += ", children={";
	for (int var = 0; var < children.size(); ++var) {
		txt += children.at(var)->toString();
	}
	txt += "}]";
	return txt;
}

