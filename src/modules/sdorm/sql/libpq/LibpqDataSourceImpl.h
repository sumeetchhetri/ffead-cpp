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
	int rows;
	std::string query;
	std::string ps;
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
	friend class FpgWire;
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
	void withParamFloat(double i);
	void withNull();
	void withParamBin(const char *i, size_t len);
	//void withParamStr(std::string& str);
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
	friend class FpgWire;
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
protected:
	LibpqAsyncReq* ritem;
	int counter = 0;
	int type;
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

#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
class PgBatchReadTask : public PgReadTask {
protected:
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

enum FpgReq { Password = 'p', Simple_Query = 'Q', Ext_Parse = 'P', Ext_Bind = 'B', Ext_Describe = 'D', Ext_Execute = 'E', Ext_Flush = 'H', Ext_Sync = 'S' };
enum FpgRes { ParameterStatus = 'S', AuthenticationOk = 'R', ErrorResponse = 'E', ReadyForQuery = 'Z', BackendKeyData = 'K', CommandComplete = 'C', DataRow = 'D', RowDescription = 'T', ParseComplete = '1', BindComplete = '2', PortalSuspended = 's', EmptyQueryResponse = 'I',
    NoticeResponse = 'N', NotificationResponse = 'A' };

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
class FpgWire : public PgReadTask, public BaseSocket {
    int pos;
    std::string upmd5;
    std::string password;
    char state;
    bool pstat;
    bool bstat;
    std::map<int, FpgWireColumnMD> currentMD;
    std::vector<FpgWireRow> rows;
	std::map<std::string, std::string> prepStMap;
	bool querystatus;
	bool isAsync;
    int rowNum;
    int readInt32() {
        int irv =(int)CommonUtils::btn(&buffer[pos], 4);
        pos += 4;
        return irv;
    }
    int readInt16() {
        int irv =(int)CommonUtils::btn(&buffer[pos], 2);
        pos += 2;
        return irv;
    }
    std::string readString(int ml) {
        pos += ml;
        return buffer.substr(pos-ml, ml);
    }
    std::string readString() {
        size_t npos = buffer.find('\0', pos);
        if(npos==std::string::npos) {
            return "";
        }
        int opos = pos;
        pos += (npos-pos+1);
        return buffer.substr(opos, npos-opos);
    }
    void writeString(const std::string& str, std::string& sendBuf) {
        sendBuf.append(str);
        sendBuf.push_back('\0');
    }
    void writeString(const char* buf, size_t len, std::string& sendBuf) {
        sendBuf.append(buf, len);
        sendBuf.push_back('\0');
    }
    void writeStringNn(const char* buf, size_t len, std::string& sendBuf) {
        sendBuf.append(buf, len);
    }
    void writeMsgType(FpgReq type, std::string& sendBuf) {
        sendBuf.push_back(type);
    }
    void writeInt32(int num, std::string& sendBuf) {
        CommonUtils::ntb(sendBuf, num, 4);
    }
    void writeInt16(int num, std::string& sendBuf) {
        CommonUtils::ntb(sendBuf, num, 2);
    }
    void writeChar(char num, std::string& sendBuf) {
         sendBuf.push_back(num);
    }
    void handleResponse() {
		//std::cout << "buffer.size " << buffer.length() << " pos " << pos << std::endl;
		if(buffer.length()==0) {
			std::cout << "No data" << std::endl;
			return;
		}
        switch(buffer[pos++]) {
            case FpgRes::AuthenticationOk: {
                readInt32();
                int is = readInt32();
                if(is==0) {
					if(isAsync) {
						#ifdef OS_MINGW
							u_long iMode = 1;
							ioctlsocket(fd, FIONBIO, &iMode);
						#else
							fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
						#endif
					}
                    std::cout << "logged in successfully" << std::endl;
                } else {
                    std::string req;
                    if(is==3) {
                        std::string rp = password;
                        req.push_back(FpgReq::Password);
                        CommonUtils::ntb(req, 4+rp.length(), 4);
                        rp.push_back('\0');
                        req.append(rp);
                    } else if(is==5) {
                        std::string rp = upmd5;
                        rp.push_back(buffer[pos++]);
                        rp.push_back(buffer[pos++]);
                        rp.push_back(buffer[pos++]);
                        rp.push_back(buffer[pos++]);
                        rp = "md5" + CryptoHandler::md5((unsigned char*)rp.data(), (unsigned int)rp.size());
                        rp.push_back('\0');
                        req.push_back(FpgReq::Password);
                        CommonUtils::ntb(req, 4+rp.length(), 4);
                        req.append(rp);
                    } else {
                        perror("Only plaintext/md5 authentication supported");
                    }
                    
                    if(send(fd, req.c_str(), req.length(), 0)<=0) {
                        perror("Can't send start packet to postgres");
                    }
					char buff[2048];
					int er = recv(fd, buff, 2048, 0);
					if(er<=0) {
                        perror("Can't receive auth packet from postgres");
                    }
					buffer.append(buff, er);
                    //std::cout << "buffer.size " << buffer.length() << " pos " << pos << std::endl;
                }
                break;
            }
            case FpgRes::ErrorResponse: {
                int ml = readInt32();
                std::string err = readString(ml-4);
                std::cout << "error received = " << err << std::endl;
				querystatus = false;
				if(q!=NULL && q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, 0);
				}
                break;
            }
            case FpgRes::ParameterStatus: {
                int ml = readInt32();
                std::string pvstr = readString(ml-4);
                std::string nc;
                nc.push_back('\0');
                std::vector<std::string> pv = StringUtil::splitAndReturn<std::vector<std::string> >(pvstr, nc);
				//std::cout << pv[0] << "=" << pv[1] << std::endl;
                break;
            }
            case FpgRes::BackendKeyData: {
                readInt32();
                int pi = readInt32();
                int sec = readInt32();
                //std::cout << "Cancellation Key Data , process and secret are " << pi << "," << sec << std::endl;
                break;
            }
            case FpgRes::ReadyForQuery: {
                int ml = readInt32();
                std::string bstat = readString(ml-4);
                state = bstat[0];//I - Idle, T - In Trx, E - In Failed Trx, Q - In Query(custom)
                break;
            }
            case FpgRes::RowDescription: {
                currentMD.clear();
                rows.clear();
                rowNum = -1;
                int ml = readInt32();
                int cc = readInt16();
                for(int i=0;i<cc;++i) {
                    FpgWireColumnMD& c = currentMD[i];
                    c.name = readString();//column name
                    c.tabOID = readInt32();//if column belongs to table, then table oid
                    c.indx = readInt16();//if column belongs to table, attribute number of column
                    c.typOID = readInt32();//column type oid
                    c.length = readInt16();//data type size
                    c.mod = readInt32();//data type modifier
                    c.format = readInt16();//zero (text) or one (binary)
                }
                break;
            }
            case FpgRes::DataRow: {
                int ml = readInt32();
                int cols = readInt16();
                /*if(isAsync) {
                    rowNum++;
                    for(int i=0;i<cols;++i) {
                        int cl = readInt32();
                        //cb(rowNum, i, cl, readString(cl), &(currentMD[i]));
                    }
                } else {
                    FpgWireRow& row = rows.emplace_back();
                    for(int i=0;i<cols;++i) {
                        FpgWireColumn& col = row.cols.emplace_back();
                        col.length = readInt32();
                        col.data = readString(col.length);
                        col.md = &(currentMD[i]);
                    }
                    rowNum++;
                }*/
				switch(q->cbType) {
					case 0: {
						//q->cb0(q->ctx, &rows);
						break;
					}
					case 1: {
						for (int j = 0; j < cols; ++j) {
							int length = readInt32();
							q->cb1(q->ctx, false, rowNum++, j, (char*)currentMD[j].name.c_str(), (char*)readString(length).c_str(), length);
						}
						break;
					}
					case 2: {
						for (int j = 0; j < cols; ++j) {
							int length = readInt32();
							q->cb2(q->ctx, false, rowNum++, j, (char*)readString(length).c_str(), length);
						}
						break;
					}
					case 3: {
						for (int j = 0; j < cols; ++j) {
							int length = readInt32();
							q->cb3(q->ctx, false, rowNum++, j, (char*)readString(length).c_str());
						}
						break;
					}
					case 4: {
						for (int j = 0; j < cols; ++j) {
							int length = readInt32();
							q->cb4(q->ctx, rowNum++, j, (char*)currentMD[j].name.c_str(), (char*)readString(length).c_str(), length);
						}
						break;
					}
					case 5: {
						for (int j = 0; j < cols; ++j) {
							int length = readInt32();
							q->cb5(q->ctx, rowNum++, j, (char*)readString(length).c_str(), length);
						}
						break;
					}
					case 6: {
						for (int j = 0; j < cols; ++j) {
							int length = readInt32();
							q->cb3(q->ctx, false, rowNum++, j, (char*)readString(length).c_str());
						}
						break;
					}
					default: {
						FpgWireRow& row = rows.emplace_back();
						for(int i=0;i<cols;++i) {
							FpgWireColumn& col = row.cols.emplace_back();
							col.length = readInt32();
							col.data = readString(col.length);
							col.md = &(currentMD[i]);
						}
						rowNum++;
						break;
					}
				}
            }
            case FpgRes::CommandComplete: {
                int ml = readInt32();
                readString(ml-4);
				querystatus = true;
				if(q!=NULL && q->fcb!=NULL) {
					q->fcb(q->ctx, true, NULL, q->query, 0);
				}
                break;
            }
            case FpgRes::EmptyQueryResponse: {
                readInt32();
				querystatus = true;
				if(q!=NULL && q->fcb!=NULL) {
					q->fcb(q->ctx, true, NULL, q->query, 0);
				}
                break;
            }
            case FpgRes::NoticeResponse: {
                int ml = readInt32();
                readString(ml-4);
                break;
            }
            case FpgRes::NotificationResponse: {
                readInt32();
                readInt32();
                readString();
                readString();
                break;
            }
            case FpgRes::ParseComplete: {
                readInt32();
                pstat = true;
				if(q!=NULL) {
					q->prepared = true;
				}
                break;
            }
            case FpgRes::BindComplete: {
                readInt32();
                bstat = true;
                break;
            }
            default:
                break;
        }
        buffer = buffer.substr(pos);
		pos = 0;
		//std::cout << "buffer.size " << buffer.length() << " pos " << pos << std::endl;
        if(buffer.length()>0) {
            handleResponse();
        }
    }
    
