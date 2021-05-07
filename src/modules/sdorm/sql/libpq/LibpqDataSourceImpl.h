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
 * LibpqDataSourceImpl.h
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */
//NOT THREAD SAFE
#ifndef LibpqDataSourceIMPL_H_
#define LibpqDataSourceIMPL_H_
#include "Compatibility.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_LIBPQ
#include "libpq-fe.h"
#endif
#include "DataSourceInterface.h"
#include "DialectHelper.h"
#include "Thread.h"
#include "SocketInterface.h"
#include "RequestReaderHandler.h"
#include <stdarg.h>

class LibpqParamsBase {
public:
	virtual int size()=0;
	virtual const char * const * vals()=0;
	virtual const int* lengths()=0;
	virtual const int* formats()=0;
	virtual void int2(unsigned short i, bool isH = true)=0;
	virtual void int4(unsigned int i, bool isH = true)=0;
	virtual void int8(long long i)=0;
	virtual void str(char* i)=0;
	virtual void bin(char* i, int len)=0;
	virtual ~LibpqParamsBase(){}
};

template <int LEN> class LibpqParams : public LibpqParamsBase
{
	const char *paramValues[LEN];
	int paramLengths[LEN];
	int paramBinary[LEN];
	unsigned short _us[LEN];
	unsigned int _ui[LEN];
	long long _ul[LEN];
	int index;
public:
	int size() {return index;}
	const char * const * vals() {return paramValues;}
	const int* lengths() {return paramLengths;}
	const int* formats() {return paramBinary;}
	LibpqParams(): index(0) {}
	void int2(unsigned short i, bool isH = true) {
		if(isH) {
			_us[index] = htons(i);
		} else {
			_us[index] = i;
		}
		paramValues[index] = (char *)&_us[index];
		paramLengths[index] = 2;
		paramBinary[index++] = 1;
	}
	void int4(unsigned int i, bool isH = true) {
		if(isH) {
			_ui[index] = htonl(i);
		} else {
			_ui[index] = i;
		}
		paramValues[index] = (char *)&_ui[index];
		paramLengths[index] = 4;
		paramBinary[index++] = 1;
	}
	void int8(long long i) {
		_ul[index] = i;
		paramValues[index] = (char *)&_ul[index];
		paramLengths[index] = 8;
		paramBinary[index++] = 1;
	}
	void str(char* i) {
		paramValues[index] = (char *)i;
		paramLengths[index] = strlen(i);
		paramBinary[index++] = 1;
	}
	void bin(char* i, int len) {
		paramValues[index] = (char *)i;
		paramLengths[index] = len;
		paramBinary[index++] = 1;
	}
};

struct LibpqParam {
	const char* p;
	uint16_t s;
	uint32_t i;
	long long li;
	size_t l;
	int t;
	bool b;
};

struct LibpqRes {
	const char* n;
	const char* d;
	int l;
};

typedef void (*LipqComplFunc) (void* ctx, bool, const std::string&, int);
typedef void (*LipqResFunc) (void* ctx, int, std::vector<LibpqRes>&);
typedef void (*LipqColResFunc1) (void* ctx, int, int, char *, char *, int);
typedef void (*LipqColResFunc2) (void* ctx, int, int, char *, int);
typedef void (*LipqColResFunc3) (void* ctx, int, int, char *);

struct __AsynQuery {
	std::string query;
	std::vector<LibpqParam> pvals;
	bool isPrepared;
	bool isSelect;
	void* ctx;
	LipqResFunc cb;
	LipqColResFunc1 cb1;
	LipqColResFunc2 cb2;
	LipqColResFunc3 cb3;
	LipqComplFunc cmcb;
};

struct __AsyncReq {
	void* ctx;
	LipqComplFunc cmcb;
	int cnt;
	bool multi;
	std::vector<__AsynQuery*> q;
};

class PgReadTask : public Task {
	__AsyncReq* ritem;
	int counter = 0;
	__AsynQuery* q;
	SocketInterface* sif;
	bool flux;
	void run();
	void submit(__AsyncReq* item);
	friend class LibpqDataSourceImpl;
public:
	virtual ~PgReadTask();
	PgReadTask(SocketInterface* sif);
};

class LibpqDataSourceImpl;

#ifdef HAVE_LIBPQ_BATCH
class PgBatchReadTask : public Task {
	__AsyncReq* ritem;
	int counter = 0;
	__AsynQuery* q;
	SocketInterface* sif;
	std::atomic<bool> queueEntries;
	std::atomic<bool> sendBatch;
	std::queue<__AsyncReq*> lQ;
	void run();
	void processPending();
	void submit(__AsyncReq* item);
	friend class LibpqDataSourceImpl;
	__AsyncReq* getNext();
	void batchQueries(__AsyncReq* ritem, int& numQueriesInBatch);
public:
	virtual ~PgBatchReadTask();
	PgBatchReadTask(SocketInterface* sif);
};
#endif

