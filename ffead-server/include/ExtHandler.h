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
 * ExtHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef EXTHANDLER_H_
#define EXTHANDLER_H_
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TemplateHandler.h"
#include "DynamicView.h"
#include "Reflector.h"
#include "Document.h"
#include "View.h"
#include <dlfcn.h>
#include "Constants.h"
#include "LoggerFactory.h"
#include "map"
#include "ConfigurationData.h"

typedef string (*DCPPtr) ();
typedef ClassInfo (*FunPtr) ();
typedef string (*TemplatePtr) (map<string, void*>);

class ExtHandler {
public:
	ExtHandler();
	virtual ~ExtHandler();
	static bool handle(HttpRequest* req, HttpResponse& res, void* dlib, void* ddlib, string ext);
};

#endif /* EXTHANDLER_H_ */
