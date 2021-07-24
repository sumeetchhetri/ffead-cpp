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
 * TeBkUmLpqAsyncUm.cpp
 *
 *  Created on: 03-Feb-2020
 *      Author: sumeetc
 */

#include "TeBkUmLpqAsync.h"

int TeBkUmLpqAsyncWorld::getId() const {
	return id;
}

void TeBkUmLpqAsyncWorld::setId(int id) {
	this->id = id;
}

int TeBkUmLpqAsyncWorld::getRandomNumber() const {
	return randomNumber;
}

void TeBkUmLpqAsyncWorld::setRandomNumber(int randomNumber) {
	this->randomNumber = randomNumber;
}

TeBkUmLpqAsyncWorld::TeBkUmLpqAsyncWorld(int id, int randomNumber) {
	this->id = id;
	this->randomNumber = randomNumber;
}

TeBkUmLpqAsyncWorld::TeBkUmLpqAsyncWorld(int id) {
	this->id = id;
	randomNumber = 0;
}

TeBkUmLpqAsyncWorld::TeBkUmLpqAsyncWorld() {
	id = 0;
	randomNumber = 0;
}

TeBkUmLpqAsyncWorld::~TeBkUmLpqAsyncWorld() {
}

int TeBkUmLpqAsyncFortune::getId() const {
	return id;
}

void TeBkUmLpqAsyncFortune::setId(int id) {
	this->id = id;
}

TeBkUmLpqAsyncFortune::TeBkUmLpqAsyncFortune(int id) {
	this->id = id;
	allocd = false;
}

TeBkUmLpqAsyncFortune::TeBkUmLpqAsyncFortune(int id, std::string message) {
	this->id = id;
	this->message_i = message;
	this->message = std::string_view(this->message_i);
	allocd = false;
}

TeBkUmLpqAsyncFortune::TeBkUmLpqAsyncFortune() {
	id = 0;
	allocd = false;
}

TeBkUmLpqAsyncFortune::~TeBkUmLpqAsyncFortune() {
	if(allocd && message.size()>0) {
		free((void *)message.data());
	}
}

bool TeBkUmLpqAsyncFortune::operator < (const TeBkUmLpqAsyncFortune& other) const {
	return message.compare(other.message)<0;
}

TeBkUmLpqAsyncMessage::TeBkUmLpqAsyncMessage() {
}

TeBkUmLpqAsyncMessage::TeBkUmLpqAsyncMessage(std::string message) {
	this->message = message;
}

TeBkUmLpqAsyncMessage::~TeBkUmLpqAsyncMessage() {
}

const std::string& TeBkUmLpqAsyncMessage::getMessage() const {
	return message;
}

void TeBkUmLpqAsyncMessage::setMessage(const std::string& message) {
	this->message = message;
}

const std::string TeBkUmLpqAsyncRouter::HELLO_WORLD = "Hello, World!";
const std::string TeBkUmLpqAsyncRouter::WORLD = "world";
const std::string TeBkUmLpqAsyncRouter::WORLD_ONE_QUERY = "select id, randomnumber from world where id = $1";
const std::string TeBkUmLpqAsyncRouter::WORLD_ALL_QUERY = "select id, randomnumber from world";
const std::string TeBkUmLpqAsyncRouter::FORTUNE_ALL_QUERY = "select id, message from fortune";
std::unordered_map<int, std::string> TeBkUmLpqAsyncRouter::_qC;
int TeBkUmLpqAsyncRouter::g_seed = 0;

