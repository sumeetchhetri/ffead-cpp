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
 * MarkerSecurityProvider.cpp
 *
 *  Created on: Aug 20, 2009
 *      Author: sumeet
 */

#include "MarkerSecurityProvider.h"

MarkerSecurityProvider::MarkerSecurityProvider()
{}

MarkerSecurityProvider::~MarkerSecurityProvider()
{}

bool MarkerSecurityProvider::authenticate(const string& username, const string& password)
{
	cout << "Username is " << username << ", Password is " << password << endl;
	return true;
}

string MarkerSecurityProvider::getUserRole(const string& username)
{
	cout << "Username is " << username << endl;
	return "ROLE_USER";
}

bool MarkerSecurityProvider::isInitialized()
{
	return true;
}
