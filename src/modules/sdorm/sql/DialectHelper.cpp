/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * DialectHelper.cpp
 *
 *  Created on: 25-Apr-2013
 *      Author: sumeetc
 */

#include "DialectHelper.h"

int DialectHelper::VALIDDB_FUNCTIONS = 0, DialectHelper::PAGINATION_OFFSET_SQL = 1, DialectHelper::PAGINATION_NO_OFFSET_SQL = 2,
		DialectHelper::IDGEN_SEQUENCE_QUERY = 3, DialectHelper::IDGEN_TABLESEL_QUERY = 4, DialectHelper::IDGEN_TABLEUPD_QUERY = 5,
		DialectHelper::IS_TRANSACTION_SUPPORTED = 6, DialectHelper::IDGEN_IDENTITY_QUERY = 7, DialectHelper::IDGEN_TABLESELNOLOCK_QUERY = 8, 
		DialectHelper::IDGEN_TABLESELNOLOCKMHL_QUERY = 9, DialectHelper::IDGEN_TABLESELMHL_QUERY = 10, 
		DialectHelper::IDGEN_TABLEUPDMHL_QUERY = 11, DialectHelper::BULK_INSERT_QUERY = 12, DialectHelper::BULK_UPDATE_QUERY = 13,
		DialectHelper::AUTOCOMMIT_QUERY = 14;
std::map<std::string, std::map<int, std::string> > DialectHelper::dialectStrMap;
bool DialectHelper::init = false;
std::string DialectHelper::ORACLE_DIALECT = "OracleDialect", DialectHelper::MYSQLMYISAM_DIALECT = "MySQLMyISAMDialect",
	   DialectHelper::POSTGRES_DIALECT = "PostgresDialect", DialectHelper::SQLSERVER_DIALECT = "SQLServerDialect",
	   DialectHelper::SQLSERVER12_DIALECT = "SQLServer12Dialect", DialectHelper::TIMESTEN_DIALECT = "TimestenDialect",
	   DialectHelper::DB2_DIALECT = "DB2Dialect", DialectHelper::SQLLITE_DIALECT = "SQLLiteDialect",
	   DialectHelper::ANY_DIALECT = "AnyDialect", DialectHelper::MYSQLINNODB_DIALECT = "MySQLInnoDBMDialect";

DialectHelper::DialectHelper() {
}

std::string DialectHelper::getSQLString(const std::string& dialect, const int& type, std::string query, StringContext params) {
	load();
	if(dialectStrMap.find(dialect)!=dialectStrMap.end())
	{
		if(dialectStrMap[ANY_DIALECT].find(type)!=dialectStrMap[ANY_DIALECT].end())
		{
			std::string& dquery = dialectStrMap[ANY_DIALECT][type];
			std::string temp = query;
			StringUtil::trim(temp);
			StringUtil::toLower(temp);
			std::string fupd;
			if(temp.find_last_of(" for update")==0)
			{
				fupd = " for update";
				temp = temp.substr(0, temp.find(" for update"));
			}
			params["query"] = temp;
			return TemplateEngine::evaluate(dquery, params) + fupd;
		}
	}
	if(type==1 || type==2)
	{
		getPaginationSQL(dialect, query, params);
	}
	return query;
}

std::string DialectHelper::getSQLString(const std::string& dialect, const int& type, StringContext params) {
	load();
	std::string dquery;
	if(dialectStrMap.find(dialect)!=dialectStrMap.end())
	{
		if(dialectStrMap[dialect].find(type)!=dialectStrMap[dialect].end())
		{
			dquery = dialectStrMap[dialect][type];
		}
		else if(dialectStrMap[ANY_DIALECT].find(type)!=dialectStrMap[ANY_DIALECT].end())
		{
			dquery = dialectStrMap[ANY_DIALECT][type];
		}
		if(dquery!="")
		{
			std::string temp = dquery;
			StringUtil::trim(temp);
			StringUtil::toLower(temp);
			std::string fupd;
			if(temp.find_last_of(" for update")==0)
			{
				fupd = " for update";
				temp = temp.substr(0, temp.find(" for update"));
			}
			params["query"] = temp;
			return TemplateEngine::evaluate(dquery, params) + fupd;
		}
	}
	return dquery;
}

