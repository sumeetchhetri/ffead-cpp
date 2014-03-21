/*
	Copyright 2009-2013, Sumeet Chhetri

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
 * DistoCacheClientUtils.cpp
 *
 *  Created on: 04-Apr-2013
 *      Author: sumeetc
 */

#include "DistoCacheClientUtils.h"

string DistoCacheClientUtils::ERR_NOELEMENTS = "No Elements in container", DistoCacheClientUtils::ERR_NOKEYCACHEMAP = "Unable to find value for Key specified",
	   DistoCacheClientUtils::ERR_INVCONTAINER = "Invalid container specified", DistoCacheClientUtils::ERR_OPNOTSUPP = "Operation not supported on container",
	   DistoCacheClientUtils::ERR_INDGRTCONTSIZ = "Index greater than container size", DistoCacheClientUtils::ERR_NOTAMAPCONT = "Not a map container",
	   DistoCacheClientUtils::ERR_NOVALUEFOUND = "No value found for key", DistoCacheClientUtils::ERR_ALLOCENTEXISTS = "Entry already exists",
	   DistoCacheClientUtils::ERR_NOVALUESPEC = "No value specified", DistoCacheClientUtils::ERR_NEGATIVEPOS = "Position value is less than 0",
	   DistoCacheClientUtils::ERR_POSNOTNUM = "Position value is not a number", DistoCacheClientUtils::ERR_NEGATIVEREP = "Repetition value is less than 0",
	   DistoCacheClientUtils::ERR_REPNOTNUM = "Repetition value is not a number", DistoCacheClientUtils::SUCCESS = "SUCCESS";

DistoCacheClientUtils::DistoCacheClientUtils(string host, int port, bool isSSL) {
	if(isSSL)
	{
		client = new SSLClient;
	}
	else
	{
		client = new Client;
	}

	bool connected = client->connection(host, port);
	if(!connected)
	{
		delete client;
		throw "Error connecting to " + host + ":" + CastUtil::lexical_cast<string>(port);
	}
}

DistoCacheClientUtils::~DistoCacheClientUtils() {
	client->closeConnection();
	delete client;
}

void DistoCacheClientUtils::allocate(string cacheKey, string type) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("allocate "+type+" "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::deallocate(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("deallocate "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::addObjectEntry(string key, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("add objentry "+key);
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::removeObjectEntry(string key) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("remove objentry "+key);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

string DistoCacheClientUtils::getObjectEntryValue(string key) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("get objentry "+key);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

void DistoCacheClientUtils::addMapEntry(string cacheKey, string key, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("add mapentry "+cacheKey + " " + key);
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::removeMapEntry(string cacheKey, string key) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("remove mapentry "+cacheKey+" "+key);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

string DistoCacheClientUtils::getMapEntryValue(string cacheKey, string key) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("get mapentry "+cacheKey+" "+key);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

string DistoCacheClientUtils::getMapEntryValueByPosition(string cacheKey, int position) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("getbypos mapentry "+cacheKey+" "+CastUtil::lexical_cast<string>(position));
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

void DistoCacheClientUtils::setMapEntryValueByPosition(string cacheKey, int position, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("set mapentry "+cacheKey + " " + CastUtil::lexical_cast<string>(position));
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::setCollectionEntryAt(string cacheKey, int position, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("set collentry "+cacheKey + " " + CastUtil::lexical_cast<string>(position));
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::addCollectionEntry(string cacheKey, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("add collentry "+cacheKey);
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::removeCollectionEntryAt(string cacheKey, int position) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("remove collentry "+cacheKey+" "+CastUtil::lexical_cast<string>(position));
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

string DistoCacheClientUtils::getCollectionEntryAt(string cacheKey, int position) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("get collentry "+cacheKey+" "+CastUtil::lexical_cast<string>(position));
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

size_t DistoCacheClientUtils::size(string cacheKey) {
	size_t siz = -1;
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("size "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	if(resp!="")
	{
		try
		{
			siz = CastUtil::lexical_cast<size_t>(resp);
		} catch(...) {
			siz = -1;
		}
	}
	return siz;
}

bool DistoCacheClientUtils::isEmpty(string cacheKey) {
	bool isEmpty = false;
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("isempty "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	if(resp!="")
	{
		try
		{
			isEmpty = CastUtil::lexical_cast<bool>(object->getPackets().at(1)->getValue());
		} catch(...) {
			isEmpty = true;
		}
	}
	return isEmpty;
}

void DistoCacheClientUtils::clear(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("clear "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::insert(string cacheKey, string value, int position) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("insert collentry "+cacheKey+" "+CastUtil::lexical_cast<string>(position));
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::insert(string cacheKey, string value, int position, int repetition) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("insert collentry "+cacheKey+" "+CastUtil::lexical_cast<string>(position)+" "+CastUtil::lexical_cast<string>(repetition));
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::popValueQueue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("pop collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

string DistoCacheClientUtils::popGetValueQueue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("popget collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

void DistoCacheClientUtils::pushBackValue(string cacheKey, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("pushfront collentry "+cacheKey);
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::pushFrontValue(string cacheKey, string value) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("pushback collentry "+cacheKey);
	object->addPacket(value);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::popFrontValue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("popfront collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

void DistoCacheClientUtils::popBackValue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("popback collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;readValueOrThrowExp(object, ignoreErrors);
}

string DistoCacheClientUtils::popGetFrontValue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("popfrontget collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

string DistoCacheClientUtils::popGetBackValue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("popbackget collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

string DistoCacheClientUtils::getFrontValue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("getfront collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

string DistoCacheClientUtils::getBackValue(string cacheKey) {
	AMEFEncoder encoder;
	AMEFDecoder decoder;

	AMEFObject* object = new AMEFObject;
	object->addPacket("getback collentry "+cacheKey);
	lock.lock();
	client->sendData(encoder.encodeB(object, false));
	string resp = client->getBinaryData(4, false);
	lock.unlock();
	delete object;
	object = decoder.decodeB(resp, true, false);
	vector<string> ignoreErrors;resp = readValueOrThrowExp(object, ignoreErrors);
	return resp;
}

string DistoCacheClientUtils::readValueOrThrowExp(AMEFObject* object, vector<string> ignoreErrors)
{
	string exp, resp;
	if(object!=NULL)
	{
		if(object->getPackets().size()>1 && object->getPackets().at(0)->getValue()=="SUCCESS")
		{
			resp = object->getPackets().at(1)->getValue();
		}
		else if(object->getPackets().at(0)->getValue()!="SUCCESS")
		{
			exp = object->getPackets().at(0)->getValue();
		}
		delete object;
	}
	if(exp!="") {
		if(ignoreErrors.size()==0)
			throw exp;
		else
		{
			for(size_t i=0;i<ignoreErrors.size();i++) {
				if(string(exp)==ignoreErrors.at(i))
					return "";
			}
		}
		throw exp;
	} else {
		return resp;
	}
}
