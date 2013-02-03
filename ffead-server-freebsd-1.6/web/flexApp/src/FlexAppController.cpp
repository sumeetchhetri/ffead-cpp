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
 * FlexAppController.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#include "FlexAppController.h"


FlexAppController::FlexAppController() {
	// TODO Auto-generated constructor stub

}

FlexAppController::~FlexAppController() {
	// TODO Auto-generated destructor stub
}

HttpResponse FlexAppController::service(HttpRequest req)
{
	HttpResponse res;
	res.setStatusCode("200");
	res.setStatusMsg("OK");
	res.setContent_type("text/plain");
	res.setContent_str("[{\"num\": 1,\"name\": \"Sumeet\", \"role\": \"Developer\"},{\"num\": 2,\"name\": \"Ravi\", \"role\": \"Quality Analyst\"},{\"num\": 3,\"name\": \"Amit\", \"role\": \"Web Designer\"}]");
	return res;
}

