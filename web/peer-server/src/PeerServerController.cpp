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
 * PeerServerController.cpp
 *
 *  Created on: Apr 17, 2020
 *      Author: sumeet
 */

#include "PeerServerController.h"

PeerState::PeerState(const std::string& id, const std::string& token, BaseSocket* sif) {
	this->id = id;
	this->sif = sif;
	this->token = token;
}

PeerServerController::PeerServerController() {
	propMap properties = ConfigurationData::getAppProperties();
	if(properties.find("nodeIdentifier")!=properties.end()) {
		nodeIdentifier = properties.find("nodeIdentifier")->second;
	} else {
		nodeIdentifier = generateId();
	}
	if(properties.find("authKey")!=properties.end()) {
		authKey = properties.find("authKey")->second;
	} else {
		authKey = generateId();
	}
	if(properties.find("isDistributedMode")!=properties.end()) {
		isDistributedMode = StringUtil::toLowerCopy(properties.find("isDistributedMode")->second)=="true";
	} else {
		isDistributedMode = false;
	}
}

PeerServerController::~PeerServerController() {
	runHandler = false;
	Thread::sSleep(5);
}

bool PeerServerController::onOpen(WebSocketData* wreq, WebSocketRespponseData* wres, std::string uniqueAddress, void* hreq) {
	HttpRequest* req = (HttpRequest*)hreq;

	std::string id, token, key;

	if(!runHandler) {
		runHandler = true;
		if(isDistributedMode) {
			Thread* peerHandlerTh = new Thread(&handle, this);
			peerHandlerTh->execute();
		}
	}

	if(req!=NULL) {
		if(req->getMethod()=="POST" && req->getContent().length()>0 && req->getHeader(HttpRequest::ContentType).find("application/json")!=std::string::npos) {
			JSONElement el;
			JSONUtil::getDocument(req->getContent(), el);
			JSONElement* tmp = el.getNodeP("key");
			if(tmp!=NULL && (key = StringUtil::trimCopy(tmp->getValue()))!=""){};
			tmp = el.getNodeP("id");
			if(tmp!=NULL && (id = StringUtil::trimCopy(tmp->getValue()))!=""){};
			tmp = el.getNodeP("token");
			if(tmp!=NULL && (token = StringUtil::trimCopy(tmp->getValue()))!=""){};
		}
		if(req->getMethod()=="POST" && req->getContent().length()>0 && req->getHeader(HttpRequest::ContentType).find("application/x-www-form-urlencoded")!=std::string::npos
				&& (id=="" || key=="" || token=="")) {
			struct yuarel_param params[20];
			int np = yuarel_parse_query((char*)req->getContent().c_str(), req->getQueryStr().size(), params, 20);
			for(int i=0;i<np;i++) {
				if(params[i].key_len==3 && strncasecmp("key", params[i].key, 3)==0) {
					key = std::string(params[i].val, params[i].val_len);
				} else if(params[i].key_len==2 && strncasecmp("id", params[i].key, 2)==0) {
					id = std::string(params[i].val, params[i].val_len);
				} else if(params[i].key_len==5 && strncasecmp("token", params[i].key, 5)==0) {
					token = std::string(params[i].val, params[i].val_len);
				}
			}
		}
		if(id=="" || key=="" || token=="") {
			std::string qd = CryptoHandler::urlDecode(std::string(req->getQueryStr()));
			struct yuarel_param params[20];
			int np = yuarel_parse_query((char*)qd.c_str(), qd.size(), params, 20);
			for(int i=0;i<np;i++) {
				if(params[i].key_len==3 && strncasecmp("key", params[i].key, 3)==0) {
					key = std::string(params[i].val, params[i].val_len);
				} else if(params[i].key_len==2 && strncasecmp("id", params[i].key, 2)==0) {
					id = std::string(params[i].val, params[i].val_len);
				} else if(params[i].key_len==5 && strncasecmp("token", params[i].key, 5)==0) {
					token = std::string(params[i].val, params[i].val_len);
				}
			}
		}
	} else if(wreq->getTextData().length()>0) {
		JSONElement el;
		JSONUtil::getDocument(wreq->getTextData(), el);
		std::string type;
		JSONElement* tmp = el.getNodeP("type");
		if(tmp!=NULL && (type = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(type!="OPEN") {
			return false;
		}

		tmp = el.getNodeP("key");
		if(tmp!=NULL && (key = StringUtil::trimCopy(tmp->getValue()))!=""){};
		tmp = el.getNodeP("id");
		if(tmp!=NULL && (id = StringUtil::trimCopy(tmp->getValue()))!=""){};
		tmp = el.getNodeP("token");
		if(tmp!=NULL && (token = StringUtil::trimCopy(tmp->getValue()))!=""){};
	}

	if(authKey!=key) {
		return false;
	}

	if(id=="") {
		return false;
	}
	if(token=="") {
		return false;
	}

	CacheInterface* cchi = CacheManager::getImpl();
	try {
		cchi->set(id, nodeIdentifier);
		CacheManager::cleanImpl(cchi);
	} catch (const std::exception& e) {
		CacheManager::cleanImpl(cchi);
	}

	PeerState* st = new PeerState(id, token, getSif());
	clients.insert(uniqueAddress, st);
	clientIds.insert(id, uniqueAddress);

	wres->pushText("{\"type\": \"OPEN\"}");

	std::cout << "WS:Req:Open " << uniqueAddress << " " << id << " " << token << std::endl;
	return true;
}

void PeerServerController::onClose(std::string uniqueAddress) {
	PeerState* st = NULL;
	if(clients.contains(uniqueAddress)) {
		st = clients.find(uniqueAddress);
		CacheInterface* cchi = CacheManager::getImpl();
		try {
			cchi->remove(st->id);
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		delete st;
		clients.erase(uniqueAddress);
		clientIds.erase(st->id);
		std::cout << "WS:Req:Close " << uniqueAddress << " " << st->id << " " << st->token << std::endl;
	}
}

bool PeerServerController::onMessage(WebSocketData *req, WebSocketRespponseData *res, std::string uniqueAddress) {
	if(req->getTextData().length()>0) {
		//std::cout << "WS:Req: " << req->getTextData() << std::endl;
		JSONElement el;
		JSONUtil::getDocument(req->getTextData(), el);
		std::string type;
		JSONElement* tmp = el.getNodeP("type");
		if(tmp!=NULL && (type = StringUtil::trimCopy(tmp->getValue()))!=""){};
		//std::cout << "WS:Req:Type " << type << std::endl;
		if(type!="OFFER" && type!="CANDIDATE" && type!="ANSWER" && type!="LEAVE" && type!="HEARTBEAT") {
			return false;
		}

		std::string id, dstId;
		PeerState* st = NULL;
		if(clients.contains(uniqueAddress)) {
			st = clients.find(uniqueAddress);
			id = st->id;
		}
		//std::cout << "WS:Req:Id " << id << std::endl;
		if(id=="") {
			return false;
		}

		std::string msgNodeIdentifier;
		CacheInterface* cchi = CacheManager::getImpl();
		try {
			msgNodeIdentifier = cchi->getValue(id);
			//std::cout << "WS:Req:Token " << etoken << std::endl;
			if(msgNodeIdentifier=="") {
				CacheManager::cleanImpl(cchi);
				return false;
			}
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}

		//std::cout << "WS:Req:Id " << id << std::endl;
		if(msgNodeIdentifier=="") {
			return false;
		}

		if(type!="HEARTBEAT") {
			tmp = el.getNodeP("dst");
			if(tmp!=NULL && (dstId = StringUtil::trimCopy(tmp->getValue()))!=""){};
			//std::cout << "WS:Req:Dst " << dstId << std::endl;
			if(dstId=="") {
				//Ignore msg without dst, but close the socket
				return false;
			}
			el.add("src", id);
			std::cout << "WS:Req: " << type << ":For:" << dstId << " " << uniqueAddress << " " << st->id << " " << st->token << std::endl;

			if(clientIds.contains(dstId)) {
				std::string dstUniqAddr = clientIds.find(dstId);
				PeerState* dstSt = clients.find(dstUniqAddr);
				res->pushText(JSONUtil::getDocumentStr(el));
				writeToPeer(res, dstSt->sif);
				std::cout << "WS:Writing: " << type << ":For:" << dstId << " " << uniqueAddress << " " << st->id << " " << st->token << std::endl;
			} else {
				CacheInterface* cchi = CacheManager::getImpl();
				try {
					cchi->addToQ(msgNodeIdentifier, dstId+"|"+JSONUtil::getDocumentStr(el));
					std::cout << "WS:Queuing: " << type << ":For:" << dstId << " " << uniqueAddress << " " << st->id << " " << st->token << std::endl;
					CacheManager::cleanImpl(cchi);
				} catch (const std::exception& e) {
					CacheManager::cleanImpl(cchi);
				}
			}
		}

		return true;
	} else {
		return false;
	}
}

void* PeerServerController::handle(void *inp) {
	PeerServerController* _i = (PeerServerController*)inp;
	while(_i->runHandler) {
		CacheInterface* cchi = CacheManager::getImpl("peer-cached", "peer-server");
		try {
			std::cout << "WS:PeerHandler:Process:Start" << std::endl;
			while(true) {
				std::string v = cchi->getFromQ(_i->nodeIdentifier);
				std::string dstId = v.substr(0, v.find("|"));
				if(v=="" || !_i->runHandler) break;
				if(_i->clientIds.contains(dstId)) {
					std::string uniqueAddress = _i->clientIds.find(dstId);
					PeerState* st = _i->clients.find(uniqueAddress);
					WebSocketRespponseData res;
					res.pushText(v.substr(v.find("|")+1));
					_i->writeToPeer(&res, st->sif);
					std::cout << "WS:PeerHandler:Process:Writing " << " to peer with id " << dstId << " " << uniqueAddress << " " << st->id << " " << st->token << std::endl;
				} else {
					std::cout << "WS:PeerHandler:Process:NoPeerFouond " << " with id " << dstId << std::endl;
				}
			}
			std::cout << "WS:PeerHandler:Process:End" << std::endl;
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		Thread::sSleep(1);
	}
	return NULL;
}

std::string PeerServerController::generateId() {
#ifdef HAVE_UUIDINC
	uuid_t idt;
	uuid_generate(idt);
	std::string ids;
	for(int i=0;i<16;i++){
		ids.push_back(idt[i]);
	}
	return ids;
#elif defined(HAVE_OSSPUUIDINC) || defined(HAVE_OSSPUUIDINC_2)
	uuid_t* idt;
	uuid_rc_t status = uuid_create(&idt);
	std::string ids((const char *) &idt, sizeof(idt));
	uuid_destroy(idt);
	return ids;
#elif HAVE_BSDUUIDINC
	uuid_t idt;
	uint32_t status;
	uuid_create(&idt, &status);
	std::string ids((const char *) &idt, sizeof(idt));
	return ids;
#endif
	return CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
}

bool PeerServerController::isWriteControl() {
	return true;
}

bool PeerServerRouter::auth(std::string_view path, HttpResponse* res, std::string& key, std::string& id, std::string& token) {
	std::vector<std::string> prts;
	StringUtil::split(prts, std::string(path), "/");
	if(prts.size()>=4) {
		key.clear();
		id.clear();
		token.clear();
		token.append(prts.at(prts.size()-2));
		id.append(prts.at(prts.size()-3));
		key.append(prts.at(prts.size()-4));

		if(!isAuthorizedKey(key, res)) {
			return false;
		}

		if(!isValidUser(id, token, res)) {
			return false;
		}

		return true;
	}
	return false;
}

bool PeerServerRouter::isAuthorizedKey(const std::string &key, HttpResponse* res) {
	if(authKey!=key) {
		res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
		return false;
	}
	return true;
}

bool PeerServerRouter::isValidUser(const std::string &id, const std::string &token, HttpResponse *res) {
	CacheInterface* cchi = CacheManager::getImpl();
	try {
		std::string etoken = cchi->getValue(id);
		if(etoken=="" || etoken!=token) {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return false;
		}
		CacheManager::cleanImpl(cchi);
		return true;
	} catch (const std::exception& e) {
		CacheManager::cleanImpl(cchi);
		return false;
	}
}

bool PeerServerRouter::route(HttpRequest* req, HttpResponse* res, BaseSocket* sif) {
	req->normalizeUrl();
	res->setDone(true);
	std::string_view path = req->getPath();
	if(path=="/peer-server/") {
		res->setContent("{\"name\":\"ffead-cpp peerjs server\",\"description\":\"SSE to broker connections between PeerJS clients.\",\"website\":\"http://peerjs.com/\"}");
		res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/id")) {
		res->setContent(PeerServerController::generateId());
		res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/offer")) {
		std::string id, token, key, type, dstId;
		if(auth(path, res, key, id, token)) {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}
		JSONElement el;
		JSONUtil::getDocument(req->getContent(), el);
		JSONElement* tmp = el.getNodeP("type");
		if(tmp!=NULL && (type = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(type!="OFFER") {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}

		tmp = el.getNodeP("dst");
		if(tmp!=NULL && (dstId = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(dstId=="") {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}
		el.add("src", id);

		CacheInterface* cchi = CacheManager::getImpl();
		try {
			cchi->addToQ(dstId+"_q", JSONUtil::getDocumentStr(el));
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/candidate")) {
		std::string id, token, key, type, dstId;
		if(auth(path, res, key, id, token)) {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}
		JSONElement el;
		JSONUtil::getDocument(req->getContent(), el);
		JSONElement* tmp = el.getNodeP("type");
		if(tmp!=NULL && (type = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(type!="CANDIDATE") {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}

		tmp = el.getNodeP("dst");
		if(tmp!=NULL && (dstId = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(dstId=="") {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}
		el.add("src", id);

		CacheInterface* cchi = CacheManager::getImpl();
		try {
			cchi->addToQ(dstId+"_q", JSONUtil::getDocumentStr(el));
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/answer")) {
		std::string id, token, key, type, dstId;
		if(auth(path, res, key, id, token)) {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}
		JSONElement el;
		JSONUtil::getDocument(req->getContent(), el);
		JSONElement* tmp = el.getNodeP("type");
		if(tmp!=NULL && (type = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(type!="ANSWER") {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}

		tmp = el.getNodeP("dst");
		if(tmp!=NULL && (dstId = StringUtil::trimCopy(tmp->getValue()))!=""){};
		if(dstId=="") {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}
		el.add("src", id);

		CacheInterface* cchi = CacheManager::getImpl();
		try {
			cchi->addToQ(dstId+"_q", JSONUtil::getDocumentStr(el));
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/leave")) {
		std::string id, token, key, type;
		if(auth(path, res, key, id, token)) {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}

		CacheInterface* cchi = CacheManager::getImpl();
		try {
			cchi->remove(id);
			cchi->remove(id+"_q");
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		res->addHeaderValue(HttpResponse::Connection, "close");
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/poll")) {
		std::string id, token, key, type;
		if(auth(path, res, key, id, token)) {
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			return true;
		}

		CacheInterface* cchi = CacheManager::getImpl();
		std::string rsc = "[";
		try {
			while(true) {
				std::string v = cchi->getFromQ(id+"_q");
				if(v=="") break;
				rsc += v + ",";
			}
			if(rsc.at(rsc.length()-1)==',') {
				rsc = rsc.substr(0, rsc.length()-1);
			}
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}
		rsc += "]";
		res->setContent(rsc);
		res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	} else if(StringUtil::endsWith(path, "/connect") || StringUtil::endsWith(path, "/peerjs")) {
		std::string id, token, key;

		if(req->getMethod()=="POST" && req->getContent().length()>0 && req->getHeader(HttpRequest::ContentType).find("application/json")!=std::string::npos) {
			JSONElement el;
			JSONUtil::getDocument(req->getContent(), el);
			JSONElement* tmp = el.getNodeP("key");
			if(tmp!=NULL && (key = StringUtil::trimCopy(tmp->getValue()))!=""){};
			tmp = el.getNodeP("id");
			if(tmp!=NULL && (id = StringUtil::trimCopy(tmp->getValue()))!=""){};
			tmp = el.getNodeP("token");
			if(tmp!=NULL && (token = StringUtil::trimCopy(tmp->getValue()))!=""){};
		}
		if(req->getMethod()=="POST" && req->getContent().length()>0 && req->getHeader(HttpRequest::ContentType).find("application/x-www-form-urlencoded")!=std::string::npos
				&& (id=="" || key=="" || token=="")) {
			struct yuarel_param params[20];
			int np = yuarel_parse_query((char*)req->getContent().c_str(), req->getQueryStr().size(), params, 20);
			for(int i=0;i<np;i++) {
				if(params[1].key_len==3 && strncasecmp("key", params[1].key, 3)==0) {
					key = std::string(params[i].val, params[i].val_len);
				} else if(params[1].key_len==2 && strncasecmp("id", params[1].key, 2)==0) {
					id = std::string(params[i].val, params[i].val_len);
				} else if(params[1].key_len==5 && strncasecmp("token", params[1].key, 5)==0) {
					token = std::string(params[i].val, params[i].val_len);
				}
			}
		}
		if(id=="" || key=="" || token=="") {
			std::string qd = CryptoHandler::urlDecode(std::string(req->getQueryStr()));
			struct yuarel_param params[20];
			int np = yuarel_parse_query((char*)qd.c_str(), qd.size(), params, 20);
			for(int i=0;i<np;i++) {
				if(params[i].key_len==3 && strncasecmp("key", params[i].key, 3)==0) {
					key = std::string(params[i].val, params[i].val_len);
				} else if(params[i].key_len==2 && strncasecmp("id", params[i].key, 2)==0) {
					id = std::string(params[i].val, params[i].val_len);
				} else if(params[i].key_len==5 && strncasecmp("token", params[i].key, 5)==0) {
					token = std::string(params[i].val, params[i].val_len);
				}
			}
		}

		if(!isAuthorizedKey(key, res)) {
			return true;
		}

		if(id=="") {
			id = PeerServerController::generateId();
		}
		if(token=="") {
			token = PeerServerController::generateId();
		}

		CacheInterface* cchi = CacheManager::getImpl();
		try {
			cchi->set(id, token);
			CacheManager::cleanImpl(cchi);
		} catch (const std::exception& e) {
			CacheManager::cleanImpl(cchi);
		}

		JSONElement er = JSONElement::object();
		er.add("id", id);
		er.add("token", token);
		res->setContent(JSONUtil::getDocumentStr(er));
		res->setContentType(ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
		res->setHTTPResponseStatus(HTTPResponseStatus::Created);
	} else if(path.find(".")!=std::string::npos) {
		std::ifstream myfile;
		std::string all;
		std::string pubUrlPath = req->getCntxt_root() + "public/";
		if(req->getUrl().find(pubUrlPath)!=0) {
			std::string post = "";
			if(req->getUrl()!=req->getCntxt_root()) {
				post = req->getUrl().substr(req->getCntxt_root().length());
			}
			if(post=="" || post=="/") {
				post = "index.html";
			}
			req->setUrl(pubUrlPath+post);
		}
		myfile.open(req->getUrl(), std::ios::in | std::ios::binary);
		if (myfile.is_open()) {
			std::string content((std::istreambuf_iterator<char>(myfile)), (std::istreambuf_iterator<char>()));
			myfile.close();
			std::string type = CommonUtils::getMimeType(req->getExt());
			res->setContent(content);
			res->setContentType(type);
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		} else {
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
		}
	} else {
		res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
	}
	return true;
}

PeerServerRouter::PeerServerRouter() {
	propMap properties = ConfigurationData::getAppProperties();
	if(properties.find("authKey")!=properties.end()) {
		authKey = properties.find("authKey")->second;
	} else {
		authKey = PeerServerController::generateId();
	}
}

PeerServerRouter::~PeerServerRouter() {
}
