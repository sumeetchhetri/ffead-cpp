/*
 * GtmAccesController.cpp
 *
 *  Created on: 25-Nov-2014
 *      Author: sumeetc
 */

#include "GtmAccesController.h"

GtmAccesController::GtmAccesController() {
	// TODO Auto-generated constructor stub

}

GtmAccesController::~GtmAccesController() {
	// TODO Auto-generated destructor stub
}

bool GtmAccesController::service(HttpRequest* req, HttpResponse* res) {
#ifdef INC_GTM
	GTM* gtma = GTM::getInstance();
	gtma->set("^A(1)", 1);
	gtma->set("^A(2)", 2);
	gtma->set("^A(3)", 3);
	string json = gtma->getJsonString("^A");
	cout << "Called GtmAccesController " << json << endl;
	res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
	res->setContent(json);
	return true;
#else
	return false;
#endif
}
