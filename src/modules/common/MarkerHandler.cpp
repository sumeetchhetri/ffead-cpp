/*
 * MarkerHandler.cpp
 *
 *  Created on: 20-Aug-2014
 *      Author: sumeetc
 */

#include "MarkerHandler.h"

MarkerHandler::MarkerHandler() {
	initMarkers();
}

void MarkerHandler::addMarker(const Marker& m) {
	validMarkers.push_back(m);
}

void MarkerHandler::initMarkers() {
	//Start DsORM markers
	validMarkers.push_back(Marker("@Entity", Marker::TYPE_CLASS, collectStr(2, "dataSourceName", "isEmbedded,false,true,false"),
			collectBool(2, false, false)));
	validMarkers.push_back(Marker("@Table", Marker::TYPE_CLASS, collectStr(1, "name")));
	validMarkers.push_back(Marker("@Column", Marker::TYPE_PROP, collectStr(1, "dbf")));
	validMarkers.push_back(Marker("@IdGenerate", Marker::TYPE_PROP, collectStr(7, "dbEntityType,,table,sequence,identity",
			"dbEntityName", "type,,increment,hilo", "hiValueColumn", "lowValue,32767", "entityColumn", "columnName")));
	validMarkers.push_back(Marker("@Id", Marker::TYPE_PROP, collectStr(1, "dbf")));
	validMarkers.push_back(Marker("@HasOne", Marker::TYPE_PROP, collectStr(2, "dmappedBy", "dfk")));
	validMarkers.push_back(Marker("@HasMany", Marker::TYPE_PROP, collectStr(2, "dmappedBy", "dfk")));
}

MarkerHandler::~MarkerHandler() {
	// TODO Auto-generated destructor stub
}

const std::map<std::string, bool>& Marker::getAttributes() const {
	return attributes;
}

const std::string& Marker::getName() const {
	return name;
}

std::vector<std::string> MarkerHandler::collectStr(int num, ...) {
	std::vector<std::string> frags;
	va_list ap;
	va_start(ap, num);
	for(int i = 0; i < num; i++) {
		std::string a = std::string(va_arg(ap, char*));
		frags.push_back(a);
	}
	va_end(ap);
	return frags;
}

std::vector<bool> MarkerHandler::collectBool(int num, ...) {
	std::vector<bool> frags;
	va_list ap;
	va_start(ap, num);
	for(int i = 0; i < num; i++) {
		bool a = va_arg(ap, int);
		frags.push_back(a);
	}
	va_end(ap);
	return frags;
}

Marker MarkerHandler::getMarker(const std::string& name, const int& where) {
	for (int var = 0; var < (int)validMarkers.size(); ++var) {
		Marker mark = validMarkers.at(var);
		if(mark.name==name && mark.type==where)
		{
			return mark;
		}
	}
	Marker m;
	return m;
}

Marker MarkerHandler::getMarker(const std::string& name) {
	for (int var = 0; var < (int)validMarkers.size(); ++var) {
		Marker mark = validMarkers.at(var);
		if(mark.name==name)
		{
			return mark;
		}
	}
	Marker m;
	return m;
}