std::string DialectHelper::getProperty(const std::string& dialect, const int& type) {
	load();
	std::string propValue;
	if(dialectStrMap.find(dialect)!=dialectStrMap.end())
	{
		if(dialectStrMap[dialect].find(type)!=dialectStrMap[dialect].end())
		{
			propValue = dialectStrMap[dialect][type];
		}
		else if(dialectStrMap[ANY_DIALECT].find(type)!=dialectStrMap[ANY_DIALECT].end())
		{
			propValue = dialectStrMap[ANY_DIALECT][type];
		}
	}
	return propValue;
}

bool DialectHelper::isTransactionSupported(const std::string& dialect) {
	if(getProperty(dialect, IS_TRANSACTION_SUPPORTED)!="true")
		return false;
	return true;
}

std::string DialectHelper::getAutoCommitSQLString(const std::string& dialect) {
	return getProperty(dialect, AUTOCOMMIT_QUERY);
}

std::string DialectHelper::getIdGenerateQueryPre(const std::string& dialect, const DataSourceEntityMapping& dsemp) {
	if(dsemp.getIdgentype()!="hilo" && dsemp.getIdgentype()!="multihilo")
	{
		std::string query;
		if(dsemp.getIdgendbEntityType()=="table")
		{
			StringContext params;
			params["idgen_tabname"] = dsemp.getIdgendbEntityName();
			params["idgen_colname"] = dsemp.getIdgencolumnName();
			if(dsemp.getIdgentype()=="multi")
			{
				params["entity_column"] = dsemp.getIdgenentityColumn();
				query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_TABLESELMHL_QUERY, params);
			}
			else
			{
				query = getSQLString(dialect, DialectHelper::IDGEN_TABLESEL_QUERY, params);
			}
		}
		else if(dsemp.getIdgendbEntityType()=="sequence")
		{
			StringContext params;
			params["seq_name"] = dsemp.getIdgendbEntityName();
			query = getSQLString(dialect, DialectHelper::IDGEN_SEQUENCE_QUERY, params);
		}
		return query;
	}
	return "";
}

std::string DialectHelper::getIdGenerateQueryPost(const std::string& dialect, const DataSourceEntityMapping& dsemp) {
	if(dsemp.getIdgentype()!="hilo" && dsemp.getIdgentype()!="multihilo" && dsemp.getIdgendbEntityType()=="table")
	{
		std::string query;
		StringContext params;
		params["idgen_tabname"] = dsemp.getIdgendbEntityName();
		params["idgen_colname"] = dsemp.getIdgencolumnName();
		if(dsemp.getIdgentype()=="multi")
		{
			params["entity_column"] = dsemp.getIdgenentityColumn();
			query = DialectHelper::getSQLString(dialect, DialectHelper::IDGEN_TABLEUPDMHL_QUERY, params);
		}
		else
		{
			query = getSQLString(dialect, DialectHelper::IDGEN_TABLEUPD_QUERY, params);
		}
		return query;
	}
	else if(dsemp.getIdgentype()!="hilo" && dsemp.getIdgentype()!="multihilo" && dsemp.getIdgendbEntityType()=="identity")
	{
		std::string query;
		StringContext params;
		query = getSQLString(dialect, DialectHelper::IDGEN_IDENTITY_QUERY, params);
		return query;
	}
	return "";
}

DialectHelper::~DialectHelper() {
}

void DialectHelper::load()
{
	if(!init)
	{
		init = true;
		loadDialectSQLStrings();
	}
}


