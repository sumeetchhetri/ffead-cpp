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
 * ConnectionSettings.cpp
 *
 *  Created on: Jan 30, 2010
 *      Author: sumeet
 */

#include "ConnectionSettings.h"

ConnectionSettings::ConnectionSettings() {
	// TODO Auto-generated constructor stub

}

ConnectionSettings::~ConnectionSettings() {
	// TODO Auto-generated destructor stub
}

ConnectionSettings::ConnectionSettings(string mod,string src,string usn,string pas,string add) {
	this->mode = mod;
	this-> source = src;
	this->username = usn;
	this->passwd = pas;
	this->address = add;
}
