/*
 * SQLDataSourceImpl.cpp
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */

#include "SQLDataSourceImpl.h"

Query SQLDataSourceImpl::fromQueryBuilder(QueryBuilder& qb) {
	Query q;
	string qs;
	qs.append("SELECT ");
	if(qb.isUnique())
		qs.append("DISTINCT ");

	string tableNm = qb.getTableName();
	string alias = qb.getAlias();
	bool isClassProps = false;

	DataSourceEntityMapping dsemp;

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
		strMap tabcolmap = this->mapping->getMappingForTable(tableNm);
		int fldsiz = (int)tabcolmap.size();
		strMap::iterator it;
		for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
		{
			string columnName = it->first;
			qs.append(qb.getClassName() + "."  + columnName);
			if(var++!=fldsiz)
				qs.append(",");
			q.aliasedColumns.push_back(qb.getClassName() + "."  + columnName);
		}
	}
	else
	{
		map<string, string> cols = qb.getColumns();
		map<string, string>::iterator it;
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
		string query;
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
		vector<map<string, GenericObject> > vecmp = execute(q);
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
		string query;
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
		string query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_SEQUENCE_QUERY, params);

		Query q(query);
		vector<map<string, GenericObject> > vecmp = execute(q);
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
		string query = DialectHelper::getIdGenerateQueryPost(dialect, dsemp);
		Query q(query);
		vector<map<string, GenericObject> > vecmp = execute(q);
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
	map<string, string> props = pool->getProperties().getProperties();
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

