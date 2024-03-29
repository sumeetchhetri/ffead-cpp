/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * OAUTHController.cpp
 *
 *  Created on: Aug 20& 2009
 *      Author: sumeet
 */

#include "OAUTH2Controller.h"


OAUTH2Controller::OAUTH2Controller() {
	// TODO Auto-generated constructor stub

}

OAUTH2Controller::~OAUTH2Controller() {
	// TODO Auto-generated destructor stub
}

bool OAUTH2Controller::service(HttpRequest* req, HttpResponse* res)
{
	if(req->getFile()=="login.auth2")
	{
		if(req->getRequestParams()["username"]!="" && req->getRequestParams()["password"]!="")
		{
			FileAuthController fauthu(req->getCntxt_root()+"/users",":");
			std::string key;
			bool flag = fauthu.getPassword(req->getRequestParams()["username"],key);
			if(flag)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
				res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
				res->setContent("Valid Login");
			}
			else
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
				res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
				res->setContent("InValid Login");
			}
			std::cout << "inside oauth controller non empty credentials" << std::endl;
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			res->setContent("Username and Password cannot be blank");
			std::cout << "inside oauth controller empty credentials" << std::endl;
		}
	}
	else if(req->getFile()=="authorizeUser.auth2")
	{
		std::string data = "http://graph.facebook.com/oauth/authorize?";
		data += "response_type=token&client_id=174968395858505&redirect_uri="+CryptoHandler::urlEncode("http://public_ip:port/oauthApp/calledback.auth2");
		std::cout << data << std::endl;

		res->setHTTPResponseStatus(HTTPResponseStatus::MovedPermanently);
		res->addHeaderValue(HttpResponse::Location, data);
		std::cout << "redirecting to third party url" << std::endl;
	}
	else if(req->getFile()=="calledback.auth2")
	{
		if(req->getRequestParams()["access_token"]!="")
		{
			std::string filen = req->getCntxt_root()+"/access2-tokens";
			std::ofstream ofs(filen.c_str());
			std::string wrf = req->getRequestParams()["access_token"];
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();

			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_HTML);
			std::string conte = "<html><head><script type='text/javascript' src='public/json2.js'></script><script type='text/javascript' src='public/prototype.js'></script><script type='text/javascript' src='public/oauth2.js'></script></head>";
			conte += "Resource: <input id='resource' type='text'/><input type='submit' onclick='getResource(\"resource\",\""+req->getRequestParams()["tusername"]+"\")'/></body>";
			conte += "</html>";
			res->setContent(conte);
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			res->setContent("Invalid user");
		}
	}
	else if(req->getFile()=="getResource.auth2")
	{
		std::string filen = req->getCntxt_root()+"/access2-tokens";
		std::ifstream ifs(filen.c_str());
		std::string tokse;
		getline(ifs,tokse);
		ifs.close();

		if(tokse!="" && req->getRequestParams()["error_status"]=="")
		{
#ifdef HAVE_SSLINC
			SSLClient client;
			client.connection("graph.facebook.com",443);
			std::string data = "GET /me/"+req->getRequestParams()["resource"]+"?access_token="+tokse;
			data += " HTTP/1.1\r\nHost: graph.facebook.com\r\nUser-Agent: Program\r\n\r\n";
			std::cout << data << std::endl;

			client.sendData(data);
			std::string call=client.getData("\r\n","Content-Length: ");
			HttpResponseParser parser(call, *res);
			client.closeConnection();

			res->setContent(parser.getContent());
#else
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			res->setContent("Access denied");
#endif
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			res->setContent("Access denied");
		}
	}
	return true;
}

