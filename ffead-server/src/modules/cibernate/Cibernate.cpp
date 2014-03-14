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
 * Cibernate.cpp
 *
 *  Created on: Jan 5, 2010
 *      Author: sumeet
 */

#include "Cibernate.h"

Cibernate::~Cibernate() {
	close(true);
	logger << "\nDestructed Cibernate" << flush;
}
Cibernate::Cibernate(string appName) {
	logger = LoggerFactory::getLogger("Cibernate");
	this->init = false;
	this->appName = appName;
	if (this->appName != "" && CibernateConnPools::isInitialized()
			&& CibernateConnPools::getPool(this->appName) != NULL) {
		this->init = true;
		this->conn = NULL;
		this->pool = CibernateConnPools::getPool(this->appName);
		this->mapping = CibernateConnPools::getMapping(this->appName);
		if(this->mapping==NULL)
			throw ("No mapping found for appname " + appName);
		if(this->pool!=NULL && this->mapping!=NULL)
		{
			this->dialect = this->pool->getDialect();
			logger << ("Got pool for application " + appName) << endl;
		}
	}
	else
		throw "Error connecting to Database server";
}

Cibernate::Cibernate() {
	this->init = false;
	this->appName = "default";
	if (this->appName != "" && CibernateConnPools::isInitialized()
			&& CibernateConnPools::getPool(this->appName) != NULL) {
		this->init = true;
		this->conn = NULL;
		this->pool = CibernateConnPools::getPool(this->appName);
		this->mapping = CibernateConnPools::getMapping(this->appName);
		if(this->mapping==NULL)
			throw ("No mapping found for appname " + appName);
		if(this->pool!=NULL && this->mapping!=NULL)
		{
			this->dialect = this->pool->getDialect();
			logger << ("Got pool for application " + appName) << endl;
		}
	}
	else
		throw "Error connecting to Database server";
}

string Cibernate::demangle(const char *mangled)
{
	int status;	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	string s(demangled);
	free(demangled);
	return s;
}

Cibernate::Cibernate(string dbName, string uname, string pass, int psize, string dialect) {
	CibernateConnPools::addPool(psize,uname,pass,dbName,"default",dialect);
	if(!CibernateConnPools::isInitialized())
		throw "Error connecting to Database server";
	this->pool = CibernateConnPools::getPool("default");
	this->mapping = CibernateConnPools::getMapping("default");
	if(this->mapping==NULL)
		throw ("No mapping found for appname " + appName);
	this->init = true;
	this->conn = NULL;
	if(this->pool!=NULL && this->mapping!=NULL)
	{
		logger << ("Got pool for application default") << endl;
	}
}


bool Cibernate::allocateStmt(bool read) {
	if(!this->init)
	{
		return false;
	}
	if (this->pool != NULL && this->conn != NULL && this->conn->type == read
			&& V_OD_hstmt != NULL) {
		refreshStmt();
		//return false;
	}
	int V_OD_erg;// result of functions
	if (read) {
		if (this->pool != NULL)
			conn = this->pool->getReadConnection();
		V_OD_hdbc = conn->conn;
	} else {
		if (this->pool != NULL)
			conn = this->pool->getWriteConnection();
		V_OD_hdbc = conn->conn;
	}
	V_OD_erg = SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		return false;
	}
	return true;
}

void Cibernate::refreshStmt() {
	SQLCloseCursor(V_OD_hstmt);
}


