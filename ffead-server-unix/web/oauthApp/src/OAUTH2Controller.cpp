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

HttpResponse OAUTH2Controller::service(HttpRequest req)
{
	HttpResponse res;

	if(req.getFile()=="login.auth2")
	{
		if(req.getRequestParams()["username"]!="" && req.getRequestParams()["password"]!="")
		{
			FileAuthController fauthu(req.getCntxt_root()+"/users",":");
			string key;
			bool flag = fauthu.getPassword(req.getRequestParams()["username"],key);
			if(flag)
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type("text/plain");
				res.setContent_str("Valid Login");
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type("text/plain");
				res.setContent_str("InValid Login");
			}
			cout << "inside oauth controller non empty credentials" << endl;
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Username and Password cannot be blank");
			cout << "inside oauth controller empty credentials" << endl;
		}
	}
	else if(req.getFile()=="authorizeUser.auth2")
	{
		string data = "http://graph.facebook.com/oauth/authorize?";
		data += "response_type=token&client_id=174968395858505&redirect_uri="+CryptoHandler::urlEncode("http://public_ip:port/oauthApp/calledback.auth2");
		cout << data << endl;

		res.setStatusCode("303");
		res.setStatusMsg("Moved Permanently\r\nLocation: "+data+"\r\n\r\n");
		cout << "redirecting to third party url" << endl;
	}
	else if(req.getFile()=="calledback.auth2")
	{
		if(req.getRequestParams()["access_token"]!="")
		{
			string filen = req.getCntxt_root()+"/access2-tokens";
			ofstream ofs(filen.c_str());
			string wrf = req.getRequestParams()["access_token"];
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();

			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/html");
			string conte = "<html><head><script type='text/javascript' src='public/json2.js'></script><script type='text/javascript' src='public/prototype.js'></script><script type='text/javascript' src='public/oauth2.js'></script></head>";
			conte += "Resource: <input id='resource' type='text'/><input type='submit' onclick='getResource(\"resource\",\""+req.getRequestParams()["tusername"]+"\")'/></body>";
			conte += "</html>";
			res.setContent_str(conte);
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Invalid user");
		}
	}
	else if(req.getFile()=="getResource.auth2")
	{
		string filen = req.getCntxt_root()+"/access2-tokens";
		ifstream ifs(filen.c_str());
		string tokse;
		getline(ifs,tokse);
		ifs.close();

		if(tokse!="" && req.getRequestParams()["error_status"]=="")
		{
			SSLClient client;
			client.connection("graph.facebook.com",443);
			string data = "GET /me/"+req.getRequestParams()["resource"]+"?access_token="+tokse;
			data += " HTTP/1.1\r\nHost: graph.facebook.com\r\nUser-Agent: Program\r\n\r\n";
			cout << data << endl;

			int bytes = client.sendData(data);
			string call=client.getData("\r\n","Content-Length: ");
			HttpResponseParser parser(call);
			client.closeConnection();

			res.setStatusCode(parser.getHeaderValue("StatusCode"));
			res.setStatusMsg(parser.getHeaderValue("StatusMsg"));
			res.setContent_type(parser.getHeaderValue("Content-Type"));
			res.setContent_str(parser.getContent());
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Access denied");
		}
	}
	return res;
}

