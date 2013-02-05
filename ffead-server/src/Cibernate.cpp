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
	close();
	logger << "\nDestructed Cibernate" << flush;
}
Cibernate::Cibernate(string appName) {
	logger = Logger::getLogger("Cibernate");
	this->init = false;
	this->appName = appName;
	if (this->appName != "" && CibernateConnPools::isInitialized()
			&& CibernateConnPools::getPool(this->appName) != NULL) {
		this->init = true;
		this->pool = CibernateConnPools::getPool(this->appName);
		this->mapping = CibernateConnPools::getMapping(this->appName);
		logger << "\ngot pool " << this->pool << " mapping " << this->mapping
				<< " for application " << appName << "\n" << flush;
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
		this->pool = CibernateConnPools::getPool(this->appName);
		this->mapping = CibernateConnPools::getMapping(this->appName);
		logger << "\ngot pool " << this->pool << " mapping " << this->mapping
				<< " for application " << appName << "\n" << flush;
	}
	else
		throw "Error connecting to Database server";
}

string Cibernate::demangle(const char *mangled)
{
	int status;	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	stringstream ss;
	ss << demangled;
	string s;
	ss >> s;
	return s;
}

Cibernate::Cibernate(string dbName, string uname, string pass) {
	this->pool = new CibernateConnectionPool(5, dbName, uname, pass);
	this->init = true;
	if(!CibernateConnPools::isInitialized())
		throw "Error connecting to Database server";
}
bool Cibernate::allocateStmt(bool read) {
	if(!this->init)
	{
		return false;
	}
	if (this->pool != NULL && this->conn != NULL && this->conn->type == read
			&& V_OD_hstmt != NULL) {
		SQLCloseCursor(V_OD_hstmt);
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
void Cibernate::procedureCall(string procName) {
	bool flagc = allocateStmt(true);
	if(!flagc)throw "Error getting Database connection";
	int V_OD_erg;// result of functions
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen, V_OD_colanz;
	SQLINTEGER V_OD_err, V_OD_rowanz;
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
					close();
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
			close();
			throw "Error in call to stored procedure";
		}
		SQLCloseCursor(V_OD_hstmt);
	}

	logger << quer << flush;
	query = (SQLCHAR*) quer.c_str();
	/*V_OD_erg = SQLPrepare(V_OD_hstmt, query, SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		printf("Error in prepare statement stored procedure %d\n", V_OD_erg);
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
				&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
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
					close();
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
					close();
					throw "Error Binding parameter";
				}
			}
			else if (params[it->first]->getTypeName() == "std::string") {
				string *parm = (string*)params[it->first]->getVoidPointer();
				char *parmv = (char*)parm->c_str();
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par , SQL_PARAM_INPUT,
						SQL_C_CHAR,SQL_VARCHAR, 0, 0, (SQLPOINTER)parm->c_str() ,parm->length(), &hotelInd);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
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
		close();
		throw "Error in call to stored procedure";
	}

	/*V_OD_erg = SQLNumResultCols(V_OD_hstmt, &V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg = SQLRowCount(V_OD_hstmt, &V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Number of RowCount " << V_OD_erg << endl;
		close();
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;*/
	//SQLCloseCursor(V_OD_hstmt);

	logger << outQuery << flush;
	SQLLEN siz;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) outQuery.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Error in Select " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
				&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
		throw "Error in call to stored procedure";
	}
	/*V_OD_erg = SQLNumResultCols(V_OD_hstmt, &V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg = SQLRowCount(V_OD_hstmt, &V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Number of RowCount " << V_OD_erg << endl;
		close();
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
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	close();
}

void Cibernate::close() {
	this->pool->closeConnection(conn);
	this->pool = NULL;
	this->mapping = NULL;
	V_OD_hdbc = NULL;
	SQLFreeHandle(SQL_HANDLE_STMT, V_OD_hstmt);
	V_OD_hstmt = NULL;
}
/*void Cibernate::closeONEXITOFSERVER()
 {
 SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
 SQLDisconnect(V_OD_hdbc);
 SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
 SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
 printf("Disconnected !\n");
 }*/
