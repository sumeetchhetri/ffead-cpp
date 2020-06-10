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
 * TeBkRestController.cpp
 *
 *  Created on: 11-Mar-2015
 *      Author: sumeetc
 */

#include "TeBkRestController.h"

const std::string TeBkRestController::HELLO_WORLD = "Hello, World!";
std::string TeBkRestController::WORLD = "world";

TeBkRestController::TeBkRestController() {
}

TeBkRestController::~TeBkRestController() {
}

TeBkMessage TeBkRestController::json() {
	TeBkMessage msg;
	msg.setMessage(HELLO_WORLD);
	return msg;
}

TeBkWorld TeBkRestController::db() {
	DataSourceInterface* sqli = DataSourceManager::getImpl();
	int rid = rand() % 10000 + 1;
	GenericObject id;
	id << rid;
	try {
		TeBkWorld w = sqli->get<TeBkWorld>(id);
		delete sqli;
		return w;
	} catch(const std::exception& e) {
		delete sqli;
		throw e;
	}
}

std::vector<TeBkWorld> TeBkRestController::queries(std::string queries) {
	std::vector<TeBkWorld> wlst;
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
			TeBkWorld w = sqli->get<TeBkWorld>(id);
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

std::vector<TeBkWorld> TeBkRestController::updates(std::string queries) {
	std::vector<TeBkWorld> wlst;
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
			TeBkWorld w = sqli->get<TeBkWorld>(id);
			int newRandomNumber = rand() % 10000 + 1;
			if(w.getRandomNumber() == newRandomNumber) {
				newRandomNumber += 1;
				if(newRandomNumber>=10000) {
					newRandomNumber = 1;
				}
			}
			w.setRandomNumber(newRandomNumber);
			wlst.push_back(w);
		}

		sqli->startTransaction();
		sqli->bulkUpdate<TeBkWorld>(wlst);
		sqli->commit();

		sqli->endSession();
		delete sqli;
		return wlst;
	} catch(const std::exception& e) {
		delete sqli;
		throw e;
	}
}

std::string TeBkRestController::plaintext() {
	return HELLO_WORLD;
}

void TeBkRestController::plaintextUM(HttpRequest* req, HttpResponse* res) {
	res->setContent(HELLO_WORLD);
}

void TeBkRestController::updateCache() {
	CacheInterface* cchi = CacheManager::getImpl();
	DataSourceInterface* sqli = DataSourceManager::getImpl();

	try {
		sqli->startSession(&WORLD);
		std::vector<TeBkWorld> wlist = sqli->getAll<TeBkWorld>();
		sqli->endSession();
		for (int c = 0; c < (int)wlist.size(); ++c) {
			TeBkWorld& w = wlist.at(c);
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

std::vector<TeBkWorld> TeBkRestController::cachedWorlds(std::string count) {
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

		std::vector<TeBkWorld> wlst = cchi->mgetO<TeBkWorld>(keys);

		delete cchi;
		return wlst;
	} catch(const std::exception& e) {
		delete cchi;
		throw e;
	}
}
