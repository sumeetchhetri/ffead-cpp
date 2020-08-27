/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "LibpqDataSourceImpl.h"

DSType LibpqDataSourceImpl::getType() {
	return SD_RAW_SQLPG;
}

LibpqDataSourceImpl::LibpqDataSourceImpl(const std::string& url) {
	this->url = url;
#ifdef HAVE_LIBPQ
	conn = NULL;
#endif
	trx = false;
}

LibpqDataSourceImpl::~LibpqDataSourceImpl() {
#ifdef HAVE_LIBPQ
	if(conn!=NULL) {
		PQfinish(conn);
	}
#endif
}

bool LibpqDataSourceImpl::init() {
#ifdef HAVE_LIBPQ
	if(conn!=NULL) return true;
	conn = PQconnectdb(url.c_str());
	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		conn = NULL;
		return false;
	}
#endif
	return true;
}

bool LibpqDataSourceImpl::begin() {
#ifdef HAVE_LIBPQ
	PGresult *res = PQexec(conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		printf("BEGIN command failed\n");
		PQclear(res);
		//PQfinish(conn);
		return false;
	}
	PQclear(res);
	trx = true;
#endif
	return true;
}

bool LibpqDataSourceImpl::commit() {
#ifdef HAVE_LIBPQ
	PGresult *res = PQexec(conn, "COMMIT");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		printf("COMMIT command failed\n");
		PQclear(res);
		//PQfinish(conn);
		return false;
	}
	PQclear(res);
	trx = false;
#endif
	return true;
}

bool LibpqDataSourceImpl::rollback() {
#ifdef HAVE_LIBPQ
	PGresult *res = PQexec(conn, "ROLLBACK");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		printf("ROLLBACK command failed\n");
		PQclear(res);
		//PQfinish(conn);
		return false;
	}
	PQclear(res);
	trx = false;
#endif
	return true;
}

void LibpqDataSourceImpl::ADD_INT2(std::vector<LibpqParam>& pvals, unsigned short i, bool isH) {
	pvals.push_back({.p = NULL, .s = (isH?htons(i):i), .i = 0, .li = 0, .l = 2, .t = 1, .b = 1});
}

void LibpqDataSourceImpl::ADD_INT4(std::vector<LibpqParam>& pvals, unsigned int i, bool isH) {
	pvals.push_back({.p = NULL, .s = 0, .i = (isH?htonl(i):i), .li = 0, .l = 4, .t = 2, .b = 1});
}

void LibpqDataSourceImpl::ADD_INT8(std::vector<LibpqParam>& pvals, long long i) {
	pvals.push_back({.p = NULL, .s = 0, .i = 0, .li = i, .l = 8, .t = 3, .b = 1});
}

void LibpqDataSourceImpl::ADD_STR(std::vector<LibpqParam>& pvals, const char *i) {
	pvals.push_back({.p = i, .s = 0, .i = 0, .li = 0, .l = strlen(i), .t = 4, .b = 0});
}

void LibpqDataSourceImpl::ADD_BIN(std::vector<LibpqParam>& pvals, const char *i, int len) {
	pvals.push_back({.p = i, .s = 0, .i = 0, .li = 0, .l = (size_t)len, .t = 5, .b = 1});
}


void LibpqDataSourceImpl::executeQuery(const std::string &query, const std::vector<LibpqParam>& pvals, void* ctx, LipqResFunc cb, bool isPrepared) {
	int psize = (int)pvals.size();
	const char *paramValues[psize];
	int paramLengths[psize];
	int paramBinary[psize];
	for (int var = 0; var < psize; ++var) {
		if(pvals.at(var).t==1) {//short
			paramValues[var] = (char *)&pvals.at(var).s;
			paramLengths[var] = pvals.at(var).l;
		} else if(pvals.at(var).t==2) {//int
			paramValues[var] = (char *)&pvals.at(var).i;
			paramLengths[var] = pvals.at(var).l;
		} else if(pvals.at(var).t==3) {//long
			paramValues[var] = (char *)&pvals.at(var).li;
			paramLengths[var] = pvals.at(var).l;
		} else {
			paramValues[var] = pvals.at(var).p;
			paramLengths[var] = pvals.at(var).l;
		}
		paramBinary[var] = pvals.at(var).b?1:0;
	}
#ifdef HAVE_LIBPQ
	PGresult *res = NULL;
	if(isPrepared) {
		std::string stmtName;
		if(prepStmtMap.find(query)==prepStmtMap.end()) {
			stmtName = CastUtil::fromNumber(prepStmtMap.size()+1);
			prepStmtMap[query] = stmtName;
			res = PQprepare(conn, stmtName.c_str(), query.c_str(), psize, NULL);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "PREPARE failed: %s", PQerrorMessage(conn));
				PQclear(res);
				return;
			}
		} else {
			stmtName = prepStmtMap[query];
		}
		res = PQexecPrepared(conn, stmtName.c_str(), psize, paramValues, paramLengths, paramBinary, 1);
	} else {
		res = PQexecParams(conn, query.c_str(), psize, NULL, paramValues, paramLengths, paramBinary, 1);
	}
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
		PQclear(res);
		return;
	}

	int cols = PQnfields(res);
	int rows = PQntuples(res);
	std::vector<LibpqRes> row;
	for(int i=0; i<rows; i++) {
		row.clear();
		for (int j = 0; j < cols; ++j) {
			row.push_back({.n = PQfname(res, j), .d = PQgetvalue(res, i, j), .l = PQgetlength(res, i, j)});
		}
		cb(ctx, i, row);
	}
	PQclear(res);
#endif
}

bool LibpqDataSourceImpl::executeUpdateQuery(const std::string &query, const std::vector<LibpqParam>& pvals, bool isPrepared) {
	int psize = (int)pvals.size();
	const char *paramValues[psize];
	int paramLengths[psize];
	int paramBinary[psize];
	for (int var = 0; var < psize; ++var) {
		if(pvals.at(var).t==1) {//short
			paramValues[var] = (char *)&pvals.at(var).s;
			paramLengths[var] = pvals.at(var).l;
		} else if(pvals.at(var).t==2) {//int
			paramValues[var] = (char *)&pvals.at(var).i;
			paramLengths[var] = pvals.at(var).l;
		} else if(pvals.at(var).t==3) {//long
			paramValues[var] = (char *)&pvals.at(var).li;
			paramLengths[var] = pvals.at(var).l;
		} else {
			paramValues[var] = pvals.at(var).p;
			paramLengths[var] = pvals.at(var).l;
		}
		paramBinary[var] = pvals.at(var).b?1:0;
	}
#ifdef HAVE_LIBPQ
	PGresult *res = NULL;
	if(isPrepared) {
		std::string stmtName;
		if(prepStmtMap.find(query)==prepStmtMap.end()) {
			stmtName = CastUtil::fromNumber(prepStmtMap.size()+1);
			prepStmtMap[query] = stmtName;
			res = PQprepare(conn, stmtName.c_str(), query.c_str(), psize, NULL);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "PREPARE failed: %s", PQerrorMessage(conn));
				PQclear(res);
				return false;
			}
		} else {
			stmtName = prepStmtMap[query];
		}
		res = PQexecPrepared(conn, stmtName.c_str(), psize, paramValues, paramLengths, paramBinary, 1);
	} else {
		res = PQexecParams(conn, query.c_str(), psize, NULL, paramValues, paramLengths, paramBinary, 1);
	}
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "UPDATE failed: %s", PQerrorMessage(conn));
		PQclear(res);
		return false;
	}
	PQclear(res);
#endif
	return true;
}
