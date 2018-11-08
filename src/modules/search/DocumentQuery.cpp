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

const std::string& DocumentQuery::getData() const {
	return data;
}

const std::map<std::string, std::string>& DocumentQuery::getFields() const {
	return fields;
}

void DocumentQuery::setFields(const std::map<std::string, std::string>& fields) {
	this->fields = fields;
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

const std::map<std::string, std::string>& DocumentQuery::getProperties() const {
	return properties;
}

void DocumentQuery::setProperties(const std::map<std::string, std::string>& properties) {
	this->properties = properties;
}