    bool sendStart(std::string user, std::string database) {
        std::string sp, req;
        sp.append("user");
        sp.push_back((char)'\0');
        sp.append(user);
        sp.push_back((char)'\0');
        sp.append("database");
        sp.push_back((char)'\0');
        sp.append(database);
        sp.push_back((char)'\0');
        sp.append("client_encoding");
        sp.push_back((char)'\0');
        sp.append("UTF8");
        sp.push_back((char)'\0');
        sp.append("DateStyle");
        sp.push_back((char)'\0');
        sp.append("ISO");
        sp.push_back((char)'\0');
        sp.push_back((char)'\0');
        CommonUtils::ntb(req, 4+4+sp.length(), 4);
        CommonUtils::ntb(req, FPG_PROTOCOL(3, 0), 4);
        req.append(sp);
        if(send(fd, req.c_str(), req.length(), 0)<=0) {
            perror("Can't send start packet to postgres");
            return false;
        }
		handleSync();
        return state=='I';
    }
    void sendSync() {
		std::string sbuff;
        writeMsgType(FpgReq::Ext_Sync, sbuff);
        writeInt32(4, sbuff);
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send sync query packet to postgres");
            return;
        }
	}
    bool sendPortalDescribe() {
		std::string sbuff;
        writeMsgType(FpgReq::Ext_Describe, sbuff);
        writeInt32(4+1+1, sbuff);
		writeChar('P', sbuff);
		writeString("", sbuff);
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send sync query packet to postgres");
            return false;
        }
		return true;
	}
    void sendPrepStDescribe() {
		std::string sbuff;
        writeMsgType(FpgReq::Ext_Describe, sbuff);
        writeInt32(4+1+1, sbuff);
		writeChar('S', sbuff);
		writeString("", sbuff);
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send sync query packet to postgres");
            return;
        }
	}
    void sendFlush() {
		std::string sbuff;
        writeMsgType(FpgReq::Ext_Flush, sbuff);
        writeInt32(4, sbuff);
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send sync query packet to postgres");
            return;
        }
	}
    bool query(const std::string& q) {
        if(state!='I') return false;
        std::string sbuff;
        writeMsgType(FpgReq::Simple_Query, sbuff);
        writeInt32(4+1+(int)q.length(), sbuff);
        writeString(q, sbuff);
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send simple query packet to postgres");
            return false;
        }
        state = '0';
		querystatus = false;
		sendSync();
        return true;
    }
	bool preparedQuery(LibpqQuery& q) {
		return sendParse(q) && sendBind(q) && sendExecute(q);
	}
    bool sendExecute(LibpqQuery& q) {
		std::string sbuff;
        writeMsgType(FpgReq::Ext_Execute, sbuff);
        writeInt32(4+1+4, sbuff);
		writeString("", sbuff);
		writeInt32(q.rows>0?q.rows:0, sbuff);
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send sync query packet to postgres");
            return false;
        }
        state = '0';
		querystatus = false;
		sendSync();
		return true;
	}
    bool sendParse(LibpqQuery& q) {
        if(state!='I') return false;
		pstat = false;
		std::map<std::string, std::string>::iterator it;
		if((it=prepStMap.find(q.query))!=prepStMap.end()) {
			q.ps = it->second;
			return true;
		}
		q.ps = std::to_string(prepStMap.size()+1);
		prepStMap.insert(std::pair<std::string, std::string>(q.query, q.ps));
        std::string sbuff;
        writeMsgType(FpgReq::Ext_Parse, sbuff);
		int nParams = q.pvals.size();
        writeInt32(4+1+(int)q.query.length()+1+q.ps.length()+2, sbuff);
        writeString(q.ps, sbuff);
		writeString(q.query, sbuff);
		writeInt16(0, sbuff);
		/*if(nParams>=0) {
			for(std::list<LibpqParam>::iterator it=q.pvals.begin(); it != q.pvals.end(); ++it) {
				writeInt32(0, sbuff);
			}
		}*/
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send simple query packet to postgres");
            return false;
        }
        //state = '0';
        return true;
    }
    bool sendBind(LibpqQuery& q) {
        if(state!='I') return false;
		bstat = false;
        std::string sbuff;
        writeMsgType(FpgReq::Ext_Bind, sbuff);
		int pvlen = 0;
		int nParams = q.pvals.size();
		if(nParams>0) {
			for(LibpqParam pt: q.pvals) {
				if(pt.t==1) {//short
					pvlen += 2;
				} else if(pt.t==2) {//int
					pvlen += 4;
				} else if(pt.t==3) {//long
					pvlen += 8;
				} else if(pt.t==6) {//double
					pvlen += 4;
				} else if(pt.t==-1) {//null
					pvlen += 0;
				} else {
					pvlen += pt.sv.l;
				}
			}
			pvlen += 2;//result format code
		}
		std::map<std::string, std::string>::iterator it;
        writeInt32(4+1+1+q.ps.length()+2+2*nParams+2+4*nParams+pvlen+2, sbuff);
        writeString("", sbuff);
        writeString(q.ps, sbuff);
		if(nParams>0) {
			//Write formats first
			writeInt16(nParams, sbuff);
			for(LibpqParam pt: q.pvals) {
				writeInt16(1, sbuff);//binary always?
			}
			//Write numparams now
			writeInt16(nParams, sbuff);
			//Write values
			for(LibpqParam pt: q.pvals) {
				if(pt.t==1) {//short
					writeInt32(2, sbuff);//length
					writeStringNn((char *)&pt.s, 2, sbuff);
				} else if(pt.t==2) {//int
					writeInt32(4, sbuff);//length
					writeStringNn((char *)&pt.i, 4, sbuff);
				} else if(pt.t==3) {//long
					writeInt32(8, sbuff);//length
					writeStringNn((char *)&pt.l, 8, sbuff);
				} else if(pt.t==6) {//double
					writeInt32(4, sbuff);//length
					writeStringNn((char *)&pt.d, 4, sbuff);
				} else if(pt.t==-1) {//null
					writeInt32(-1, sbuff);
				} else {
					writeInt32(pt.sv.l, sbuff);//length
					writeStringNn(pt.sv.p, pt.sv.l, sbuff);
				}
			}
			writeInt16(1, sbuff);
			writeInt16(1, sbuff);
		} else {
			writeInt16(0, sbuff);
			writeInt16(0, sbuff);
			writeInt16(0, sbuff);
		}
        if(send(fd, sbuff.c_str(), sbuff.length(), 0)<=0) {
            perror("Can't send simple query packet to postgres");
            return false;
        }
        //state = '0';
        return true;
    }
