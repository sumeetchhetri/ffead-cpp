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

typedef map<string,Object*> Params;
class Cibernate {
private:
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
	map<string,string> ntmap,ntmap1;
	void clearMaps(){ntmap.clear();params.clear();ntmap1.clear();params1.clear();}
	void binPrams(string &query);
	string selectQuery(string clasName);
	string selectQuery(vector<string> cols,string clasName);
	string insertQuery(vector<string> cols,string clasName,void *);
	string updateQuery(vector<string> cols,string clasName,void *t);
	void* getElements(string clasName);
	void* getElements(vector<string> cols,string clasName);
	bool init;
public:
	Cibernate();
	Cibernate(string);
	Cibernate(string,string,string);
	virtual ~Cibernate();
	bool allocateStmt(bool);
	void close();
	void startTransaction();
	void commit();
	void rollback();
	//void execute();
	void procedureCall(string);
	void addParam(string name,string type,Object &obj){ntmap[name]=type;params[name]=&obj;}
	void addParam(string name,Object &obj){params[name]=&obj;}
	Object getParam(string name){return *params[name];}
	void addParam1(string name,string type,Object &obj){ntmap1[name]=type;params1[name]=&obj;}
	void addParam1(string name,Object &obj){params1[name]=&obj;}
	Object getParam1(string name){return *params1[name];}
	vector<map<string,void*> > getARSCW(string tableName,vector<string> cols,vector<string> coltypes);
	void* getARACW(string clasName);
	void* getORW(string clasName);
	void* getARSCW(string clasName,vector<string> cols);
	void insertORSC(void* t,vector<string> cols,string className);
	void updateORSC(void* t,vector<string> cols,string className);
	void* sqlfuncs(string type,string clasName);

