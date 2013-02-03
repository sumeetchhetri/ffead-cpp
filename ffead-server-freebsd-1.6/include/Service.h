/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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
 * Service.h
 *
 *  Created on: Jan 29, 2010
 *      Author: sumeet
 */

#ifndef SERVICE_H_
#define SERVICE_H_
#include "string"
#include "vector"
using namespace std;
class Service {
private:
	string name;
	string address;
	vector<string> accessList;
	string type;
public:
	Service();
	virtual ~Service();
};

#endif /* SERVICE_H_ */
/*
<service>
    <name>ExampleHTTPService</name>
    <address>http://example.com:5098/service</address>
    <accessList>
                <mem>user1</mem>
                <mem>user2</mem>
                <mem>user3</mem>
    </accessList>
</service>
<service>
    <name>ExampleTCPService</name>
    <address>tcp://example.com:5098/service</address>
    <accessList>
                <mem>user1</mem>
                <mem>user2</mem>
                <mem>user3</mem>
    </accessList>
</service>
<service>
    <name>ExampleUDPService</name>
    <address>udp://example.com:5098/service</address>
    <accessList>
                <mem>user1</mem>
                <mem>user2</mem>
                <mem>user3</mem>
    </accessList>
</service>
*/
