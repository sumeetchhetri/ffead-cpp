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
 * MarkerIOFilter.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#include "MarkerIOFilter.h"

MarkerIOFilter::MarkerIOFilter() {
	// TODO Auto-generated constructor stub

}

MarkerIOFilter::~MarkerIOFilter() {
	// TODO Auto-generated destructor stub
}

void MarkerIOFilter::doInputFilter(HttpRequest *req)
{
	//Process the input request here...
	std::cout << "Processed input request" << std::endl;
}

void MarkerIOFilter::doOutputFilter(HttpResponse *res)
{
	//Process the output response here...
	std::cout << "Processed output response" << std::endl;
}

bool MarkerIOFilter::doHandle(HttpRequest *req, HttpResponse* res)
{
	bool continue_proc_request = false;
	res->setContent("Filter handled your request as you visited a *.marker location");
	res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	return continue_proc_request;
}