void Cibernate::delAll(string clasName) {
	empty(clasName);
}

void Cibernate::binPrams(string &query)
{
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	int V_OD_erg;// result of functions
	unsigned var=0,par=1;
	Params::iterator ite;
	if(!igPaWC)
	{
		query += " where ";
		for(ite=params.begin();ite!=params.end();++ite)
		{
			if(params[ite->first]->getTypeName()=="int")
			{
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, params[ite->first]->getVoidPointer() , 20, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			else if(params[ite->first]->getTypeName()=="std::string")
			{
				string parm = *(string*)(params[ite->first]->getVoidPointer());
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_VARCHAR, 0, 0, &parm[0] ,parm.length(), NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			query += (ite->first + " = ?");
			if(var++!=params.size()-1)
				query += (" and ");
		}
	}
}


void Cibernate::delW(string clasName) {
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	if (params.size() == 0)
		return;
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string query = "delete from "+tableName;
	binPrams(query);
	logger << query << flush;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in Delete " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}

	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	return;
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
		close();
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
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
	ClassInfo clas = reflector.getClassInfo(clasName);
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	fldMap fields = clas.getFields();
	fldMap::iterator it;
	void *vecT = reflector.getNewVector(clasName);

	void *col = NULL;
	V_OD_erg=SQLFetch(V_OD_hstmt);
	while(V_OD_erg != SQL_NO_DATA)
	{
		unsigned int var = 0;
		args argus1;
		Constructor ctor = clas.getConstructor(argus1);
		void *t = reflector.newInstanceGVP(ctor);
		if(cols.size()<=0)
		{
			//for(it=fields.begin();it!=fields.end();++it)
			for(int ii=0;ii<(int)clas.getFieldVec().size();ii++)
			{
				col = NULL;
				SQLRETURN ret;
				SQLLEN indicator;
				Field fe = clas.getFieldVec().at(ii);
				string te = fe.getType();
				if(te=="int")
				{
					col = new int;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,col, sizeof(col), &indicator);
				}
				else if(te=="long")
				{
					col = new long;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,col, sizeof(col), &indicator);
				}
				else if(te=="short")
				{
					col = new short;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_SMALLINT,col, sizeof(col), &indicator);
				}
				else if(te=="double")
				{
					col = new double;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE,col, sizeof(col), &indicator);
				}
				else if(te=="float")
				{
					col = new float;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE,col, sizeof(col), &indicator);
				}
				else if(te=="bool")
				{
					col = new bool;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT,col, sizeof(col), &indicator);
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
					logger << *temp << "\n" << flush;
				}
				else if(te=="binary")
				{
					col = new bool;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY,col, sizeof(col), &indicator);
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
					Date *date = datf.parse(temp);
					col = date;
					logger << temp << "\n" << flush;
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
					logger << buf << "\n" << flush;
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
					string methname = "set"+AfcUtil::camelCased(fe.getFieldName());
					Method meth = clas.getMethod(methname,argus);
					reflector.invokeMethod<void*>(t,meth,valus);
					var++;
				}
			}
		}
		else
		{
			for(unsigned int u=0;u<cols.size();u++)
			{
				SQLRETURN ret;
				SQLLEN indicator;
				Field fe = fields[cols.at(u)];
				string te = fe.getType();
				if(te=="int")
				{
					col = new int;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,
														 col, sizeof(col), &indicator);
				}
				else if(te=="long")
				{
					col = new long;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,col, sizeof(col), &indicator);
				}
				else if(te=="short")
				{
					col = new short;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_SMALLINT,col, sizeof(col), &indicator);
				}
				else if(te=="double")
				{
					col = new double;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE,col, sizeof(col), &indicator);
				}
				else if(te=="float")
				{
					col = new float;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE,col, sizeof(col), &indicator);
				}
				else if(te=="bool")
				{
					col = new bool;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT,col, sizeof(col), &indicator);
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
					logger << *temp << "\n" << flush;
				}
				else if(te=="binary")
				{
					col = new bool;
					ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY,col, sizeof(col), &indicator);
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
					Date *date = datf.parse(temp);
					col = date;
					logger << temp << "\n" << flush;
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
					logger << buf << "\n" << flush;
				}
				if(col!=NULL)
				{
					args argus;
					argus.push_back(te);
					vals valus;
					valus.push_back(col);
					string methname = "set"+AfcUtil::camelCased(fe.getFieldName());
					Method meth = clas.getMethod(methname,argus);
					reflector.invokeMethod<void*>(t,meth,valus);
					var++;
				}
			}
		}
		for (int var1 = 0; var1 < (int)relv.size(); ++var1)
		{
			DBRel relation = relv.at(var1);
			if(relation.type==1)
			{
				string tableName1 = this->mapping->getAppTableClassMapping(relation.clsName);
				if(tableName1!="")
				{
					args argus1;
					ClassInfo clas1 = reflector.getClassInfo(relation.clsName);
					fldMap fields1 = clas1.getFields();
					Constructor ctor = clas1.getConstructor(argus1);
					void *tt = reflector.newInstanceGVP(ctor);
					//for(it=fields1.begin();it!=fields1.end();++it)
					for(int ii=0;ii<(int)clas.getFieldVec().size();ii++)
					{
						col = NULL;
						SQLRETURN ret;
						SQLLEN indicator;
						Field fe = clas.getFieldVec().at(ii);
						string te = fe.getType();
						if(te=="int")
						{
							col = new int;
							//SQLBindCol(V_OD_hstmt,var,SQL_C_LONG, col,10,sizes);
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,
																 col, sizeof(col), &indicator);
						}
						else if(te=="long")
						{
							col = new long;
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_LONG,col, sizeof(col), &indicator);
						}
						else if(te=="short")
						{
							col = new short;
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_SMALLINT,col, sizeof(col), &indicator);
						}
						else if(te=="double")
						{
							col = new double;
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE,col, sizeof(col), &indicator);
						}
						else if(te=="float")
						{
							col = new float;
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_DOUBLE,col, sizeof(col), &indicator);
						}
						else if(te=="bool")
						{
							col = new bool;
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BIT,col, sizeof(col), &indicator);
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
							logger << *temp << "\n" << flush;
						}
						else if(te=="binary")
						{
							col = new bool;
							ret = SQLGetData(V_OD_hstmt, var+1, SQL_C_BINARY,col, sizeof(col), &indicator);
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
							Date *date = datf.parse(temp);
							col = date;
							logger << temp << "\n" << flush;
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
							logger << buf << "\n" << flush;
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
							string methname = "set"+AfcUtil::camelCased(fe.getFieldName());
							Method meth = clas1.getMethod(methname,argus);
							reflector.invokeMethod<void*>(tt,meth,valus);
							var++;
						}
					}
					args argus;
					vals valus;
					valus.push_back(tt);
					argus.push_back(relation.clsName);
					string methname = "set"+AfcUtil::camelCased(relation.clsName);
					Method meth = clas.getMethod(methname,argus);
					reflector.invokeMethodGVP(t,meth,valus);
				}
			}
		}
		reflector.vectorPushBack(vecT,t,clasName);
		V_OD_erg=SQLFetch(V_OD_hstmt);
	}
	SQLCloseCursor(V_OD_hstmt);
	int vecsiz = reflector.getVectorSize(vecT,clasName);
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
				void *tt = reflector.getVectorElement(vecT,var,clasName);
				void *ns = reflector.invokeMethodGVP(tt,meth,valus);
				Object on;
				on << ns;
				on.setTypeName(meth.getReturnType());
				addParam(relation.fk,on);
				igPaWC = false;
				col = getARACW(relation.clsName);
				valus.push_back(col);
				argus.push_back("vector<"+relation.clsName+">");
				methname = "set"+AfcUtil::camelCased(relation.field);
				meth = clas.getMethod(methname,argus);
				reflector.invokeMethodGVP(tt,meth,valus);
			}
		}
	}
	clearMaps();
	return vecT;
}


