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
 * DocumentQuery.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "DocumentQuery.h"

DocumentQuery::DocumentQuery() {

}

DocumentQuery::~DocumentQuery() {
}

std::string& DocumentQuery::getData() {
	return data;
}

const std::string& DocumentQuery::getId() const {
	return id;
}

void DocumentQuery::setId(const std::string& id) {
	this->id = id;
}

const std::string& DocumentQuery::getIndexName() const {
	return indexName;
}

void DocumentQuery::setIndexName(const std::string& indexName) {
	this->indexName = indexName;
}

std::map<std::string, std::string>& DocumentQuery::getProperties() {
	return properties;
}

void DocumentQuery::setProperties(const std::map<std::string, std::string>& properties) {
	this->properties = properties;
}

void DocumentQuery::setData(const std::string &data) {
	this->data = data;
}

const std::string& DocumentQuery::getOpType() const {
	return opType;
}

void DocumentQuery::setOpType(const std::string &opType) {
	this->opType = opType;
}
