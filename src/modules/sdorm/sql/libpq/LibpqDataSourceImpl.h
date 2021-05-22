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
#include "deque"
#include "list"
#ifdef HAVE_LIBPQ
#include "libpq-fe.h"
#endif
#include "DataSourceInterface.h"
#include "DialectHelper.h"
#include "Thread.h"
#include "SocketInterface.h"
#include "RequestReaderHandler.h"

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

struct _string_view {
	const char* p;
	size_t l;
};

struct LibpqParam {
	union {
		unsigned short s;
		unsigned int i;
		long long l;
		_string_view sv;
	};
	int t;
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

class LibpqQuery {
	std::list<LibpqParam> pvals;
	bool isPrepared;
	bool isSelect;
	bool isMulti;
	std::string query;
	void* ctx;
	LipqResFunc cb;
	LipqColResFunc1 cb1;
	LipqColResFunc2 cb2;
	LipqColResFunc3 cb3;
	LipqComplFunc cb4;
	friend class LibpqDataSourceImpl;
	friend class LibpqAsyncReq;
	friend class PgReadTask;
	void reset();
#ifdef HAVE_LIBPQ_BATCH
	friend class PgBatchReadTask;
#endif
public:
	LibpqQuery& withSelectQuery(const std::string& query, bool isPrepared = false);
	LibpqQuery& withUpdateQuery(const std::string& query, bool isPrepared = false);
	LibpqQuery& withPrepared();
	LibpqQuery& withMulti();//multi statement non parameterized queries
	LibpqQuery& withContext(void* ctx);
	LibpqQuery& withCb(LipqResFunc cb);//for select queries
	LibpqQuery& withCb(LipqColResFunc1 cb1);//for select queries
	LibpqQuery& withCb(LipqColResFunc2 cb2);//for select queries
	LibpqQuery& withCb(LipqColResFunc3 cb3);//for select queries
	LibpqQuery& withCb(LipqComplFunc cmcb);//for update queries
	LibpqQuery();
	void withParamInt2(unsigned short i);
	void withParamInt4(unsigned int i);
	void withParamInt8(long long i);
	void withParamStr(const char* i);
	void withParamBin(const char *i, size_t len);
};

class LibpqAsyncReq {
	void* ctx;
	LipqComplFunc cmcb;
	int cnt;
	std::deque<LibpqQuery> q;
	friend class LibpqDataSourceImpl;
	friend class PgReadTask;
#ifdef HAVE_LIBPQ_BATCH
	friend class PgBatchReadTask;
#endif
	LibpqQuery* peek();
	void pop();
public:
	LibpqQuery* getQuery();
};

class PgReadTask : public Task {
	LibpqAsyncReq* ritem;
	int counter = 0;
	LibpqQuery* q;
	SocketInterface* sif;
	bool flux;
	void run();
	void submit(LibpqAsyncReq* item);
	friend class LibpqDataSourceImpl;
	LibpqAsyncReq* get();
public:
	virtual ~PgReadTask();
	PgReadTask(SocketInterface* sif);
};

class LibpqDataSourceImpl;

#ifdef HAVE_LIBPQ_BATCH
class PgBatchReadTask : public Task {
	LibpqAsyncReq* ritem;
	int counter = 0;
	LibpqQuery* q;
	SocketInterface* sif;
	std::atomic<bool> queueEntries;
	std::atomic<bool> sendBatch;
	std::deque<LibpqAsyncReq> lQ;
	void run();
	void processPending();
	void submit(LibpqAsyncReq* item);
	friend class LibpqDataSourceImpl;
	LibpqAsyncReq* peek();
	void pop();
	void batchQueries(LibpqAsyncReq* ritem, int& numQueriesInBatch);
	LibpqAsyncReq* get();
public:
	virtual ~PgBatchReadTask();
	PgBatchReadTask(SocketInterface* sif);
};
#endif

class LibpqDataSourceImpl : public DataSourceType, public SocketInterface {
	std::map<std::string, std::string> prepStmtMap;
	Logger logger;
	std::string url;
	bool isAsync;
	std::deque<LibpqAsyncReq> Q;
	static std::atomic<bool> done;
	ConditionMutex c_mutex;
	std::atomic<bool> cvar;
	LibpqQuery syncQuery;//only used for synchronous mode operations
#ifdef HAVE_LIBPQ
	PGconn* conn; //statement
	PGresult* executeSync();
#endif
	static void* handle(void* inp);
#ifdef HAVE_LIBPQ_BATCH
	static void* handleBatchReminder(void* inp);
#endif

	LibpqAsyncReq* peek();
	void pop();

	friend class PgReadTask;
	friend class PgBatchReadTask;
public:
	DSType getType();
	LibpqDataSourceImpl(const std::string&, bool isAsync);
	virtual ~LibpqDataSourceImpl();
	//LibpqParamsBase* getParams(int size);

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

	//Synchronous mode operations, NOT THREAD SAFE
	LibpqQuery* getQuery();
	bool begin();
	bool commit();
	bool rollback();
	void executeQuery(LipqComplFunc cmcb = NULL);
	void executeMultiQuery(LipqComplFunc cmcb = NULL);
	bool executeUpdateQuery(LipqComplFunc cmcb = NULL);
	void executeUpdateMultiQuery(LipqComplFunc cmcb = NULL);
	//Synchronous mode operations, NOT THREAD SAFE

	//Asynchronous mode operations, NOT THREAD SAFE
	void beginAsync(LibpqAsyncReq* vitem);
	void commitAsync(LibpqAsyncReq* vitem);
	void rollbackAsync(LibpqAsyncReq* vitem);
	LibpqAsyncReq* getAsyncRequest();
	void postAsync(LibpqAsyncReq* vitem, void* ctx, LipqComplFunc cmcb);
	void postAsync(LibpqAsyncReq* vitem, int numQ, void* ctx, LipqComplFunc cmcb);//post async request with n number of multi queries
	//Asynchronous mode operations, NOT THREAD SAFE
};

#endif /* LibpqDataSourceIMPL_H_ */
