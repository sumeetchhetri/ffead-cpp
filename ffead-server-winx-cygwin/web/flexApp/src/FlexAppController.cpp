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