Marker MarkerHandler::processMarker(std::string markerText, const int& where) {
	StringUtil::trim(markerText);
	if(markerText.find("#pragma ")!=0)
	{
		throw std::string("Invalid Marker, all markers should start with the #pragma pre-processor directive");
	}
	StringUtil::replaceFirst(markerText, "#pragma ", "");
	std::vector<std::string> fragments = StringUtil::splitAndReturn<std::vector<std::string> >(markerText, " ");
	std::string name = fragments.at(0);
	if(name=="")
	{
		throw std::string("NOT_MARKER");
	}

	if(validMarkers.size()==0)
	{
		initMarkers();
	}

	Marker targetMarker = getMarker(name, where);
	if(targetMarker.name=="")
	{
		std::string err = "Could not find the "+Marker::getTypeName(where)+" type marker with name " + name;
		throw err;
	}
	targetMarker = getMarker(name);
	if(targetMarker.name=="")
	{
		std::string err = "Could not find a marker with name " + name;
		throw err;
	}
	if(targetMarker.reqAttrSize>0 && (int)fragments.size()==1)
	{
		std::string err = "No attributes specified for the marker " + targetMarker.name;
		throw err;
	}
	else
	{
		std::map<std::string, std::string> tvalues;
		for (int i = 1; i < (int)fragments.size(); ++i) {
			if(fragments.at(i)!="")
			{
				std::vector<std::string> attr = StringUtil::splitAndReturn<std::vector<std::string> >(fragments.at(i), "=");
				std::string attname = attr.at(0);
				std::string value = attr.at(1);
				StringUtil::trim(attname);
				StringUtil::trim(value);
				if(value.at(0)!='"' && value.at(value.length()-1)!='"')
				{
					std::string err = "Attribute value for "+attname+" should be within double quotes (\"\") for the marker " + targetMarker.name;
					throw err;
				}
				if(value!="\"\"")
				{
					value = value.substr(1, value.length()-2);
				}
				else
				{
					value = "";
				}
				tvalues[attname] = value;
			}
		}
		std::map<std::string, bool>::iterator it = targetMarker.attributes.begin();
		for(;it!=targetMarker.attributes.end();++it) {
			if(tvalues.find(it->first)==tvalues.end())
			{
				if(it->second) {
					std::string err = "No value specified for mandatory attribute "+it->first+" for the marker " + targetMarker.name;
					throw err;
				}
				std::cout << "Ignoring attribute " + it->first + " for marker " + targetMarker.name << std::endl;
				tvalues.erase(it->first);
			}
			else
			{
				std::vector<std::string> vss = targetMarker.valueSet[it->first];
				std::string value = tvalues[it->first];
				if(value=="")
				{
					std::string err = "Attribute value for "+it->first+" cannot be blank for the marker " + targetMarker.name;
					throw err;
				}
				if(vss.size()>0)
				{
					bool valid = false;
					for (int var = 0; var < (int)vss.size(); ++var) {
						if(vss.at(var)==value)
						{
							valid = true;
							break;
						}
					}
					if(!valid)
					{
						std::string err = "Attribute "+it->first+" cannot have a value of "+value+" for the marker " + targetMarker.name;
						throw err;
					}
				}
			}
		}
		targetMarker.attributeValues = tvalues;
	}
	return targetMarker;
}

Marker::Marker() {
	this->reqAttrSize = 0;
	this->type = -1;
}

Marker::Marker(const std::string& name, const int& type) {
	this->name = name;
	this->type = type;
	this->reqAttrSize = 0;
}

const bool Marker::isTypeClass() const {
	return TYPE_CLASS == type;
}

const bool Marker::isTypeProp() const {
	return TYPE_PROP == type;
}

const bool Marker::isTypeMeth() const {
	return TYPE_METH == type;
}

const bool Marker::isTypeArg() const {
	return TYPE_ARG == type;
}

std::string Marker::getTypeName() {
	if(isTypeClass())
		return "Class";
	else if(isTypeMeth())
		return "Method";
	else if(isTypeProp())
		return "Property";
	else if(isTypeArg())
		return "Argument";
	else
		return "Invalid";
}

std::string Marker::getTypeName(const int& type) {
	if(type == TYPE_CLASS)
		return "Class";
	else if(type == TYPE_METH)
		return "Method";
	else if(type == TYPE_PROP)
		return "Property";
	else if(type == TYPE_ARG)
		return "Argument";
	else
		return "Invalid";
}

Marker::Marker(const std::string& name, const int& type, const std::vector<std::string>& attributes) {
	this->name = name;
	this->type = type;
	this->reqAttrSize = (int)attributes.size();
	for (int i = 0; i < (int)attributes.size(); ++i) {
		std::string text = attributes[i];
		std::vector<std::string> fragments = StringUtil::splitAndReturn<std::vector<std::string> >(text, ",");
		std::string name = fragments.at(0);
		this->attributes[name] = true;
		if(fragments.size()>1)
		{
			this->defValues[name] = fragments.at(1);
		}
		if(fragments.size()>2)
		{
			for (int var = 2; var < (int)fragments.size(); ++var) {
				this->valueSet[name].push_back(fragments.at(var));
			}
		}
	}
}

Marker::Marker(const std::string& name, const int& type, const std::vector<std::string>& attributes, const std::vector<bool>& reqLst) {
	this->name = name;
	this->type = type;
	this->reqAttrSize = 0;
	for (int i = 0; i < (int)attributes.size(); ++i) {
		std::string text = attributes[i];
		std::vector<std::string> fragments = StringUtil::splitAndReturn<std::vector<std::string> >(text, ",");
		std::string name = fragments.at(0);
		this->attributes[name] = (int)reqLst.size()>i?reqLst.at(i):true;
		if(this->attributes[name]) {
			this->reqAttrSize++;
		}
		if(fragments.size()>1)
		{
			this->defValues[name] = fragments.at(1);
		}
		if(fragments.size()>2)
		{
			for (int var = 2; var < (int)fragments.size(); ++var) {
				this->valueSet[name].push_back(fragments.at(var));
			}
		}
	}
}

std::string Marker::getAttributeValue(const std::string& name)
{
	if(attributeValues.find(name)!=attributeValues.end())
	{
		return attributeValues[name];
	}
	if(defValues.find(name)!=defValues.end())
	{
		return defValues[name];
	}
	return "";
}