string Cibernate::selectQuery(string clasName)
{
	vector<string> cols;
	return selectQuery(cols,clasName);
}
string Cibernate::selectQuery(vector<string> cols,string clasName)
{
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	string query = "select ";
	if(cols.size()>0)
	{
		for (unsigned int var = 0; var < cols.size(); var++)
		{
			string columnName = cols.at(var);
			if(this->mapping->getAppTableColMapping(tableName,columnName)!="")
				columnName = this->mapping->getAppTableColMapping(tableName,columnName);
			query += (clasName + "_Alias."  + columnName);
			if(var!=cols.size()-1)
				query += ",";
		}
	}
	else
		query += clasName + "_Alias.*";
	string reltabs = " from ";
	for (int var1 = 0; var1 < (int)relv.size(); var1++)
	{
		DBRel relation = relv.at(var1);
		if(relation.type==1)
		{
			string tableName1 = this->mapping->getAppTableClassMapping(relation.clsName);
			if(tableName1!="")
			{
				query += ","+relation.clsName+"_Alias.*";
				reltabs += tableName1 + " " + relation.clsName +"_Alias,";
			}
		}
	}
	query += (reltabs + tableName + " " + clasName + "_Alias");
	binPrams(query);
	for (int var1 = 0; var1 < (int)relv.size(); var1++)
	{
		DBRel relation = relv.at(var1);
		if(relation.type==1)
		{
			string tableName1 = this->mapping->getAppTableClassMapping(relation.clsName);
			if(tableName1!="")
			{
				if(query.find("where")==string::npos)
					query += " where ";
				else
					query += " and ";
				query += clasName + "_Alias"+"."+relation.pk+"="+relation.clsName +"_Alias."+relation.fk;
			}
		}
	}
	logger << query << flush;
	return query;
}

