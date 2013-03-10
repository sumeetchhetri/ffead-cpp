/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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
 * Cibernate.h
 *
 *  Created on: Jan 5, 2010
 *      Author: sumeet
 */

#ifndef CIBERNATE_H_
#define CIBERNATE_H_
#include <stdlib.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "Reflector.h"
#include "AfcUtil.h"
#include <iostream>
#include "Object.h"
#include "Timer.h"
#include "CibernateConnPools.h"
#include "DateFormat.h"
#include "BinaryData.h"
#include "Logger.h"
#include "CibernateQuery.h"
#include "RegexUtil.h"

typedef map<string,Object*> Params;
class Cibernate {
private:
	Logger logger;
	string demangle(const char *mangled);
	CibernateConnectionPool* pool;
	Mapping* mapping;
	string appName;
	Connection *conn;
	SQLHENV	V_OD_Env;// Handle ODBC environment
	SQLHDBC	V_OD_hdbc;// Handle connection
	SQLHSTMT V_OD_hstmt;//statement
	Params params,params1;
	bool igPaWC;
	bool allocateStmt(bool);
	map<string,string> ntmap,ntmap1;
	void clearMaps(){ntmap.clear();params.clear();ntmap1.clear();params1.clear();}
	void* getElements();
	void* getElements(string clasName);
	void* getElements(vector<string> cols,string clasName);
	void* executeQuery(CibernateQuery query);
	void bindQueryParams(CibernateQuery& query);
	void* executeQuery(CibernateQuery query, vector<string> cols);
	bool executeInsert(CibernateQuery cquery, vector<string> cols, void* t);
	bool executeUpdate(CibernateQuery cquery, vector<string> cols, void* t);
	int storeProperty(ClassInfo clas, void* t, int var, string fieldName);
	int getProperty(int dataType, int columnSize, map<string, void*>& colValMap, string colName, int var);
	void* sqlfuncs(string type,string clasName);
	bool init;
public:
	Cibernate();
	Cibernate(string);
	Cibernate(string,string,string,int);
	virtual ~Cibernate();
	void close();
	bool startTransaction();
	bool commit();
	bool rollback();
	void procedureCall(string);
	void addParam(string name,string type,Object &obj){ntmap[name]=type;params[name]=&obj;}
	void addParam(string name,Object &obj){params[name]=&obj;}
	Object getParam(string name){return *params[name];}
	void addParam1(string name,string type,Object &obj){ntmap1[name]=type;params1[name]=&obj;}
	void addParam1(string name,Object &obj){params1[name]=&obj;}
	Object getParam1(string name){return *params1[name];}
	vector<map<string,void*> > getARSCW(string tableName,vector<string> cols,vector<string> coltypes);
	void truncate(string clasName);
	void empty(string clasName);


