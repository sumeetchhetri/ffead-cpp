/*
 * MongoDBDataSourceImpl.h
 *
 *  Created on: 02-Jun-2014
 *      Author: sumeetc
 */

#ifndef MONGODBDATASOURCEIMPL_H_
#define MONGODBDATASOURCEIMPL_H_

#include "TemplateEngine.h"
#include "mongoc.h"
#include "DataSourceInterface.h"

class QueryComponent {
	bool isAnd;
	bool undecided;
	std::vector<Condition> andClauses;
	std::vector<Condition> orClauses;
	std::vector<Condition> tempClauses;
	std::vector<QueryComponent*> andChildren;
	std::vector<QueryComponent*> orChildren;
	std::vector<QueryComponent*> tempChildren;
	QueryComponent* parent;
	bson_t* actualQuery;
	friend class MongoDBDataSourceImpl;
	QueryComponent();
	virtual ~QueryComponent();
};

class MongoContext {
	mongoc_collection_t *collection;
	Connection* conn;
	friend class MongoDBDataSourceImpl;
	MongoContext();
	virtual ~MongoContext();
};

class MongoDBDataSourceImpl: public DataSourceInterface {
	Logger logger;
	static QueryComponent* getQueryComponent(const std::vector<Condition>& conds);
	static void populateQueryComponents(QueryComponent* sq);
	static bson_t* createSubMongoQuery(std::vector<Condition>& conds);
	static void appendGenericObject(bson_t* b, const std::string& name, GenericObject& o);
	static std::map<std::string, std::map<std::string, Condition> > toMap(std::vector<Condition>& conds);
	void getBSONObjectFromObject(const std::string& clasName, void* object, bson_t*, const bool& isIdBsonAppend= true);
	std::string initializeQueryParts(Query& cquery, bson_t** fields, bson_t** querySpec, std::string& operationName);
	std::string initializeDMLQueryParts(Query& cquery, bson_t** data, bson_t** query, std::string& operationName);
	std::string getQueryForRelationship(const std::string& column, const std::string& type, void* val);
	void getMapOfProperties(bson_t* data, std::map<std::string, GenericObject>* map);
	void* getObject(bson_t* data, uint8_t* buf, uint32_t len, const std::string& clasName);
	void storeProperty(ClassInfo* clas, void* t, void* colV, const Field& fe);
	void* getResults(const std::string& tableNm, Query& cquery, bson_t* fields, bson_t* querySpec, const bool& isObj, const bool& isCountQuery);
	void* getResults(const std::string& tableNm, QueryBuilder& qb, bson_t* fields, bson_t* query, const bool& isObj);
	Connection* _conn();
	mongoc_collection_t* _collection(Connection*, const char*);
	void _release(Connection*, mongoc_collection_t*);
public:
	MongoDBDataSourceImpl(ConnectionPooler* pool, Mapping* mapping);
	~MongoDBDataSourceImpl();
	bool startTransaction();
	bool commit();
	bool rollback();
	void procedureCall(const std::string&);
	void empty(const std::string& clasName);
	long getNumRows(const std::string& clasName);
	bool executeUpdate(Query& query);
	std::vector<std::map<std::string, GenericObject> > execute(QueryBuilder& qb);
	std::vector<std::map<std::string, GenericObject> > execute(Query& query);
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

#endif /* MONGODBDATASOURCEIMPL_H_ */
