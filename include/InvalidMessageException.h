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
 * InvalidMessageException.h
 *
 *  Created on: Sep 27, 2009
 *      Author: sumeet
 */

#ifndef INVALIDMESSAGEEXCEPTION_H_
#define INVALIDMESSAGEEXCEPTION_H_
#include "Exception.h"
#include "string"
using namespace std;

class InvalidMessageException : public Exception {
public:
	InvalidMessageException(string);
	virtual ~InvalidMessageException() throw();
};

#endif /* INVALIDMESSAGEEXCEPTION_H_ */
