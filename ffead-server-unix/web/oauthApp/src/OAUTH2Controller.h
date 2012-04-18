/*
 * OAUTH2Controller.h
 *
 *  Created on: Aug 20, 2009
 *      Author: sumeet
 */

#ifndef OAUTH2CONTROLLER_H
#define OAUTH2CONTROLLER_H
#include <iostream>
#include "Controller.h"
#include "FileAuthController.h"
#include "Client.h"
#include "HttpResponseParser.h"
#include "CryptoHandler.h"
#include "SSLClient.h"

class OAUTH2Controller : public Controller{
public:
	OAUTH2Controller();
	virtual ~OAUTH2Controller();
	HttpResponse service(HttpRequest);
};

#endif /* OAUTH2CONTROLLER_H */
