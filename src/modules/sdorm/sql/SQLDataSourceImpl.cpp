/*
 * SQLDataSourceImpl.cpp
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */

#include "SQLDataSourceImpl.h"

Query SQLDataSourceImpl::fromQueryBuilder(QueryBuilder& qb) {
	Query q;
	std::string qs;
	qs.append("SELECT ");
	if(qb.isUnique())
		qs.append("DISTINCT ");

	std::string tableNm = qb.getTableName();
	std::string alias = qb.getAlias();
	bool isClassProps = false;

	DataSourceEntityMapping temp;
	DataSourceEntityMapping& dsemp = temp;

	q.setClassName(qb.getClassName());
	if(qb.getClassName()!="") {
		dsemp = mapping->getDataSourceEntityMapping(qb.getClassName());
		tableNm = dsemp.getTableName();
		alias = qb.getClassName();
		isClassProps = true;
	}

	if(alias=="") {
		alias = tableNm + "_";
	}

	if(qb.isAllCols() || qb.getColumns().size()==0)
	{
		int var = 1;
		strMap& tabcolmap = this->mapping->getMappingForTable(tableNm);
		int fldsiz = (int)tabcolmap.size();
		strMap::iterator it;
		for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
		{
			std::string columnName = it->first;
			qs.append(qb.getClassName() + "."  + columnName);
			if(var++!=fldsiz)
				qs.append(",");
			q.aliasedColumns.push_back(qb.getClassName() + "."  + columnName);
		}
	}
	else
	{
		std::map<std::string, std::string> cols = qb.getColumns();
		std::map<std::string, std::string>::iterator it;
		for (it=cols.begin(); it!=cols.end(); ++it) {
			if(!isClassProps)
			{
				qs.append(alias);
				qs.append(".");
				qs.append(it->first);
				if(it->second!="")
				{
					qs.append(" AS ");
					qs.append(it->second);
					qs.append(",");
				}
			}
			else if(dsemp.getColumnForProperty(it->first)!="")
			{
				qs.append(alias);
				qs.append(".");
				qs.append(dsemp.getColumnForProperty(it->first));
				qs.append(",");
			}
		}
	}
	if(qs.at(qs.length()-1)==',') {
		qs = qs.substr(0, qs.length()-1);
	}

	qs.append(" FROM ");
	qs.append(tableNm);
	if(alias!="")
	{
		qs.append(" ");
		qs.append(alias);
	}

	int paramPosition = 1;
	if(qb.getConditions().getConds().size()>0) {
		qs.append(" WHERE ");
		for (int var = 0; var < (int)qb.getConditions().getConds().size(); ++var) {
			Condition cond = qb.getConditions().getConds().at(var);
			if(cond.getClause() != QueryClause::NONE) {
				switch(cond.getClause().get())
				{
					case 2:
						qs.append("(");
						break;
					case 3:
						qs.append(")");
						break;
					case 4:
						qs.append(" AND ");
						break;
					case 5:
						qs.append(" OR ");
						break;
					default:
						break;
				}
			}
			if(cond.getOper() != QueryOperator::NONE) {
				qs.append(alias);
				qs.append(".");
				qs.append(cond.getLhs());
				switch(cond.getOper().get())
				{
					case 2:
						qs.append(" = ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 3:
						qs.append(" <> ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 7:
						qs.append(" > ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 8:
						qs.append(" < ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 9:
						qs.append(" >= ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 10:
						qs.append(" <= ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 6:
						qs.append(" BETWEEN");
						qs.append(" ?");
						qs.append(" AND");
						qs.append(" ?");
						q.addParameter(paramPosition++, cond.getRhs());
						q.addParameter(paramPosition++, cond.getRhs(1));
						break;
					default:
						break;
				}
			}
		}
	}

	if(qb.getColumnsAsc().size()>0 || qb.getColumnsDesc().size()>0) {
		qs.append(" ORDER BY ");
	}

	if(qb.getColumnsAsc().size()>0) {
		for (int var = 0; var < (int)qb.getColumnsAsc().size(); ++var) {
			qs.append(alias);
			qs.append(".");
			qs.append(qb.getColumnsAsc().at(var));
			qs.append(" ASC,");
		}
	}
	if(qs.at(qs.length()-1)==',') {
		qs = qs.substr(0, qs.length()-1);
	}

	if(qb.getColumnsDesc().size()>0) {
		for (int var = 0; var < (int)qb.getColumnsDesc().size(); ++var) {
			qs.append(alias);
			qs.append(".");
			qs.append(qb.getColumnsDesc().at(var));
			qs.append(" DESC,");
		}
	}
	if(qs.at(qs.length()-1)==',') {
		qs = qs.substr(0, qs.length()-1);
	}

	if(qb.getGroup().getColumns().size()>0) {
		qs.append(" GROUP BY ");
		for (int var = 0; var < (int)qb.getGroup().getColumns().size(); ++var) {
			qs.append(alias);
			qs.append(".");
			qs.append(qb.getGroup().getColumns().at(var));
			qs.append(",");
		}
	}
	if(qs.at(qs.length()-1)==',') {
		qs = qs.substr(0, qs.length()-1);
	}

	if(qb.getGroup().getConditions().getConds().size()>0) {
		qs.append(" HAVING ");
		for (int var = 0; var < (int)qb.getGroup().getConditions().getConds().size(); ++var) {
			Condition cond = qb.getGroup().getConditions().getConds().at(var);
			if(cond.getClause() != QueryClause::NONE) {
				switch(cond.getClause().get())
				{
					case 2:
						qs.append("(");
						break;
					case 3:
						qs.append(")");
						break;
					case 4:
						qs.append(" AND ");
						break;
					case 5:
						qs.append(" OR ");
						break;
					default:
						break;
				}
			}
			if(cond.getOper() != QueryOperator::NONE) {
				qs.append(alias);
				qs.append(".");
				qs.append(cond.getLhs());
				switch(cond.getOper().get())
				{
					case 2:
						qs.append(" = ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 3:
						qs.append(" <> ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 7:
						qs.append(" > ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 8:
						qs.append(" < ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 9:
						qs.append(" >= ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 10:
						qs.append(" <= ?");
						q.addParameter(paramPosition++, cond.getRhs());
						break;
					case 6:
						qs.append(" BETWEEN");
						qs.append(" ?");
						qs.append(" AND");
						qs.append(" ?");
						q.addParameter(paramPosition++, cond.getRhs());
						q.addParameter(paramPosition++, cond.getRhs(1));
						break;
					default:
						break;
				}
			}
		}
	}

	q.setQuery(qs);
	q.setStart(qb.getStart());
	q.setCount(qb.getCount());
	return q;
}

void SQLDataSourceImpl::executePreTable(DataSourceEntityMapping& dsemp, GenericObject& idv) {
	long long id = -1;
	if(dsemp.getIdgendbEntityType()=="table")
	{
		std::string query;
		Query q;
		if(dsemp.getIdgentype()=="hilo" || dsemp.getIdgentype()=="multihilo")
		{
			StringContext params;
			params["idgen_tabname"] = dsemp.getIdgendbEntityName();
			params["idgen_colname"] = dsemp.getIdgencolumnName();
			if(dsemp.getIdgentype()=="multihilo")
			{
				params["entity_column"] = dsemp.getIdgenentityColumn();
				query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_TABLESELMHL_QUERY, params);
				q.getPropPosVaues()[1].set(dsemp.getTableName());
			}
			else
			{
				query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_TABLESEL_QUERY, params);
			}
		}
		else
		{
			query = DialectHelper::getIdGenerateQueryPre(dialect, dsemp);
			if(dsemp.getIdgentype()=="multi") {
				q.getPropPosVaues()[1].set(dsemp.getTableName());
			}
		}

		q.setQuery(query);
		std::vector<std::map<std::string, GenericObject> > vecmp = execute(q);
		if(vecmp.size()>0 && vecmp.at(0).size()>0)
		{
			if(vecmp.at(0).begin()->second.getTypeName()!="")
			{
				vecmp.at(0).begin()->second.get(id);
			}
		}
	}
	idv.set(id);
}

void SQLDataSourceImpl::executePostTable(DataSourceEntityMapping& dsemp, GenericObject& idv) {
	if(dsemp.getIdgendbEntityType()=="table")
	{
		std::string query;
		Query q;
		if(dsemp.getIdgentype()=="hilo" || dsemp.getIdgentype()=="multihilo")
		{
			StringContext params;
			params["idgen_tabname"] = dsemp.getIdgendbEntityName();
			params["idgen_colname"] = dsemp.getIdgencolumnName();
			if(dsemp.getIdgentype()=="multihilo")
			{
				params["entity_column"] = dsemp.getIdgenentityColumn();
				query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_TABLEUPDMHL_QUERY, params);
				q.getPropPosVaues()[3].set(dsemp.getTableName());
			}
			else
			{
				query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_TABLEUPD_QUERY, params);
			}
		}
		else
		{
			query = DialectHelper::getIdGenerateQueryPost(dialect, dsemp);
			if(dsemp.getIdgentype()=="multi") {
				q.getPropPosVaues()[3].set(dsemp.getTableName());
			}
		}

		q.getPropPosVaues()[2] = idv;

		long long prev;
		idv.get(prev);
		prev = prev+1;
		GenericObject nid;
		nid.set(prev);

		q.getPropPosVaues()[1] = nid;

		q.setQuery(query);
		execute(q);
	}
}

void SQLDataSourceImpl::executeSequence(DataSourceEntityMapping& dsemp, GenericObject& idv) {
	long long id = -1;
	if(dsemp.getIdgendbEntityType()=="sequence")
	{
		StringContext params;
		params["seq_name"] = dsemp.getIdgendbEntityName();
		std::string query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_SEQUENCE_QUERY, params);

		Query q(query);
		std::vector<std::map<std::string, GenericObject> > vecmp = execute(q);
		if(vecmp.size()>0 && vecmp.at(0).size()>0)
		{
			if(vecmp.at(0).begin()->second.getTypeName()!="")
			{
				vecmp.at(0).begin()->second.get(id);
			}
		}
	}
	idv.set(id);
}

void SQLDataSourceImpl::executeIdentity(DataSourceEntityMapping& dsemp, GenericObject& idv) {
	long long id = -1;
	if(dsemp.getIdgendbEntityType()=="identity")
	{
		std::string query = DialectHelper::getIdGenerateQueryPost(dialect, dsemp);
		Query q(query);
		std::vector<std::map<std::string, GenericObject> > vecmp = execute(q);
		if(vecmp.size()>0 && vecmp.at(0).size()>0)
		{
			if(vecmp.at(0).begin()->second.getTypeName()!="")
			{
				vecmp.at(0).begin()->second.get(id);
			}
		}
	}
	idv.set(id);
}

SQLDataSourceImpl::SQLDataSourceImpl(ConnectionPooler* pool, Mapping* mapping) {
	this->pool = pool;
	this->mapping = mapping;
	std::map<std::string, std::string> props = pool->getProperties().getProperties();
	this->dialect = props["dialect"];
	this->appName = mapping->getAppName();
	V_OD_hdbc = NULL;
	V_OD_hstmt = NULL;
	isTransaction = false;
	conn = NULL;
	logger = LoggerFactory::getLogger("SQLDataSourceImpl");
}

SQLDataSourceImpl::~SQLDataSourceImpl() {
}

void SQLDataSourceImpl::executeCustom(DataSourceEntityMapping& dsemp, const std::string& customMethod, GenericObject& idv) {
	long long id = -1;
	if(dsemp.getIdgendbEntityType().find("custom:")==0)
	{}
	idv.set(id);
}

bool SQLDataSourceImpl::allocateStmt(const bool& read) {
#ifdef HAVE_LIBODBC
	if(this->pool!=NULL && !this->pool->isInitialized())
	{
		return false;
	}
	if (this->pool != NULL && this->conn != NULL && this->conn->isType() == read && V_OD_hstmt != NULL) {
		refreshStmt();
		//return false;
	}
	int V_OD_erg;// result of functions
	if (read) {
		if (this->pool != NULL)
			conn = this->pool->checkout(false);
		V_OD_hdbc = conn->getConn();
	} else {
		if (this->pool != NULL)
			conn = this->pool->checkout(true);
		V_OD_hdbc = conn->getConn();
	}
	V_OD_erg = SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		return false;
	}
	return true;
#endif
	return false;
}

void SQLDataSourceImpl::refreshStmt() {
#ifdef HAVE_LIBODBC
	SQLFreeStmt(V_OD_hstmt, SQL_UNBIND);
	SQLFreeStmt(V_OD_hstmt, SQL_RESET_PARAMS);
	SQLFreeStmt(V_OD_hstmt, SQL_CLOSE);
#endif
}

void SQLDataSourceImpl::close() {
#ifdef HAVE_LIBODBC
	this->pool->release(conn);
	V_OD_hdbc = NULL;
	SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	V_OD_hstmt = NULL;
#endif
}

void* SQLDataSourceImpl::getElements(Query& q)
{
	std::vector<std::string> cols;
	return getElements(cols,q);
}

void* SQLDataSourceImpl::getElements(const std::vector<std::string>& cols, Query& q)
{
#ifdef HAVE_LIBODBC
	std::string clasName = q.getClassName();
	int V_OD_erg;// result of functions

	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(clasName);

	ClassInfo clas = reflector->getClassInfo(clasName, appName);
	std::string tableName = dsemp.getTableName();
	std::vector<DataSourceInternalRelation> relv = dsemp.getRelations();

	fldMap fields = clas.getFields();
	fldMap::iterator it;
	void *vecT = reflector->getNewContainer(clasName,"std::vector",appName);
	SQLSMALLINT	V_OD_colanz;

	std::map<std::string, void*> rel2Insmap;
	std::map<std::string, void*> rel2Vecmap;
	std::map<std::string, std::string> fldvalrel2clsmap;
	std::map<std::string, std::string> fldnmrel2clsmap;
	std::map<std::string, int> rel2reltype;

	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;
		args argus1;
		std::map<std::string, void*> instances;
		void* relVecEle = NULL;
		std::string fldVal;
		const Constructor& ctor = clas.getConstructor(argus1);
		void *t = reflector->newInstanceGVP(ctor);

		V_OD_erg = SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			close();
		}
		bool norel = true;
		//logger << "Number of Columns " << V_OD_colanz << std::endl;
		for(int i=1;i<=V_OD_colanz;i++)
		{
			std::string columnName = q.aliasedColumns.at(i-1);

			std::string thisTableName = tableName;

			if(columnName.find(".")!=std::string::npos)
			{
				std::string tclas = columnName.substr(0, columnName.find("."));
				thisTableName = mapping->getTableForClass(tclas);
				columnName = columnName.substr(columnName.find(".")+1);
			}

			StringUtil::toLower(columnName);

			void* instance = NULL;
			ClassInfo instanceClas;
			DataSourceInternalRelation instanceRelation;

			std::string fieldName = this->mapping->getPropertyForColumn(thisTableName, columnName);

			if(thisTableName==tableName && fieldName!="")
			{
				instanceClas = clas;
				instance = t;
			}
			else
			{
				for (int var1 = 0; var1 < (int)relv.size(); ++var1)
				{
					DataSourceInternalRelation relation = relv.at(var1);
					if(relation.getType()==1 || relation.getType()==2)
					{
						norel = false;
						rel2reltype[fldVal] = relation.getType();
						fldnmrel2clsmap[fldVal] = relation.getField();
						fldvalrel2clsmap[fldVal] = relation.getClsName();
						std::string tableName1 = this->mapping->getTableForClass(relation.getClsName());
						fieldName = this->mapping->getPropertyForColumn(tableName1, columnName);
						if(fieldName!="" && tableName1==thisTableName)
						{
							thisTableName = tableName1;
							args argus1;
							ClassInfo clas1 = reflector->getClassInfo(relation.getClsName(),appName);
							if(relation.getType()==1)
							{
								if(instances.find(tableName1)==instances.end())
								{
									const Constructor& ctor = clas1.getConstructor(argus1);
									void *tt = reflector->newInstanceGVP(ctor);
									instances[tableName1] = tt;
									rel2Vecmap[fldVal] = tt;
								}
								instance = instances[thisTableName];
							}
							else
							{
								if(rel2Vecmap.find(fldVal)==rel2Vecmap.end())
								{
									void *dvecT = reflector->getNewContainer(relation.getClsName(),"std::vector",appName);
									rel2Vecmap[fldVal] = dvecT;
								}
								if(relVecEle==NULL)
								{
									const Constructor& ctor = clas1.getConstructor(argus1);
									relVecEle = reflector->newInstanceGVP(ctor);
								}
								instance = relVecEle;
							}

							instanceClas = clas1;
							instanceRelation = relation;
							break;
						}
					}
				}
			}

			std::string fldvalt;
			var = storeProperty(instanceClas, instance, var, fieldName, fldvalt);
			if(thisTableName==tableName && fieldName!="" && fieldName==dsemp.getIdPropertyName()) {
				fldVal = fldvalt;
			}
		}
		if(fldVal!="" && rel2reltype[fldVal]==2 && rel2Vecmap.find(fldVal)!=rel2Vecmap.end() && fldvalrel2clsmap.find(fldVal)!=fldvalrel2clsmap.end())
		{
			reflector->addToContainer(rel2Vecmap[fldVal],relVecEle,fldvalrel2clsmap[fldVal],"std::vector",appName);
			delete relVecEle;
		}
		rel2Insmap[fldVal] = t;

		if(norel)
		{
			reflector->addToContainer(vecT,t,clasName,"std::vector",appName);
			reflector->destroy(t, clasName, appName);
		}

		V_OD_erg=SQLFetch(V_OD_hstmt);
	}
	if(fldvalrel2clsmap.size()>0)
	{
		std::map<std::string, std::string>::iterator fldvalrel2clsmapit;
		for(fldvalrel2clsmapit=fldvalrel2clsmap.begin();fldvalrel2clsmapit!=fldvalrel2clsmap.end();++fldvalrel2clsmapit)
		{
			std::string flv = fldvalrel2clsmapit->first;
			void* rvect = rel2Vecmap[flv];
			if (rvect != NULL) {
				vals valus;
				args argus;
				valus.push_back(rvect);
				if(rel2reltype[flv]==2)
					argus.push_back("vector<"+fldvalrel2clsmapit->second+">");
				else
					argus.push_back(fldvalrel2clsmapit->second);
				std::string methname = "set"+StringUtil::capitalizedCopy(fldnmrel2clsmap[flv]);
				Method meth = clas.getMethod(methname,argus);
				reflector->invokeMethodGVP(rel2Insmap[flv],meth,valus);
				reflector->addToContainer(vecT,rel2Insmap[flv],clasName,"std::vector",appName);
				reflector->destroy(rel2Insmap[flv], clasName, appName);
				delete rvect;
			}
		}
	}
	refreshStmt();
	close();
	clearMaps();
	return vecT;
#endif
	return NULL;
}

void* SQLDataSourceImpl::getElements()
{
#ifdef HAVE_LIBODBC
	int V_OD_erg;// result of functions
	SQLCHAR colName[256];
	SQLSMALLINT	V_OD_colanz, colNameLen, dataType, numDecimalDigits, allowsNullValues;
	SQLULEN columnSize;

	std::vector<std::map<std::string, GenericObject> >* vecT = new std::vector<std::map<std::string, GenericObject> >;

	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;

		V_OD_erg = SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			close();
		}

		//logger << "Number of Columns " << V_OD_colanz << std::endl;

		std::map<std::string, GenericObject> colValMap;

		for(int i=1;i<=V_OD_colanz;i++)
		{
			V_OD_erg = SQLDescribeCol(V_OD_hstmt, i, colName, 255, &colNameLen, &dataType, &columnSize, &numDecimalDigits, &allowsNullValues);
			std::string columnName((char*)colName, colNameLen);

			//string thisTableName = tableName;

			/*if(columnName.find(".")!=std::string::npos)
			{
				thisTableName = columnName.substr(0, columnName.find("."));
				columnName = columnName.substr(columnName.find(".")+1);
			}*/

			StringUtil::toLower(columnName);
			var = getProperty(dataType, columnSize, colValMap, columnName, var);
		}
		V_OD_erg=SQLFetch(V_OD_hstmt);
		vecT->push_back(colValMap);
	}
	refreshStmt();
	close();
	return vecT;
#endif
	return NULL;
}


int SQLDataSourceImpl::storeProperty(ClassInfo& clas, void* t, int var, const std::string& fieldName, std::string& fldVal)
{
#ifdef HAVE_LIBODBC
	//void* col = NULL;
	SQLRETURN ret;
	SQLLEN indicator;
	Field fe = clas.getField(fieldName);
	std::string te = fe.getType();

	SQLSMALLINT    colNameLen;
	SQLSMALLINT    colDataType;
	SQLULEN        colDataSize;
	SQLSMALLINT    colDataDigits;
	SQLSMALLINT    colDataNullable;
	ret = SQLDescribeCol (V_OD_hstmt,                    // Select Statement (Prepared)
					var+1,                      // coln Number
	                NULL,            // col Name (returned)
	                0,         // size of col Name buffer
	                &colNameLen,        // Actual size of col name
	                &colDataType,       // SQL Data type of col
	                &colDataSize,       // Data size of col in table
	                &colDataDigits,     // Number of decimal digits
	                &colDataNullable);

	unsigned long long dn;
	double dd;
	float df;
	bool db;
	Date ddt;
	std::string ds;

	switch (colDataType) {
		case SQL_SMALLINT:
		{
			unsigned short us;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_USHORT, &us, sizeof(us), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(us);
			dn = us;
			break;
		}
		case SQL_INTEGER:
		{
			unsigned int us;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_ULONG, &us, sizeof(us), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(us);
			dn = us;
			break;
		}
		case SQL_TINYINT:
		{
			unsigned char us;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_UTINYINT, &us, sizeof(us), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(us);
			dn = us;
			break;
		}
		case SQL_BIGINT:
		{
			unsigned long long us;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_UBIGINT, &us, sizeof(us), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(us);
			dn = us;
			break;
		}

		case SQL_REAL:
		{
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_FLOAT, &df, sizeof(df), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(df);
			break;
		}
		case SQL_FLOAT:
		case SQL_DOUBLE:
		{
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE, &dd, sizeof(dd), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(dd);
			break;
		}
		case SQL_DECIMAL:
		case SQL_NUMERIC:

		case SQL_BIT:
		{
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT, &db, sizeof(db), &indicator);
			fldVal = CastUtil::lexical_cast<std::string>(db);
			break;
		}

#if (ODBCVER >= 0x0300)
		case SQL_TYPE_DATE:
#endif
		case SQL_DATE:
		{
			DATE_STRUCT ds;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_TYPE_DATE, &ds, sizeof(ds), &indicator);
			ddt = Date(ds.year, ds.month, ds.day);
			fldVal = ddt.toString();
			break;
		}
#if (ODBCVER >= 0x0300)
		//case SQL_TYPE_UTCTIME:
		case SQL_TYPE_TIME:
#endif
		case SQL_TIME:
		{
			TIME_STRUCT ts;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_TYPE_TIME, &ts, sizeof(ts), &indicator);
			ddt.setTime(ts.hour, ts.minute, ts.second);
			fldVal = ddt.toString();
			break;
		}