void TeBkUmLpqAsyncRouter::dbAsync(AsyncDbReq* req) {
	LibpqDataSourceImpl* sqli = getDb(5);
	int rid = CommonUtils::fastrand(g_seed) % 10000 + 1;
	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	LibpqQuery* q = areq->getQuery();
	q->withParamInt4(rid);
	q->withSelectQuery(WORLD_ONE_QUERY).withContext(req).withCb3([](void* ctx, bool endofdata, int rn, int cn, char * d) {
		AsyncDbReq* req = (AsyncDbReq*)ctx;
		if(cn==0)req->w.setId(ntohl(*((uint32_t *) d)));
		if(cn==1)req->w.setRandomNumber(ntohl(*((uint32_t *) d)));

		if(endofdata) {
			HttpResponse r;
			JSONSerialize::serializeObject(&req->w, w_ser, r.getContentP());
			std::string h;
			r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, req->conn_clos);
			req->sif->writeDirect(h, r.getContent());
			req->sif->unUse();
			delete req;
		}
	});
	sqli->postAsync(areq);
}

void TeBkUmLpqAsyncRouter::queriesAsync(const char* q, int ql, AsyncQueriesReq* req) {
	int queryCount = 0;
	CommonUtils::fastStrToNum(q, ql, queryCount);
	queryCount = std::max(1, std::min(queryCount, 500));

	req->vec.reserve(queryCount);

	LibpqDataSourceImpl* sqli = getDb(3);
	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	for (int c = 0; c < queryCount; ++c) {
		int rid = CommonUtils::fastrand(g_seed) % 10000 + 1;
		LibpqQuery* q = areq->getQuery();
		q->withParamInt4(rid);
		q->withSelectQuery(WORLD_ONE_QUERY).withContext(req).withCb3([](void* ctx, bool endofdata, int rn, int cn, char * d) {
			AsyncQueriesReq* req = (AsyncQueriesReq*)ctx;
			if(cn==0) {
				req->vec.emplace_back(ntohl(*((uint32_t *) d)));
			} else {
				req->vec.back().setRandomNumber(ntohl(*((uint32_t *) d)));
			}
		});
	}
	areq->withFinalCb(req, [](void *ctx, bool status, const std::string &q, int counter) {
		AsyncQueriesReq* req = (AsyncQueriesReq*)ctx;
		HttpResponse r;
		JSONSerialize::serializeObjectCont(&req->vec, wcont_ser, "vector", r.getContentP());
		std::string h;
		r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, req->conn_clos);
		req->sif->writeDirect(h, r.getContent());
		req->sif->unUse();
		delete req;
	});
	sqli->postAsync(areq);
}

#ifndef HAVE_LIBPQ_BATCH
void TeBkUmLpqAsyncRouter::queriesMultiAsync(const char* q, int ql, AsyncQueriesReq* req) {
	int queryCount = 0;
	CommonUtils::fastStrToNum(q, ql, queryCount);
	queryCount = std::max(1, std::min(queryCount, 500));

	req->vec.reserve(queryCount);

	LibpqDataSourceImpl* sqli = getDb(3);

	std::stringstream ss;
	for (int c = 0; c < queryCount; ++c) {
		int rid = CommonUtils::fastrand(g_seed) % 10000 + 1;
		ss << "select id, randomnumber from world where id = " << rid << ";";
	}

	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	LibpqQuery* qu = areq->getQuery();
	qu->withSelectQuery(ss.str()).withMulti().withContext(req).withCb2([](void* ctx, bool endofdata, int rn, int cn, char * d, int l) {
		AsyncQueriesReq* req = (AsyncQueriesReq*)ctx;
		int tmp = 0;
		CommonUtils::fastStrToNum(d, l, tmp);
		if(cn==0) {
			req->vec.emplace_back(tmp);
		} else {
			req->vec.back().setRandomNumber(tmp);
		}
	});
	areq->withFinalCb(req, [](void *ctx, bool status, const std::string &q, int counter) {
		AsyncQueriesReq* req = (AsyncQueriesReq*)ctx;
		HttpResponse r;
		JSONSerialize::serializeObjectCont(&req->vec, wcont_ser, "vector", r.getContentP());
		std::string h;
		r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, req->conn_clos);
		req->sif->writeDirect(h, r.getContent());
		req->sif->unUse();
		delete req;
	});
	sqli->postAsync(areq, queryCount);
}

