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
 * DistoCacheServiceHandler.cpp
 *
 *  Created on: 27-Mar-2013
 *      Author: sumeetc
 */

#include "DistoCacheServiceHandler.h"

DistoCacheServiceHandler::DistoCacheServiceHandler(int fd, bool isSSLEnabled, SSL_CTX *ctx) {
	this->fd = fd;
	this->isSSLEnabled = isSSLEnabled;
	this->ctx = ctx;
	logger = LoggerFactory::getLogger("DistoCacheServiceHandler");
}

DistoCacheServiceHandler::DistoCacheServiceHandler(int fd) {
	this->fd = fd;
	this->isSSLEnabled = false;
	logger = LoggerFactory::getLogger("DistoCacheServiceHandler");
}

DistoCacheServiceHandler::~DistoCacheServiceHandler() {
}

int DistoCacheServiceHandler::getLength(string header,int size)
{
	int totsize = header[size-1] & 0xff;
	for (int var = 0; var < size-1; var++)
	{
		totsize |= ((header[var] & 0xff) << (size-1-var)*8);
	}
	return totsize;
}

bool DistoCacheServiceHandler::validQuery(vector<string> parts, int size, string cmd1, string cmd2)
{
	bool flag = true;
	flag = (int)parts.size()==size;
	if(!flag)
	{
		return flag;
	}
	for (int var = 0; var < (int)parts.size(); ++var) {
		if(parts.at(var)=="" || (var==0 && cmd1!="" && parts.at(var)!=cmd1)
				|| (var==1 && cmd2!="" && parts.at(var)!=cmd2))
		{
			return false;
		}
	}
	return flag;
}

