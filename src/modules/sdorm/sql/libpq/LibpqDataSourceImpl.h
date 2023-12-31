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

class FpgIter {
public:
	virtual std::string_view next()=0;
};

#ifdef HAVE_LIBPQ
typedef void (*LipqCbFunc0) (void** ctx, PGresult* res);
typedef void (*LipqCbFuncF) (void** ctx, bool status, std::vector<PGresult*>* results, const std::string& query, int counter);
#else
typedef void (*LipqCbFunc0) (void** ctx, void* res);
typedef void (*LipqCbFuncF) (void** ctx, bool status, void* results, const std::string& query, int counter);
#endif
typedef void (*LipqCbFuncF0) (void** ctx, bool status, const std::string& query, int counter);

typedef void (*LipqCbFunc1) (void** ctx, bool endofdata, int row, int col, char* name, char* value, int vlen);
typedef void (*LipqCbFunc2) (void** ctx, bool endofdata, int row, int col, char* value, int vlen);
typedef void (*LipqCbFunc3) (void** ctx, bool endofdata, int row, int col, char* value);
typedef void (*LipqCbFunc4) (void** ctx, int row, int col, char* name, char* value, int vlen);
typedef void (*LipqCbFunc5) (void** ctx, int row, int col, char* value, int vlen);
typedef void (*LipqCbFunc6) (void** ctx, int row, int col, char* value);
typedef void (*LipqCbFunc7) (void** ctx, int row, FpgIter* iter);



class LibpqQuery {
	std::list<LibpqParam> pvals;
	bool isPrepared;
	bool prepared;
	bool isSelect;
	bool isMulti;
	bool isTrx;
	int rows;
	int pos;
	int mulQCnt;
	std::string query;
	std::string ps;
	/*std::function< void(void*, PGresult*) > cb0;
	std::function< void(void*, bool, int, int, char*, char*, int) > cb1;
	std::function< void(void*, bool, int, int, char*, int) > cb2;
	std::function< void(void*, bool, int, int, char*) > cb3;
	std::function< void(void*, int, int, char*, char*, int) > cb4;
	std::function< void(void*, int, int, char*, int) > cb5;
	std::function< void(void*, int, int, char*) > cb6;
	std::function< void(void*, int, const std::function< std::string_view() > ) > cb7;
	std::function< void(void*, bool, std::vector<PGresult*>*, const std::string&, int) > fcb;
	std::function< void(void*, bool, const std::string&, int) > fcb1;*/
	LipqCbFunc0 cb0;
	LipqCbFunc1 cb1;
	LipqCbFunc2 cb2;
	LipqCbFunc3 cb3;
	LipqCbFunc4 cb4;
	LipqCbFunc5 cb5;
	LipqCbFunc6 cb6;
	LipqCbFunc7 cb7;
	LipqCbFuncF fcb;
	LipqCbFuncF0 fcb1;
	void* ctx[5];
	int cbType;
	friend class LibpqDataSourceImpl;
	friend class LibpqAsyncReq;
	friend class PgReadTask;
#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
	friend class PgBatchReadTask;
#endif
	friend class FpgWire;
public:
	void reset();
	LibpqQuery& withSelectQuery(const std::string& query, bool isPrepared = true);
	LibpqQuery& withUpdateQuery(const std::string& query, bool isPrepared = true);
	LibpqQuery& withPrepared();
	LibpqQuery& withContext(void* ctx, void* ctx1 = NULL, void* ctx2 = NULL, void* ctx3 = NULL, void* ctx4 = NULL);
	LibpqQuery& withMulti(int mulQCnt = 1);//multi-statement non parameterized queries
	template<typename FuncCb0>
	LibpqQuery& withCb0(FuncCb0 cb) {//LibpqQuery& withCb0(std::function< void(void*, PGresult*) > cb) {
		this->cb0 = cb;
		this->cbType = 0;
		return *this;
	}
	template<typename FuncCb1>
	LibpqQuery& withCb1(FuncCb1 cb) {//LibpqQuery& withCb1(std::function< void(void*, bool, int, int, char*, char*, int) > cb) {
		this->cb1 = cb;
		this->cbType = 1;
		return *this;
	}
	template<typename FuncCb2>
	LibpqQuery& withCb2(FuncCb2 cb) {//LibpqQuery& withCb2(std::function< void(void*, bool, int, int, char*, int) > cb) {
		this->cb2 = cb;
		this->cbType = 2;
		return *this;
	}
	template<typename FuncCb3>
	LibpqQuery& withCb3(FuncCb3 cb) {//LibpqQuery& withCb3(std::function< void(void*, bool, int, int, char*) > cb) {
		this->cb3 = cb;
		this->cbType = 3;
		return *this;
	}
	template<typename FuncCb4>
	LibpqQuery& withCb4(FuncCb4 cb) {//LibpqQuery& withCb4(std::function< void(void*, int, int, char*, char*, int) > cb) {
		this->cb4 = cb;
		this->cbType = 4;
		return *this;
	}
	template<typename FuncCb5>
	LibpqQuery& withCb5(FuncCb5 cb) {//LibpqQuery& withCb5(std::function< void(void*, int, int, char*, int) > cb) {
		this->cb5 = cb;
		this->cbType = 5;
		return *this;
	}
	template<typename FuncCb6>
	LibpqQuery& withCb6(FuncCb6 cb) {//LibpqQuery& withCb6(std::function< void(void*, int, int, char*) > cb) {
		this->cb6 = cb;
		this->cbType = 6;
		return *this;
	}
	template<typename FuncCb7>
	LibpqQuery& withCb7(FuncCb7 cb) {//LibpqQuery& withCb7(std::function< void(void*, int, const std::function< std::string_view() > ) > cb) {
		this->cb7 = cb;
		this->cbType = 0;
		return *this;
	}
	template<typename FuncF>
	LibpqQuery& withFinalCb(FuncF cb) {//LibpqQuery& withFinalCb(std::function< void(void*, bool, std::vector<PGresult*>*, const std::string&, int) > cb) {
		this->fcb = cb;
		return *this;
	}
	template<typename FuncF1>
	LibpqQuery& withFinalCb1(FuncF1 cb) {//LibpqQuery& withFinalCb1(std::function< void(void*, bool, const std::string&, int) > cb) {
		this->fcb1 = cb;
		return *this;
	}
	LibpqQuery();
	void withParamInt2(unsigned short i);
	void withParamInt4(unsigned int i);
	void withParamInt8(long long i);
	void withParamStr(const char* i);
	void withParamFloat(double i);
	void withNull();
	void withParamBin(const char *i, size_t len);
	void withMultiQueryCount(const int i);
	//void withParamStr(std::string& str);
};

