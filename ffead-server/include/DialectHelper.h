/*
 * DialectHelper.h
 *
 *  Created on: 25-Apr-2013
 *      Author: sumeetc
 */

#ifndef DIALECTHELPER_H_
#define DIALECTHELPER_H_
#include "string"
#include "map"
#include "TemplateEngine.h"
#include "CastUtil.h"
using namespace std;

class DialectHelper {
	static bool init;
	static map<string, map<int, string> > dialectStrMap;
	static void loadDialectSQLStrings();
	static void loadOracleDialectStrings();
	static void loadMySQLDialectStrings();
	static void loadPostgresDialectStrings();
	static void loadSQLServerDialectStrings();
	static void loadDB22DialectStrings();
	static void getPaginationSQL(string dialect, string& query, StringContext params);
	DialectHelper();
public:
	static string ORACLE_DIALECT, MYSQL_DIALECT, POSTGRES_DIALECT,
				  SQLSERVER_DIALECT, SQLSERVER12_DIALECT, TIMESTEN_DIALECT,
				  DB2_DIALECT;
	static int PAGINATION_OFFSET_SQL, PAGINATION_NO_OFFSET_SQL, VALIDDB_FUNCTIONS;
	static string getSQLString(string dialect, int type, string query, StringContext params);
	static void load();
	virtual ~DialectHelper();
};

#endif /* DIALECTHELPER_H_ */