void TeBkUmLpqAsyncRouter::updatesMulti(const char* q, int ql, AsyncUpdatesReq* req) {
	int queryCount = 0;
	CommonUtils::fastStrToNum(q, ql, queryCount);
	queryCount = std::max(1, std::min(queryCount, 500));

	req->vec.reserve(queryCount);

	LibpqDataSourceImpl* sqli = getDb(3);
	req->sqli = sqli;

	std::stringstream ss;
	for (int c = 0; c < queryCount; ++c) {
		int rid = CommonUtils::fastrand(g_seed) % 10000 + 1;
		ss << "select id, randomnumber from world where id = " << rid << ";";
	}

	//req->ss << "begin;";//NEVER USE - this creates a deadlock issue (like, DETAIL:  Process 16 waits for ShareLock on transaction 995; blocked by process 19.)
	req->ss << "begin;update world as t set randomnumber = case id ";

	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	LibpqQuery* qu = areq->getQuery();
	qu->withSelectQuery(ss.str()).withMulti().withContext(req).withCb2([](void *ctx, bool endofdata, int rn, int cn, char* d, int l) {
		AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
		int tmp = 0;
		CommonUtils::fastStrToNum(d, l, tmp);
		if(cn==0) {
			req->vec.emplace_back(tmp);
		} else {
			TeBkUmLpqAsyncWorld& w = req->vec.back();
			int newRandomNumber = CommonUtils::fastrand(g_seed) % 10000 + 1;
			if(tmp == newRandomNumber) {
				newRandomNumber += 1;
				if(newRandomNumber>=10000) {
					newRandomNumber = 1;
				}
			}
			w.setRandomNumber(newRandomNumber);
			req->ss << "when ";
			req->ss << w.getId();
			req->ss << " then ";
			req->ss << newRandomNumber;
		}
	});
	areq->withFinalCb(req, [](void *ctx, bool status, const std::string &q, int counter) {
		AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
		if(status) {
			LibpqDataSourceImpl* sqli = req->sqli;
			int queryCount = (int)req->vec.size();

			req->ss << "else randomnumber end where id in (";
			for (int c = 0; c < queryCount; ++c) {
				req->ss << req->vec.at(c).getId();
				if(c<queryCount-1) {
					req->ss << ",";
				}
			}
			req->ss << ");commit;";

			LibpqAsyncReq* areq = sqli->getAsyncRequest();
			LibpqQuery* qu = areq->getQuery();
			qu->withUpdateQuery(req->ss.str()).withMulti();
			areq->withFinalCb(req, [](void *ctx, bool status, const std::string &q, int counter) {
				AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
				if(status) {
					HttpResponse r;
					JSONSerialize::serializeObjectCont(&req->vec, wcont_ser, "vector", r.getContentP());
					std::string h;
					r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, req->conn_clos);
					req->sif->writeDirect(h, r.getContent());
				} else {
					HttpResponse r;
					std::string h;
					r.httpStatus(HTTPResponseStatus::InternalServerError).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, true);
					req->sif->writeDirect(h);
				}
				req->sif->unUse();
				delete req;
			});
			sqli->postAsync(areq, 3);
		}
	});
	sqli->postAsync(areq, queryCount);
}
#endif