void DistoCacheServiceHandler::run()
{
	SSL *ssl=NULL;
	BIO *sbio=NULL;
	BIO *io=NULL,*ssl_bio=NULL;

	if(isSSLEnabled)
	{
		sbio=BIO_new_socket(fd,BIO_CLOSE);
		ssl=SSL_new(ctx);
		SSL_set_bio(ssl,sbio,sbio);

		io=BIO_new(BIO_f_buffer());
		ssl_bio=BIO_new(BIO_f_ssl());
		BIO_set_ssl(ssl_bio,ssl,BIO_CLOSE);
		BIO_push(io,ssl_bio);

		int r = SSL_accept(ssl);
		SSL_get_error(ssl,r);
		if(r<=0)
		{
			SSLHandler::getInstance()->closeSSL(fd, ssl, io);
			return;
		}
	}
	while(true)
	{
		int cntlen = 0;
		char buf[MAXBUFLENM];
		string alldat;

		if(isSSLEnabled)
		{
			int er=-1;
			er = BIO_read(io,buf,4);
			switch(SSL_get_error(ssl,er))
			{
				case SSL_ERROR_NONE:
				{
					cntlen -= er;
					break;
				}
				case SSL_ERROR_ZERO_RETURN:
				default:
				{
					SSLHandler::getInstance()->closeSSL(fd, ssl, io);
					return;
				}
			}
			for(int i=0;i<er;i++)
				alldat.push_back(buf[i]);
			memset(&buf[0], 0, sizeof(buf));
		}
		else
		{
			int er = recv(fd, buf, 4, MSG_WAITALL);
			if(er==0)
			{
				close(fd);
				return;
			}
			else if(er>0)
			{
				for(int i=0;i<er;i++)
					alldat.push_back(buf[i]);
				memset(&buf[0], 0, sizeof(buf));
			}
		}

		int lengthm = getLength(alldat,4);

		if(lengthm>10000000)
		{
			if(isSSLEnabled)
			{
				SSLHandler::getInstance()->closeSSL(fd, ssl, io);
			}
			else
			{
				close(fd);
			}
			logger << "WARNING>> Invalid data or someone trying to attack the cache or maybe a simple telnet attempt...ignore..." << endl;
			return;
		}

		if(isSSLEnabled)
		{
			int er=-1;
			while(lengthm>0)
			{
				er = BIO_read(io,buf,lengthm);
				switch(SSL_get_error(ssl,er))
				{
					case SSL_ERROR_NONE:
					{
						lengthm -= er;
						break;
					}
					case SSL_ERROR_ZERO_RETURN:
					default:
					{
						SSLHandler::getInstance()->closeSSL(fd, ssl, io);
						return;
					}
				}

				for(int i=0;i<er;i++)
					alldat.push_back(buf[i]);
				memset(&buf[0], 0, sizeof(buf));
			}
		}
		else
		{
			int er=-1;
			while(lengthm>0)
			{
				er =  recv(fd, buf, lengthm, 0);
				if(er==0)
				{
					close(fd);
					return;
				}
				else if(er>0)
				{
					lengthm -= er;
					for(int i=0;i<er;i++)
					{
						alldat.push_back(buf[i]);
					}
					memset(&buf[0], 0, sizeof(buf));
				}
			}
		}

		string responseMsg = CacheMap::SUCCESS;
		string responseValue;

		try {
			AMEFDecoder decoder;
			AMEFObject* query = decoder.decodeB(alldat,true,false);
			if(query!=NULL && query->getPackets().size()>0)
			{
				string quer = query->getPackets().at(0)->getValue();

				vector<string> parts;
				StringUtil::trim(quer);
				StringUtil::split(parts, quer, (" "));

				if(validQuery(parts, 3, CacheMap::ALLOCATE))
				{
					CacheMap::allocate(parts.at(2), parts.at(1));
				}
				else if(validQuery(parts, 2, CacheMap::DEALLOCATE))
				{
					CacheMap::deallocate(parts.at(1));
				}
				else if(validQuery(parts, 3, CacheMap::ADD, CacheMap::OBJ_ENTRY))
				{
					if(query->getPackets().size()==2)
					{
						string value = query->getPackets().at(1)->getValue();
						CacheMap::addObjectEntry(parts.at(2), value);
					}
					else
					{
						responseMsg = CacheMap::ERR_NOVALUESPEC;
					}
				}
				else if(validQuery(parts, 4, CacheMap::ADD, CacheMap::MAP_ENTRY))
				{
					if(query->getPackets().size()==2)
					{
						string value = query->getPackets().at(1)->getValue();
						CacheMap::addMapEntry(parts.at(2), parts.at(3), value);
					}
					else
					{
						responseMsg = CacheMap::ERR_NOVALUESPEC;
					}
				}
				else if(validQuery(parts, 3, CacheMap::ADD, CacheMap::COLL_ENTRY))
				{
					if(query->getPackets().size()==2)
					{
						string value = query->getPackets().at(1)->getValue();
						CacheMap::addCollectionEntry(parts.at(2), value);
					}
					else
					{
						responseMsg = CacheMap::ERR_NOVALUESPEC;
					}
				}
				else if(validQuery(parts, 3, CacheMap::PUSH_FRONT, CacheMap::COLL_ENTRY))
				{
					if(query->getPackets().size()==2)
					{
						string value = query->getPackets().at(1)->getValue();
						CacheMap::pushFrontValue(parts.at(2), value);
					}
					else
					{
						responseMsg = CacheMap::ERR_NOVALUESPEC;
					}
				}
				else if(validQuery(parts, 3, CacheMap::PUSH_BACK, CacheMap::COLL_ENTRY))
				{
					if(query->getPackets().size()==2)
					{
						string value = query->getPackets().at(1)->getValue();
						CacheMap::pushBackValue(parts.at(2), value);
					}
					else
					{
						responseMsg = CacheMap::ERR_NOVALUESPEC;
					}
				}
				else if(validQuery(parts, 4, CacheMap::INSERT, CacheMap::COLL_ENTRY))
				{
					int position = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && position<size)
						{
							if(query->getPackets().size()==2)
							{
								string value = query->getPackets().at(1)->getValue();
								CacheMap::insert(parts.at(2), value, position);
							}
							else
							{
								responseMsg = CacheMap::ERR_NOVALUESPEC;
							}
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
					} catch(...) {
						responseMsg = CacheMap::ERR_POSNOTNUM;
					}
				}
				else if(validQuery(parts, 5, CacheMap::INSERT, CacheMap::COLL_ENTRY))
				{
					int position = -1, repeats = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						repeats = CastUtil::lexical_cast<int>(parts.at(4));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && repeats>=0 && position<size)
						{
							if(query->getPackets().size()==2)
							{
								string value = query->getPackets().at(1)->getValue();
								CacheMap::insert(parts.at(2), value, position, repeats);
							}
							else
							{
								responseMsg = CacheMap::ERR_NOVALUESPEC;
							}
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else if(position<0)
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEREP;
						}
					} catch(...) {
						if(position==-1)
						{
							responseMsg = CacheMap::CacheMap::ERR_POSNOTNUM;
						}
						else
						{
							responseMsg = CacheMap::CacheMap::ERR_REPNOTNUM;
						}
					}
				}
				else if(validQuery(parts, 4, CacheMap::SET, CacheMap::MAP_ENTRY))
				{
					int position = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && position<size)
						{
							if(query->getPackets().size()==2)
							{
								string value = query->getPackets().at(1)->getValue();
								CacheMap::setMapEntryValueByPosition(parts.at(2), position, value);
							}
							else
							{
								responseMsg = CacheMap::ERR_NOVALUESPEC;
							}
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
					} catch(...) {
						responseMsg = CacheMap::ERR_POSNOTNUM;
					}
				}
				else if(validQuery(parts, 4, CacheMap::SET, CacheMap::COLL_ENTRY))
				{
					int position = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && position<size)
						{
							if(query->getPackets().size()==2)
							{
								string value = query->getPackets().at(1)->getValue();
								CacheMap::setCollectionEntryAt(parts.at(2), position, value);
							}
							else
							{
								responseMsg = CacheMap::ERR_NOVALUESPEC;
							}
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
					} catch(...) {
						responseMsg = CacheMap::ERR_POSNOTNUM;
					}
				}
				else if(validQuery(parts, 3, CacheMap::GET, CacheMap::OBJ_ENTRY))
				{
					responseValue = CacheMap::getObjectEntryValue(parts.at(2));
				}
				else if(validQuery(parts, 4, CacheMap::GET, CacheMap::MAP_ENTRY))
				{
					responseValue = CacheMap::getMapEntryValue(parts.at(2), parts.at(3));
				}
				else if(validQuery(parts, 4, CacheMap::GETBYPOS, CacheMap::MAP_ENTRY))
				{
					int position = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && position<size)
						{
							responseValue = CacheMap::getMapEntryValueByPosition(parts.at(2), position);
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
					} catch(...) {
						responseMsg = CacheMap::ERR_POSNOTNUM;
					}
				}
				else if(validQuery(parts, 4, CacheMap::GET, CacheMap::COLL_ENTRY))
				{
					int position = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && position<size)
						{
							responseValue = CacheMap::getCollectionEntryAt(parts.at(2), position);
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
					} catch(...) {
						responseMsg = CacheMap::ERR_POSNOTNUM;
					}
				}
				else if(validQuery(parts, 3, CacheMap::GET_FRONT, CacheMap::COLL_ENTRY))
				{
					responseValue = CacheMap::getFrontValue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::GET_BACK, CacheMap::COLL_ENTRY))
				{
					responseValue = CacheMap::getBackValue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::POPGET, CacheMap::COLL_ENTRY))
				{
					responseValue = CacheMap::popGetValueQueue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::POP_FRONTGET, CacheMap::COLL_ENTRY))
				{
					responseValue = CacheMap::popGetFrontValue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::POP_BACKGET, CacheMap::COLL_ENTRY))
				{
					responseValue = CacheMap::popGetBackValue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::REMOVE, CacheMap::OBJ_ENTRY))
				{
					CacheMap::removeObjectEntry(parts.at(2));
				}
				else if(validQuery(parts, 4, CacheMap::REMOVE, CacheMap::MAP_ENTRY))
				{
					CacheMap::removeMapEntry(parts.at(2), parts.at(3));
				}
				else if(validQuery(parts, 4, CacheMap::REMOVE, CacheMap::COLL_ENTRY))
				{
					int position = -1;
					try {
						position = CastUtil::lexical_cast<int>(parts.at(3));
						int size = CacheMap::size(parts.at(2));
						if(position>=0 && position<size)
						{
							CacheMap::removeCollectionEntryAt(parts.at(2), position);
						}
						else if(size<=position)
						{
							responseMsg = CacheMap::ERR_INDGRTCONTSIZ;
						}
						else
						{
							responseMsg = CacheMap::ERR_NEGATIVEPOS;
						}
					} catch(...) {
						responseMsg = CacheMap::ERR_POSNOTNUM;
					}
				}
				else if(validQuery(parts, 3, CacheMap::POP, CacheMap::COLL_ENTRY))
				{
					CacheMap::popValueQueue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::POP_FRONT, CacheMap::COLL_ENTRY))
				{
					CacheMap::popFrontValue(parts.at(2));
				}
				else if(validQuery(parts, 3, CacheMap::POP_BACK, CacheMap::COLL_ENTRY))
				{
					CacheMap::popBackValue(parts.at(2));
				}
				else if(validQuery(parts, 2, CacheMap::SIZE))
				{
					responseValue = CastUtil::lexical_cast<string>(CacheMap::size(parts.at(1)));
				}
				else if(validQuery(parts, 2, CacheMap::CLEAR))
				{
					CacheMap::clear(parts.at(1));
				}
				else if(validQuery(parts, 2, CacheMap::IS_EMPTY))
				{
					responseValue = CastUtil::lexical_cast<string>(CacheMap::isEmpty(parts.at(1)));
				}
				else
				{
					responseMsg = "Invalid command specified";
				}

				delete query;
			}
			else
			{
				responseMsg = "Invalid query specified";
			}
		} catch(string& err) {
			responseMsg = err;
		} catch(const char* err) {
			responseMsg = err;
		} catch(...) {
			responseMsg = "Application Error Occurred";
		}


		AMEFObject responseObject;
		responseObject.addPacket(responseMsg);
		if(responseValue!="")
		{
			responseObject.addPacket(responseValue);
		}
		AMEFEncoder encoder;

		string response = encoder.encodeB(&responseObject, false);

		int toto = response.length();
		if(isSSLEnabled)
		{
			int er = BIO_write(io, response.c_str(), toto);
			switch(SSL_get_error(ssl,er))
			{
				case SSL_ERROR_NONE:
				{
					break;
				}
				case SSL_ERROR_ZERO_RETURN:
				default:
				{
					SSLHandler::getInstance()->closeSSL(fd, ssl, io);
					return;
				}
			}
			BIO_flush(io);
		}
		else
		{
			while (toto > 0) {
				int bytes = send(fd, response.c_str(), response.length(), 0);
				if (bytes == 0)
				{
					close(fd);
					return;
				}
				response = response.substr(bytes);
				toto -= bytes;
			}
		}
	}
}
