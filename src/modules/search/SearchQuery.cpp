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
 * SearchQuery.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "SearchQuery.h"

SearchQuery::SearchQuery() {
}

SearchQuery::~SearchQuery() {
}

const std::string& SearchQuery::getIndexName() const {
	return indexName;
}

void SearchQuery::setIndexName(const std::string &indexName) {
	this->indexName = indexName;
}

std::map<std::string, std::string>& SearchQuery::getProperties() {
	return properties;
}

const std::string& SearchQuery::getId() const {
	return id;
}

void SearchQuery::setData(const std::string &data) {
}

std::string& SearchQuery::getData() {
	return data;
}

void SearchQuery::setId(const std::string &id) {
	this->id = id;
}

void SearchQuery::setProperties(const std::map<std::string, std::string> &properties) {
	this->properties = properties;
}