#if (ODBCVER >= 0x0300)
		case SQL_TYPE_TIMESTAMP:
		//case SQL_TYPE_UTCDATETIME:
#endif
		case SQL_TIMESTAMP:
		{
			TIMESTAMP_STRUCT ts;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_TYPE_TIMESTAMP, &ts, sizeof(ts), &indicator);
			ddt = Date(ts.year, ts.month, ts.day);
			ddt.setTime(ts.hour, ts.minute, ts.second, ts.fraction);
			fldVal = ddt.toString();
			break;
		}

		case SQL_GUID:
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
		case SQL_WCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		{
			size_t numBytes;
			unsigned char buf[1024];
			// Retrieve and display each row of data.
			while ((ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf, sizeof(buf), &indicator)) != SQL_NO_DATA) {
				numBytes = (indicator > 1024) || (indicator == SQL_NO_TOTAL) ? 1024 : indicator;
				ds.append((const char*)&buf[0], numBytes);
			}
			fldVal = CastUtil::lexical_cast<std::string>(ds);
			break;
		}
	}

	if(te=="char")
	{
		char o = (char)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="unsigned char")
	{
		unsigned char o = (unsigned char)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="short")
	{
		short o = (short)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="unsigned short")
	{
		unsigned short o = (unsigned short)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="int")
	{
		int o = (int)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="unsigned int")
	{
		unsigned int o = (unsigned int)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="long")
	{
		long o = (long)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="unsigned long")
	{
		unsigned long o = (unsigned long)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="long long")
	{
		long long o = (long long)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="unsigned long long")
	{
		unsigned long long o = (unsigned long long)dn;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="double")
	{
		double o = (double)dd;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="float")
	{
		float o = (float)dd;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="bool")
	{
		float o = (float)db;
		storePropertyInt(clas, t, &o, fe, var);
	}
	else if(te=="string" || te=="std::string")
	{
		storePropertyInt(clas, t, &ds, fe, var);
	}
	else if(te=="Date")
	{
		storePropertyInt(clas, t, &ddt, fe, var);
	}
	else if(te=="BinaryData")
	{
		BinaryData temp;
		temp.append(ds);
		storePropertyInt(clas, t, &temp, fe, var);
	}
	return var;
