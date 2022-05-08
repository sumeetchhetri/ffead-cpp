/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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

std::map<std::string, std::string>& IndexQuery::getMappings() {
	return mappings;
}

IndexQuery& IndexQuery::setMappings(const std::map<std::string, std::string>& mappings) {
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

std::map<std::string, std::string>& IndexQuery::getProperties() {
	return properties;
}

IndexQuery& IndexQuery::setProperties(const std::map<std::string, std::string>& properties) {
	this->properties = properties;
	return *this;
}

std::map<std::string, std::string>& IndexQuery::getExtras() {
	return extras;
}

IndexQuery& IndexQuery::setExtras(const std::map<std::string, std::string>& extras) {
	this->extras = extras;
	return *this;
}

int IndexQuery::getStatus() const {
	return status;
}

IndexQuery& IndexQuery::setStatus(int status) {
	this->status = status;
	return *this;
}

const std::string& IndexQuery::getOpType() const {
	return opType;
}

void IndexQuery::setOpType(const std::string &opType) {
	this->opType = opType;
}

IndexQuery::~IndexQuery() {
	
}

