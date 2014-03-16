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
 * RestController.h
 *
 *  Created on: Feb 4, 2012
 *      Author: sumeet
 */

#ifndef RESTCONTROLLER_H_
#define RESTCONTROLLER_H_
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "Constants.h"
#include <dlfcn.h>
#include "LoggerFactory.h"
#ifdef INC_XMLSER
#include "XMLSerialize.h"
#endif
#include "JSONSerialize.h"

class RestController {
	Logger logger;
public:
	HttpRequest* request;
	HttpResponse* response;
	RestController();
	virtual ~RestController();
	void buildResponse(HTTPResponseStatus status, string className, void* entity);
	void buildResponseVector(HTTPResponseStatus status, string className, void* entity);
};

#endif /* RESTCONTROLLER_H_ */