#endif
	return 0;
}


void SQLDataSourceImpl::storePropertyInt(const ClassInfo& clas, void* t, void* col, const Field& fe, int& var) {
	if(col!=NULL)
	{
		args argus;
		argus.push_back(fe.getType());
		vals valus;
		valus.push_back(col);
		std::string methname = "set"+StringUtil::capitalizedCopy(fe.getFieldName());
		Method meth = clas.getMethod(methname,argus);
		reflector->invokeMethodUnknownReturn(t,meth,valus);
		var++;
	}
}

int SQLDataSourceImpl::getProperty(const int& dataType, const int& columnSize, std::map<std::string, GenericObject>& colValMap, const std::string& colName, const int& var)
{
#ifdef HAVE_LIBODBC
	SQLRETURN ret;
	SQLLEN indicator;
	switch (dataType) {
		case SQL_BIT:
		{
			bool col;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT, &col, sizeof(col), &indicator);
			colValMap[colName].set(col);
		}
		break;
		case SQL_REAL:
		{
			float col;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_FLOAT, &col, sizeof(col), &indicator);
			colValMap[colName].set(col);
		}
		break;
		case SQL_DECIMAL:
		case SQL_DOUBLE:
		{
			double col;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE, &col, sizeof(col), &indicator);
			colValMap[colName].set(col);
		}
		break;
		case SQL_VARCHAR:
		case SQL_CHAR:
		default:
		{
			char buf[24];
			std::string temp;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
			temp.append(buf);
			if(indicator > (long)24)
			{
				int len = indicator-24;
				char buf1[len];
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
				temp.append(buf1);
			}
			colValMap[colName].set(temp);
		}
		break;
		case SQL_BINARY:
		{
			unsigned char buf[24];
			BinaryData temp;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf, sizeof(buf), &indicator);
			if(indicator > (long)24)
			{
				int len = indicator-24;
				unsigned char buf1[len];
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf1, sizeof(buf1), &indicator);
				temp.append(buf,24);
				temp.append(buf1,len);
			}
			else
				temp.append(buf,indicator);
			colValMap[colName].set(temp);
		}
		break;
		case SQL_INTEGER:
		{
			if(columnSize>10)
			{
				long col;
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG, &col, sizeof(col), &indicator);
				colValMap[colName].set(col);
			}
			else
			{
				int col;
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG, &col, sizeof(col), &indicator);
				colValMap[colName].set(col);
			}
		}
		break;
		case SQL_BIGINT:
		{
			char buf[24];
			std::string temp;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
			temp.append(buf);
			if(indicator > (long)24)
			{
				int len = indicator-24;
				char buf1[len];
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
				temp.append(buf1);
			}
			long long number = CastUtil::lexical_cast<long long>(temp);
			colValMap[colName].set(number);
		}
		break;
		case SQL_DATE:
		case SQL_TIME:
		case SQL_TIMESTAMP:
		{
			char buf[24];
			std::string temp;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
			temp.append(buf);
			if(indicator > (long)24)
			{
				int len = indicator-24;
				char buf1[len];
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
				temp.append(buf1);
			}
			std::string fmstr;
			if(dataType==SQL_DATE)
				fmstr = "yyyy-mm-dd";
			else if(dataType==SQL_TIME)
				fmstr = "hh:mi:ss.nnnnnn";
			else if(dataType==SQL_TIMESTAMP)
				fmstr = "yyyy-mm-dd hh:mi:ss.nnnnnn";
			DateFormat datf(fmstr);
			Date *date = datf.parse(temp);
			colValMap[colName].set(*date);
			delete date;
		}
		break;
	}
	return var+1;
