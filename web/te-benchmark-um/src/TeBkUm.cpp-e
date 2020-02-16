/*
 * TeBkUmUm.cpp
 *
 *  Created on: 03-Feb-2020
 *      Author: sumeetc
 */
#include "TeBkUm.h"

int TeBkUmFortune::getId() const {
	return id;
}

void TeBkUmFortune::setId(int id) {
	this->id = id;
}

const std::string& TeBkUmFortune::getMessage() const {
	return message;
}

void TeBkUmFortune::setMessage(const std::string& message) {
	this->message = message;
}

TeBkUmFortune::TeBkUmFortune() {
	id = 0;
}

TeBkUmFortune::~TeBkUmFortune() {
}

bool TeBkUmFortune::operator < (const TeBkUmFortune& other) const {
	return message.compare(other.message)<0;
}

TeBkUmMessage::~TeBkUmMessage() {
}

const std::string& TeBkUmMessage::getMessage() const {
	return message;
}

void TeBkUmMessage::setMessage(const std::string& message) {
	this->message = message;
}

const std::string TeBkUmRestController::HELLO_WORLD = "Hello, World!";
std::string TeBkUmRestController::WORLD = "world";

TeBkUmRestController::TeBkUmRestController() {
}

TeBkUmRestController::~TeBkUmRestController() {
}

TeBkUmMessage TeBkUmRestController::json() {
	TeBkUmMessage msg;
	msg.setMessage(HELLO_WORLD);
	return msg;
}

TeBkUmWorld TeBkUmRestController::db() {
	DataSourceInterface* sqli = DataSourceManager::getImpl();
	int rid = rand() % 10000 + 1;
	GenericObject id;
	id << rid;
	try {
		TeBkUmWorld w = sqli->get<TeBkUmWorld>(id);
		delete sqli;
		return w;
	} catch(const std::exception& e) {
		delete sqli;
		throw e;
	}
}

std::vector<TeBkUmWorld> TeBkUmRestController::queries(std::string queries) {
	std::vector<TeBkUmWorld> wlst;
	int queryCount = 1;
	try {
		queryCount = CastUtil::toInt(queries);
	} catch(const std::exception& e) {
	}
	if(queryCount<1)queryCount=1;
	else if(queryCount>500)queryCount=500;

	DataSourceInterface* sqli = DataSourceManager::getImpl();

	try {
		sqli->startSession(&WORLD);
		for (int c = 0; c < queryCount; ++c) {
			int rid = rand() % 10000 + 1;
			GenericObject id;
			id << rid;
			TeBkUmWorld w = sqli->get<TeBkUmWorld>(id);
			wlst.push_back(w);
		}
		sqli->endSession();
		delete sqli;
		return wlst;
	} catch(const std::exception& e) {
		delete sqli;
		throw e;
	}
}

std::vector<TeBkUmWorld> TeBkUmRestController::updates(std::string queries) {
	std::vector<TeBkUmWorld> wlst;
	int queryCount = 1;
	try {
		queryCount = CastUtil::toInt(queries);
	} catch(const std::exception& e) {
	}
	if(queryCount<1)queryCount=1;
	else if(queryCount>500)queryCount=500;

	DataSourceInterface* sqli = DataSourceManager::getImpl();

	try {
		sqli->startSession(&WORLD);
		for (int c = 0; c < queryCount; ++c) {
			int rid = rand() % 10000 + 1;
			GenericObject id;
			id << rid;
			TeBkUmWorld w = sqli->get<TeBkUmWorld>(id);
			int newRandomNumber = rand() % 10000 + 1;
			if(w.getRandomNumber() == newRandomNumber) {
				newRandomNumber -= 1;
			}
			w.setRandomNumber(newRandomNumber);
			wlst.push_back(w);
		}

		sqli->startTransaction();
		sqli->bulkUpdate<TeBkUmWorld>(wlst);
		sqli->commit();

		sqli->endSession();
		delete sqli;
		return wlst;
	} catch(const std::exception& e) {
		delete sqli;
		throw e;
	}
}

std::string TeBkUmRestController::plaintext() {
	return HELLO_WORLD;
}

void TeBkUmRestController::updateCache() {
	CacheInterface* cchi = CacheManager::getImpl();
	DataSourceInterface* sqli = DataSourceManager::getImpl();

	try {
		sqli->startSession(&WORLD);
		std::vector<TeBkUmWorld> wlist = sqli->getAll<TeBkUmWorld>();
		sqli->endSession();
		for (int c = 0; c < (int)wlist.size(); ++c) {
			TeBkUmWorld& w = wlist.at(c);
			cchi->setO(CastUtil::fromNumber(w.getId()), w);
		}
		delete sqli;
		delete cchi;
	} catch(const std::exception& e) {
		delete sqli;
		delete cchi;
		throw e;
	}
}

