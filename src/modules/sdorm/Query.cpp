/*
 * Query.cpp
 *
 *  Created on: 13-May-2014
 *      Author: sumeetc
 */

#include "Query.h"

Query::Query(const std::string& query) {
	this->start = 0;
	this->count = 0;
	this->query = query;
}

Query::Query(const std::string& query, const std::string& className) {
	this->start = 0;
	this->count = 0;
	this->query = query;
	this->className = className;
}

Query Query::addParameter(const std::string& paramName, const GenericObject& paramValue) {
	this->propNameVaues[paramName] = paramValue;
	return *this;
}

Query Query::addParameters(const Parameters& propNameVaues) {
	this->propNameVaues = propNameVaues;
	return *this;
}

Query Query::addParameter(const int& paramPos, const GenericObject& paramValue) {
	this->propPosVaues[paramPos] = paramValue;
	return *this;
}

Query Query::addParameters(const PosParameters& propPosVaues) {
	this->propPosVaues = propPosVaues;
	return *this;
}

Query Query::addColumnBinding(const std::string& columnName, const GenericObject& columnValue)
{
	this->columnBindings[columnName] = columnValue;
	return *this;
}

Query Query::addColumnBindings(const Parameters& columnBindings)
{
	this->columnBindings = columnBindings;
	return *this;
}

Query Query::paginate(const int& start, const int& count) {
	this->start = start;
	this->count = count;
	return *this;
}

Query::~Query() {
}

bool Query::isUpdate()
{
	if(StringUtil::toLowerCopy(query).find("select")==0)
	{
		return false;
	}
	return true;
}

Parameters& Query::getColumnBindings() {
	return columnBindings;
}

Parameters& Query::getPropNameVaues() {
	return propNameVaues;
}

PosParameters& Query::getPropPosVaues() {
	return propPosVaues;
}

int Query::getCount() const {
	return count;
}

void Query::setCount(const int& count) {
	this->count = count;
}

Query::Query() {
	start = -1;
	count = -1;
}

void Query::setStart(const int& start) {
	this->start = start;
}

const std::string& Query::getQuery() const {
	return query;
}

const std::string& Query::getClassName() const {
	return className;
}

int Query::getStart() const {
	return start;
}

void Query::setQuery(const std::string& query) {
	this->query = query;
}

const std::string& Query::getTableName() const {
	return tableName;
}

void Query::setTableName(const std::string& tableName) {
	this->tableName = tableName;
}

void Query::setClassName(const std::string& className) {
	this->className = className;
}
