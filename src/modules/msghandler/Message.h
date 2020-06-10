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
 * Message.h
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include "Destination.h"

#include "InvalidMessageException.h"
#include "XmlParser.h"

class Message {
	Destination destination;
	std::string timestamp;
	std::string type;
	std::string priority;
	std::string userId;
	std::string encoding;
	std::string body;
public:
	Message(const std::string&);
	Message(Document&);
	Message();
	virtual ~Message();
    Destination getDestination() const;
    void setDestination(const Destination&);
    std::string getTimestamp() const;
    void setTimestamp(const std::string&);
    std::string getType() const;
    void setType(const std::string&);
    std::string getPriority() const;
    void setPriority(const std::string&);
    std::string getUserId() const;
    void setUserId(const std::string&);
    std::string getBody() const;
    void setBody(const std::string&);
    std::string getEncoding() const;
    void setEncoding(const std::string&);
    std::string toXml() const;
};

#endif /* MESSAGE_H_ */
