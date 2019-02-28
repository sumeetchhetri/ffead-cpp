/*
 * IndexQuery.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "IndexQuery.h"

IndexQuery::IndexQuery() {
	status = -1;
}

const std::string& IndexQuery::getError() const {
	return error;
}

IndexQuery& IndexQuery::setError(const std::string& error) {
	this->error = error;
	return *this;
}

const std::map<std::string, std::string>& IndexQuery::getMappings() const {
	return mappings;
}

IndexQuery& IndexQuery::setMappings(
		const std::map<std::string, std::string>& mappings) {
	this->mappings = mappings;
	return *this;
}

const std::string& IndexQuery::getName() const {
	return name;
}

IndexQuery& IndexQuery::setName(const std::string& name) {
	this->name = name;
	return *this;
}

const std::map<std::string, std::string>& IndexQuery::getProperties() const {
	return properties;
}

IndexQuery& IndexQuery::setProperties(
		const std::map<std::string, std::string>& properties) {
	this->properties = properties;
	return *this;
}

int IndexQuery::getStatus() const {
	return status;
}

IndexQuery& IndexQuery::setStatus(int status) {
	this->status = status;
	return *this;
}

IndexQuery::~IndexQuery() {
	// TODO Auto-generated destructor stub
}