void SQLDataSourceImpl::executeCustom(DataSourceEntityMapping& dsemp, const string& customMethod, GenericObject& idv) {
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
	if (this->pool != NULL && this->conn != NULL && this->conn->isType() == read
			&& V_OD_hstmt != NULL) {
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
	SQLCloseCursor(V_OD_hstmt);
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
	vector<string> cols;
	return getElements(cols,q);
}

void* SQLDataSourceImpl::getElements(const vector<string>& cols, Query& q)
{
#ifdef HAVE_LIBODBC
	string clasName = q.getClassName();
	int V_OD_erg;// result of functions

	DataSourceEntityMapping dsemp = mapping->getDataSourceEntityMapping(clasName);

	ClassInfo clas = reflector->getClassInfo(clasName, appName);
	string tableName = dsemp.getTableName();
	vector<DataSourceInternalRelation> relv = dsemp.getRelations();

	fldMap fields = clas.getFields();
	fldMap::iterator it;
	void *vecT = reflector->getNewContainer(clasName,"std::vector",appName);
	SQLSMALLINT	V_OD_colanz;

	map<string, void*> rel2Insmap;
	map<string, void*> rel2Vecmap;
	map<string, string> fldvalrel2clsmap;
	map<string, string> fldnmrel2clsmap;
	map<string, int> rel2reltype;

	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;
		args argus1;
		map<string, void*> instances;
		void* relVecEle = NULL;
		string fldVal;
		const Constructor& ctor = clas.getConstructor(argus1);
		void *t = reflector->newInstanceGVP(ctor);

		V_OD_erg = SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			close();
		}
		bool norel = true;
		//logger << "Number of Columns " << V_OD_colanz << endl;
		for(int i=1;i<=V_OD_colanz;i++)
		{
			string columnName = q.aliasedColumns.at(i-1);

			string thisTableName = tableName;

			if(columnName.find(".")!=string::npos)
			{
				string tclas = columnName.substr(0, columnName.find("."));
				thisTableName = mapping->getTableForClass(tclas);
				columnName = columnName.substr(columnName.find(".")+1);
			}

			StringUtil::toLower(columnName);

			void* instance = NULL;
			ClassInfo instanceClas;
			DataSourceInternalRelation instanceRelation;

			string fieldName = this->mapping->getPropertyForColumn(thisTableName, columnName);

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
						string tableName1 = this->mapping->getTableForClass(relation.getClsName());
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

			string fldvalt;
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
			delete t;
		}

		V_OD_erg=SQLFetch(V_OD_hstmt);
	}
	if(fldvalrel2clsmap.size()>0)
	{
		map<string, string>::iterator fldvalrel2clsmapit;
		for(fldvalrel2clsmapit=fldvalrel2clsmap.begin();fldvalrel2clsmapit!=fldvalrel2clsmap.end();++fldvalrel2clsmapit)
		{
			string flv = fldvalrel2clsmapit->first;
			void* rvect = rel2Vecmap[flv];
			if (rvect != NULL) {
				vals valus;
				args argus;
				valus.push_back(rvect);
				if(rel2reltype[flv]==2)
					argus.push_back("vector<"+fldvalrel2clsmapit->second+">");
				else
					argus.push_back(fldvalrel2clsmapit->second);
				string methname = "set"+StringUtil::capitalizedCopy(fldnmrel2clsmap[flv]);
				Method meth = clas.getMethod(methname,argus);
				reflector->invokeMethodGVP(rel2Insmap[flv],meth,valus);
				reflector->addToContainer(vecT,rel2Insmap[flv],clasName,"std::vector",appName);
				delete rel2Insmap[flv];
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

	vector<map<string, GenericObject> >* vecT = new vector<map<string, GenericObject> >;

	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;

		V_OD_erg = SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			close();
		}

		//logger << "Number of Columns " << V_OD_colanz << endl;

		map<string, GenericObject> colValMap;

		for(int i=1;i<=V_OD_colanz;i++)
		{
			V_OD_erg = SQLDescribeCol(V_OD_hstmt, i, colName, 255, &colNameLen, &dataType, &columnSize, &numDecimalDigits, &allowsNullValues);
			string columnName((char*)colName, colNameLen);

			//string thisTableName = tableName;

			/*if(columnName.find(".")!=string::npos)
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


int SQLDataSourceImpl::storeProperty(ClassInfo& clas, void* t, int var, const string& fieldName, string& fldVal)
{
#ifdef HAVE_LIBODBC
	void* col = NULL;
	SQLRETURN ret;
	SQLLEN indicator;
	Field fe = clas.getField(fieldName);
	string te = fe.getType();
	if(te=="int")
	{
		col = new int;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG, col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(int*)col);
	}
	else if(te=="unsigned int")
	{
		col = new unsigned int;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_ULONG,col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(unsigned int*)col);
	}
	else if(te=="long")
	{
		col = new long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(long*)col);
	}
	else if(te=="unsigned long")
	{
		col = new unsigned long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_ULONG,col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(unsigned long*)col);
	}
	else if(te=="long long")
	{
		col = new long long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_SBIGINT, col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(long long*)col);
	}
	else if(te=="unsigned long long")
	{
		col = new unsigned long long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_UBIGINT, col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(unsigned long long*)col);
	}
	else if(te=="short")
	{
		col = new short;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_SHORT,col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(short*)col);
	}
	else if(te=="unsigned short")
	{
		col = new unsigned short;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_USHORT,col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(unsigned short*)col);
	}
	else if(te=="double")
	{
		col = new double;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE, col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(double*)col);
	}
	else if(te=="float")
	{
		col = new float;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_FLOAT, col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(float*)col);
	}
	else if(te=="bool")
	{
		col = new bool;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT, col, sizeof(col), &indicator);
		fldVal = CastUtil::lexical_cast<string>(*(bool*)col);
	}
	else if(te=="string")
	{
		/*col = new string;
		SQLBindCol(V_OD_hstmt,var,SQL_C_CHAR, col,10,sizes);*/
		char buf[24];
		string *temp = new string;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
		temp->append(buf);
		//logger << indicator << endl;
		if(indicator > (long)24)
		{
			int len = indicator-24;
			char buf1[len];
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
			temp->append(buf1);
			//logger << buf1 << endl;
		}
		col = temp;
		fldVal = CastUtil::lexical_cast<string>(*temp);
		//logger << *temp << "\n" << endl;
	}
	else if(te=="Date")
	{
		//col = new Date;
		//ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DATE,col, sizeof(col), &indicator);
		/*col = new string;
		SQLBindCol(V_OD_hstmt,var,SQL_C_CHAR, col,10,sizes);*/
		char buf[24];

		string temp;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
		temp.append(buf);
		//logger << indicator << endl;
		if(indicator > (long)24)
		{
			int len = indicator-24;
			char buf1[len];
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
			temp.append(buf1);
			//logger << buf1 << endl;
		}
		DateFormat datf("yyyy-mm-dd");
		if(temp.length()>10)
			datf.setFormatspec("yyyy-mm-dd hh:mi:ss");
		else
			datf.setFormatspec("yyyy-mm-dd hh:mi:ss.nnnnnn");
		Date *date = datf.parse(temp);
		col = date;
		fldVal = CastUtil::lexical_cast<string>(temp);
		//logger << temp << "\n" << endl;
	}
	else if(te=="BinaryData")
	{
		/*col = new string;
		SQLBindCol(V_OD_hstmt,var,SQL_C_CHAR, col,10,sizes);*/
		unsigned char buf[24];
		BinaryData *temp = new BinaryData;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf, sizeof(buf), &indicator);
		//temp->append(buf,indicator);
		//logger << indicator << endl;

		if(indicator > (long)24)
		{
			int len = indicator-24;
			unsigned char buf1[len];
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf1, sizeof(buf1), &indicator);
			temp->append(buf,24);
			temp->append(buf1,len);
		}
		else
			temp->append(buf,indicator);
		col = temp;
		fldVal = CastUtil::lexical_cast<string>(temp);
		//logger << buf << "\n" << endl;
	}
	else//if its not a vector means its a one-to-one relationship
	{

	}
	if(col!=NULL)
	{
		args argus;
		argus.push_back(te);
		vals valus;
		//valus.push_back(columns.at(var));
		valus.push_back(col);
		string methname = "set"+StringUtil::capitalizedCopy(fe.getFieldName());
		Method meth = clas.getMethod(methname,argus);
		reflector->invokeMethodUnknownReturn(t,meth,valus);
		var++;
		delete col;
	}
	return var;
