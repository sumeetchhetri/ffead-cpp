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
	int storeProperty(ClassInfo& clas, void* t, int var, const std::string& fieldName, std::string& fldVal);
	int getProperty(const int& dataType, const int& columnSize, std::map<std::string, GenericObject>& colValMap, const std::string& colName, const int& var);
	long getNumRows(const std::string& clasName);
	void empty(const std::string& clasName);
	void storePropertyInt(const ClassInfo& clas, void* t, void* colV, const Field& fe, int& var);
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

	void* getContext(void* details);
	void destroyContext(void*);
};

#endif /* SQLDATASOURCEIMPL_H_ */
