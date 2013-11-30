/*
	Copyright 2010, Sumeet Chhetri

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

int DialectHelper::VALIDDB_FUNCTIONS = 0, DialectHelper::PAGINATION_OFFSET_SQL = 1, DialectHelper::PAGINATION_NO_OFFSET_SQL = 2;
map<string, map<int, string> > DialectHelper::dialectStrMap;
bool DialectHelper::init = false;
string DialectHelper::ORACLE_DIALECT = "OracleDialect", DialectHelper::MYSQL_DIALECT = "MySQLDialect",
	   DialectHelper::POSTGRES_DIALECT = "PostgresDialect", DialectHelper::SQLSERVER_DIALECT = "SQLServerDialect",
	   DialectHelper::SQLSERVER12_DIALECT = "SQLServer12Dialect", DialectHelper::TIMESTEN_DIALECT = "TimestenDialect",
	   DialectHelper::DB2_DIALECT = "DB2Dialect";

DialectHelper::DialectHelper() {
}

string DialectHelper::getSQLString(string dialect, int type, string query, StringContext params) {
	if(dialectStrMap.find(dialect)!=dialectStrMap.end())
	{
		if(dialectStrMap[dialect].find(type)!=dialectStrMap[dialect].end())
		{
			string dquery = dialectStrMap[dialect][type];
			string temp = query;
			StringUtil::trim(temp);
			StringUtil::toLower(temp);
			string fupd;
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
}


void DialectHelper::loadOracleDialectStrings() {
	string PAGINATION_OFFSET = "select * from ( select row_.*, ROWNUM rownum_ from ( $query ) row_ ) where rownum_ <= $count and rownum_ > $start";
	string PAGINATION_NO_OFFSET = "select * from ( $query ) where ROWNUM <= $count";
	string VALID_DBFUNCS = ",abs,sign,acos,asin,atan,cos,cosh,exp,ln,sin,sinh,stddev,sqrt,tan,"
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

	dialectStrMap[TIMESTEN_DIALECT][VALIDDB_FUNCTIONS] = ",upper,rtrim,concat,mod,to_char,to_date,sysdate,getdate,nvl,";
}

void DialectHelper::loadMySQLDialectStrings() {
	string PAGINATION_OFFSET = "$query LIMIT $count";
	string PAGINATION_NO_OFFSET = "$query LIMIT $start OFFSET $count";
	string VALID_DBFUNCS = ",ascii,bin,char_length,character_length,lcase,lower,ltrim,ord,"
			"quote,reverse,rtrim,soundex,space,ucase,upper,unhex,abs,sign,acos,asin,atan,"
			"cos,cot,crc32,exp,ln,log,log2,log10,pi,rand,sin,sqrt,tan,radians,degrees,ceiling,"
			"ceil,floor,round,datediff,timediff,date_format,curdate,curtime,current_date,"
			"current_time,current_timestamp,date,day,dayofmonth,dayname,dayofweek,dayofyear,"
			"from_days,from_unixtime,hour,last_day,localtime,localtimestamp,microseconds,"
			"minute,month,monthname,now,quarter,second,sec_to_time,sysdate,time,timestamp,"
			"time_to_sec,to_days,unix_timestamp,utc_date,utc_time,utc_timestamp,week,weekday,"
			"weekofyear,year,yearweek,hex,oct,octet_length,bit_length,bit_count,encrypt,md5,"
			"sha1,sha,concat,";

	dialectStrMap[MYSQL_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[MYSQL_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[MYSQL_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
}

void DialectHelper::loadPostgresDialectStrings() {
	string PAGINATION_OFFSET = "$query LIMIT $count";
	string PAGINATION_NO_OFFSET = "$query LIMIT $start OFFSET $count";
	string VALID_DBFUNCS = ",ltrim,rtrim,soundex,sysdate,rowid,rownum,instr,lpad,replace,"
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
}

void DialectHelper::loadSQLServerDialectStrings() {
	string PAGINATION_OFFSET = "$query OFFSET $start ROWS FETCH NEXT $count ROWS ONLY";
	string PAGINATION_NO_OFFSET = "$query FETCH FIRST $count ROWS ONLY";
	string VALID_DBFUNCS = ",datepart,locate,mod,datalength,trim,current_timestamp,row_number,";

	dialectStrMap[SQLSERVER12_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[SQLSERVER12_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[SQLSERVER12_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;

	dialectStrMap[SQLSERVER_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
}

void DialectHelper::loadDB22DialectStrings() {
	string PAGINATION_OFFSET = "select * from ( select inner2_.*, rownumber() over(order by order of inner2_) as rownumber_ from ( $query fetch first $count rows only ) as inner2_ ) as inner1_ where rownumber_ > $start order by rownumber_";
	string PAGINATION_NO_OFFSET = "$query FETCH FIRST $count ROWS ONLY";
	string VALID_DBFUNCS = ",avg,abs,absval,sign,ceiling,ceil,floor,round,acos,asin,atan,cos,cot,"
			"degrees,exp,float,hex,ln,log,log10,radians,rand,sin,soundex,sqrt,stddev,tan,variance,"
			"julian_day,microsecond,midnight_seconds,minute,month,monthname,quarter,hour,second,"
			"current_date,date,day,dayname,dayofweek,dayofweek_iso,dayofyear,days,current_time,time,"
			"current_times,timestamp,timestamp_iso,week,week_iso,year,double,varchar,real,bigint,"
			"char,integer,smallint,digits,chr,upper,lower,ucase,lcase,ltrim,rtrim,substr,posstr,"
			"substring,length,trim,concat,rtrim,";

	dialectStrMap[DB2_DIALECT][PAGINATION_OFFSET_SQL] = PAGINATION_OFFSET;
	dialectStrMap[DB2_DIALECT][PAGINATION_NO_OFFSET_SQL] = PAGINATION_NO_OFFSET;
	dialectStrMap[DB2_DIALECT][VALIDDB_FUNCTIONS] = VALID_DBFUNCS;
}

void DialectHelper::getPaginationSQL(string dialect, string& query, StringContext params)
{
	if(dialect == SQLSERVER_DIALECT)
	{
		StringUtil::trim(query);
		query.insert(6, " TOP " + params["count"]);
	}
	else if(dialect == TIMESTEN_DIALECT)
	{
		StringUtil::trim(query);
		query.insert(6, " FIRST " + params["count"]);
	}
}