void Cibernate::procedureCall(string procName) {
	bool flagc = allocateStmt(true);
	if(!flagc)throw "Error getting Database connection";
	int V_OD_erg;// result of functions
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200], *query;
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
			if (params[it->first]->getTypeName() == "int") {
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par++,
						SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
						params[it->first]->getVoidPointer(), 20, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close(true);
					throw "Error in call to stored procedure";
				}
			}
		}
	}
	if (inoutq)
	{
		logger << inoutQuery << flush;
		V_OD_erg
				= SQLExecDirect(V_OD_hstmt, (SQLCHAR*) inoutQuery.c_str(), SQL_NTS);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
			logger << "Error in call to stored procedure " << V_OD_erg << endl;
			SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
					&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
			logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
			close(true);
			throw "Error in call to stored procedure";
		}
		refreshStmt();
	}

	logger << quer << flush;
	query = (SQLCHAR*) quer.c_str();
	/*V_OD_erg = SQLPrepare(V_OD_hstmt, query, SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		printf("Error in prepare statement stored procedure %d\n", V_OD_erg);
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
				&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close(true);
	}*/
	par = 1;
	for (it = ntmap.begin(); it != ntmap.end(); ++it) {
		SQLLEN  hotelInd;hotelInd = SQL_NTS;
		if (it->second == "OUT" || it->second == "out" || it->second == "INOUT"
				|| it->second == "inout")
		{
			revType["@" + it->first] = params[it->first]->getTypeName();
		}
		if (it->second == "IN" || it->second == "in") {
			logger << "binding in parameter " << params[it->first]->getVoidPointer() << endl;
			if (params[it->first]->getTypeName() == "int") {
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT,
						SQL_C_LONG, SQL_INTEGER, 0, 0,
						params[it->first]->getVoidPointer(), 20, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close(true);
					throw "Error Binding parameter";
				}
			}
			else if (params[it->first]->getTypeName() == "short") {
				short* parmv =  (short*)params[it->first]->getVoidPointer();
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT,
						SQL_C_SHORT, SQL_SMALLINT, 0, 0,
						parmv, 20, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close(true);
					throw "Error Binding parameter";
				}
			}
			else if (params[it->first]->getTypeName() == "std::string") {
				string *parm = (string*)params[it->first]->getVoidPointer();
				//char *parmv = (char*)parm->c_str();
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par , SQL_PARAM_INPUT,
						SQL_C_CHAR,SQL_VARCHAR, 0, 0, (SQLPOINTER)parm->c_str() ,parm->length(), &hotelInd);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close(true);
					throw "Error Binding parameter";
				}
			}
		}
		par++;
	}
	V_OD_erg =  SQLExecDirect(V_OD_hstmt, query, SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Error in call to stored procedure " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_STMT, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
				&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close(true);
		throw "Error in call to stored procedure";
	}

	/*V_OD_erg = SQLNumResultCols(V_OD_hstmt, &V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		close(true);
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg = SQLRowCount(V_OD_hstmt, &V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Number of RowCount " << V_OD_erg << endl;
		close(true);
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;*/
	//refreshStmt();

	logger << outQuery << flush;
	SQLLEN siz;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) outQuery.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Error in Select " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
				&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close(true);
		throw "Error in call to stored procedure";
	}
	/*V_OD_erg = SQLNumResultCols(V_OD_hstmt, &V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		close(true);
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg = SQLRowCount(V_OD_hstmt, &V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Number of RowCount " << V_OD_erg << endl;
		close(true);
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;*/
	V_OD_erg = SQLFetch(V_OD_hstmt);
	while (V_OD_erg != SQL_NO_DATA) {
		for (unsigned int var = 0; var < outargs.size(); ++var) {
			if (revType[outargs.at(var)] == "int") {

				StringUtil::replaceFirst(outargs.at(var), "@", "");
				SQLGetData(V_OD_hstmt, var + 1, SQL_C_LONG, params[outargs.at(
						var)]->getVoidPointer(),
						sizeof(params[outargs.at(var)]->getVoidPointer()), &siz);
				logger << *(int*) params[outargs.at(var)]->getVoidPointer() << endl;
			}
			else if (revType[outargs.at(var)] == "short") {
				StringUtil::replaceFirst(outargs.at(var), "@", "");
				SQLGetData(V_OD_hstmt, var + 1, SQL_C_SHORT, params[outargs.at(
						var)]->getVoidPointer(),
						sizeof(params[outargs.at(var)]->getVoidPointer()), &siz);
				logger << *(short*) params[outargs.at(var)]->getVoidPointer() << endl;
			}
		}
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}
	refreshStmt();
	clearMaps();
	close(!isTransaction);
}


void Cibernate::close(bool clos) {
	if(clos)
	{
		this->pool->closeConnection(conn);
		this->pool = NULL;
		this->mapping = NULL;
		V_OD_hdbc = NULL;
		SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
		V_OD_hstmt = NULL;
	}
}


