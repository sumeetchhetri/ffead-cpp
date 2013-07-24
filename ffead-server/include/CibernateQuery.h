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
 * CibernateQuery.h
 *
 *  Created on: 19-Feb-2013
 *      Author: sumeetc
 */

#ifndef CIBERNATEQUERY_H_
#define CIBERNATEQUERY_H_
#include "map"
#include "string"
#include "Object.h"
#include "StringUtil.h"
using namespace std;
typedef map<string, Object> Parameters;
typedef map<int, Object> PosParameters;

class CibernateQuery {
	friend class Cibernate;
	/*The column bindings used in the where clause for the entity*/
	Parameters columnBindings;
	/*The property/column names to be queried for the entity*/
	Parameters propNameVaues;
	/*The property/column positions to be queried for the entity*/
	PosParameters propPosVaues;
	/*The actual Query string*/
	string query;
	/*The actual Order By string*/
	map<string, bool> orderByDescCols;
	map<string, bool> orderByAscCols;
	/*The start and count values required for pagination*/
	int start, count;
	/*The class for criteria building*/
	string className;
public:
	CibernateQuery(string query);
	CibernateQuery(string query,string className);
	CibernateQuery addParameter(string,Object);
	CibernateQuery addParameters(Parameters propNameVaues);
	CibernateQuery addParameter(int,Object);
	CibernateQuery addParameters(PosParameters propPosVaues);
	CibernateQuery addColumnBinding(string,Object);
	CibernateQuery addColumnBindings(Parameters columnBindings);
	CibernateQuery orderByAsc(string column);
	CibernateQuery orderByDesc(string column);
	CibernateQuery paginate(int,int);
	virtual ~CibernateQuery();
	bool isUpdate();
};

#endif /* CIBERNATEQUERY_H_ */