	vector<map<string, void*> > execute(CibernateQuery query);
	template<class T> vector<T> getList(CibernateQuery query)
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		if(clasName!=query.className)
		{
			vector<T> vecT;
			return vecT;
		}
		vector<T> vecT;
		void* vect = executeQuery(query);
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
		}
		return vecT;
	}

	template<class T> T get(CibernateQuery query)
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		if(clasName!=query.className)
		{
			vector<T> vecT;
			return vecT;
		}
		vector<T> vecT;
		void* vect = executeQuery(query);
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
			if(vecT.size()>0)
			{
				t = vecT.at(0);
			}
			delete vect;
		}
		return t;
	}

	template<class T> bool executeUpdate(CibernateQuery query)
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		if(clasName!=query.className)
		{
			return false;
		}
		void* vect = executeQuery(query);
		if(vect!=NULL)
		{
			bool* flag = (bool*)vect;
			bool ffl = *flag;
			delete flag;
			return ffl;
		}
		return false;
	}

	template<class T> vector<T> getAll()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<string> cols;
		CibernateQuery query("", clasName);
		vector<T> vecT;
		void* vect = executeQuery(query, cols);
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
			delete vect;
		}
		return vecT;
	}

	template<class T> T get(int id)
	{
		Object oid;
		oid << id;
		vector<string> cols;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		CibernateQuery query("", clasName);
		query.addColumnBinding("id", oid);
		vector<T> vecT;
		void* vect = executeQuery(query, cols);
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
			if(vecT.size()>0)
			{
				t = vecT.at(0);
			}
			delete vect;
		}
		return t;
	}

	template<class T, class R> vector<R> getColumnValues(string name)
	{
		vector<string> cols;
		cols.push_back(name);
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		CibernateQuery query("", clasName);
		vector<T> vecT;
		void* vect = executeQuery(query, cols);
		vector<R> vecR;
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
			delete vect;
			if(vecT.size()>0)
			{
				Reflector reflector;
				ClassInfo clas = reflector.getClassInfo(clasName);
				args argus;
				string methname = "get"+AfcUtil::camelCased(name);
				Method meth = clas.getMethod(methname,argus);
				vals valus;
				for (int var = 0; var < (int)vecT.size(); ++var) {
					void *ns = reflector.invokeMethodGVP(&(vecT.at(var)),meth,valus);
					vecR.push_back(*(R*)ns);
					delete ns;
				}
			}
		}
		return vecR;
	}
	template<class T, class R> vector<R> getColumnValues(string name, PosParameters propPosVaues)
	{
		vector<string> cols;
		cols.push_back(name);
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		CibernateQuery query("", clasName);
		query.propPosVaues = propPosVaues;
		vector<T> vecT;
		void* vect = executeQuery(query, cols);
		vector<R> vecR;
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
			delete vect;
			if(vecT.size()>0)
			{
				Reflector reflector;
				ClassInfo clas = reflector.getClassInfo(clasName);
				args argus;
				string methname = "get"+AfcUtil::camelCased(name);
				Method meth = clas.getMethod(methname,argus);
				vals valus;
				for (int var = 0; var < (int)vecT.size(); ++var) {
					void *ns = reflector.invokeMethodGVP(&(vecT.at(var)),meth,valus);
					vecR.push_back(*(R*)ns);
					delete ns;
				}
			}
		}
		return vecR;
	}
	template<class T, class R> vector<R> getColumnValues(string name, Parameters propNameVaues)
	{
		vector<string> cols;
		cols.push_back(name);
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		CibernateQuery query("", clasName);
		query.propNameVaues = propNameVaues;
		vector<T> vecT;
		void* vect = executeQuery(query, cols);
		vector<R> vecR;
		if(vect!=NULL)
		{
			vecT = *(vector<T>*)vect;
			delete vect;
			if(vecT.size()>0)
			{
				Reflector reflector;
				ClassInfo clas = reflector.getClassInfo(clasName);
				args argus;
				string methname = "get"+AfcUtil::camelCased(name);
				Method meth = clas.getMethod(methname,argus);
				vals valus;
				for (int var = 0; var < (int)vecT.size(); ++var) {
					void *ns = reflector.invokeMethodGVP(&(vecT.at(var)),meth,valus);
					vecR->push_back(*(R*)ns);
					delete ns;
				}
				delete vecT;
			}
		}
		return vecR;
	}

	template<class T> void insert(T t)
	{
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		CibernateQuery cquery("", clasName);
		vector<string> cols;
		executeInsert(cquery, cols, &t);
	}

	template<class T> void bulkInsert(vector<T> vecT)
	{
		for(unsigned int k=0;k<vecT.size();k++)
		{
			T t = vecT.at(k);
			insert<T>(t);
		}
	}

	template<class T> void update(T t)
	{
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		CibernateQuery cquery("", clasName);
		vector<string> cols;
		executeUpdate(cquery, cols, &t);
	}

	template<class T> void truncate()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		empty(clasName);
	}

	template<class T> void empty()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		empty(clasName);
	}

	template<class T> long getNumRows()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		long size = *(long*)sqlfuncs("COUNT(*)",clasName);
		return size;
	}

	template<class T> int getSumValue(string col)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("SUM("+col+")","int");
		igPaWC = false;
		return size;
	}

	template<class T> int getAvgValue(string col)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("AVG("+col+")","int");
		igPaWC = false;
		return size;
	}

	template<class T> T getFirstValue(string col)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("first("+col+")",clasName);
		igPaWC = false;
		return t;
	}

	template<class T> T getLastValue(string col)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("last("+col+")",clasName);
		igPaWC = false;
		return t;
	}

	template<class T> T getMinValue(string col)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("min("+col+")",clasName);
		igPaWC = false;
		return t;
	}

	template<class T> T getMaxValue(string col)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("max("+col+")",clasName);
		igPaWC = false;
		return t;
	}
};

#endif /* CIBERNATE_H_ */