class LibpqAsyncReq {
	//std::function< void(void*, bool, std::vector<PGresult*>*, const std::string&, int) > fcb;
	//std::function< void(void*, bool, const std::string&, int) > fcb1;
	LipqCbFuncF fcb;
	LipqCbFuncF0 fcb1;
	void* ctx[5];
	bool processed;
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
	friend class FpgWire;
	LibpqQuery* peek();
	void pop();
public:
	virtual ~LibpqAsyncReq();
	LibpqAsyncReq();
	LibpqQuery* getQuery();
	LibpqAsyncReq& withContext(void* ctx, void* ctx1 = NULL, void* ctx2 = NULL, void* ctx3 = NULL, void* ctx4 = NULL);
	template<typename FuncAqCb>
	LibpqAsyncReq& withFinalCb(FuncAqCb cb) {//LibpqAsyncReq& withFinalCb(std::function< void(void*, bool, std::vector<PGresult*>*, const std::string&, int) > fcb) {
		this->fcb = cb;
		return *this;
	}
	template<typename FuncAqCb1>
	LibpqAsyncReq& withFinalCb1(FuncAqCb1 cb) {//LibpqAsyncReq& withFinalCb1(std::function< void(void*, bool, const std::string&, int) > fcb) {
		this->fcb1 = cb;
		return *this;
	}
};

class PgReadTask : public Task {
	friend class FpgWire;
protected:
	LibpqAsyncReq* ritem;
	int type;
	LibpqQuery* q;
	SocketInterface* sif;
	bool flux;
	virtual void run();
	virtual void submit(LibpqAsyncReq* item);
	friend class LibpqDataSourceImpl;
	LibpqAsyncReq* get();
public:
	virtual ~PgReadTask();
	PgReadTask(SocketInterface* sif);
};

#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
class PgBatchReadTask : public PgReadTask {
protected:
	//std::atomic<int> in, out;
	//std::atomic<bool> queueEntries;
	std::atomic<bool> sendBatch;
	//std::deque<LibpqAsyncReq> lQ;
	void run();
	//void processPending();
	void submit(LibpqAsyncReq* item);
	friend class LibpqDataSourceImpl;
	//LibpqAsyncReq* peek();
	//void pop();
	void batchQueries(LibpqAsyncReq* ritem, int& numQueriesInBatch);
	LibpqAsyncReq* get();
public:
	virtual ~PgBatchReadTask();
	PgBatchReadTask(SocketInterface* sif);
};
#endif

