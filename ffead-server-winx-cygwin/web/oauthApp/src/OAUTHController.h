/*
 * DefaultController.h
 *
 *  Created on: Aug 20, 2009
 *      Author: sumeet
 */

#ifndef OAUTHCONTROLLER_H
#define OAUTHCONTROLLER_H
#include <iostream>
#include "Controller.h"
#include "FileAuthController.h"
#include "Client.h"
#include "HttpResponseParser.h"
#include "CryptoHandler.h"

class OAUTHController : public Controller{
public:
	OAUTHController();
	virtual ~OAUTHController();
	HttpResponse service(HttpRequest);
};

#endif /* OAUTHCONTROLLER_H */