#endif
	return 0;
}


void* SQLDataSourceImpl::executeQueryObject(Query& cquery)
{
	std::string clasName = cquery.getClassName();

	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(clasName);
	std::string tableName = dsemp.getTableName();
	ClassInfo clas = reflector->getClassInfo(clasName,appName);
	std::vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	std::string query = "select ";
	strMap& tabcolmap = this->mapping->getMappingForTable(tableName);
	strMap::iterator it;
	int fldsiz = (int)tabcolmap.size();
	int var = 1;
	for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
	{
		std::string columnName = it->first;
		query += (clasName + "."  + columnName);
		if(var++!=fldsiz)
			query += ",";
		cquery.aliasedColumns.push_back(clasName + "."  + columnName);
	}
	std::string reltabs = " from " + tableName + " " + clasName + " ";
	for (int var1 = 0; var1 < (int)relv.size(); var1++)
	{
		DataSourceInternalRelation relation = relv.at(var1);
		if(relation.getType()==1 || relation.getType()==2)
		{
			std::string tableName1 = this->mapping->getTableForClass(relation.getClsName());
			if(tableName1!="")
			{
				query += ",";
				DataSourceEntityMapping& rdsemp = mapping->getDataSourceEntityMapping(relation.getClsName());
				strMap tabcolmap = rdsemp.getColumnPropertyMapping();
				strMap::iterator it;
				int fldsiz = (int)tabcolmap.size();
				int var = 1;
				for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
				{
					std::string columnName = it->first;
					query += (relation.getClsName() + "."  + columnName);
					if(var++!=fldsiz)
						query += ",";
					cquery.aliasedColumns.push_back(relation.getClsName() + "."  + columnName);
				}
				reltabs += "left outer join " + tableName1 + " " + relation.getClsName() +" ";
				reltabs += "on " + clasName + "." + relation.getDmappedBy() + " = "
						+ relation.getClsName() +"." +  relation.getDfk() + " ";
			}
		}
	}
	query += (reltabs /*+ tableName + " " + clasName + "_Alias"*/);
	cquery.setQuery(query);
	return executeQueryInternal(cquery, true);
}

