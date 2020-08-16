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

void LibpqDataSourceImpl::executeQuery(const std::string &query, const std::vector<const char*>& pvals, void* ctx, LipqResFunc cb) {
	int psize = (int)pvals.size();
	const char *paramValues[psize];
	for (int var = 0; var < psize; ++var) {
		paramValues[var] = pvals.at(var);
	}
#ifdef HAVE_LIBPQ
	PGresult *res = PQexecParams(conn, query.c_str(), psize, NULL, paramValues, NULL, NULL, 0);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		//printf("No data retrieved\n");
		PQclear(res);
		return;
	}

	int cols = PQnfields(res);
	int rows = PQntuples(res);
	std::vector<LibpqRes> row;
	for(int i=0; i<rows; i++) {
		row.clear();
		for (int j = 0; j < cols; ++j) {
			row.push_back({.d = PQgetvalue(res, i, j), .l = PQgetlength(res, i, j)});
		}
		cb(ctx, i, row);
	}
	PQclear(res);
#endif
}

bool LibpqDataSourceImpl::executeUpdateQuery(const std::string &query, const std::vector<const char*>& pvals) {
#ifdef HAVE_LIBPQ
	int psize = (int)pvals.size();
	const char *paramValues[psize];
	for (int var = 0; var < psize; ++var) {
		paramValues[var] = pvals.at(var);
	}
	PGresult *res = PQexecParams(conn, query.c_str(), psize, NULL, paramValues, NULL, NULL, 0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		//printf("Update query failed\n");
		PQclear(res);
		return false;
	}
	PQclear(res);
#endif
	return true;
}
