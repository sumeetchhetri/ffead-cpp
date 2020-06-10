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
 * Query.h
 *
 *  Created on: 13-May-2014
 *      Author: sumeetc
 */

#ifndef QUERY_H_
#define QUERY_H_
#include "map"
#include "string"
#include "GenericObject.h"
#include "StringUtil.h"

typedef std::map<std::string, GenericObject> Parameters;
typedef std::map<int, GenericObject> PosParameters;

class Query {
	/*The column bindings used in the where clause for the entity*/
	Parameters columnBindings;
	/*The property/column names to be queried for the entity*/
	Parameters propNameVaues;
	/*The property/column positions to be queried for the entity*/
	PosParameters propPosVaues;
	/*The actual Query std::string*/
	std::string query;
	/*The start and count values required for pagination*/
	int start, count;
	/*The class for criteria building*/
	std::string className;
	std::string tableName;
	std::vector<std::string> aliasedColumns;
	friend class SQLDataSourceImpl;
public:
	Query();
	Query(const std::string& query);
	Query(const std::string& query, const std::string& className);
	Query addParameter(const std::string&, const GenericObject&);
	Query addParameters(const Parameters& propNameVaues);
	Query addParameter(const int&, const GenericObject&);
	Query addParameters(const PosParameters& propPosVaues);
	Query addColumnBinding(const std::string&, const GenericObject&);
	Query addColumnBindings(const Parameters& columnBindings);
	Query paginate(const int&, const int&);
	bool isUpdate();
	virtual ~Query();
	Parameters& getColumnBindings();
	Parameters& getPropNameVaues();
	PosParameters& getPropPosVaues();
	void setQuery(const std::string& query);
	int getCount() const;
	int getStart() const;
	const std::string& getClassName() const;
	const std::string& getQuery() const;
	void setCount(const int& count);
	void setStart(const int& start);
	const std::string& getTableName() const;
	void setTableName(const std::string& tableName);
	void setClassName(const std::string& className);
};

#endif /* QUERY_H_ */
