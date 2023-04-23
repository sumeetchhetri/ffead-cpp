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
 * Client.h
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#ifndef CLIENT_H_
#define CLIENT_H_
#include "AppDefines.h"
#include "ClientInterface.h"

class Client : public ClientInterface {
	bool connected;
	SOCKET sockfd;
	std::string buffer;
	//Logger logger;
public:
	Client();
	virtual ~Client();
	static int conn(const std::string&, const int&);
	bool connection(const std::string&, const int&);
	bool connectionNB(const std::string&, const int&);
	//bool connectionUnresolv(const std::string& host, const int& port);
	int sendData(std::string);
	std::string getTextData(const std::string& hdrdelm, const std::string& cntlnhdr);
	static std::string getHttpData(const std::string& hdrdelm, const std::string& cntlnhdr, bool connected, std::string& buffer);
	std::string getBinaryData(const int&, const bool&);
	void closeConnection();
	bool isConnected();
	bool isReady(int mode);
	int receive(std::string& buf, const int& flag);
	int receivelen(std::string& buf, const int& len, const int& flag);
	std::string getData();
	int sendlen(const std::string& buf, const int& len);
};

#endif /* CLIENT_H_ */