void Cibernate::empty(string clasName) {
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string query = "tuncate table "+tableName;
	logger << query << flush;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in Truncate " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close(true);
	}
	refreshStmt();
	clearMaps();
	close(!isTransaction);
	return;
}


void* Cibernate::getElements(string clasName)
{
	vector<string> cols;
	return getElements(cols,clasName);
}

void* Cibernate::getElements(vector<string> cols,string clasName)
{
	int V_OD_erg;// result of functions
	Reflector reflector;
	ClassInfo clas = reflector.getClassInfo(clasName, appName);
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	fldMap fields = clas.getFields();
	fldMap::iterator it;
	void *vecT = reflector.getNewVector(clasName,appName);
	SQLCHAR colName[256];
	SQLSMALLINT	V_OD_colanz, colNameLen, dataType, numDecimalDigits, allowsNullValues;
	SQLULEN columnSize;

	void *col = NULL;
	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;
		args argus1;
		map<string, void*> instances;
		Constructor ctor = clas.getConstructor(argus1);
		void *t = reflector.newInstanceGVP(ctor,appName);
		//if(cols.size()<=0)
		//{
		V_OD_erg = SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			close(true);
		}
		//logger << "Number of Columns " << V_OD_colanz << endl;
		for(int i=1;i<=V_OD_colanz;i++)
		{
			V_OD_erg = SQLDescribeCol(V_OD_hstmt, i, colName, 255, &colNameLen, &dataType, &columnSize, &numDecimalDigits, &allowsNullValues);
			string columnName((char*)colName);

			string thisTableName = tableName;

			if(columnName.find(".")!=string::npos)
			{
				thisTableName = columnName.substr(0, columnName.find("."));
				columnName = columnName.substr(columnName.find(".")+1);
			}

			StringUtil::toLower(columnName);

			void* instance = NULL;
			ClassInfo instanceClas;
			DBRel instanceRelation;

			string fieldName = this->mapping->getTableAppColMapping(thisTableName, columnName);

			if(thisTableName==tableName && fieldName!="")
			{
				instanceClas = clas;
				instance = t;
			}
			else
			{
				for (int var1 = 0; var1 < (int)relv.size(); ++var1)
				{
					DBRel relation = relv.at(var1);
					if(relation.type==1)
					{
						string tableName1 = this->mapping->getAppTableClassMapping(relation.clsName);
						fieldName = this->mapping->getTableAppColMapping(tableName1, columnName);
						if(fieldName!="" || tableName1==thisTableName)
						{
							thisTableName = tableName1;
							args argus1;
							ClassInfo clas1 = reflector.getClassInfo(relation.clsName,appName);
							if(instances.find(thisTableName)==instances.end())
							{
								Constructor ctor = clas1.getConstructor(argus1);
								void *tt = reflector.newInstanceGVP(ctor,appName);
								instances[tableName1] = tt;
							}
							instanceClas = clas1;
							instance = instances[thisTableName];
							instanceRelation = relation;
							break;
						}
					}
				}
			}

			var = storeProperty(instanceClas, instance, var, fieldName);
			if(thisTableName!=tableName)
			{
				args argus;
				vals valus;
				valus.push_back(instance);
				argus.push_back(instanceRelation.clsName);
				string methname = "set"+AfcUtil::camelCased(instanceRelation.field);
				Method meth = clas.getMethod(methname,argus);
				reflector.invokeMethodGVP(t,meth,valus,appName);
			}
		}
		reflector.vectorPushBack(vecT,t,clasName,appName);
		delete t;
		V_OD_erg=SQLFetch(V_OD_hstmt);
	}
	refreshStmt();
	int vecsiz = reflector.getVectorSize(vecT,clasName,appName);
	for (int var = 0; var < vecsiz; ++var)
	{
		for (int var1 = 0; var1 < (int)relv.size(); ++var1)
		{
			DBRel relation = relv.at(var1);
			if(relation.type==2)
			{
				clearMaps();
				args argus;
				string methname = "get"+AfcUtil::camelCased(relation.pk);
				Method meth = clas.getMethod(methname,argus);
				vals valus;
				void *tt = reflector.getVectorElement(vecT,var,clasName,appName);
				void *ns = reflector.invokeMethodGVP(tt,meth,valus,appName);
				Object on;
				on << ns;
				on.setTypeName(meth.getReturnType());
				//addParam(relation.fk,on);
				//col = getARACW(relation.clsName);
				valus.push_back(col);
				argus.push_back("vector<"+relation.clsName+">");
				methname = "set"+AfcUtil::camelCased(relation.field);
				meth = clas.getMethod(methname,argus);
				reflector.invokeMethodGVP(tt,meth,valus,appName);
			}
		}
	}
	refreshStmt();
	close(!isTransaction);
	clearMaps();
	return vecT;
}