string Cibernate::insertQuery(vector<string> cols,string clasName,void *t)
{
	int V_OD_erg;// result of functions
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	string query = "insert into "+tableName+"(";
	Reflector reflector;
	string vals;
	ClassInfo clas = reflector.getClassInfo(clasName);
	fldMap fields = clas.getFields();
	unsigned par = 1,var=0;
	if(cols.size()>0)
	{
		for (unsigned int var = 0; var < cols.size(); var++)
		{
			string columnName = cols.at(var);
			if(this->mapping->getAppTableColMapping(tableName,columnName)!="")
				columnName = this->mapping->getAppTableColMapping(tableName,columnName);
			args argus;
			vector<void *> valus;
			Field fld = fields[cols.at(var)];
			string methname = "get"+AfcUtil::camelCased(cols.at(var));
			Method meth = clas.getMethod(methname,argus);
			if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus);
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &temp , 0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			else if(fld.getType()=="string")
			{
				string str = reflector.invokeMethod<string>(t,meth,valus);
				char *parm = (char*)str.c_str();
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_CHAR, 0, 0, parm ,0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
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
			if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus);
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &temp , 0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			else if(fld.getType()=="string")
			{
				string str = reflector.invokeMethod<string>(t,meth,valus);
				char *parm = (char*)str.c_str();
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_CHAR, 0, 0, parm ,0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
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
	binPrams(query);
	logger << query << flush;
	return query;
}


string Cibernate::updateQuery(vector<string> cols,string clasName,void *t)
{
	int V_OD_erg;// result of functions
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	vector<DBRel> relv = this->mapping->getAppTablerelMapping(clasName);
	string query = "update "+tableName+" set ";
	Reflector reflector;
	ClassInfo clas = reflector.getClassInfo(clasName);
	fldMap fields = clas.getFields();
	unsigned par = 1,var=0;
	if(cols.size()>0)
	{
		for (unsigned int var = 0; var < cols.size(); var++)
		{
			string columnName = cols.at(var);
			if(this->mapping->getAppTableColMapping(tableName,columnName)!="")
				columnName = this->mapping->getAppTableColMapping(tableName,columnName);
			args argus;
			vector<void *> valus;
			Field fld = fields[cols.at(var)];
			string methname = "get"+AfcUtil::camelCased(cols.at(var));
			Method meth = clas.getMethod(methname,argus);
			if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus);
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &temp , 0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			else if(fld.getType()=="string")
			{
				string str = reflector.invokeMethod<string>(t,meth,valus);
				char *parm = (char*)str.c_str();
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_CHAR, 0, 0, parm ,0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
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
			if(fld.getType()=="int")
			{
				int temp = reflector.invokeMethod<int>(t,meth,valus);
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_LONG,SQL_INTEGER, 0, 0, &temp , 0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			else if(fld.getType()=="string")
			{
				string str = reflector.invokeMethod<string>(t,meth,valus);
				char *parm = (char*)str.c_str();
				V_OD_erg= SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT, SQL_C_CHAR,SQL_CHAR, 0, 0, parm ,0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
				{
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			query += (it->first+" = ?");
			if(var++!=fields.size()-1)
			{
				query += ",";
			}
		}
	}
	binPrams(query);
	logger << query << flush;
	return query;
}

vector<map<string, void*> > Cibernate::getARSCW(string tableName,
		vector<string> cols, vector<string> coltypes)
{
	vector<map<string, void*> > retValueVec;
	bool flagc = allocateStmt(true);
		if(!flagc)return retValueVec;

	if (tableName == "" || (params.size() == 0 || cols.size() == 0
			|| coltypes.size() == 0) || igPaWC || (coltypes.size()
			!= cols.size()))
		return retValueVec;

	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT V_OD_mlen, V_OD_colanz;
	SQLINTEGER V_OD_err;
	SQLLEN V_OD_rowanz;
	SQLCHAR V_OD_msg[200];

	string query = "select ";
	for (unsigned int var = 0; var < cols.size(); var++) {
		string columnName = cols.at(var);
		if (this->mapping->getAppTableColMapping(tableName, columnName) != "")
			columnName = this->mapping->getAppTableColMapping(tableName, columnName);
		query += (tableName + "_Alias." + columnName);
		if (var != cols.size() - 1)
			query += ",";
	}
	query += (" from " + tableName + " " + tableName + "_Alias");
	//fldMap fields = clas.getFields();
	//fldMap::iterator it;
	Params::iterator ite;
	if (!igPaWC) {
		query += " where ";
		unsigned int var = 0, par = 1;
		for (ite = params.begin(); ite != params.end(); ++ite) {
			if (params[ite->first]->getTypeName() == "int") {
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT,
						SQL_C_LONG, SQL_INTEGER, 0, 0,
						params[ite->first]->getVoidPointer(), 20, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			} else if (params[ite->first]->getTypeName() == "std::string") {
				string *str = params[ite->first]->getPointer<string> ();
				char *parm = (char*) str->c_str();
				V_OD_erg = SQLBindParameter(V_OD_hstmt, par++, SQL_PARAM_INPUT,
						SQL_C_CHAR, SQL_VARCHAR, 0, 0, parm, 0, NULL);
				if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg
						!= SQL_SUCCESS_WITH_INFO)) {
					logger << "Error in binding parameter " << V_OD_erg << endl;
					SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1,
							(SQLCHAR*) V_OD_stat, &V_OD_err, V_OD_msg, 100,
							&V_OD_mlen);
					logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
					close();
				}
			}
			query += (ite->first + " = ?");
			if (var++ != params.size() - 1)
				query += (" and ");
		}
	}
	logger << query << flush;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Error in Select " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc, 1, (SQLCHAR*) V_OD_stat,
				&V_OD_err, V_OD_msg, 100, &V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	V_OD_erg = SQLNumResultCols(V_OD_hstmt, &V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg = SQLRowCount(V_OD_hstmt, &V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)) {
		logger << "Number of RowCount " << V_OD_erg << endl;
		close();
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;
	V_OD_erg = SQLFetch(V_OD_hstmt);
	while (V_OD_erg != SQL_NO_DATA) {
		map<string, void*> retValue;
		for (unsigned int u = 0; u < cols.size(); u++) {
			void* col = NULL;
			SQLRETURN ret;
			SQLLEN indicator;
			string columnName = cols.at(u);
			if (this->mapping->getAppTableColMapping(tableName, columnName) != "")
				columnName = this->mapping->getAppTableColMapping(tableName,
						columnName);
			if (coltypes.at(u) == "int") {
				col = new int;
				ret = SQLGetData(V_OD_hstmt, u + 1, SQL_C_LONG, col,
						sizeof(col), &indicator);
			} else if (coltypes.at(u) == "string" || coltypes.at(u)
					== "std::string") {
				char buf[24];
				string *temp = new string;
				ret = SQLGetData(V_OD_hstmt, u + 1, SQL_C_CHAR, buf,
						sizeof(buf), &indicator);
				temp->append(buf);
				logger << indicator << flush;
				if (indicator > (long) 24) {
					char buf1[indicator - 22];
					ret = SQLGetData(V_OD_hstmt, u + 1, SQL_C_CHAR, buf1,
							sizeof(buf1), &indicator);
					temp->append(buf1);
				}
				col = temp;
				logger << *temp << "\n" << flush;
			}
			retValue[columnName] = col;
		}
		retValueVec.push_back(retValue);
		V_OD_erg = SQLFetch(V_OD_hstmt);
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	return retValueVec;
}

void* Cibernate::getORW(string clasName)
{
	void *vecT = NULL;
	bool flagc = allocateStmt(true);
			if(!flagc)return vecT;

	if(params.size()==0 && !igPaWC)
		return vecT;
	int V_OD_erg;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen,V_OD_colanz;
	SQLINTEGER V_OD_err;
	SQLLEN V_OD_rowanz;
	SQLCHAR V_OD_msg[200];
	string query = selectQuery(clasName);
	query += " LIMIT 1";
	V_OD_erg=SQLExecDirect(V_OD_hstmt,(SQLCHAR*)query.c_str(),SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	   logger << "Error in Select " << V_OD_erg << endl;
	   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
	   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
	   close();
	}
	V_OD_erg=SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	  logger << "Number of RowCount " << V_OD_erg << endl;
	  close();
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;
	return getElements(clasName);
}

void* Cibernate::getARACW(string clasName)
{
	void *vecT = NULL;
	bool flagc = allocateStmt(true);
			if(!flagc)return vecT;

	if(params.size()==0 && !igPaWC)
		return vecT;
	int V_OD_erg;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen,V_OD_colanz;
	SQLINTEGER V_OD_err;
	SQLLEN V_OD_rowanz;
	SQLCHAR V_OD_msg[200];
	string query = selectQuery(clasName);
	V_OD_erg=SQLExecDirect(V_OD_hstmt,(SQLCHAR*)query.c_str(),SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	   logger << "Error in Select " << V_OD_erg << endl;
	   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
	   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
	   close();
	}
	V_OD_erg=SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	  logger << "Number of RowCount " << V_OD_erg << endl;
	  close();
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;
	return getElements(clasName);
}

void* Cibernate::getARSCW(string clasName,vector<string> cols)
{
	bool flagc = allocateStmt(true);
			if(!flagc)return NULL;
	void *vecT = NULL;
	if((params.size()==0 || cols.size()==0) && !igPaWC)
		return vecT;
	int V_OD_erg;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen,V_OD_colanz;
	SQLINTEGER V_OD_err;
	SQLLEN V_OD_rowanz;
	SQLCHAR V_OD_msg[200];
	string query = selectQuery(cols,clasName);
	V_OD_erg=SQLExecDirect(V_OD_hstmt,(SQLCHAR*)query.c_str(),SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in Select " << V_OD_erg << endl;
	   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
	   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
	   close();
	}
	V_OD_erg=SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	  logger << "Number of RowCount " << V_OD_erg << endl;
	  close();
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;
	return getElements(clasName);
}

void Cibernate::insertORSC(void* t,vector<string> cols,string className)
{
	bool flagc = allocateStmt(false);
		if(!flagc)return;
	Reflector reflector;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string vals;
	string query = insertQuery(cols,className,t);
	V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.c_str(),SQL_NTS);
	if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in prepare statement " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	V_OD_erg=SQLExecute(V_OD_hstmt);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	   logger << "Error in Insert " << V_OD_erg << endl;
	   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
	   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
	   close();
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
}

void Cibernate::updateORSC(void* t,vector<string> cols,string className)
{
	bool flagc = allocateStmt(false);
		if(!flagc)return;
	Reflector reflector;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string vals;
	string query = updateQuery(cols,className,t);
	V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.c_str(),SQL_NTS);
	if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in prepare statement " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	V_OD_erg=SQLExecute(V_OD_hstmt);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	   logger << "Error in Update " << V_OD_erg << endl;
	   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
	   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
	   close();
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
}

void* Cibernate::sqlfuncs(string type,string clasName)
{
	bool flagc = allocateStmt(true);
		if(!flagc)return NULL;
	void *vecT = NULL;
	if(params.size()==0 && !igPaWC)
		return vecT;
	int V_OD_erg;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen,V_OD_colanz;
	SQLINTEGER V_OD_err;
	SQLLEN V_OD_rowanz;
	SQLCHAR V_OD_msg[200];
	string tableName = this->mapping->getAppTableClassMapping(clasName);
	string query = "select "+type+" from "+tableName;
	binPrams(query);
	V_OD_erg = SQLPrepare(V_OD_hstmt,(SQLCHAR*)query.c_str(),SQL_NTS);
	if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in prepare statement " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	V_OD_erg=SQLExecute(V_OD_hstmt);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	   logger << "Error in Function " << type.c_str() << ", " << V_OD_erg << endl;
	   SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
	   logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
	   close();
	}
	V_OD_erg=SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		close();
	}
	logger << "Number of Columns " << V_OD_colanz << endl;
	V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
	  logger << "Number of RowCount " << V_OD_erg << endl;
	  close();
	}
	logger << "Number of Rows " << (int)V_OD_rowanz << endl;
	V_OD_erg = SQLFetch(V_OD_hstmt);
	void* col = NULL;
	while (V_OD_erg != SQL_NO_DATA)
	{
		SQLRETURN ret;
		SQLLEN indicator;
		if (clasName == "int")
		{
			col = new int;
			ret = SQLGetData(V_OD_hstmt, 1, SQL_C_LONG, col,sizeof(col), &indicator);
		}
		else if (clasName == "string" || clasName== "std::string")
		{
			char buf[24];
			string *temp = new string;
			ret = SQLGetData(V_OD_hstmt, 1, SQL_C_CHAR, buf,sizeof(buf), &indicator);
			temp->append(buf);
			logger << indicator << flush;
			if (indicator > (long) 24)
			{
				char buf1[indicator - 22];
				ret = SQLGetData(V_OD_hstmt, 1, SQL_C_CHAR, buf1,sizeof(buf1), &indicator);
				temp->append(buf1);
			}
			col = temp;
			logger << *temp << "\n" << flush;
		}
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	return col;
}

void Cibernate::startTransaction()
{
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string vals;
	string query = "start transaction";
	logger << query << flush;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in Start transaction " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	return;
}

void Cibernate::commit()
{
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string vals;
	string query = "commit";
	logger << query << flush;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in commit " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	return;
}

void Cibernate::rollback()
{
	bool flagc = allocateStmt(true);
		if(!flagc)return;
	int V_OD_erg;// result of functions
	//SQLINTEGER V_OD_id;
	char V_OD_stat[10];
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err;
	SQLCHAR V_OD_msg[200];
	string vals;
	string query = "rollback";
	logger << query << flush;
	V_OD_erg = SQLExecDirect(V_OD_hstmt, (SQLCHAR*) query.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error in rollback " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, (SQLCHAR*)V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int) V_OD_err << ")" << endl;
		close();
	}
	SQLCloseCursor(V_OD_hstmt);
	clearMaps();
	return;
}