	/**********SELECT QUERIES ONE ROW/ALL COLS/SOME COLS(WHR),ALL ROWS/ALL COLS/SOME COLS(WHR),ALL ROWS/ONE COL(WHR) START********/
	template<class T> vector<T> getARAC()
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getARACW(clasName);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = false;
		return *vecT;
	}
	template<class T> vector<T> getARACW()
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getARACW(clasName);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = true;
		return *vecT;
	}
	template<class T> vector<T> getARSCW(vector<string> cols)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getARSCW(clasName,cols);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = true;
		return *vecT;
	}
	template<class T> vector<T> getARSC(vector<string> cols)
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getARSCW(clasName,cols);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = false;
		return *vecT;
	}
	template<class T> T getOR(int id)
	{
		igPaWC = false;
		clearMaps();
		Object on;
		on << id;
		addParam("id",on);
		vector<string> cols;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getORW(clasName);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = true;
		return vecT->at(0);
	}
	template<class T> T getORW()
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getORW(clasName);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = true;
		return vecT->at(0);
	}
	template<class T> vector<T> getAROC(string name)
	{
		igPaWC = true;
		vector<string> cols;
		cols.push_back(name);
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getARSCW(clasName,cols);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = false;
		return *vecT;
	}
	template<class T> vector<T> getAROCW(string name)
	{
		igPaWC = false;
		vector<string> cols;
		cols.push_back(name);
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<T> *vecT = new vector<T>;
		void* vect = getARSCW(clasName,cols);
		if(vect!=NULL)
			vecT = (vector<T>*)vect;
		igPaWC = true;
		return *vecT;
	}
	/**********SELECT QUERIES ONE ROW/ALL COLS/SOME COLS(WHR),ALL ROWS/ALL COLS/SOME COLS(WHR),ALL ROWS/ONE COL(WHR) END********/


	/**********INSERT QUERIES ONE ROW/ALL COLS/SOME COLS(WHR),BULK INSERTS START**********/
	template<class T> void insertORSC(T t,vector<string> cols)
	{
		igPaWC = true;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		insertORSC(&t,cols,clasName);
		igPaWC = false;
	}
	template<class T> void insertORSCW(T t,vector<string> cols)
	{
		igPaWC = false;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		insertORSC(&t,cols,clasName);
		igPaWC = true;
	}
	template<class T> void insertORAC(T t)
	{
		igPaWC = true;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<string> cols;
		insertORSC(&t,cols,clasName);
		igPaWC = false;
	}
	template<class T> void insertORACW(T t,vector<string> cols)
	{
		igPaWC = false;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		insertORSC(&t,cols,clasName);
		igPaWC = true;
	}
	template<class T> void bulkInsertRAC(vector<T> vecT)
	{
		allocateStmt(true);
		for(unsigned int k=0;k<vecT.size();k++)
		{
			T t = vecT.at(k);
			insertORAC<T>(t);
		}
	}
	template<class T> void bulkInsertRACW(vector<T> vecT)
	{
		allocateStmt(true);
		for(unsigned int k=0;k<vecT.size();k++)
		{
			T t = vecT.at(k);
			insertORACW<T>(t);
		}
	}
	template<class T> void bulkInsertRSC(vector<T> vecT,vector<string> cols)
	{
		allocateStmt(true);
		for(unsigned int k=0;k<vecT.size();k++)
		{
			T t = vecT.at(k);
			insertORSC<T>(t,cols);
		}
	}
	template<class T> void bulkInsertRSCW(vector<T> vecT,vector<string> cols)
	{
		allocateStmt(true);
		for(unsigned int k=0;k<vecT.size();k++)
		{
			T t = vecT.at(k);
			insertORSCW<T>(t,cols);
		}
	}
	/**********INSERT QUERIES ONE ROW/ALL COLS/SOME COLS(WHR),BULK INSERTS END**********/


	/**********UPDATE QUERIES ROWS/ALL COLS/SOME COLS(WHR) START**********/
	template<class T> void updateRsAC(T t)
	{
		igPaWC = true;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<string> cols;
		updateORSC(&t,cols,clasName);
		igPaWC = false;
	}
	template<class T> void updateRsACW(T t)
	{
		igPaWC = false;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		vector<string> cols;
		updateORSC(&t,cols,clasName);
		igPaWC = true;
	}
	template<class T> void updateRsSC(T t,vector<string> cols)
	{
		igPaWC = true;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		updateORSC(&t,cols,clasName);
		igPaWC = false;
	}
	template<class T> void updateRsSCW(T t,vector<string> cols)
	{
		igPaWC = false;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		updateORSC(&t,cols,clasName);
		igPaWC = true;
	}
	/**********UPDATE QUERIES ROWS/ALL COLS/SOME COLS(WHR) END**********/

	template<class T> void delAll()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		delAll(clasName);
	}
	template<class T> void delW()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		delW(clasName);
	}
	template<class T> void empty()
	{
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		empty(clasName);
	}
	void delAll(string clasName);
	void delW(string clasName);
	void empty(string clasName);

	template<class T> int getNumRows()
	{
		igPaWC = true;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("COUNT(*)","int");
		igPaWC = false;
		return size;
	}
	template<class T> int getNumRowsW()
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("COUNT(*)","int");
		igPaWC = true;
		return size;
	}
	template<class T> int getNumRowsCW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("COUNT("+col+")","int");
		igPaWC = true;
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
	template<class T> int getSumValueW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("SUM("+col+")","int");
		igPaWC = true;
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
	template<class T> int getAvgValueW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		int size = *(int*)sqlfuncs("AVG("+col+")","int");
		igPaWC = true;
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
	template<class T> T getFirstValueW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("first("+col+")",clasName);
		igPaWC = true;
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
	template<class T> T getLastValueW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("last("+col+")",clasName);
		igPaWC = true;
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
	template<class T> T getMinValueW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("min("+col+")",clasName);
		igPaWC = true;
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
	template<class T> T getMaxValueW(string col)
	{
		igPaWC = false;
		T t;
		const char *mangled = typeid(t).name();
		string clasName = demangle(mangled);
		t = *(T*)sqlfuncs("max("+col+")",clasName);
		igPaWC = true;
		return t;
	}
};

#endif /* CIBERNATE_H_ */