void SQLDataSourceImpl::bindQueryParams(Query& query)
{
#ifdef HAVE_LIBODBC
	int V_OD_erg;// result of functions
	int par = 1;
	Parameters qparams = query.getPropNameVaues();
	Parameters columnBindings = query.getColumnBindings();
	PosParameters* propPosVaues;
	Parameters::iterator ite;
	std::string queryString = query.getQuery();
	int posst = 1;
	if(qparams.size()>0)
	{
		PosParameters propPosVaues;
		std::vector<std::string> tst = RegexUtil::search(queryString, ":");
		int counter = tst.size();
		while(counter-->0 && queryString.find(":")!=std::string::npos)
		{
			for(ite=qparams.begin();ite!=qparams.end();++ite)
			{
				if(queryString.find(":")!=std::string::npos &&  queryString.find(":"+ite->first)!=queryString.find(":"))
				{
					propPosVaues[posst++] = ite->second;
					queryString = queryString.substr(0, queryString.find(":")) + "?" +
							queryString.substr(queryString.find(":"+ite->first)+ite->first.length()+1);
				}
			}
		}
		query.setQuery(queryString);
	}
	else
	{
		propPosVaues = &(query.getPropPosVaues());
	}

	//logger << query.getQuery() << std::endl;

	int totalParams = propPosVaues->size();
	while(totalParams-->0)
	{
		if(propPosVaues->find(par)==propPosVaues->end())
			throw ("No parameter value found for position " + CastUtil::lexical_cast<std::string>(par));

		GenericObject* paramValue = &(propPosVaues->find(par)->second);
		if(paramValue->getTypeName()=="short")
		{
			short* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="unsigned short")
		{
			unsigned short* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_USHORT, SQL_SMALLINT, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="int" || paramValue->getTypeName()=="long")
		{
			long* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="unsigned int" || paramValue->getTypeName()=="unsigned long")
		{
			unsigned long* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="long long" || paramValue->getTypeName()=="unsigned long long")
		{
			long long* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="double")
		{
			double* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="float")
		{
			float* sv;
			paramValue->get(sv);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, sv, 20, NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else if(paramValue->getTypeName()=="std::string" || paramValue->getTypeName()=="string")
		{
			std::string* parm;
			paramValue->get(parm);
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, 0, 0, (void*)parm->c_str(), parm->length(), NULL);
			if (!SQL_SUCCEEDED(V_OD_erg))
			{
				showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
				close();
			}
		}
		else
			throw "Cannot bind value";

		par++;
	}
#endif
}

void SQLDataSourceImpl::showError(const char *fn, const SQLHANDLE& handle, const SQLSMALLINT& type)
{
#ifdef HAVE_LIBODBC
	SQLINTEGER i = 0;
	SQLINTEGER native;
	SQLCHAR state[7];
	SQLCHAR text[256];
	SQLSMALLINT len;
	SQLRETURN ret;
	fprintf(stderr, "\n" "The driver reported the following diagnostics whilst running " "%s\n\n", fn);

	do
	{
		ret = SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text), &len );
		if (SQL_SUCCEEDED(ret))
		printf("%s:%d:%d:%s\n", state, i, native, text);
	}
	while( ret == SQL_SUCCESS );
#endif
}

bool SQLDataSourceImpl::startTransaction()
{
#ifdef HAVE_LIBODBC
	if(!DialectHelper::isTransactionSupported(dialect))return false;
	bool flagc = allocateStmt(true);
	if(!flagc)
	{
		return false;
	}

	isTransaction = true;

	flagc = false;
	//Set autocommit to false on the connection, start the transaction
	SQLRETURN sqlreturn = SQLSetConnectAttr(V_OD_hdbc,SQL_ATTR_AUTOCOMMIT,(void*)SQL_AUTOCOMMIT_OFF,0);
	if (sqlreturn == SQL_SUCCESS || sqlreturn == SQL_SUCCESS_WITH_INFO)
	{
		flagc = true;
	}
	return flagc;
#endif
	return false;
}

bool SQLDataSourceImpl::commit()
{
#ifdef HAVE_LIBODBC
	if(!DialectHelper::isTransactionSupported(dialect))return false;
	if(!isTransaction) return false;
	bool flagc = allocateStmt(true);
	if(!flagc)
	{
		return false;
	}

	isTransaction = false;

	flagc = false;

	//Commit the transaction
	SQLRETURN sqlreturn = SQLEndTran(SQL_HANDLE_DBC, V_OD_hdbc, SQL_COMMIT);
	if (sqlreturn == SQL_SUCCESS || sqlreturn == SQL_SUCCESS_WITH_INFO)
	{
		flagc = true;
	}
	//Turn autocommit to true
	SQLSetConnectAttr(V_OD_hdbc,SQL_ATTR_AUTOCOMMIT,(void*)SQL_AUTOCOMMIT_ON,0);
	close();
	return flagc;
#endif
	return false;
}

