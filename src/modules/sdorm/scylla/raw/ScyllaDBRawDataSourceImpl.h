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
/*
 * ScyllaDBRawDataSourceImpl.h
 *
 *  Created on: 8-May-2021
 *      Author: sumeetc
 */
//NOT THREAD SAFE
#ifndef ScyllaDBRawDataSourceIMPL_H_
#define ScyllaDBRawDataSourceIMPL_H_
#include "Compatibility.h"
#include <stdio.h>
#include <stdlib.h>
#include "cassandra.h"
#include "DataSourceInterface.h"
#include "LoggerFactory.h"

typedef void (*ScyllaCbFunc) (void* arg1, void* arg2);

class ScyllaQuery {
	std::list<LibpqParam> pvals;
	std::string query;
	ScyllaCbFunc cb;
	friend class ScyllaDBRawDataSourceImpl;
public:
	template<typename Func1>
	ScyllaQuery& withCb(Func1 cb) {
		this->cb = cb;
		return *this;
	}
	void withParamInt2(unsigned short i);
	void withParamInt4(unsigned int i);
	void withParamInt8(long long i);
	void withParamStr(const char* i);
	void withParamBin(const char *i, size_t len);
	//void withParamStr(std::string& str);
};

class ScyllaDBRawDataSourceImpl : public DataSourceType {
	Logger logger;
	ConnectionProperties* props;
	CassCluster* cluster;
	CassSession* session;
	void setArgs(CassStatement* statement, std::list<LibpqParam>& pvals);
public:
	DSType getType();
	ScyllaDBRawDataSourceImpl(ConnectionPooler* pool);
	virtual ~ScyllaDBRawDataSourceImpl();
	bool init();
	template<typename Func1>
	void executeQueryAsync(std::string& query, Func1 cb, void* arg) {
		CassStatement* statement = cass_statement_new(query.c_str(), 0);
		CassFuture* select_future = cass_session_execute(session, statement);
		cass_future_set_callback(select_future, cb, arg);
		cass_statement_free(statement);
		cass_future_free(select_future);
	}
	template<typename Func1>
	void executeQuery(std::string& query, Func1 cb, void* arg) {
		CassStatement* statement = cass_statement_new(query.c_str(), 0);
		CassFuture* select_future = cass_session_execute(session, statement);
		if (cass_future_error_code(select_future) == CASS_OK) {
			//CassResult* res = cass_future_get_result(select_future);
			cb(select_future, arg);
			//cass_result_free(res);
		}
		cass_statement_free(statement);
		cass_future_free(select_future);
	}
	template<typename Func1>
	void executeQueryAsync(ScyllaQuery& sq, void* arg) {
		CassStatement* statement = cass_statement_new(sq.query.c_str(), 0);
		if(sq.pvals.size()>0) {
			setArgs(statement, sq.pvals);
		}
		CassFuture* select_future = cass_session_execute(session, statement);
		cass_future_set_callback(select_future, (CassFutureCallback)sq.cb, arg);
		cass_statement_free(statement);
		cass_future_free(select_future);
	}
	template<typename Func1>
	void executeQuery(ScyllaQuery& sq, Func1 cb, void* arg) {
		CassStatement* statement = cass_statement_new(sq.query.c_str(), 0);
		if(sq.pvals.size()>0) {
			setArgs(statement, sq.pvals);
		}
		CassFuture* select_future = cass_session_execute(session, statement);
		if (cass_future_error_code(select_future) == CASS_OK) {
			//CassResult* res = cass_future_get_result(select_future);
			cb(select_future, arg);
			//cass_result_free(res);
		}
		cass_statement_free(statement);
		cass_future_free(select_future);
	}
	template<typename Func1>
	void executeBatchedQuery(ScyllaQuery& sq, Func1 cb, void* arg, std::vector<std::list<LibpqParam>>& arglst) {
		CassFuture* future = cass_session_prepare(session, sq.query.c_str());
		if (cass_future_error_code(future) == CASS_OK) {
			const CassPrepared* prepared = cass_future_get_prepared(future);
			CassBatch* batch = cass_batch_new(CASS_BATCH_TYPE_LOGGED);
			for(std::list<LibpqParam> argrow: arglst) {
				CassStatement* statement = cass_prepared_bind(prepared);
				cass_batch_add_statement(batch, statement);
				setArgs(statement, argrow);
				cass_statement_free(statement);
			}
			cass_batch_free(batch);
			cass_prepared_free(prepared);
		}
		cass_future_free(future);
	}
};

#endif /* ScyllaDBRawDataSourceIMPL_H_ */