#endif
	return 0;
}


int SQLDataSourceImpl::getProperty(const int& dataType, const int& columnSize, map<string, GenericObject>& colValMap, const string& colName, const int& var)
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
			string temp;
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
			string temp;
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
			string temp;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
			temp.append(buf);
			if(indicator > (long)24)
			{
				int len = indicator-24;
				char buf1[len];
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
				temp.append(buf1);
			}
			string fmstr;
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
	string clasName = cquery.getClassName();

	DataSourceEntityMapping dsemp = mapping->getDataSourceEntityMapping(clasName);
	string tableName = dsemp.getTableName();
	ClassInfo clas = reflector->getClassInfo(clasName,appName);
	vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	string query = "select ";
	strMap tabcolmap = this->mapping->getMappingForTable(tableName);
	strMap::iterator it;
	int fldsiz = (int)tabcolmap.size();
	int var = 1;
	for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
	{
		string columnName = it->first;
		query += (clasName + "."  + columnName);
		if(var++!=fldsiz)
			query += ",";
		cquery.aliasedColumns.push_back(clasName + "."  + columnName);
	}
	string reltabs = " from " + tableName + " " + clasName + " ";
	for (int var1 = 0; var1 < (int)relv.size(); var1++)
	{
		DataSourceInternalRelation relation = relv.at(var1);
		if(relation.getType()==1 || relation.getType()==2)
		{
			string tableName1 = this->mapping->getTableForClass(relation.getClsName());
			if(tableName1!="")
			{
				query += ",";
				DataSourceEntityMapping rdsemp = mapping->getDataSourceEntityMapping(relation.getClsName());
				strMap tabcolmap = rdsemp.getColumnPropertyMapping();
				strMap::iterator it;
				int fldsiz = (int)tabcolmap.size();
				int var = 1;
				for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
				{
					string columnName = it->first;
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
	string queryString = query.getQuery();
	int posst = 1;
	if(qparams.size()>0)
	{
		PosParameters propPosVaues;
		vector<string> tst = RegexUtil::search(queryString, ":");
		int counter = tst.size();
		while(counter-->0 && queryString.find(":")!=string::npos)
		{
			for(ite=qparams.begin();ite!=qparams.end();++ite)
			{
				if(queryString.find(":")!=string::npos &&  queryString.find(":"+ite->first)!=queryString.find(":"))
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

	logger << query.getQuery() << endl;

	int totalParams = propPosVaues->size();
	while(totalParams-->0)
	{
		if(propPosVaues->find(par)==propPosVaues->end())
			throw ("No parameter value found for position " + CastUtil::lexical_cast<string>(par));

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
		else if(paramValue->getTypeName()=="long long")
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
			string* parm;
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
		printf("%s:%ld:%ld:%s\n", state, i, native, text);
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

void SQLDataSourceImpl::procedureCall(const string& procName) {
#ifdef HAVE_LIBODBC
	bool flagc = allocateStmt(true);
	if(!flagc)throw "Error getting Database connection";
	int V_OD_erg;// result of functions
	SQLCHAR *query;
	string quer = "{call " + procName + "(";
	map<string, string>::iterator it;
	map<string, string> revType;
	bool outq = false, inoutq = false;
	vector<string> outargs, inoutargs;
	string outQuery = "select ", inoutQuery;
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
		string temp = "select ";
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
					showError("SQLExecDirect", V_OD_hstmt, SQL_HANDLE_STMT);
					close();
					throw "Error in call to stored procedure";
				}
			}
		}
	}
	if (inoutq)
	{
		logger << inoutQuery << endl;
		V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) inoutQuery.c_str(), SQL_NTS);
		if (!SQL_SUCCEEDED(V_OD_erg))
		{
			showError("SQLExecDirect", V_OD_hstmt, SQL_HANDLE_STMT);
			close();
			throw "Error in call to stored procedure";
		}
		refreshStmt();
	}

	logger << quer << endl;
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
			else if (params[it->first].getTypeName() == "std::string") {
				string sv;
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

	logger << outQuery << endl;
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
				logger << sv << endl;
			}
			else if (revType[outargs.at(var)] == "short") {
				short sv;
				params[outargs.at(var)].get(sv);
				StringUtil::replaceFirst(outargs.at(var), "@", "");
				SQLGetData(V_OD_hstmt, var + 1, SQL_C_SHORT, &sv, sizeof(short), &siz);
				logger << sv << endl;
			}
		}
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}
	refreshStmt();
	clearMaps();
	close();
#endif
}

