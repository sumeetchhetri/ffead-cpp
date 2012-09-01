/*
	Copyright 2010, Sumeet Chhetri 
  
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
 * MessageUtil.h
 *
 *  Created on: Oct 1, 2009
 *      Author: sumeet
 */

#ifndef MESSAGEUTIL_H_
#define MESSAGEUTIL_H_
#include "Message.h"
#include "XmlParser.h"
#include "MessageHandler.h"
#include "StringUtil.h"
#include "Logger.h"

class MessageUtil {
public:
	MessageUtil(string);
	virtual ~MessageUtil();
	Destination getDestination();
	bool sendMessage(Message);
	Message receiveMessage();
	void subscribe(string);
	void unSubscribe(string);
    string getSubscriber() const;
    void setSubscriber(string);
private:
    Logger logger;
	Destination destination;
	string host;
	string port;
	string subscriber;
};

#endif /* MESSAGEUTIL_H_ */