std::vector<TeBkUmWorld> TeBkUmRestController::cachedWorlds(std::string count) {
	int queryCount = 1;
	try {
		queryCount = CastUtil::toInt(count);
	} catch(const std::exception& e) {
	}
	if(queryCount<1)queryCount=1;
	else if(queryCount>500)queryCount=500;

	CacheInterface* cchi = CacheManager::getImpl();

	try {
		std::vector<std::string> keys;
		for (int c = 0; c < queryCount; ++c) {
			int rid = rand() % 10000 + 1;
			keys.push_back(CastUtil::fromNumber(rid));
		}

		std::vector<TeBkUmWorld> wlst = cchi->mgetO<TeBkUmWorld>(keys);

		delete cchi;
		return wlst;
	} catch(const std::exception& e) {
		delete cchi;
		throw e;
	}
}

void TeBkUmRestController::getContext(HttpRequest* request, Context* context)
{
	DataSourceInterface* sqli = DataSourceManager::getImpl();

	try {
		std::vector<TeBkUmFortune> flst = sqli->getAll<TeBkUmFortune>();
		for(int i=0;i<(int)flst.size();i++)
		{
			std::string nm = flst.at(i).getMessage();
			CryptoHandler::sanitizeHtml(nm);
			flst.at(i).setMessage(nm);
		}

		TeBkUmFortune nf;
		nf.setId(0);
		nf.setMessage("Additional fortune added at request time.");
		flst.push_back(nf);
		std::sort (flst.begin(), flst.end());
		delete sqli;

		context->insert(std::pair<std::string, GenericObject>("fortunes", GenericObject()));
		context->find("fortunes")->second << flst;
	} catch(...) {
		delete sqli;
		throw;
	}
}

void TebBkUmRouter::route(HttpRequest* req, HttpResponse* res, void* dlib, void* ddlib) {
	Timer t;
	t.start();
	std::string_view path = req->getPath();
	if(StringUtil::endsWith(path, "/fortunes.tpe")) {
		Context ctx;
		rc.getContext(req, &ctx);

		std::string fname = "_fortunestpeemittTemplateHTML";
		void* mkr = dlsym(ddlib, fname.c_str());
		if(mkr!=NULL)
		{
			TeBkUmTemplatePtr f =  (TeBkUmTemplatePtr)mkr;
			std::string msg = f(&ctx);
			res->setContent(msg);
			res->setContentType(ContentTypes::CONTENT_TYPE_TEXT_SHTML);
		}
	} else {
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		if(StringUtil::endsWith(path, "/json")) {
			t.end();
			CommonUtils::tsContRstLkp += t.timerNanoSeconds();
			t.start();
			TeBkUmMessage msg = rc.json();
			t.end();
			CommonUtils::tsContRstExec += t.timerNanoSeconds();
			t.start();
			res->setContent(JSONSerialize::serializeUnknown(&msg, 0, "TeBkUmMessage"));
			res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
			t.end();
			CommonUtils::tsContRstSer += t.timerNanoSeconds();
		} else if(StringUtil::endsWith(path, "/plaintext")) {
			t.end();
			CommonUtils::tsContRstLkp += t.timerNanoSeconds();
			t.start();
			std::string msg = rc.plaintext();
			t.end();
			CommonUtils::tsContRstExec += t.timerNanoSeconds();
			t.start();
			res->setContent(msg);
			res->setContentType(ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			t.end();
			CommonUtils::tsContRstSer += t.timerNanoSeconds();
		} else if(StringUtil::endsWith(path, "/db")) {
			t.end();
			CommonUtils::tsContRstLkp += t.timerNanoSeconds();
			TeBkUmWorld msg = rc.db();
			res->setContent(JSONSerialize::serializeUnknown(&msg, 0, "TeBkUmWorld"));
			res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		} else if(StringUtil::endsWith(path, "/queries")) {
			t.end();
			CommonUtils::tsContRstLkp += t.timerNanoSeconds();
			struct yuarel_param params[1];
			yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
			std::vector<TeBkUmWorld> msg = rc.queries(std::string(params[0].val, params[0].val_len));
			res->setContent(JSONSerialize::serializeUnknown(&msg, 100, "std::vector<TeBkUmWorld>"));
			res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		} else if(StringUtil::endsWith(path, "/updates")) {
			t.end();
			CommonUtils::tsContRstLkp += t.timerNanoSeconds();
			struct yuarel_param params[1];
			yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
			std::vector<TeBkUmWorld> msg = rc.updates(std::string(params[0].val, params[0].val_len));
			res->setContent(JSONSerialize::serializeUnknown(&msg, 100, "std::vector<TeBkUmWorld>"));
			res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		} else if(StringUtil::endsWith(path, "/cached-worlds")) {
			t.end();
			CommonUtils::tsContRstLkp += t.timerNanoSeconds();
			struct yuarel_param params[1];
			yuarel_parse_query((char*)req->getQueryStr().data(), req->getQueryStr().size(), params, 1);
			std::vector<TeBkUmWorld> msg = rc.cachedWorlds(std::string(params[0].val, params[0].val_len));
			res->setContent(JSONSerialize::serializeUnknown(&msg, 100, "std::vector<TeBkUmWorld>"));
			res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		} else {
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
		}
	}
	res->setDone(true);
}