void DialectHelper::loadDialectSQLStrings()
{
	loadOracleDialectStrings();
	loadMySQLDialectStrings();
	loadPostgresDialectStrings();
	loadSQLServerDialectStrings();
	loadDB22DialectStrings();
	dialectStrMap[ANY_DIALECT][IDGEN_TABLESELNOLOCK_QUERY] = "select ${idgen_colname} from ${idgen_tabname}";
	dialectStrMap[ANY_DIALECT][IDGEN_TABLESEL_QUERY] = "select ${idgen_colname} from ${idgen_tabname} for update";
	dialectStrMap[ANY_DIALECT][IDGEN_TABLEUPD_QUERY] = "update ${idgen_tabname} set ${idgen_colname} = ? where ${idgen_colname} = ?";
	dialectStrMap[ANY_DIALECT][IDGEN_TABLESELNOLOCKMHL_QUERY] = "select ${idgen_colname} from ${idgen_tabname} where ${entity_column} = ?";
	dialectStrMap[ANY_DIALECT][IDGEN_TABLESELMHL_QUERY] = "select ${idgen_colname} from ${idgen_tabname} where ${entity_column} = ? for update";
	dialectStrMap[ANY_DIALECT][IDGEN_TABLEUPDMHL_QUERY] = "update ${idgen_tabname} set ${idgen_colname} = ? where ${idgen_colname} = ? and ${entity_column} = ?";
}