void* Cibernate::getElements()
{
	int V_OD_erg;// result of functions
	SQLCHAR colName[256];
	SQLSMALLINT	V_OD_colanz, colNameLen, dataType, numDecimalDigits, allowsNullValues;
	SQLULEN columnSize;

	vector<map<string, void*> >* vecT = new vector<map<string, void*> >;

	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;

		V_OD_erg = SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			close(true);
		}

		//logger << "Number of Columns " << V_OD_colanz << endl;

		map<string, void*> colValMap;

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
	close(!isTransaction);
	return vecT;
}


int Cibernate::storeProperty(ClassInfo clas, void* t, int var, string fieldName)
{
	void* col = NULL;
	SQLRETURN ret;
	SQLLEN indicator;
	Field fe = clas.getField(fieldName);
	string te = fe.getType();
	if(te=="int")
	{
		col = new int;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG, col, sizeof(col), &indicator);
	}
	else if(te=="unsigned int")
	{
		col = new unsigned int;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_ULONG,col, sizeof(col), &indicator);
	}
	else if(te=="long")
	{
		col = new long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,col, sizeof(col), &indicator);
	}
	else if(te=="unsigned long")
	{
		col = new unsigned long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_ULONG,col, sizeof(col), &indicator);
	}
	else if(te=="long long")
	{
		col = new long long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_SBIGINT, col, sizeof(col), &indicator);
	}
	else if(te=="unsigned long long")
	{
		col = new unsigned long long;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_UBIGINT, col, sizeof(col), &indicator);
	}
	else if(te=="short")
	{
		col = new short;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_SHORT,col, sizeof(col), &indicator);
	}
	else if(te=="unsigned short")
	{
		col = new unsigned short;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_USHORT,col, sizeof(col), &indicator);
	}
	else if(te=="double")
	{
		col = new double;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE, col, sizeof(col), &indicator);
	}
	else if(te=="float")
	{
		col = new float;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_FLOAT, col, sizeof(col), &indicator);
	}
	else if(te=="bool")
	{
		col = new bool;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT, col, sizeof(col), &indicator);
	}
	else if(te=="string")
	{
		/*col = new string;
		SQLBindCol(V_OD_hstmt,var,SQL_C_CHAR, col,10,sizes);*/
		char buf[24];
		string *temp = new string;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
		temp->append(buf);
		//logger << indicator << flush;
		if(indicator > (long)24)
		{
			int len = indicator-24;
			char buf1[len];
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
			temp->append(buf1);
			//logger << buf1 << flush;
		}
		col = temp;
		//logger << *temp << "\n" << flush;
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
		//logger << indicator << flush;
		if(indicator > (long)24)
		{
			int len = indicator-24;
			char buf1[len];
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
			temp.append(buf1);
			//logger << buf1 << flush;
		}
		DateFormat datf("yyyy-mm-dd");
		if(temp.length()>10)
			datf.setFormatspec("yyyy-mm-dd hh:mi:ss");
		else
			datf.setFormatspec("yyyy-mm-dd hh:mi:ss.nnnnnn");
		Date *date = datf.parse(temp);
		col = date;
		//logger << temp << "\n" << flush;
	}
	else if(te=="BinaryData")
	{
		/*col = new string;
		SQLBindCol(V_OD_hstmt,var,SQL_C_CHAR, col,10,sizes);*/
		unsigned char buf[24];
		BinaryData *temp = new BinaryData;
		ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf, sizeof(buf), &indicator);
		//temp->append(buf,indicator);
		//logger << indicator << flush;

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
		//logger << buf << "\n" << flush;
	}
	else//if its not a vector means its a one-to-one relationship
	{

	}
	if(col!=NULL)
	{
		Reflector reflector;
		args argus;
		argus.push_back(te);
		vals valus;
		//valus.push_back(columns.at(var));
		valus.push_back(col);
		string methname = "set"+AfcUtil::camelCased(fe.getFieldName());
		Method meth = clas.getMethod(methname,argus);
		reflector.invokeMethod<void*>(t,meth,valus,appName);
		var++;
	}
	return var;
}