bool SQLDataSourceImpl::rollback()
{
#ifdef HAVE_LIBODBC
	if(!DialectHelper::isTransactionSupported(dialect))return false;
	bool flagc = allocateStmt(true);
	if(!flagc)
	{
		return false;
	}

	isTransaction = false;

	flagc = false;

	//Rollback the transaction
	SQLRETURN sqlreturn = SQLEndTran(SQL_HANDLE_DBC, V_OD_hdbc, SQL_ROLLBACK);
	if (sqlreturn == SQL_SUCCESS || sqlreturn == SQL_SUCCESS_WITH_INFO)
	{
		flagc = true;
	}
	//Turn autocommit to true
	SQLSetConnectAttr(V_OD_hdbc,SQL_ATTR_AUTOCOMMIT,(void*)SQL_AUTOCOMMIT_ON,0);
	close();
	return flagc;
#endif
	return false;
}

void SQLDataSourceImpl::procedureCall(const std::string& procName) {
#ifdef HAVE_LIBODBC
	bool flagc = allocateStmt(true);
	if(!flagc)throw "Error getting Database connection";
	int V_OD_erg;// result of functions
	SQLCHAR *query;
	std::string quer = "{call " + procName + "(";
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string> revType;
	bool outq = false, inoutq = false;
	std::vector<std::string> outargs, inoutargs;
	std::string outQuery = "select ", inoutQuery;
	int outC = 1;
	int size = ntmap.size();
	for (it = ntmap.begin(); it != ntmap.end(); ++it) {
		if (it->second == "IN" || it->second == "in") {
			quer += "?";
		} else if (it->second == "OUT" || it->second == "out" || it->second
				== "INOUT" || it->second == "inout") {
			quer += ("@" + it->first);
			outq = true;

			outargs.push_back("@" + it->first);
			if (it->second == "INOUT" || it->second == "inout") {
				inoutq = true;
				inoutargs.push_back("@" + it->first);
			}
		}
		if (outC++ != size) {
			quer += ",";
		}
	}
	quer += ")}";
	if (outq) {
		for (unsigned int var = 0; var < outargs.size(); ++var) {
			outQuery += (outargs.at(var));
			if (var != outargs.size() - 1)
				outQuery += ",";
		}
	}
	if (inoutq) {
		std::string temp = "select ";
		for (unsigned int var = 0; var < inoutargs.size(); ++var) {
			inoutQuery += (inoutargs.at(var));
			temp += "?";
			if (var != inoutargs.size() - 1) {
				inoutQuery += ",";
				temp += ",";
			}
		}
		inoutQuery = (temp + " into " + inoutQuery);
	}

	int par = 1;
	for (it = ntmap.begin(); it != ntmap.end(); ++it) {
		if (it->second == "INOUT" || it->second == "inout") {
			if (params[it->first].getTypeName() == "int") {
				int sv;
				params[it->first].get(sv);
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sv, 20, NULL);
				if (!SQL_SUCCEEDED(V_OD_erg))
				{
					showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
					close();
					throw "Error in call to stored procedure";
				}
			}
		}
	}
	if (inoutq)
	{
		//logger << inoutQuery << std::endl;
		V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) inoutQuery.c_str(), SQL_NTS);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLExecDirect", V_OD_hstmt, SQL_HANDLE_STMT);
			close();
			throw "Error in call to stored procedure";
		}
		refreshStmt();
	}

	//logger << quer << std::endl;
	query = (SQLCHAR*) quer.c_str();

	par = 1;
	for (it = ntmap.begin(); it != ntmap.end(); ++it) {
		SQLLEN  hotelInd;hotelInd = SQL_NTS;
		if (it->second == "OUT" || it->second == "out" || it->second == "INOUT"
				|| it->second == "inout")
		{
			revType["@" + it->first] = params[it->first].getTypeName();
		}
		if (it->second == "IN" || it->second == "in") {
			if (params[it->first].getTypeName() == "int") {
				int sv;
				params[it->first].get(sv);
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &sv, 20, NULL);
				if (!SQL_SUCCEEDED(V_OD_erg))
				{
					showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
					close();
					throw "Error Binding parameter";
				}
			}
			else if (params[it->first].getTypeName() == "short") {
				short sv;
				params[it->first].get(sv);
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &sv, 20, NULL);
				if (!SQL_SUCCEEDED(V_OD_erg))
				{
					showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
					close();
					throw "Error Binding parameter";
				}
			}
			else if (params[it->first].getTypeName() == "std::string" || params[it->first].getTypeName() == "string") {
				std::string sv;
				params[it->first].get(sv);
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par , SQL_PARAM_INPUT,
						SQL_C_CHAR,SQL_VARCHAR, 0, 0, (SQLPOINTER)sv.c_str() ,sv.length(), &hotelInd);
				if (!SQL_SUCCEEDED(V_OD_erg))
				{
					showError("SQLBindParameter", V_OD_hstmt, SQL_HANDLE_STMT);
					close();
					throw "Error Binding parameter";
				}
			}
		}
		par++;
	}
	V_OD_erg =  SQLExecDirect(V_OD_hstmt, query, SQL_NTS);
	if (!SQL_SUCCEEDED(V_OD_erg))
	{
		showError("SQLExecDirect", V_OD_hstmt, SQL_HANDLE_STMT);
		close();
		throw "Error in call to stored procedure";
	}

	//logger << outQuery << std::endl;
	SQLLEN siz;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) outQuery.c_str(), SQL_NTS);
	if (!SQL_SUCCEEDED(V_OD_erg))
	{
		showError("SQLExecDirect", V_OD_hstmt, SQL_HANDLE_STMT);
		close();
		throw "Error in call to stored procedure";
	}

	V_OD_erg = SQLFetch(V_OD_hstmt);
	while (V_OD_erg != SQL_NO_DATA) {
		for (unsigned int var = 0; var < outargs.size(); ++var) {
			if (revType[outargs.at(var)] == "int") {
				int sv;
				params[outargs.at(var)].get(sv);
				StringUtil::replaceFirst(outargs.at(var), "@", "");
				SQLGetData(V_OD_hstmt, var + 1, SQL_C_LONG, &sv, sizeof(long), &siz);
				//logger << sv << std::endl;
			}
			else if (revType[outargs.at(var)] == "short") {
				short sv;
				params[outargs.at(var)].get(sv);
				StringUtil::replaceFirst(outargs.at(var), "@", "");
				SQLGetData(V_OD_hstmt, var + 1, SQL_C_SHORT, &sv, sizeof(short), &siz);
				//logger << sv << std::endl;
			}
		}
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}
	refreshStmt();
	clearMaps();
	close();
#endif
}

void SQLDataSourceImpl::empty(const std::string& clasName) {
#ifdef HAVE_LIBODBC
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	std::string tableName = mapping->getTableForClass(clasName);
	std::string query = "truncate table "+tableName;
	//logger << query << std::endl;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if (!SQL_SUCCEEDED(V_OD_erg))
	{
		showError("SQLExecDirect", V_OD_hstmt, SQL_HANDLE_STMT);
	}
	refreshStmt();
	clearMaps();
	close();
#endif
}

