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
#include "LoggerFactory.h"
#include "ConfigurationData.h"


class WsUtil {
	Logger logger;
public:
	WsUtil();
	virtual ~WsUtil();
	void handleWebService(std::string& ws_funcs, const WsDetails& wsd, std::map<std::string, std::map<std::string, ClassStructure, std::less<> >, std::less<> >& clsstrucMaps, const std::string& resp, std::string &headers, Reflection& ref);
	std::vector<WsDetails> getWsDetails(const std::vector<std::string>& apps, const std::string& serverRootDirectory);
	std::string generateAllWSDL(const std::vector<WsDetails>& wsdvec, const std::string& resp, Reflection& ref, std::map<std::string, std::map<std::string, ClassStructure, std::less<> >, std::less<> >& clsstrucMaps);
};

#endif /* WSUTIL_H_ */