int Cibernate::getProperty(int dataType, int columnSize, map<string, void*>& colValMap, string colName, int var)
{
	void* col = NULL;
	SQLRETURN ret;
	SQLLEN indicator;
	switch (dataType) {
		case SQL_BIT:
		{
			col = new bool;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT, col, sizeof(col), &indicator);
			colValMap[colName] = col;
		}
		case SQL_REAL:
		{
			col = new float;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_FLOAT, col, sizeof(col), &indicator);
			colValMap[colName] = col;
		}
		case SQL_DECIMAL:
		case SQL_DOUBLE:
		{
			col = new double;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE, col, sizeof(col), &indicator);
			colValMap[colName] = col;
		}
		case SQL_VARCHAR:
		case SQL_CHAR:
		default:
		{
			char buf[24];
			string *temp = new string;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf, sizeof(buf), &indicator);
			temp->append(buf);
			if(indicator > (long)24)
			{
				int len = indicator-24;
				char buf1[len];
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_CHAR, buf1, sizeof(buf1), &indicator);
				temp->append(buf1);
			}
			col = temp;
			colValMap[colName] = col;
			break;
		}
		case SQL_BINARY:
		{
			unsigned char buf[24];
			BinaryData *temp = new BinaryData;
			ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY, buf, sizeof(buf), &indicator);
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
			colValMap[colName] = col;
			break;
		}
		case SQL_INTEGER:
		{
			if(columnSize>10)
			{
				col = new long;
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG, col, sizeof(col), &indicator);
				colValMap[colName] = col;
			}
			else
			{
				col = new int;
				ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG, col, sizeof(col), &indicator);
				colValMap[colName] = col;
			}
			break;
		}
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
			long long *number = new long long(CastUtil::lexical_cast<long long>(temp));
			colValMap[colName] = number;
			break;
		}
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
			col = date;
			colValMap[colName] = col;
			break;
		}
	}
	return var+1;
}


void* Cibernate::sqlfuncs(string type,string clasName)
{
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	string query = "select "+type+" from "+tableName;
	CibernateQuery cquery(query);
	vector<map<string, void*> > vec = execute(cquery);
	if(vec.size()>0 && vec.at(0).size()>0)
	{
		return vec.at(0).begin()->second;
	}
	return NULL;
}

bool Cibernate::startTransaction()
{
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
}

bool Cibernate::commit()
{
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
	close(true);
	return flagc;
}

bool Cibernate::rollback()
{
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
	close(true);
	return flagc;
}