void DialectHelper::loadOracleDialectStrings() {
	std::string PAGINATION_OFFSET = "select * from ( select row_.*, ROWNUM rownum_ from ( $query ) row_ ) where rownum_ <= $count and rownum_ > $start";
	std::string PAGINATION_NO_OFFSET = "select * from ( $query ) where ROWNUM <= $count";
	std::string VALID_DBFUNCS = ",abs,sign,acos,asin,atan,cos,cosh,exp,ln,sin,sinh,stddev,sqrt,tan,"
			"tanh,variance,round,trunc,ceil,floor,chr,initcap,lower,ltrim,rtrim,soundex,upper,"
			"ascii,to_char,to_date,current_date,current_time,current_timestamp,last_day,sysdate,"
			"systimestamp,uid,user,rowid,rownum,concat,instr,instrb,lpad,replace,rpad,substr,"
			"substrb,translate,substring,instr,vsize,coalesce,atan2,log,mod,nvl,nvl2,power,"
			"add_months,months_between,next_day,str,abs,sign,acos,asin,atan,cos,cosh,exp,ln,sin,"
			"sinh,stddev,sqrt,tan,tanh,variance,round,trunc,ceil,floor,chr,initcap,lower,ltrim,"
			"rtrim,soundex,upper,ascii,to_char,to_date,current_date,current_time,current_timestamp,"
			"last_day,sysdate,systimestamp,uid,user,rowid,rownum,concat,instr,instrb,lpad,replace,"
			"rpad,substr,substrb,translate,substring,instr,vsize,coalesce,atan2,log,mod,nvl,"
			"nvl2,power,add_months,months_between,next_day,str,";

	dialectStrMap[ORACLE_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[ORACLE_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[ORACLE_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[ORACLE_DIALECT][IDGEN_SEQUENCE_QUERY] = "select ${seq_name}.nextval from dual";
	dialectStrMap[ORACLE_DIALECT][AUTOCOMMIT_QUERY] = "SET AUTOCOMMIT ON";

	dialectStrMap[TIMESTEN_DIALECT][VALIDDB_FUNCTIONS] = ",upper,rtrim,concat,mod,to_char,to_date,sysdate,getdate,nvl,";
}

void DialectHelper::loadMySQLDialectStrings() {
	std::string PAGINATION_OFFSET = "$query LIMIT $count";
	std::string PAGINATION_NO_OFFSET = "$query LIMIT $start OFFSET $count";
	std::string VALID_DBFUNCS = ",ascii,bin,char_length,character_length,lcase,lower,ltrim,ord,"
			"quote,reverse,rtrim,soundex,space,ucase,upper,unhex,abs,sign,acos,asin,atan,"
			"cos,cot,crc32,exp,ln,log,log2,log10,pi,rand,sin,sqrt,tan,radians,degrees,ceiling,"
			"ceil,floor,round,datediff,timediff,date_format,curdate,curtime,current_date,"
			"current_time,current_timestamp,date,day,dayofmonth,dayname,dayofweek,dayofyear,"
			"from_days,from_unixtime,hour,last_day,localtime,localtimestamp,microseconds,"
			"minute,month,monthname,now,quarter,second,sec_to_time,sysdate,time,timestamp,"
			"time_to_sec,to_days,unix_timestamp,utc_date,utc_time,utc_timestamp,week,weekday,"
			"weekofyear,year,yearweek,hex,oct,octet_length,bit_length,bit_count,encrypt,md5,"
			"sha1,sha,concat,";

	dialectStrMap[MYSQLMYISAM_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[MYSQLMYISAM_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[MYSQLMYISAM_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[MYSQLMYISAM_DIALECT][IDGEN_IDENTITY_QUERY] = "SELECT LAST_INSERT_ID()";
	dialectStrMap[MYSQLMYISAM_DIALECT][AUTOCOMMIT_QUERY] = "SET AUTOCOMMIT=1";

	dialectStrMap[MYSQLINNODB_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[MYSQLINNODB_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[MYSQLINNODB_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[MYSQLINNODB_DIALECT][IS_TRANSACTION_SUPPORTED] = "TRUE";
	dialectStrMap[MYSQLINNODB_DIALECT][IDGEN_IDENTITY_QUERY] = "SELECT LAST_INSERT_ID()";
	dialectStrMap[MYSQLINNODB_DIALECT][AUTOCOMMIT_QUERY] = "SET AUTOCOMMIT=1";

	dialectStrMap[SQLLITE_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[SQLLITE_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
}

void DialectHelper::loadPostgresDialectStrings() {
	std::string PAGINATION_OFFSET = "$query LIMIT $count";
	std::string PAGINATION_NO_OFFSET = "$query LIMIT $start OFFSET $count";
	std::string VALID_DBFUNCS = ",ltrim,rtrim,soundex,sysdate,rowid,rownum,instr,lpad,replace,"
			"rpad,translate,substring,coalesce,atan2,mod,nvl,nvl2,power,add_months,"
			"months_between,next_day,abs,sign,acos,asin,atan,cos,cot,exp,ln,log,sin,sqrt,"
			"cbrt,tan,radians,degrees,stddev,variance,random,round,trunc,ceil,floor,chr,lower,"
			"upper,substr,initcap,to_ascii,quote_ident,quote_literal,md5,ascii,char_length,"
			"bit_length,octet_length,age,current_date,current_time,current_timestamp,date_trunc,"
			"localtime,localtimestamp,now,timeofday,current_user,session_user,user,"
			"current_database,current_schema,to_char,to_date,to_timestamp,to_number,concat,"
			"locate,cast,";

	dialectStrMap[POSTGRES_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[POSTGRES_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[POSTGRES_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[POSTGRES_DIALECT][IDGEN_SEQUENCE_QUERY] = "select nextval('${seq_name}') from dual";
	dialectStrMap[POSTGRES_DIALECT][IS_TRANSACTION_SUPPORTED] = "TRUE";
	dialectStrMap[POSTGRES_DIALECT][IDGEN_IDENTITY_QUERY] = "SELECT lastval()";
	dialectStrMap[POSTGRES_DIALECT][AUTOCOMMIT_QUERY] = "SET AUTOCOMMIT ON";
}

void DialectHelper::loadSQLServerDialectStrings() {
	std::string PAGINATION_OFFSET = "$query OFFSET $start ROWS FETCH NEXT $count ROWS ONLY";
	std::string PAGINATION_NO_OFFSET = "$query FETCH FIRST $count ROWS ONLY";
	std::string VALID_DBFUNCS = ",datepart,locate,mod,datalength,trim,current_timestamp,row_number,";

	dialectStrMap[SQLSERVER12_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[SQLSERVER12_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[SQLSERVER12_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[SQLSERVER12_DIALECT][IDGEN_SEQUENCE_QUERY] = "select next value for ${seq_name}";
	dialectStrMap[SQLSERVER12_DIALECT][IS_TRANSACTION_SUPPORTED] = "TRUE";
	dialectStrMap[SQLSERVER12_DIALECT][IDGEN_IDENTITY_QUERY] = "SELECT SCOPE_IDENTITY()";
	dialectStrMap[SQLSERVER12_DIALECT][AUTOCOMMIT_QUERY] = "SET IMPLICIT_TRANSACTIONS OFF";

	dialectStrMap[SQLSERVER_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[SQLSERVER_DIALECT][IS_TRANSACTION_SUPPORTED] = "TRUE";
	dialectStrMap[SQLSERVER_DIALECT][IDGEN_IDENTITY_QUERY] = "SELECT SCOPE_IDENTITY()";
	dialectStrMap[SQLSERVER_DIALECT][AUTOCOMMIT_QUERY] = "SET IMPLICIT_TRANSACTIONS OFF";
}

void DialectHelper::loadDB22DialectStrings() {
	std::string PAGINATION_OFFSET = "select * from ( select inner2_.*, rownumber() over(order by order of inner2_) as rownumber_ from ( $query fetch first $count rows only ) as inner2_ ) as inner1_ where rownumber_ > $start order by rownumber_";
	std::string PAGINATION_NO_OFFSET = "$query FETCH FIRST $count ROWS ONLY";
	std::string VALID_DBFUNCS = ",avg,abs,absval,sign,ceiling,ceil,floor,round,acos,asin,atan,cos,cot,"
			"degrees,exp,float,hex,ln,log,log10,radians,rand,sin,soundex,sqrt,stddev,tan,variance,"
			"julian_day,microsecond,midnight_seconds,minute,month,monthname,quarter,hour,second,"
			"current_date,date,day,dayname,dayofweek,dayofweek_iso,dayofyear,days,current_time,time,"
			"current_times,timestamp,timestamp_iso,week,week_iso,year,double,varchar,real,bigint,"
			"char,integer,smallint,digits,chr,upper,lower,ucase,lcase,ltrim,rtrim,substr,posstr,"
			"substring,length,trim,concat,rtrim,";

	dialectStrMap[DB2_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[DB2_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[DB2_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
	dialectStrMap[DB2_DIALECT][IDGEN_SEQUENCE_QUERY] = "values nextval for ${seq_name}";
	dialectStrMap[DB2_DIALECT][IS_TRANSACTION_SUPPORTED] = "TRUE";
	dialectStrMap[DB2_DIALECT][AUTOCOMMIT_QUERY] = "update command options using c on";
}

void DialectHelper::getPaginationSQL(const std::string& dialect, std::string& query, const StringContext& params)
{
	std::string count = StringUtil::trimCopy(params.find("count")->second);
	if(count!="")return;
	if(dialect == SQLSERVER_DIALECT)
	{
		StringUtil::trim(query);
		int selectIndex = StringUtil::toLowerCopy(query).find( "select" );
		int selectDistinctIndex = StringUtil::toLowerCopy(query).find( "select distinct" );
		int insertionPoint = selectIndex + (selectDistinctIndex == selectIndex ? 15 : 6);
		query.insert(insertionPoint, " TOP " + count);
	}
	else if(dialect == TIMESTEN_DIALECT)
	{
		StringUtil::trim(query);
		query.insert(6, " FIRST " + count);
	}
}