std::string& TeBkUmLpqAsyncRouter::getUpdQuery(int count) {
	std::unordered_map<int, std::string>::iterator it = _qC.find(count);
	if(it!=_qC.end()) {
		return it->second;
	}

	std::stringstream ss;
	ss << "update world as t set randomnumber = case id ";

	int pc = 1;
	for (int c = 0; c < count; ++c) {
		ss << "when $";
		ss << pc++;
		ss << " then $";
		ss << pc++;
	}
	ss << "else randomnumber end where id in (";
	for (int c = 0; c < count; ++c) {
		ss << "$" << pc++ << ",";
	}
	std::string q = ss.str();
	q = q.substr(0, q.length()-1);
	q += ")";

	_qC[count] = std::move(q);
	return _qC[count];
}
void TeBkUmLpqAsyncRouter::updatesAsyncb(const char* q, int ql, AsyncUpdatesReq* req) {
	int queryCount = 0;
	CommonUtils::fastStrToNum(q, ql, queryCount);
	queryCount = std::max(1, std::min(queryCount, 500));

	req->vec.reserve(queryCount);

	LibpqDataSourceImpl* sqli = getDb(3);
	req->sqli = sqli;

	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	for (int c = 0; c < queryCount; ++c) {
		int rid = CommonUtils::fastrand(g_seed) % 10000 + 1;
		LibpqQuery* q = areq->getQuery();
		q->withParamInt4(rid);
		q->withSelectQuery(WORLD_ONE_QUERY).withContext(req).withCb3([](void* ctx, bool endofdata, int rn, int cn, char * d) {
			AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
			if(cn==0) {
				req->vec.emplace_back(ntohl(*((uint32_t *) d)));
			} else {
				req->vec.back().setRandomNumber(ntohl(*((uint32_t *) d)));
			}
		});
	}
	areq->withFinalCb(req, [](void *ctx, bool status, const std::string &query, int counter) {
		AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;

		int queryCount = (int)req->vec.size();

		LibpqAsyncReq* areq = req->sqli->getAsyncRequest();
		req->sqli->beginAsync(areq);
		LibpqQuery* q = areq->getQuery();
		q->withUpdateQuery(getUpdQuery(queryCount)).withContext(req);
		for(std::vector<TeBkUmLpqAsyncWorld>::iterator it=req->vec.begin(); it != req->vec.end(); ++it) {
			int newRandomNumber = CommonUtils::fastrand(g_seed) % 10000 + 1;
			if((*it).getRandomNumber() == newRandomNumber) {
				newRandomNumber += 1;
				if(newRandomNumber>=10000) {
					newRandomNumber = 1;
				}
			}
			(*it).setRandomNumber(newRandomNumber);
			q->withParamInt4((*it).getId());
			q->withParamInt4((*it).getRandomNumber());
		}
		for(std::vector<TeBkUmLpqAsyncWorld>::iterator it=req->vec.begin(); it != req->vec.end(); ++it) {
			q->withParamInt4((*it).getId());
		}
		req->sqli->commitAsync(areq);

		areq->withFinalCb(req, [](void* ctx, bool status, const std::string& q, int counter) {
			AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
			HttpResponse r;
			JSONSerialize::serializeObjectCont(&req->vec, wcont_ser, "vector", r.getContentP());
			std::string h;
			r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, req->conn_clos);
			req->sif->writeDirect(h, r.getContent());
			req->sif->unUse();
			delete req;
		});
		req->sqli->postAsync(areq);
	});
	sqli->postAsync(areq);
}

