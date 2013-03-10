/*
 * CibernateQuery.cpp
 *
 *  Created on: 19-Feb-2013
 *      Author: sumeetc
 */

#include "CibernateQuery.h"

CibernateQuery::CibernateQuery(string query) {
	this->query = query;
}

CibernateQuery::CibernateQuery(string query, string className) {
	this->query = query;
	this->className = className;
}

CibernateQuery CibernateQuery::addParameter(string paramName, Object paramValue) {
	this->propNameVaues[paramName] = paramValue;
	return *this;
}

CibernateQuery CibernateQuery::addParameters(Parameters propNameVaues) {
	this->propNameVaues = propNameVaues;
	return *this;
}

CibernateQuery CibernateQuery::addParameter(int paramPos, Object paramValue) {
	this->propPosVaues[paramPos] = paramValue;
	return *this;
}

CibernateQuery CibernateQuery::addParameters(PosParameters propPosVaues) {
	this->propPosVaues = propPosVaues;
	return *this;
}

CibernateQuery CibernateQuery::addColumnBinding(string columnName, Object columnValue)
{
	this->columnBindings[columnName] = columnValue;
	return *this;
}

CibernateQuery CibernateQuery::addColumnBindings(Parameters columnBindings)
{
	this->columnBindings = columnBindings;
	return *this;
}

CibernateQuery CibernateQuery::orderByAsc(string column) {
	this->orderByDescCols[column] = true;
	return *this;
}

CibernateQuery CibernateQuery::orderByDesc(string column) {
	this->orderByAscCols[column] = true;
	return *this;
}

CibernateQuery CibernateQuery::paginate(int start, int count) {
	this->start = start;
	this->count = count;
	return *this;
}

CibernateQuery::~CibernateQuery() {
	// TODO Auto-generated destructor stub
}

