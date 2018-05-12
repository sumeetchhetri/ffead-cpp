/*
 * SQLConnectionPool.cpp
 *
 *  Created on: 14-May-2014
 *      Author: sumeetc
 */

#include "SQLConnectionPool.h"

SQLConnectionPool::SQLConnectionPool(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("SQLConnectionPool");
	createPool(props);
}

void SQLConnectionPool::initEnv() {
	int V_OD_erg;// result of functions
	// 1. allocate Environment handle and register version
#ifdef HAVE_LIBODBC
	SQLHENV	V_OD_Env = getEnv();
	V_OD_erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&V_OD_Env);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error AllocHandle" << std::endl;
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		V_OD_Env = NULL;
		throw "Unable to initialize odbc/sql handle";
	}
	else
	{
		V_OD_erg=SQLSetEnvAttr(V_OD_Env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			logger << "Error SetEnv" << std::endl;
			SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		}
		setEnv(V_OD_Env);
	}
#endif
}

void* SQLConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
#ifdef HAVE_LIBODBC
	SQLHENV	V_OD_Env = getEnv();
	int V_OD_erg;// result of functions
	SQLCHAR V_OD_msg[200],V_OD_stat[10];		// Status SQL;
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err_s;
	SQLHDBC conn;
	ConnectionProperties props = getProperties();
	if(V_OD_Env==NULL)return NULL;
	// 2. allocate connection handle, set timeout
	V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, V_OD_Env, &conn);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error AllocHDB " << V_OD_erg << std::endl;
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		return NULL;
		//exit(0);
	}
	SQLSetConnectAttr(conn, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
	// 3. Connect to the datasource "MySQL-test"
	V_OD_erg = SQLConnect(conn, (SQLCHAR*) node.getDsn().c_str(), node.getDsn().length(),
									 (SQLCHAR*) node.getUsername().c_str(), node.getUsername().length(),
									 (SQLCHAR*) node.getPassword().c_str(), node.getPassword().length());
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error SQLConnect " << V_OD_erg << std::endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, conn, 1, V_OD_stat, &V_OD_err_s,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int)V_OD_err_s <<  ")" << std::endl;
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		SQLFreeHandle(SQL_HANDLE_DBC, conn);
		return NULL;
		//exit(0);
	}
	return conn;
#else
	return NULL;
#endif
}

void SQLConnectionPool::closeConnection(void* conn) {
#ifdef HAVE_LIBODBC
	SQLDisconnect(conn);
	SQLFreeHandle(SQL_HANDLE_DBC, conn);
#endif
}

void SQLConnectionPool::destroy() {
#ifdef HAVE_LIBODBC
	SQLHENV	V_OD_Env = getEnv();
	SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
#endif
}

SQLConnectionPool::~SQLConnectionPool() {
	destroyPool();
}