protected:
	void run() {

	}
	void submit(LibpqAsyncReq* item);
public:
    FpgWire(SocketInterface* sif, bool isAsync = false): PgReadTask(sif), pos(0), state('0'), pstat(false), bstat(false), rowNum(-1), querystatus(false) {
        this->isAsync = isAsync;
    }
	bool connect(std::string url, bool isAsync) {
		std::vector<std::string> parts = StringUtil::splitAndReturn<std::vector<std::string> >(url, " ");
		std::string host, database, user, password;
		int port = 5432;
		for(std::string part: parts) {
			std::vector<std::string> kv = StringUtil::splitAndReturn<std::vector<std::string> >(part, "=");
			if(StringUtil::toLowerCopy(StringUtil::trimCopy(kv.at(0)))=="host") {
				host = StringUtil::trimCopy(kv.at(1));
			} else if(StringUtil::toLowerCopy(StringUtil::trimCopy(kv.at(0)))=="user") {
				user = StringUtil::trimCopy(kv.at(1));
			} else if(StringUtil::toLowerCopy(StringUtil::trimCopy(kv.at(0)))=="password") {
				password = StringUtil::trimCopy(kv.at(1));
			} else if(StringUtil::toLowerCopy(StringUtil::trimCopy(kv.at(0)))=="dbname") {
				database = StringUtil::trimCopy(kv.at(1));
			} else if(StringUtil::toLowerCopy(StringUtil::trimCopy(kv.at(0)))=="port") {
				port = CastUtil::toInt(StringUtil::trimCopy(kv.at(1)));
			}
		}
		connect(host, port, isAsync, database, user, password);
	}
    bool connect(std::string host, int port, bool isAsync, std::string database, std::string user, std::string password) {
        std::string rp = password + user;
        upmd5 = CryptoHandler::md5((unsigned char*)rp.data(), (unsigned int)rp.size());
        fd = Client::conn(host, port);
        if(fd==-1) return false;
        this->password = password;
		this->isAsync = isAsync;
        return sendStart(user, database);
    }
	void reset() {
		pos = 0; 
		//state = '0';
		pstat = false;
		bstat = false;
		rowNum = -1;
		querystatus = false;
		rows.clear();
		currentMD.clear();
	}
    bool updateQuery(LibpqQuery& q) {
		if(!isAsync) {
			if(!q.isPrepared || q.isMulti) {
				if(!query(q.query)) return false;
			} else {
				if(!preparedQuery(q)) return false;
			}
			state = '0';
			while(state!='I') {
				handleSync();
			}
			return querystatus;
		}
		return true;
	}
    std::vector<FpgWireRow>& selectQuery(LibpqQuery& q) {
		if(!isAsync) {
			if(!q.isPrepared || q.isMulti) {
				if(!query(q.query)) return rows;
			} else {
				if(!preparedQuery(q)) return rows;
			}
			state = '0';
			while(state!='I') {
				handleSync();
			}
		}
		return rows;
	}
    bool handleSync() {
		if(readSync()==0) {
            return true;
        }
        
        handleResponse();
        
        if(isClosed()) {
            return true;
        } else {
            doneRead();
        }
        return false;
    }
    bool handle() {
        if(readFrom()==0) {
            return true;
        }
        
        handleResponse();
        
        if(isClosed()) {
            return true;
        } else {
            doneRead();
        }
        return false;
    }
};

class LibpqDataSourceImpl : public DataSourceType, public SocketInterface {
	std::map<std::string, std::string> prepStmtMap;
	Logger logger;
	std::string url;
	bool isAsync;
	bool isBatch;
	bool isWire;
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
	//friend class PgWireReadTask;
public:
	DSType getType();
	LibpqDataSourceImpl(const std::string&, bool isAsync, bool isBatch, bool isWire = false);
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

	bool handle();
};

#endif /* LibpqDataSourceIMPL_H_ */
