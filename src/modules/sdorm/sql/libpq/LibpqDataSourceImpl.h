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
#include "RequestHandler2.h"
#include "Server.h"
#include <variant>

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

#ifdef HAVE_LIBPQ
typedef void (*LipqCbFunc0) (void* ctx, PGresult* res);
typedef void (*LipqCbFuncF) (void* ctx, bool status, std::vector<PGresult*>* results, const std::string& query, int counter);
#else
typedef void (*LipqCbFunc0) (void* ctx, void* res);
typedef void (*LipqCbFuncF) (void* ctx, bool status, void* results, const std::string& query, int counter);
#endif

typedef void (*LipqCbFunc1) (void* ctx, bool endofdata, int row, int col, char* name, char* value, int vlen);
typedef void (*LipqCbFunc2) (void* ctx, bool endofdata, int row, int col, char* value, int vlen);
typedef void (*LipqCbFunc3) (void* ctx, bool endofdata, int row, int col, char* value);

typedef void (*LipqCbFunc4) (void* ctx, int row, int col, char* name, char* value, int vlen);
typedef void (*LipqCbFunc5) (void* ctx, int row, int col, char* value, int vlen);
typedef void (*LipqCbFunc6) (void* ctx, int row, int col, char* value);

class LibpqQuery {
	std::list<LibpqParam> pvals;
	bool isPrepared;
	bool prepared;
	bool isSelect;
	bool isMulti;
	std::string query;
	LipqCbFunc0 cb0;
	LipqCbFunc1 cb1;
	LipqCbFunc2 cb2;
	LipqCbFunc3 cb3;
	LipqCbFunc4 cb4;
	LipqCbFunc5 cb5;
	LipqCbFunc6 cb6;
	LipqCbFuncF fcb;
	void* ctx;
	int cbType;
	friend class LibpqDataSourceImpl;
	friend class LibpqAsyncReq;
	friend class PgReadTask;
#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
	friend class PgBatchReadTask;
#endif
public:
	void reset();
	LibpqQuery& withSelectQuery(const std::string& query, bool isPrepared = true);
	LibpqQuery& withUpdateQuery(const std::string& query, bool isPrepared = true);
	LibpqQuery& withPrepared();
	LibpqQuery& withContext(void* ctx);
	LibpqQuery& withMulti();//multi-statement non parameterized queries
	template<typename Func1>
	LibpqQuery& withCb0(Func1 cb) {
		this->cb0 = cb;
		this->cbType = 0;
		return *this;
	}
	template<typename Func1>
	LibpqQuery& withCb1(Func1 cb) {
		this->cb1 = cb;
		this->cbType = 1;
		return *this;
	}
	template<typename Func2>
	LibpqQuery& withCb2(Func2 cb) {
		this->cb2 = cb;
		this->cbType = 2;
		return *this;
	}
	template<typename Func3>
	LibpqQuery& withCb3(Func3 cb) {
		this->cb3 = cb;
		this->cbType = 3;
		return *this;
	}
	template<typename Func4>
	LibpqQuery& withCb4(Func4 cb) {
		this->cb4 = cb;
		this->cbType = 4;
		return *this;
	}
	template<typename Func5>
	LibpqQuery& withCb5(Func5 cb) {
		this->cb5 = cb;
		this->cbType = 5;
		return *this;
	}
	template<typename Func6>
	LibpqQuery& withCb6(Func6 cb) {
		this->cb6 = cb;
		this->cbType = 6;
		return *this;
	}
	template<typename FuncF>
	LibpqQuery& withFinalCb(FuncF fcb) {
		this->fcb = fcb;
		return *this;
	}
	LibpqQuery();
	void withParamInt2(unsigned short i);
	void withParamInt4(unsigned int i);
	void withParamInt8(long long i);
	void withParamStr(const char* i);
	void withParamBin(const char *i, size_t len);
};

class LibpqAsyncReq {
	LipqCbFuncF fcb;
	void* ctx;
	int cnt;
#ifdef HAVE_LIBPQ
	std::vector<PGresult*> results;
#else
	std::vector<void*> results;
#endif
	std::deque<LibpqQuery> q;
	friend class LibpqDataSourceImpl;
	friend class PgReadTask;
#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
	friend class PgBatchReadTask;
#endif
	LibpqQuery* peek();
	void pop();
public:
	virtual ~LibpqAsyncReq();
	LibpqAsyncReq();
	LibpqQuery* getQuery();
	template<typename FuncF>
	LibpqAsyncReq& withFinalCb(void* ctx, FuncF fcb) {
		this->ctx = ctx;
		this->fcb = fcb;
		return *this;
	}
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

#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
class PgBatchReadTask : public Task {
protected:
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
	bool isBatch;
	std::deque<LibpqAsyncReq> Q;
	static std::atomic<bool> done;
	ConditionMutex c_mutex;
	std::atomic<bool> cvar;
#ifdef HAVE_LIBPQ
	PGconn* conn; //statement
	PGresult* executeSync(LibpqQuery* q);
#endif
	static void* handle(void* inp);

	LibpqAsyncReq* peek();
	void pop();

	friend class PgReadTask;
	friend class PgBatchReadTask;
public:
	DSType getType();
	LibpqDataSourceImpl(const std::string&, bool isAsync, bool isBatch);
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
	bool begin();
	bool commit();
	bool rollback();
	void executeQuery(LibpqQuery* q);
	void executeMultiQuery(LibpqQuery* q);
	bool executeUpdateQuery(LibpqQuery* q);
	void executeUpdateMultiQuery(LibpqQuery* q);
	//Synchronous mode operations, NOT THREAD SAFE

	//Asynchronous mode operations, NOT THREAD SAFE
	void beginAsync(LibpqAsyncReq* vitem);
	void commitAsync(LibpqAsyncReq* vitem);
	void rollbackAsync(LibpqAsyncReq* vitem);
	LibpqAsyncReq* getAsyncRequest();
	void postAsync(LibpqAsyncReq* vitem);
	void postAsync(LibpqAsyncReq* vitem, int numQ);//post async request with n number of multi queries
	//Asynchronous mode operations, NOT THREAD SAFE

	void handle();
};

#endif /* LibpqDataSourceIMPL_H_ */
