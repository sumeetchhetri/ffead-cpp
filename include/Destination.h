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
 * Destination.h
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#ifndef DESTINATION_H_
#define DESTINATION_H_
#include "string"
using namespace std;

class Destination {
	string name;
	string type;
public:
	Destination();
	virtual ~Destination();
    string getName() const;
    void setName(string);
    string getType() const;
    void setType(string);
};

#endif /* DESTINATION_H_ */