void TeBkUmLpqAsyncRouter::updatesAsync(const char* q, int ql, AsyncUpdatesReq* req) {
	int queryCount = 0;
	CommonUtils::fastStrToNum(q, ql, queryCount);
	queryCount = std::max(1, std::min(queryCount, 500));

	req->vec.reserve(queryCount);

	req->sqli = getDb(3);

	LibpqAsyncReq* areq = req->sqli->getAsyncRequest();
	for (int c = 0; c < queryCount; ++c) {
		int rid = CommonUtils::fastrand(g_seed) % 10000 + 1;
		LibpqQuery* qu = areq->getQuery();
		qu->withParamInt4(rid);
		qu->withSelectQuery(WORLD_ONE_QUERY).withContext(req).withCb2([](void* ctx, bool endofdata, int rn, int cn, char * d, int l) {
			AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
			int tmp = 0;
			CommonUtils::fastStrToNum(d, l, tmp);
			if(cn==0) {
				req->vec.emplace_back(tmp);
			} else {
				req->vec.back().setRandomNumber(tmp);
			}
		});
	}
	areq->withFinalCb(req, [](void *ctx, bool status, const std::string &query, int counter) {
		AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
		LibpqAsyncReq* areq = req->sqli->getAsyncRequest();
		for(std::vector<TeBkUmLpqAsyncWorld>::iterator it=req->vec.begin(); it != req->vec.end(); ++it) {
			int newRandomNumber = CommonUtils::fastrand(g_seed) % 10000 + 1;
			if((*it).getRandomNumber() == newRandomNumber) {
				newRandomNumber += 1;
				if(newRandomNumber>=10000) {
					newRandomNumber = 1;
				}
			}
			(*it).setRandomNumber(newRandomNumber);

			std::stringstream ss;
			ss << "update world set randomnumber = " << newRandomNumber << " where id = " << (*it).getId();

			req->sqli->beginAsync(areq);
			LibpqQuery* q = areq->getQuery();
			q->withUpdateQuery(ss.str(), false);
			req->sqli->commitAsync(areq);
		}

		areq->withFinalCb(req, [](void* ctx, bool status, const std::string& q, int counter) {
			AsyncUpdatesReq* req = (AsyncUpdatesReq*)ctx;
			HttpResponse r;
			JSONSerialize::serializeObjectCont(&req->vec, wcont_ser, "vector", r.getContentP());
			std::string h;
			r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON, req->httpVers, req->conn_clos);
			req->sif->writeDirect(h, r.getContent());
			req->sif->unUse();
			delete req;
		});
		req->sqli->postAsync(areq);
	});
	req->sqli->postAsync(areq);
}

void TeBkUmLpqAsyncRouter::updateCache() {
	LibpqDataSourceImpl* sqli = getDb(1);

	AsyncCacheReq* req = new AsyncCacheReq;
	req->cchi = CacheManager::getImpl();

	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	LibpqQuery* q = areq->getQuery();
	q->withSelectQuery(WORLD_ALL_QUERY).withContext(req).withCb3([](void* ctx, bool endofdata, int row, int col, char* value) {
		AsyncCacheReq* req = (AsyncCacheReq*)ctx;
		if(col==0) {
			req->vec.emplace_back(ntohl(*((uint32_t *) value)));
		} else {
			req->vec.back().setRandomNumber(ntohl(*((uint32_t *) value)));
		}

		if(endofdata) {
			CacheInterface* cchi = req->cchi;
			try {
				for(std::vector<TeBkUmLpqAsyncWorld>::iterator it=req->vec.begin(); it != req->vec.end(); ++it) {
					char str[12];
					sprintf(str, "%d;%d", (*it).getId(), (*it).getRandomNumber());
					cchi->setRaw((*it).getId(), str);
				}
				CacheManager::cleanImpl(cchi);
				delete req;
				CacheManager::triggerAppInitCompletion("te-benchmark-um-pq-async");
			} catch(const std::exception& e) {
				CacheManager::cleanImpl(cchi);
				delete req;
			}
		}
	});
	sqli->postAsync(areq);
}
void TeBkUmLpqAsyncRouter::cachedWorlds(const char* q, int ql, std::vector<TeBkUmLpqAsyncWorld>& wlst) {
	int queryCount = 0;
	CommonUtils::fastStrToNum(q, ql, queryCount);
	queryCount = std::max(1, std::min(queryCount, 500));

	wlst.reserve(queryCount);

	CacheInterface* cchi = CacheManager::getImpl();

	std::vector<unsigned long long> keys;
	for (int c = 0; c < queryCount; ++c) {
		keys.emplace_back(CommonUtils::fastrand(g_seed) % 10000 + 1);
	}
	std::vector<std::string> values;
	cchi->getValues(keys, values);
	for (int c = 0; c < queryCount; ++c) {
		std::string& v = values.at(c);
		size_t fn = v.find(";");
		int tmp = 0;
		CommonUtils::fastStrToNum(v.substr(0, fn).c_str(), fn, tmp);
		int tmp1 = 0;
		CommonUtils::fastStrToNum(v.substr(fn+1).c_str(), v.length()-fn-1, tmp1);
		wlst.emplace_back(tmp, tmp1);
	}
	CacheManager::cleanImpl(cchi);
}