long SQLDataSourceImpl::getNumRows(const std::string& clasName) {
	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(clasName);
	Query query("SELECT COUNT(*) FROM "+dsemp.getTableName(), clasName);
	std::vector<std::map<std::string, GenericObject> > tv;
	void* temp = executeQueryInternal(query, false);
	long size = -1;
	if(temp!=NULL) {
		tv = *(std::vector<std::map<std::string, GenericObject> >*) temp;
		tv.at(0).begin()->second.get(size);
		delete temp;
	}
	return size;
}

std::vector<std::map<std::string, GenericObject> > SQLDataSourceImpl::execute(Query& query) {
	std::vector<std::map<std::string, GenericObject> > tv;
	void* temp = executeQueryInternal(query, false);
	if(temp!=NULL)
	{
		if(!query.isUpdate())
		{
			tv = *(std::vector<std::map<std::string, GenericObject> >*)temp;
		}
		delete temp;
	}
	return tv;
}

bool SQLDataSourceImpl::executeUpdate(Query& query) {
	bool tv = false;
	void* temp = executeQueryInternal(query, false);
	if(temp!=NULL)
	{
		if(query.isUpdate())
		{
			tv = *(bool*)temp;
		}
		delete temp;
	}
	return tv;
}

std::vector<std::map<std::string, GenericObject> > SQLDataSourceImpl::execute(QueryBuilder& qb) {
	Query q = fromQueryBuilder(qb);
	std::vector<std::map<std::string, GenericObject> > tv;
	void* temp = executeQueryInternal(q, false);
	if(temp!=NULL)
	{
		if(!q.isUpdate())
		{
			tv = *(std::vector<std::map<std::string, GenericObject> >*)temp;
		}
		delete temp;
	}
	return tv;
}

bool SQLDataSourceImpl::executeInsert(Query& cquery, void* entity) {
	std::string clasName = cquery.getClassName();

	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(clasName);
	ClassInfo clas = reflector->getClassInfo(clasName,appName);
	fldMap fields = clas.getFields();

	std::string tableName = dsemp.getTableName();
	std::vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	std::string query = "insert into "+tableName+"(";
	unsigned var=0;

	std::string vals;

	std::vector<Field> remFields;
	std::vector<std::string> vldFields;
	fldMap::iterator it;
	int counter = 0;
	for(it=fields.begin();it!=fields.end();++it,counter++)
	{
		Field fld = it->second;
		std::string propertyName = fld.getFieldName();
		std::string columnName = propertyName;
		if(dsemp.getColumnForProperty(propertyName)!="")
			columnName = dsemp.getColumnForProperty(propertyName);
		args argus;
		std::vector<void *> valus;
		std::string methname = "get"+StringUtil::capitalizedCopy(it->first);
		Method meth = clas.getMethod(methname,argus);
		if(fld.getType()=="short" || fld.getType()=="unsigned short" || fld.getType()=="int"
				|| fld.getType()=="unsigned int" || fld.getType()=="long" || fld.getType()=="unsigned long"
				|| fld.getType()=="float" || fld.getType()=="double" || fld.getType()=="string" || fld.getType()=="std::string"
				|| fld.getType()=="bool" || fld.getType()=="long long" || fld.getType()=="unsigned long long"
				|| fld.getType()=="Date" || fld.getType()=="char" || fld.getType()=="unsigned char")
		{
			void* temp = reflector->invokeMethodGVP(entity,meth,valus);
			cquery.getPropPosVaues()[var+1].set(temp, fld.getType());
			vldFields.push_back(columnName);
			reflector->destroy(temp, fld.getType(), appName);
		}
		else
		{
			remFields.push_back(fld);
			continue;
		}

		var++;
	}

	for (int var = 0; var < (int)vldFields.size(); ++var) {
		query += (vldFields.at(var));
		vals += "?";
		if(var!=(int)vldFields.size()-1)
		{
			query += ",";
			vals += ",";
		}
	}

	query += (") values("+vals+")");
	cquery.setQuery(query);

	bool* flag = (bool*)executeQueryInternal(cquery, true);
	bool ffl = *flag;
	delete flag;

	/*if(remFields.size()>0) {
		for (int i=0;i<(int)remFields.size();i++) {
			for (int j=0;j<(int)relv.size();j++) {
				DataSourceInternalRelation relation = relv.at(j);
				if(relation.getField()==remFields.at(i).getFieldName() && (relation.getType()==1 || relation.getType()==2))
				{
					args argus;
					std::vector<void *> valus;
					std::string methname = "get"+StringUtil::capitalizedCopy(relation.getField());
					Method meth = clas.getMethod(methname,argus);
					std::string clasName = relation.getClsName();
					Query iq("", clasName);
					std::vector<std::string> icols;
					void* ival = reflector->invokeMethodGVP(entity,meth,valus,appName);
					if(relation.getType()==1)
					{
						//ffl &= executeInsert(iq, icols, ival);
					}
					else
					{

					}
				}
			}
		}
	}*/

	return ffl;
}

bool SQLDataSourceImpl::isGetDbEntityForBulkInsert() {
	return false;
}

void* SQLDataSourceImpl::getDbEntityForBulkInsert(void* entity, const std::string& clasName, std::string& error) {
	return NULL;
}

bool SQLDataSourceImpl::executeInsertBulk(Query& query, std::vector<void*> entities, std::vector<void*> dbEntities) {
	for (unsigned int k = 0; k < entities.size(); k++) {
		query.getColumnBindings().clear();
		query.getPropNameVaues().clear();
		query.getPropPosVaues().clear();
		executeInsert(query, entities.at(k));
	}
	return true;
}

bool SQLDataSourceImpl::executeUpdateBulk(Query& query, std::vector<void*> entities, std::vector<void*> dbEntities) {
	for (unsigned int k = 0; k < entities.size(); k++) {
		query.getColumnBindings().clear();
		query.getPropNameVaues().clear();
		query.getPropPosVaues().clear();
		executeUpdate(query, entities.at(k));
	}
	return true;
}

