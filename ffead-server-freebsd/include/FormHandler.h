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
 * FormHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef FORMHANDLER_H_
#define FORMHANDLER_H_
#include "Reflector.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "Document.h"
#include <dlfcn.h>
#include "Constants.h"

typedef void* (*toVoidP) (string);

class FormHandler {
public:
	FormHandler();
	virtual ~FormHandler();
	static string handle(HttpRequest* req, HttpResponse& res, map<string, Element> formMap, void* dlib);
};

#endif /* FORMHANDLER_H_ */
