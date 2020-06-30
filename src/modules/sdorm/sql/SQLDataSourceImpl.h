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
 * SQLDataSourceImpl.h
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */

#ifndef SQLDATASOURCEIMPL_H_
#define SQLDATASOURCEIMPL_H_
#include "Compatibility.h"
#ifdef HAVE_LIBODBC
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#endif
#include "DataSourceInterface.h"
#include "DialectHelper.h"

typedef std::map<std::string, GenericObject> Params;

class SQLContext {
	Connection* conn;
	SQLHDBC V_OD_hdbc;
	SQLHSTMT V_OD_hstmt;
	friend class SQLDataSourceImpl;
	SQLContext();
	virtual ~SQLContext();
};

class SQLDataSourceImpl: public DataSourceInterface {
	Logger logger;
#ifdef HAVE_LIBODBC
	SQLHDBC V_OD_hdbc; // Handle connection
	SQLHSTMT V_OD_hstmt; //statement
#endif
	Params params;
	std::string appName, dialect;
	bool isTransaction;
	bool isSession;
	Connection *conn;
	bool allocateStmt(const bool&);
	void refreshStmt();
	std::map<std::string, std::string> ntmap;
	void clearMaps() {
		ntmap.clear();
		params.clear();
	}
	void* getElements();
	void* getElements(Query& q);
	void* getElements(const std::vector<std::string>& cols, Query& q);
	void showError(const char *fn, const SQLHANDLE& handle, const SQLSMALLINT& type);
	void close();
	void bindQueryParams(Query& query);
	Query fromQueryBuilder(QueryBuilder& qb);
	void* executeQueryObject(Query& cquery);
	void* executeQueryInternal(Query& query, const bool& isObj);
	int storeProperty(ClassInfo* clas, void* t, int var, const std::string& fieldName, std::string& fldVal);
	int getProperty(const int& dataType, const int& columnSize, std::map<std::string, GenericObject>& colValMap, const std::string& colName, const int& var);
	long getNumRows(const std::string& clasName);
	void empty(const std::string& clasName);
	void storePropertyInt(ClassInfo* clas, void* t, void* colV, const Field& fe, int& var);
public:
	SQLDataSourceImpl(ConnectionPooler* pool, Mapping* mapping);
	~SQLDataSourceImpl();
	bool startTransaction();
	bool commit();
	bool rollback();
	void procedureCall(const std::string&);
	std::vector<std::map<std::string, GenericObject> > execute(Query& query);
	bool executeUpdate(Query& query);
	std::vector<std::map<std::string, GenericObject> > execute(QueryBuilder& qb);

	bool startSession(void*);
	bool startSession();
	bool endSession();
protected:
	bool executeInsert(Query& query, void* entity);
	bool isGetDbEntityForBulkInsert();
	void* getDbEntityForBulkInsert(void* entity, const std::string& clasName, std::string& error);
	bool executeInsertBulk(Query& query, std::vector<void*> entities, std::vector<void*> dbEntities);
	bool executeUpdateBulk(Query& query, std::vector<void*> entities, std::vector<void*> dbEntities);
	bool executeUpdate(Query& query, void* entity);
	bool remove(const std::string& clasName, GenericObject& id);

	void* executeQuery(Query& query, const bool& isObj);
	void* executeQuery(QueryBuilder& qb, const bool& isObj);

	void executePreTable(DataSourceEntityMapping& dsemp, GenericObject& idv);
	void executePostTable(DataSourceEntityMapping& dsemp, GenericObject& idv);
	void executeSequence(DataSourceEntityMapping& dsemp, GenericObject& idv);
	void executeIdentity(DataSourceEntityMapping& dsemp, GenericObject& idv);
	void executeCustom(DataSourceEntityMapping& dsemp, const std::string& customMethod, GenericObject& idv);
};

#endif /* SQLDATASOURCEIMPL_H_ */
