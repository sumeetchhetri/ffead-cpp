/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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
	string timestamp;
	string type;
	string priority;
	string userId;
	string encoding;
	string body;
public:
	Message(const string&);
	Message(Document&);
	Message();
	virtual ~Message();
    Destination getDestination() const;
    void setDestination(const Destination&);
    string getTimestamp() const;
    void setTimestamp(const string&);
    string getType() const;
    void setType(const string&);
    string getPriority() const;
    void setPriority(const string&);
    string getUserId() const;
    void setUserId(const string&);
    string getBody() const;
    void setBody(const string&);
    string getEncoding() const;
    void setEncoding(const string&);
    string toXml() const;
};

#endif /* MESSAGE_H_ */