class LibpqDataSourceImpl : public DataSourceType, public SocketInterface {
	std::map<std::string, std::string> prepStmtMap;
	Logger logger;
	std::string url;
	bool trx;
	bool isAsync;
	std::queue<__AsyncReq*> Q;
	static std::atomic<bool> done;
	ConditionMutex c_mutex;
	std::atomic<bool> cvar;
#ifdef HAVE_LIBPQ
	PGconn* conn; //statement
	PGresult* executeQueryInt(const std::string &query, LibpqParamsBase* pvals, bool isMulti, bool isPrepared);
	void executeQueryIntAsync(const std::string &query, std::vector<LibpqParam>&& pvals, bool isPrepared, int& status,
			void* ctx, LipqResFunc cb, LipqColResFunc1 cb1, LipqColResFunc2 cb2, LipqColResFunc3 cb3, LipqComplFunc cmcb, void* vitem, bool isSelect, __AsyncReq** areq);
#endif
	static void* handle(void* inp);
#ifdef HAVE_LIBPQ_BATCH
	static void* handleBatchReminder(void* inp);
#endif
	__AsyncReq* getNext();
	friend class PgReadTask;
	friend class PgBatchReadTask;
public:
	DSType getType();
	LibpqDataSourceImpl(const std::string&, bool isAsync);
	virtual ~LibpqDataSourceImpl();
	LibpqParamsBase* getParams(int size);

	std::string getProtocol(void* context){return "pgsql";}
	int getType(void* context){return 1;}
	int getTimeout(){return -1;}
	bool readRequest(void* request, void*& context, int& pending, int& reqPos){return true;}
	bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos){return true;}
	void onOpen(){}
	void onClose(){}
	void addHandler(SocketInterface* handler){}
	bool isEmbedded(){return false;}

	bool init();

	bool begin();
	bool commit();
	bool rollback();

	void completeAsync(void* vitem, void* ctx = NULL, LipqComplFunc cmcb = NULL);
	void completeAsync(void* vitem, int numQ);

	void* beginAsync(void* vitem = NULL);
	void* commitAsync(void* vitem = NULL);
	void* rollbackAsync(void* vitem = NULL);

	static void ADD_INT2(std::vector<LibpqParam>& pvals, unsigned short i, bool isH = true);
	static void ADD_INT4(std::vector<LibpqParam>& pvals, unsigned int i, bool isH = true);
	static void ADD_INT8(std::vector<LibpqParam>& pvals, long long i);
	static void ADD_STR(std::vector<LibpqParam>& pvals, const char* i);
	static void ADD_BIN(std::vector<LibpqParam>& pvals, const char* i, int len);

	void executeQuery(const std::string &query, LibpqParamsBase* pvals, void* ctx, LipqResFunc cb, bool isPrepared = true);
	void executeQuery(const std::string &query, LibpqParamsBase* pvals, void* ctx, LipqColResFunc1 cb, bool isPrepared = true);
	void executeQuery(const std::string &query, LibpqParamsBase* pvals, void* ctx, LipqColResFunc2 cb, bool isPrepared = true);
	void executeQuery(const std::string &query, LibpqParamsBase* pvals, void* ctx, LipqColResFunc3 cb, bool isPrepared = true);
	void executeMultiQuery(const std::string &query, void* ctx, LipqResFunc cb, LipqComplFunc cmcb);
	void executeMultiQuery(const std::string &query, void* ctx, LipqColResFunc1 cb, LipqComplFunc cmcb);
	void executeMultiQuery(const std::string &query, void* ctx, LipqColResFunc2 cb, LipqComplFunc cmcb);
	void executeMultiQuery(const std::string &query, void* ctx, LipqColResFunc3 cb, LipqComplFunc cmcb);
	bool executeUpdateQuery(const std::string &query, LibpqParamsBase* pvals, bool isPrepared = true);
	void executeUpdateMultiQuery(const std::string &query, void* ctx, LipqComplFunc cmcb);

	void* executeQueryAsync(const std::string &query, std::vector<LibpqParam>&& pvals, void* ctx, LipqResFunc cb, LipqComplFunc cmcb, void* item, bool isPrepared = true);
	void* executeQueryAsync(const std::string &query, std::vector<LibpqParam>&& pvals, void* ctx, LipqColResFunc1 cb, LipqComplFunc cmcb, void* item, bool isPrepared = true);
	void* executeQueryAsync(const std::string &query, std::vector<LibpqParam>&& pvals, void* ctx, LipqColResFunc2 cb, LipqComplFunc cmcb, void* item, bool isPrepared = true);
	void* executeQueryAsync(const std::string &query, std::vector<LibpqParam>&& pvals, void* ctx, LipqColResFunc3 cb, LipqComplFunc cmcb, void* item, bool isPrepared = true);
	void* executeMultiQueryAsync(const std::string &query, void* ctx, LipqResFunc cb, LipqComplFunc cmcb);
	void* executeMultiQueryAsync(const std::string &query, void* ctx, LipqColResFunc1 cb, LipqComplFunc cmcb);
	void* executeMultiQueryAsync(const std::string &query, void* ctx, LipqColResFunc2 cb, LipqComplFunc cmcb);
	void* executeMultiQueryAsync(const std::string &query, void* ctx, LipqColResFunc3 cb, LipqComplFunc cmcb);
	void* executeUpdateQueryAsync(const std::string &query, std::vector<LibpqParam>&& pvals, void* ctx, LipqComplFunc cmcb, void* item, bool isPrepared = true);
};

#endif /* LibpqDataSourceIMPL_H_ */