enum FpgReq { Password = 'p', Simple_Query = 'Q', Ext_Parse = 'P', Ext_Bind = 'B', Ext_Describe = 'D', Ext_Execute = 'E', Ext_Flush = 'H', Ext_Sync = 'S' };
enum FpgRes { ParameterStatus = 'S', AuthenticationOk = 'R', ErrorResponse = 'E', ReadyForQuery = 'Z', BackendKeyData = 'K', CommandComplete = 'C', DataRow = 'D', RowDescription = 'T', ParseComplete = '1', BindComplete = '2', PortalSuspended = 's', EmptyQueryResponse = 'I',
    NoticeResponse = 'N', NotificationResponse = 'A', Null = '\0' };

class FpgWireColumnMD {
    std::string name;
    int tabOID;
    int indx;
    int typOID;
    int length;
    int mod;
    int format;
    friend class FpgWire;
};
class FpgWireColumn {
    FpgWireColumnMD* md;
    int length;
    std::string data;
    friend class FpgWireRow;
    friend class FpgWire;
};
class FpgWireRow {
    std::vector<FpgWireColumn> cols;
    friend class FpgWire;
};

#define FPG_PROTOCOL(m,n)    (((m) << 16) | (n))
class FpgWire;
class FpgWire : public PgReadTask, public BaseSocket, public FpgIter {
    int pos;
    std::string upmd5;
    std::string password;
    char state;
    bool pstat;
    bool bstat;
    std::map<int, FpgWireColumnMD> currentMD;
    std::vector<FpgWireRow> rows;
	std::map<std::string, std::string> prepStMap;
	int querystatus;
	std::string err;
	bool isAsync;
    int rowNum;
	void* sif;
    char readChar();
    int readInt32();
    int readInt16();
    std::string readString(int ml);
    std::string readString();
    void writeString(const std::string& str, std::string& sendBuf);
    void writeString(const char* buf, size_t len, std::string& sendBuf);
    void writeStringNn(const char* buf, size_t len, std::string& sendBuf);
    void writeMsgType(FpgReq type, std::string& sendBuf);
    void writeInt32(int num, std::string& sendBuf);
    void writeInt16(int num, std::string& sendBuf);
    void writeChar(char num, std::string& sendBuf);
    int handleResponse();
    bool sendStart(std::string user, std::string database);
    void sendSync();
    void sendSync(std::string& sbuff);
    bool sendPortalDescribe();
    void sendPrepStDescribe();
    void sendFlush();
    bool query(const std::string& q);
	bool preparedQuery(LibpqQuery& q);
    bool query(const std::string& q, ResponseData& rd);
	bool preparedQuery(LibpqQuery& q, ResponseData& rd);
    bool sendExecute(LibpqQuery& q, std::string& sbuff);
    bool sendParse(LibpqQuery& q, std::string& sbuff);
    bool sendBind(LibpqQuery& q, std::string& sbuff);
	void checkUnderFlowAndRead(int);
	std::string_view next();
	friend class LibpqDataSourceImpl;
protected:
	void run();
	void submit(LibpqAsyncReq* nitem);
public:
    FpgWire(SocketInterface* sif, bool isAsync = false);
	bool connect(std::string url, bool isAsync, bool isAutoCommit);
    bool connect(std::string host, int port, bool isAsync, std::string database, std::string user, std::string password);
	void reset();
    bool updateQuery(LibpqQuery& q);
    std::vector<FpgWireRow>& selectQuery(LibpqQuery& q);
    bool handleSync();
};

class LibpqDataSourceImpl : public DataSourceType, public SocketInterface {
	std::map<std::string, std::string> prepStmtMap;
	Logger logger;
	std::string url;
	bool isAsync;
	bool isPipelined;
	bool isPipelinedStreamMode; 
	bool isWire;
	bool isAutoCommitMode;
	bool stEvhMode;//seperate event handler thread mode
	std::deque<LibpqAsyncReq> Q;
	static std::atomic<bool> done;
	ConditionMutex c_mutex;
	std::atomic<bool> cvar;
#ifdef HAVE_LIBPQ
	PGconn* conn; 
	PGresult* executeSync(LibpqQuery* q);
#endif
	FpgWire* wire;
	static void* handle(void* inp);

	LibpqAsyncReq* peek();
	void pop();

	friend class PgReadTask;
	friend class PgBatchReadTask;
	friend class FpgWire;
	//friend class PgWireReadTask;
public:
	DSType getType();
	LibpqDataSourceImpl(const ConnectionNode&, const ConnectionProperties&);
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
	//Asynchronous mode operations, NOT THREAD SAFE

	bool handle();
};

#endif /* LibpqDataSourceIMPL_H_ */