void TeBkUmLpqAsyncRouter::getContextAsync(AsyncFortuneReq* req) {
	LibpqDataSourceImpl* sqli = getDb(7);
	LibpqAsyncReq* areq = sqli->getAsyncRequest();
	LibpqQuery* q = areq->getQuery();
	q->withSelectQuery(FORTUNE_ALL_QUERY).withContext(req).withCb2([](void* ctx, bool endofdata, int rn, int cn, char * d, int l) {
		AsyncFortuneReq* req = (AsyncFortuneReq*)ctx;
		if(cn==0) {
			req->flst.emplace_back(ntohl(*((uint32_t *) d)));
		} else {
			TeBkUmLpqAsyncFortune& w = req->flst.back();
			w.message = CryptoHandler::sanitizeHtmlFast((const uint8_t *)d, (size_t)l, w.message_i, w.allocd);
		}

		if(endofdata) {
			Context context;

			req->flst.emplace_back(0, "Additional fortune added at request time.");
			req->flst.sort();

			context.emplace("fortunes", &req->flst);

			fcpstream str;
			tmplFunc(&context, str);
			std::string out = str.str();
			HttpResponse r;
			std::string h;
			r.httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_TEXT_HTML, req->httpVers, req->conn_clos, (int)out.length());
			req->sif->writeDirect(h, out);
			req->sif->unUse();

			delete req;
		}
	});
	sqli->postAsync(areq);
}

bool TeBkUmLpqAsyncRouter::route(HttpRequest* req, HttpResponse* res, SocketInterface* sif) {
	sif->use();
	if(StringUtil::endsWith(req->getPath(), "/plaintext")) {
		std::string h;
		res->httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_TEXT_PLAIN, (int)HELLO_WORLD.length());
		sif->writeDirect(h, HELLO_WORLD);
		sif->unUse();
	} else if(StringUtil::endsWith(req->getPath(), "/json")) {
		TeBkUmLpqAsyncMessage msg;
		msg.setMessage(HELLO_WORLD);
		JSONSerialize::serializeObject(&msg, m_ser, res->getContentP());
		std::string h;
		res->httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		sif->writeDirect(h, res->getContent());
		sif->unUse();
	} else if(StringUtil::endsWith(req->getPath(), "/db")) {
		AsyncDbReq* ar = new AsyncDbReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		dbAsync(ar);
	} else if(StringUtil::endsWith(req->getPath(), "/queries")) {
		struct yuarel_param params[1];
		yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
		AsyncQueriesReq* ar = new AsyncQueriesReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		queriesAsync(params[0].val, params[0].val_len, ar);
	}
#ifndef HAVE_LIBPQ_BATCH
	else if(StringUtil::endsWith(req->getPath(), "/queriem")) {
		struct yuarel_param params[1];
		yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
		AsyncQueriesReq* ar = new AsyncQueriesReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		queriesMultiAsync(params[0].val, params[0].val_len, ar);
	}
	else if(StringUtil::endsWith(req->getPath(), "/updatem")) {
		struct yuarel_param params[1];
		yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
		AsyncUpdatesReq* ar = new AsyncUpdatesReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		updatesMulti(params[0].val, params[0].val_len, ar);
	}