void SQLDataSourceImpl::empty(const string& clasName) {
#ifdef HAVE_LIBODBC
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	string tableName = mapping->getTableForClass(clasName);
	string query = "truncate table "+tableName;
	logger << query << endl;
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

long SQLDataSourceImpl::getNumRows(const string& clasName) {
	DataSourceEntityMapping dsemp = mapping->getDataSourceEntityMapping(clasName);
	Query query("SELECT COUNT(*) FROM "+dsemp.getTableName(), clasName);
	vector<map<string, GenericObject> > tv;
	void* temp = executeQueryInternal(query, false);
	long size = -1;
	if(temp!=NULL) {
		tv = *(vector<map<string, GenericObject> >*) temp;
		tv.at(0).begin()->second.get(size);
		delete temp;
	}
	return size;
}

vector<map<string, GenericObject> > SQLDataSourceImpl::execute(Query& query) {
	vector<map<string, GenericObject> > tv;
	void* temp = executeQueryInternal(query, false);
	if(temp!=NULL)
	{
		if(!query.isUpdate())
		{
			tv = *(vector<map<string, GenericObject> >*)temp;
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

vector<map<string, GenericObject> > SQLDataSourceImpl::execute(QueryBuilder& qb) {
	Query q = fromQueryBuilder(qb);
	vector<map<string, GenericObject> > tv;
	void* temp = executeQueryInternal(q, false);
	if(temp!=NULL)
	{
		if(!q.isUpdate())
		{
			tv = *(vector<map<string, GenericObject> >*)temp;
		}
		delete temp;
	}
	return tv;
}

bool SQLDataSourceImpl::executeInsert(Query& cquery, void* entity) {
	string clasName = cquery.getClassName();

	DataSourceEntityMapping dsemp = mapping->getDataSourceEntityMapping(clasName);
	ClassInfo clas = reflector->getClassInfo(clasName,appName);
	fldMap fields = clas.getFields();

	string tableName = dsemp.getTableName();
	vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	string query = "insert into "+tableName+"(";
	unsigned var=0;

	string vals;

	vector<Field> remFields;
	vector<string> vldFields;
	fldMap::iterator it;
	int counter = 0;
	for(it=fields.begin();it!=fields.end();++it,counter++)
	{
		Field fld = it->second;
		string propertyName = fld.getFieldName();
		string columnName = propertyName;
		if(dsemp.getColumnForProperty(propertyName)!="")
			columnName = dsemp.getColumnForProperty(propertyName);
		args argus;
		vector<void *> valus;
		string methname = "get"+StringUtil::capitalizedCopy(it->first);
		Method meth = clas.getMethod(methname,argus);
		if(fld.getType()=="short" || fld.getType()=="unsigned short" || fld.getType()=="int"
				|| fld.getType()=="unsigned int" || fld.getType()=="long" || fld.getType()=="unsigned long"
				|| fld.getType()=="float" || fld.getType()=="double" || fld.getType()=="string")
		{
			void* temp = reflector->invokeMethodGVP(entity,meth,valus);
			cquery.getPropPosVaues()[var+1].set(temp, fld.getType());
			vldFields.push_back(columnName);
			delete temp;
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
					vector<void *> valus;
					string methname = "get"+StringUtil::capitalizedCopy(relation.getField());
					Method meth = clas.getMethod(methname,argus);
					string clasName = relation.getClsName();
					Query iq("", clasName);
					vector<string> icols;
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

void* SQLDataSourceImpl::getDbEntityForBulkInsert(void* entity, const string& clasName, string& error) {
	return NULL;
}

bool SQLDataSourceImpl::executeInsertBulk(Query& query, vector<void*> entities, vector<void*> dbEntities) {
	for (unsigned int k = 0; k < entities.size(); k++) {
		executeInsert(query, entities.at(k));
	}
	return true;
}

bool SQLDataSourceImpl::executeUpdateBulk(Query& query, vector<void*> entities, vector<void*> dbEntities) {
	for (unsigned int k = 0; k < entities.size(); k++) {
		executeUpdate(query, entities.at(k));
	}
	return true;
}

bool SQLDataSourceImpl::executeUpdate(Query& cquery, void* entity) {
	string clasName = cquery.getClassName();

	DataSourceEntityMapping dsemp = mapping->getDataSourceEntityMapping(clasName);

	string tableName = dsemp.getTableName();
	vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	string query = "update "+tableName+" set ";

	string vals;
	ClassInfo clas = reflector->getClassInfo(clasName,appName);
	fldMap fields = clas.getFields();
	vector<Field> remFields;
	vector<string> vldFields;
	unsigned var=0;
	fldMap::iterator it;
	for(it=fields.begin();it!=fields.end();++it)
	{
		args argus;
		vector<void *> valus;
		Field fld = it->second;
		string columnName = fld.getFieldName();
		if(dsemp.getColumnForProperty(columnName)!="")
			columnName = dsemp.getColumnForProperty(columnName);
		string methname = "get"+StringUtil::capitalizedCopy(it->first);
		Method meth = clas.getMethod(methname,argus);
		if(fld.getType()=="short" || fld.getType()=="unsigned short" || fld.getType()=="int"
				|| fld.getType()=="unsigned int" || fld.getType()=="long" || fld.getType()=="unsigned long"
				|| fld.getType()=="float" || fld.getType()=="double" || fld.getType()=="string")
		{
			void* temp = reflector->invokeMethodGVP(entity,meth,valus);
			cquery.getPropPosVaues()[var+1].set(temp, fld.getType());
			vldFields.push_back(columnName);
			delete temp;
		}
		else
		{
			remFields.push_back(fld);
			continue;
		}

		if(dsemp.getIdPropertyName()==fld.getFieldName()) {
			string idColName = dsemp.getColumnForProperty(fld.getFieldName());
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
					vector<void *> valus;
					string methname = "get"+StringUtil::capitalizedCopy(relation.getField());
					Method meth = clas.getMethod(methname,argus);
					string clasName = relation.getClsName();
					Query iq("", clasName);
					vector<string> icols;
					void* ival = reflector->invokeMethodGVP(entity,meth,valus,appName);
				}
			}
		}
	}*/

	return ffl;
}

bool SQLDataSourceImpl::remove(const string& clasName, GenericObject& id) {
	Query cquery("", clasName);
	DataSourceEntityMapping dsemp = mapping->getDataSourceEntityMapping(clasName);

	string idcolname = dsemp.getColumnForProperty(dsemp.getIdPropertyName());
	string tableName = dsemp.getTableName();
	vector<DataSourceInternalRelation> relv = dsemp.getRelations();
	string query = string("DELETE FROM ") + tableName + string(" WHERE ") + idcolname + " = ?";
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
			params["start"] = CastUtil::lexical_cast<string>(query.getStart());
			params["count"] = CastUtil::lexical_cast<string>(query.getCount());
			query.setQuery(DialectHelper::getSQLString(dialect, DialectHelper::PAGINATION_OFFSET_SQL, query.getQuery(), params));
		}
		else if(query.getCount()>0)
		{
			StringContext params;
			params["count"] = CastUtil::lexical_cast<string>(query.getCount());
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

	string queryString = query.getQuery();
	if(StringUtil::toLowerCopy(query.getQuery()).find(" where ")==string::npos && query.getColumnBindings().size()>0)
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
		logger << "Number of Rows " << (int)V_OD_rowanz << endl;
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