void* Cibernate::executeQuery(CibernateQuery cquery, vector<string> cols)
{
	Reflector reflector;
	string clasName = cquery.className;
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	ClassInfo clas = reflector.getClassInfo(clasName,appName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	string query = "select ";
	if(cols.size()>0)
	{
		for (unsigned int var = 0; var < cols.size(); var++)
		{
			string propertyName = cols.at(var);
			string columnName = propertyName;
			if(this->mapping->getAppTableColMapping(clasName,propertyName)!="")
				columnName = this->mapping->getAppTableColMapping(clasName,propertyName);
			query += (clasName + "_Alias."  + columnName);
			if(var!=cols.size()-1)
				query += ",";
		}
	}
	else
	{
		strMap tabcolmap = this->mapping->getAppTableColMapping(clasName);
		strMap::iterator it;
		int fldsiz = (int)tabcolmap.size();
		int var = 1;
		for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
		{
			string columnName = it->first;
			query += (clasName + "_Alias."  + columnName);
			if(var++!=fldsiz)
				query += ",";
		}
	}
	string reltabs = " from ";
	for (int var1 = 0; var1 < (int)relv.size(); var1++)
	{
		DBRel relation = relv.at(var1);
		if(relation.type==1)
		{
			string tableName1 = this->mapping->getAppTableClassMapping(relation.clsName);
			if(tableName1!="")
			{
				query += ",";
				strMap tabcolmap = this->mapping->getAppTableColMapping(relation.clsName);
				strMap::iterator it;
				int fldsiz = (int)tabcolmap.size();
				int var = 1;
				for(it=tabcolmap.begin();it!=tabcolmap.end();it++)
				{
					string columnName = it->first;
					query += (relation.clsName + "_Alias."  + columnName);
					if(var++!=fldsiz)
						query += ",";
				}
				reltabs += tableName1 + " " + relation.clsName +"_Alias,";
			}
		}
	}
	query += (reltabs + tableName + " " + clasName + "_Alias");
	//logger << query << flush;
	cquery.query = query;
	return executeQuery(cquery);
}


bool Cibernate::executeInsert(CibernateQuery cquery, vector<string> cols, void* t)
{
	string clasName = cquery.className;
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	string query = "insert into "+tableName+"(";
	unsigned var=0;
	Reflector reflector;
	string vals;
	ClassInfo clas = reflector.getClassInfo(clasName,appName);
	fldMap fields = clas.getFields();
	unsigned par = 1;
	if(cols.size()>0)
	{
		for (unsigned int var = 0; var < cols.size(); var++)
		{
			string propertyName = cols.at(var);
			string columnName = propertyName;
			if(this->mapping->getAppTableColMapping(clasName,propertyName)!="")
				columnName = this->mapping->getAppTableColMapping(clasName,propertyName);
			args argus;
			vector<void *> valus;
			Field fld = fields[cols.at(var)];
			string methname = "get"+AfcUtil::camelCased(cols.at(var));
			Method meth = clas.getMethod(methname,argus);
			if(fld.getType()=="short")
			{
				short temp = reflector.invokeMethod<short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned short")
			{
				unsigned short temp = reflector.invokeMethod<unsigned short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned int")
			{
				unsigned int temp = reflector.invokeMethod<unsigned int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="long")
			{
				long temp = reflector.invokeMethod<long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned long")
			{
				unsigned long temp = reflector.invokeMethod<unsigned long>(t,meth,valus);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="double")
			{
				double temp = reflector.invokeMethod<double>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="float")
			{
				float temp = reflector.invokeMethod<float>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="string")
			{
				string temp = reflector.invokeMethod<string>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}

			query += (columnName);
			vals += "?";
			if(var!=cols.size()-1)
			{
				query += ",";
				vals += ",";
			}
		}
	}
	else
	{
		fldMap::iterator it;
		for(it=fields.begin();it!=fields.end();++it)
		{
			args argus;
			vector<void *> valus;
			Field fld = it->second;
			string methname = "get"+AfcUtil::camelCased(it->first);
			Method meth = clas.getMethod(methname,argus);
			if(fld.getType()=="short")
			{
				short temp = reflector.invokeMethod<short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned short")
			{
				unsigned short temp = reflector.invokeMethod<unsigned short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned int")
			{
				unsigned int temp = reflector.invokeMethod<unsigned int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="long")
			{
				long temp = reflector.invokeMethod<long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned long")
			{
				unsigned long temp = reflector.invokeMethod<unsigned long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="double")
			{
				double temp = reflector.invokeMethod<double>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="float")
			{
				float temp = reflector.invokeMethod<float>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="string")
			{
				string temp = reflector.invokeMethod<string>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			query += (it->first);
			vals += "?";
			if(var++!=fields.size()-1)
			{
				query += ",";
				vals += ",";
			}
		}
	}
	query += (") values("+vals+")");
	cquery.query = query;
	//logger << query << flush;

	bool* flag = (bool*)executeQuery(cquery);
	bool ffl = *flag;
	delete flag;
	return ffl;
}


bool Cibernate::executeUpdate(CibernateQuery cquery, vector<string> cols, void* t)
{
	string clasName = cquery.className;
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	string query = "update "+tableName+" set ";
	Reflector reflector;
	string vals;
	ClassInfo clas = reflector.getClassInfo(clasName,appName);
	fldMap fields = clas.getFields();
	unsigned par = 1,var=0;
	if(cols.size()>0)
	{
		for (unsigned int var = 0; var < cols.size(); var++)
		{
			string propertyName = cols.at(var);
			string columnName = propertyName;
			if(this->mapping->getAppTableColMapping(clasName,propertyName)!="")
				columnName = this->mapping->getAppTableColMapping(clasName,propertyName);
			args argus;
			vector<void *> valus;
			Field fld = fields[cols.at(var)];
			string methname = "get"+AfcUtil::camelCased(cols.at(var));
			Method meth = clas.getMethod(methname,argus);
			if(fld.getType()=="short")
			{
				short temp = reflector.invokeMethod<short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned short")
			{
				unsigned short temp = reflector.invokeMethod<unsigned short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned int")
			{
				unsigned int temp = reflector.invokeMethod<unsigned int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="long")
			{
				long temp = reflector.invokeMethod<long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned long")
			{
				unsigned long temp = reflector.invokeMethod<unsigned long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="double")
			{
				double temp = reflector.invokeMethod<double>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="float")
			{
				float temp = reflector.invokeMethod<float>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="string")
			{
				string temp = reflector.invokeMethod<string>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}

			query += (columnName+" = ?");
			if(var!=cols.size()-1)
			{
				query += ",";
			}
		}
	}
	else
	{
		fldMap::iterator it;
		for(it=fields.begin();it!=fields.end();++it)
		{
			args argus;
			vector<void *> valus;
			Field fld = it->second;
			string methname = "get"+AfcUtil::camelCased(it->first);
			Method meth = clas.getMethod(methname,argus);
			if(fld.getType()=="short")
			{
				short temp = reflector.invokeMethod<short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned short")
			{
				unsigned short temp = reflector.invokeMethod<unsigned short>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned int")
			{
				unsigned int temp = reflector.invokeMethod<unsigned int>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="long")
			{
				long temp = reflector.invokeMethod<long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="unsigned long")
			{
				unsigned long temp = reflector.invokeMethod<unsigned long>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="double")
			{
				double temp = reflector.invokeMethod<double>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="float")
			{
				float temp = reflector.invokeMethod<float>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}
			else if(fld.getType()=="string")
			{
				string temp = reflector.invokeMethod<string>(t,meth,valus,appName);
				Object o;
				o << temp;
				cquery.propPosVaues[var+1] = o;
			}

			query += (it->first+" = ?");
			if(var++!=fields.size()-1)
			{
				query += ",";
			}
		}
	}
	cquery.query = query;
	//logger << query << flush;

	bool* flag = (bool*)executeQuery(cquery);
	bool ffl = *flag;
	delete flag;
	return ffl;
}

void* Cibernate::executeQuery(CibernateQuery query)
{
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
		if(query.start>0 && query.count>0)
		{
			StringContext params;
			params["start"] = query.start;
			params["count"] = query.count;
			query.query = DialectHelper::getSQLString(dialect, DialectHelper::PAGINATION_OFFSET_SQL, query.query, params);
		}
		else if(query.count>0)
		{
			StringContext params;
			params["count"] = query.count;
			query.query = DialectHelper::getSQLString(dialect, DialectHelper::PAGINATION_NO_OFFSET_SQL, query.query, params);
		}
	}

	int V_OD_erg;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen,V_OD_colanz;
	SQLINTEGER V_OD_err;
	SQLLEN V_OD_rowanz;
	SQLCHAR V_OD_msg[200];

	if(query.propPosVaues.size()>0 && query.propNameVaues.size()>0)
	{
		throw "Cannot mix positional and named parameters";
	}

	SQLFreeStmt(V_OD_hstmt, SQL_RESET_PARAMS);
	bindQueryParams(query);
	if(query.isUpdate())
	{
		bool* flag = new bool(true);
		V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.query.c_str(),SQL_NTS);
		if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			*flag = false;
			logger << "Error in prepare statement " << V_OD_erg << endl;
			SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
			logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
			close(true);
			return flag;
		}
		V_OD_erg=SQLExecute(V_OD_hstmt);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
		   *flag = false;
		   logger << "Error in Insert " << V_OD_erg << endl;
		   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		   close(true);
		}
		refreshStmt();
		close(!isTransaction);
		return flag;
	}
	else
	{
		V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.query.c_str(),SQL_NTS);
		if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			logger << "Error in prepare statement " << V_OD_erg << endl;
			SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
			logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
			close(true);
			return vecT;
		}
		V_OD_erg=SQLExecute(V_OD_hstmt);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
		   logger << "Error in Select " << V_OD_erg << endl;
		   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		   close(true);
		   return vecT;
		}
		V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
		  logger << "Number of RowCount " << V_OD_erg << endl;
		  close(true);
		  return vecT;
		}
		logger << "Number of Rows " << (int)V_OD_rowanz << endl;
		if(query.className!="")
			return getElements(query.className);
		else
			return getElements();
	}
}


void Cibernate::bindQueryParams(CibernateQuery& query)
{
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	int V_OD_erg;// result of functions
	unsigned par = 1;
	Parameters qparams = query.propNameVaues;
	Parameters columnBindings = query.columnBindings;
	PosParameters propPosVaues;
	Parameters::iterator ite;
	string queryString = query.query;
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
		query.query = queryString;
	}
	else
	{
		propPosVaues = query.propPosVaues;
	}

	if(StringUtil::toLowerCopy(query.query).find(" where ")==string::npos && columnBindings.size()>0)
	{
		query.query += " where ";
		int position = 1;
		for(ite=columnBindings.begin();ite!=columnBindings.end();++ite)
		{
			propPosVaues[position] = ite->second;
			query.query += (ite->first + " = ? ");
			if(position++!=columnBindings.size())
			{
				query.query += " AND ";
			}
		}
	}

	if(query.orderByDescCols.size()>0 || query.orderByAscCols.size()>0)
	{
		map<string, bool>::iterator iter;
		int position = 1;
		for(iter=query.orderByDescCols.begin();iter!=query.orderByDescCols.end();++iter)
		{
			query.query += (iter->first + " DESC ");
			if(position++!=query.orderByDescCols.size())
			{
				query.query += ",";
			}
		}
		if(query.orderByDescCols.size()>0 && query.orderByAscCols.size()>0)
			query.query += ",";

		position = 1;
		for(iter=query.orderByAscCols.begin();iter!=query.orderByAscCols.end();++iter)
		{
			query.query += (iter->first + " ASC ");
			if(position++!=query.orderByAscCols.size())
			{
				query.query += ",";
			}
		}
	}

	logger << query.query << endl;

	int totalParams = propPosVaues.size();
	while(totalParams-->0)
	{
		if(propPosVaues.find(par)==propPosVaues.end())
			throw ("No parameter value found for position " + CastUtil::lexical_cast<string>(par));
		Object paramValue = propPosVaues[par];
		if(paramValue.getTypeName()=="short")
		{
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, paramValue.getVoidPointer() , 20, NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else if(paramValue.getTypeName()=="unsigned short")
		{
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_USHORT, SQL_SMALLINT, 0, 0, paramValue.getVoidPointer() , 20, NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else if(paramValue.getTypeName()=="int" || paramValue.getTypeName()=="long")
		{
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, paramValue.getVoidPointer() , 20, NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else if(paramValue.getTypeName()=="unsigned int" || paramValue.getTypeName()=="unsigned long")
		{
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, paramValue.getVoidPointer() , 20, NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else if(paramValue.getTypeName()=="double")
		{
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, paramValue.getVoidPointer() , 20, NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else if(paramValue.getTypeName()=="float")
		{
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, paramValue.getVoidPointer() , 20, NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else if(paramValue.getTypeName()=="std::string")
		{
			string parm = *(string*)(paramValue.getVoidPointer());
			V_OD_erg= SQLBindParameter(V_OD_hstmt, par, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, 0, 0, &parm[0] ,parm.length(), NULL);
			if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			{
				logger << "Error in binding parameter " << V_OD_erg << endl;
				SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
				logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
				close(true);
			}
		}
		else
			throw "Cannot bind value";
		par++;
	}
}

vector<map<string, void*> > Cibernate::execute(CibernateQuery query)
{
	vector<map<string, void*> > tv;
	void* temp = executeQuery(query);
	if(temp!=NULL)
	{
		tv = *(vector<map<string, void*> >*)temp;
		delete temp;
	}
	return tv;
}