bool SQLDataSourceImpl::executeUpdate(Query& cquery, void* entity) {
	std::string clasName = cquery.getClassName();

	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(clasName);

	std::string tableName = dsemp.getTableName();
	std::vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	std::string query = "update "+tableName+" set ";

	std::string vals;
	ClassInfo clas = reflector->getClassInfo(clasName,appName);
	fldMap fields = clas.getFields();
	std::vector<Field> remFields;
	std::vector<std::string> vldFields;
	unsigned var=0;
	fldMap::iterator it;
	for(it=fields.begin();it!=fields.end();++it)
	{
		args argus;
		std::vector<void *> valus;
		Field fld = it->second;
		std::string columnName = fld.getFieldName();
		if(dsemp.getColumnForProperty(columnName)!="")
			columnName = dsemp.getColumnForProperty(columnName);
		std::string methname = "get"+StringUtil::capitalizedCopy(it->first);
		Method meth = clas.getMethod(methname,argus);
		if(fld.getType()=="short" || fld.getType()=="unsigned short" || fld.getType()=="int"
				|| fld.getType()=="unsigned int" || fld.getType()=="long" || fld.getType()=="unsigned long"
				|| fld.getType()=="float" || fld.getType()=="double" || fld.getType()=="string" || fld.getType()=="std::string"
				|| fld.getType()=="bool" || fld.getType()=="long long" || fld.getType()=="unsigned long long"
				|| fld.getType()=="Date" || fld.getType()=="char" || fld.getType()=="unsigned char")
		{
			void* temp = reflector->invokeMethodGVP(entity,meth,valus);
			cquery.getPropPosVaues()[var+1].set(temp, fld.getType());
			vldFields.push_back(columnName);
			reflector->destroy(temp, fld.getType(), appName);
		}
		else
		{
			remFields.push_back(fld);
			continue;
		}

		if(dsemp.getIdPropertyName()==fld.getFieldName()) {
			std::string idColName = dsemp.getColumnForProperty(fld.getFieldName());
			cquery.addColumnBinding(idColName, cquery.getPropPosVaues()[var+1]);
		}

		var++;
	}

	for (int var = 0; var < (int)vldFields.size(); ++var) {
		query += (vldFields.at(var));
		query += " = ?";
		if(var!=(int)vldFields.size()-1)
		{
			query += ",";
		}
	}

	cquery.setQuery(query);

	bool* flag = (bool*)executeQueryInternal(cquery, true);
	bool ffl = *flag;
	delete flag;

	/*if(remFields.size()>0) {
		for (int i=0;i<(int)remFields.size();i++) {
			for (int j=0;j<(int)relv.size();j++) {
				DataSourceInternalRelation relation = relv.at(j);
				if(relation.getField()==remFields.at(i).getFieldName() && (relation.getType()==1 || relation.getType()==2))
				{
					args argus;
					std::vector<void *> valus;
					std::string methname = "get"+StringUtil::capitalizedCopy(relation.getField());
					Method meth = clas.getMethod(methname,argus);
					std::string clasName = relation.getClsName();
					Query iq("", clasName);
					std::vector<std::string> icols;
					void* ival = reflector->invokeMethodGVP(entity,meth,valus,appName);
				}
			}
		}
	}*/

	return ffl;
}

bool SQLDataSourceImpl::remove(const std::string& clasName, GenericObject& id) {
	Query cquery("", clasName);
	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(clasName);

	std::string idcolname = dsemp.getColumnForProperty(dsemp.getIdPropertyName());
	std::string tableName = dsemp.getTableName();
	std::vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	std::string query = std::string("DELETE FROM ") + tableName + std::string(" WHERE ") + idcolname + " = ?";
	if(id.getTypeName()!="") {
		cquery.getPropPosVaues()[1] = id;
	}
	cquery.setQuery(query);

	bool* flag = (bool*)executeQueryInternal(cquery, false);
	bool ffl = *flag;
	delete flag;

	return ffl;
}

void* SQLDataSourceImpl::executeQueryInternal(Query& query, const bool& isObj) {
#ifdef HAVE_LIBODBC
	void *vecT = NULL;
	bool flagc = allocateStmt(true);
	if(!flagc)
	{
		if(query.isUpdate())
		{
			bool* flag = new bool(false);
			return flag;
		}
		else
			return vecT;
	}

	if(!query.isUpdate())
	{
		if(query.getStart()>0 && query.getCount()>0)
		{
			StringContext params;
			params["start"] = CastUtil::lexical_cast<std::string>(query.getStart());
			params["count"] = CastUtil::lexical_cast<std::string>(query.getCount());
			query.setQuery(DialectHelper::getSQLString(dialect, DialectHelper::PAGINATION_OFFSET_SQL, query.getQuery(), params));
		}
		else if(query.getCount()>0)
		{
			StringContext params;
			params["count"] = CastUtil::lexical_cast<std::string>(query.getCount());
			query.setQuery(DialectHelper::getSQLString(dialect, DialectHelper::PAGINATION_NO_OFFSET_SQL, query.getQuery(), params));
		}
	}

	SQLRETURN V_OD_erg;
	SQLLEN V_OD_rowanz;

	if(query.getPropPosVaues().size()>0 && query.getPropNameVaues().size()>0)
	{
		throw "Cannot mix positional and named parameters";
	}

	SQLFreeStmt(V_OD_hstmt, SQL_RESET_PARAMS);

	std::string queryString = query.getQuery();
	if(StringUtil::toLowerCopy(query.getQuery()).find(" where ")==std::string::npos && query.getColumnBindings().size()>0)
	{
		queryString += " where ";
		int position = query.getPropPosVaues().size();
		if(position==0) position = 1;
		int counter = 1;
		Parameters::iterator ite;
		for(ite=query.getColumnBindings().begin();ite!=query.getColumnBindings().end();++ite)
		{
			position++;
			query.getPropPosVaues()[position] = ite->second;
			queryString += (ite->first + " = ? ");
			if(counter++!=(int)query.getColumnBindings().size())
			{
				queryString += " AND ";
			}
		}
	}
	query.setQuery(queryString);

	if(StringUtil::toLowerCopy(pool->getProperties().getProperty("logsql"))=="true")
	{
		std::string out = queryString;
		int par = 1;
		int totalParams = query.getPropPosVaues().size();
		while(totalParams-->0)
		{
				if(query.getPropPosVaues().find(par)!=query.getPropPosVaues().end())
				{
						out += (" ("+query.getPropPosVaues().find(par)->second.getSerilaizedState()+")" );
				}
				par++;
		}
		//std::cout << out << std::endl;
	}

	if(query.isUpdate())
	{
		bool* flag = new bool(true);
		V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.getQuery().c_str(),SQL_NTS);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLPrepare", V_OD_hstmt, SQL_HANDLE_STMT);
			*flag = false;
			close();
			return flag;
		}
		bindQueryParams(query);
		V_OD_erg=SQLExecute(V_OD_hstmt);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLExecute", V_OD_hstmt, SQL_HANDLE_STMT);
			*flag = false;
		}
		refreshStmt();
		close();
		return flag;
	}
	else
	{
		V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.getQuery().c_str(),SQL_NTS);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLPrepare", V_OD_hstmt, SQL_HANDLE_STMT);
			close();
			return vecT;
		}
		bindQueryParams(query);
		V_OD_erg=SQLExecute(V_OD_hstmt);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLExecute", V_OD_hstmt, SQL_HANDLE_STMT);
			close();
			return vecT;
		}
		V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLRowCount", V_OD_hstmt, SQL_HANDLE_STMT);
			close();
			return vecT;
		}
		//logger << "Number of Rows " << (int)V_OD_rowanz << std::endl;
		if(query.getClassName()!="" && isObj)
			return getElements(query);
		else
			return getElements();
	}
#endif
	return NULL;
}

void* SQLDataSourceImpl::executeQuery(Query& query, const bool& isObj) {
	if(!isObj) {
		return executeQueryInternal(query, isObj);
	} else {
		return executeQueryObject(query);
	}
}

void* SQLDataSourceImpl::executeQuery(QueryBuilder& qb, const bool& isObj) {
	Query q = fromQueryBuilder(qb);
	return executeQueryInternal(q, isObj);
}

void* SQLDataSourceImpl::getContext(void* details) {
	return NULL;
}

void SQLDataSourceImpl::destroyContext(void* cntxt) {
}

SQLContext::SQLContext() {
	conn = NULL;
}

SQLContext::~SQLContext() {
}
