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
 * WsUtil.h
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#ifndef WSUTIL_H_
#define WSUTIL_H_
#include "XmlParser.h"
#include "Reflection.h"
#include "TemplateEngine.h"
#include "AfcUtil.h"

class WsUtil {
public:
	WsUtil();
	virtual ~WsUtil();
	string generateWSDL(string file,string usrinc,string resp,string &headers,map<string,string> &wsmap,string appname);
	string generateAllWSDL(vector<string> files,string resp,map<string,string> &wsmap);
};

#endif /* WSUTIL_H_ */