#endif
	else if(StringUtil::endsWith(req->getPath(), "/fortunes")) {
		AsyncFortuneReq* ar = new AsyncFortuneReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		getContextAsync(ar);
	} else if(StringUtil::endsWith(req->getPath(), "/bupdates") || StringUtil::endsWith(req->getPath(), "/updates")) {
		struct yuarel_param params[1];
		yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
		AsyncUpdatesReq* ar = new AsyncUpdatesReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		updatesAsyncb(params[0].val, params[0].val_len, ar);
	} else if(StringUtil::endsWith(req->getPath(), "/update_")) {
		struct yuarel_param params[1];
		yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
		AsyncUpdatesReq* ar = new AsyncUpdatesReq;
		ar->sif = sif;
		ar->httpVers = req->getHttpVers();
		ar->conn_clos = req->isClose();
		updatesAsync(params[0].val, params[0].val_len, ar);
	} else if(StringUtil::endsWith(req->getPath(), "/cached-worlds")) {
		struct yuarel_param params[1];
		yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
		std::vector<TeBkUmLpqAsyncWorld> msg;
		cachedWorlds(params[0].val, params[0].val_len, msg);
		JSONSerialize::serializeObjectCont(&msg, wcont_ser, "vector", res->getContentP());
		std::string h;
		res->httpStatus(HTTPResponseStatus::Ok).generateHeadResponse(h, ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		sif->writeDirect(h, res->getContent());
		sif->unUse();
	} else {
		std::string h;
		res->httpStatus(HTTPResponseStatus::NotFound).generateHeadResponse(h, req->getHttpVers(), true);
		sif->writeDirect(h);
		sif->unUse();
	}
	return false;
}

TemplatePtr TeBkUmLpqAsyncRouter::tmplFunc;
Ser TeBkUmLpqAsyncRouter::m_ser;
Ser TeBkUmLpqAsyncRouter::w_ser;
SerCont TeBkUmLpqAsyncRouter::wcont_ser;

TeBkUmLpqAsyncRouter::TeBkUmLpqAsyncRouter() {
	sqli = NULL;
	tmplFunc = TemplateUtil::getTemplateFunc("te-benchmark-um-pq-async", "tpe/fortunes.tpe");
	m_ser = Serializer::getSerFuncForObject("te-benchmark-um-pq-async", "TeBkUmLpqAsyncMessage");
	w_ser = Serializer::getSerFuncForObject("te-benchmark-um-pq-async", "TeBkUmLpqAsyncWorld");
	wcont_ser = Serializer::getSerFuncForObjectCont("te-benchmark-um-pq-async", "TeBkUmLpqAsyncWorld", "std::vector");
}

TeBkUmLpqAsyncRouter::~TeBkUmLpqAsyncRouter() {
	if(sqli!=NULL) {
		DataSourceManager::cleanRawImpl(sqli);
	}
}

LibpqDataSourceImpl* TeBkUmLpqAsyncRouter::getDb(int max) {
	if(sqli==NULL) {
		sqli = static_cast<LibpqDataSourceImpl*>(DataSourceManager::getRawImpl("PostgreSQL-DSN", "te-benchmark-um-pq-async"));
	}
	return sqli;
}

LibpqDataSourceImpl* TeBkUmLpqAsyncRouterPooled::getDb(int max) {
	if(max==0) {
		max = maxconns;
	}
	int pc = 0;
	if(inited) {
		pc = ++opt;
		if(pc>=INT_MAX-1) {
			opt = 0;
		}
	} else {
		for (int var = 0; var < maxconns; ++var) {
			pool.push_back(static_cast<LibpqDataSourceImpl*>(DataSourceManager::getRawImpl("PostgreSQL-DSN", "te-benchmark-um-pq-async", true)));
		}
		inited = true;
	}
	return pool.at(pc%max);
}

TeBkUmLpqAsyncRouterPooled::TeBkUmLpqAsyncRouterPooled() {
	maxconns = 7;
	propMap props = ConfigurationData::getAppProperties();
	if(props.size()>0) {
		if(props.find("dbpoolsize")!=props.end()) {
			try {
				maxconns = CastUtil::toInt(props["dbpoolsize"]);
			} catch(...) {
			}
		}
	}
	inited = false;
	opt = 0;
}

TeBkUmLpqAsyncRouterPooled::~TeBkUmLpqAsyncRouterPooled() {
	for(auto sqli: pool) {
		if(sqli!=NULL) {
			DataSourceManager::cleanRawImpl(sqli);
		}
	}
}
