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
 * ServicePool.h
 *
 *  Created on: Jan 29, 2010
 *      Author: sumeet
 */

#ifndef SERVICEPOOL_H_
#define SERVICEPOOL_H_
#include "string"
#include "map"
#include "vector"
#include "Service.h"
#include <time.h>
#include "CastUtil.h"
using namespace std;
class ServicePool {
private:
	ServicePool();
    virtual ~ServicePool();
public:
    static ServicePool* getInstance();
    static string registerService(string name,Service service);// will return a unique identification for this service
    static bool unRegisterService(string id);//unregister will require the unique id
    static vector<string> getServices(string);//return a list of available services
    Service getService(string);//return a service
};

#endif /* SERVICEPOOL_H_ */
