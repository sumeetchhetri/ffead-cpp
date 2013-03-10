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
	bool isUpdate()
	{
		if(StringUtil::toLowerCopy(query).find("select")==0)
		{
			return false;
		}
		return true;
	}
};

#endif /* CIBERNATEQUERY_H_ */
