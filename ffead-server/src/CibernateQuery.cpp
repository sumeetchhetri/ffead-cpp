/*
	Copyright 2010, Sumeet Chhetri

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
 * CibernateQuery.cpp
 *
 *  Created on: 19-Feb-2013
 *      Author: sumeetc
 */

#include "CibernateQuery.h"

CibernateQuery::CibernateQuery(string query) {
	this->start = -1;
	this->count = -1;
	this->query = query;
}

CibernateQuery::CibernateQuery(string query, string className) {
	this->start = -1;
	this->count = -1;
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

bool CibernateQuery::isUpdate()
{
	if(StringUtil::toLowerCopy(query).find("select")==0)
	{
		return false;
	}
	return true;
}
