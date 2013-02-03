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
 * OAUTHController.cpp
 *
 *  Created on: Aug 20& 2009
 *      Author: sumeet
 */

#include "OAUTHController.h"


OAUTHController::OAUTHController() {
	// TODO Auto-generated constructor stub

}

OAUTHController::~OAUTHController() {
	// TODO Auto-generated destructor stub
}

HttpResponse OAUTHController::service(HttpRequest req)
{
	HttpResponse res;
	map<string,string> reqParams = req.getAllParams();
	string hostp = req.getHost();
	if(req.getFile()=="login.auth")
	{
		if(reqParams["username"]!="" && reqParams["password"]!="")
		{
			FileAuthController fauthu(req.getCntxt_root()+"/users",":");
			string key;
			bool flag = fauthu.getPassword(reqParams["username"],key);
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
	else if(req.getFile()=="requestToken.auth")
	{
		Client client;
		client.connection("localhost",8080);
		string data = "GET /request.oauth?";
		data += "oauth_callback=http://"+hostp+"/oauthApp/calledback.auth&oauth_consumer_key=sumeet&oauth_nonce=&oauth_timestamp=0&oauth_token=&oauth_signature_method=HMAC-SHA1&oauth_version=1.0&tusername="+reqParams["tusername"]+"&";
		string sig = "GET&" + CryptoHandler::urlEncode("http://"+hostp+"/request.oauth") + "&";
		sig += CryptoHandler::urlEncode("oauth_callback=http://"+hostp+"/oauthApp/calledback.auth&oauth_consumer_key=sumeet&oauth_nonce=&oauth_signature_method=HMAC-SHA1&oauth_timestamp=0&oauth_token=&oauth_version=1.0&tusername="+reqParams["tusername"]);

		cout << sig << endl;

		char* resst = (char*)CryptoHandler::hmac_sha1((char*)sig.c_str(),"sumeet&",true);
		data += "oauth_signature=";
		string sign(resst);
		data.append(CryptoHandler::urlEncode(sign));

		data += " HTTP/1.1\r\nHost: "+hostp+"\r\nUser-Agent: Program\r\n\r\n";

		cout << data << endl;

		int bytes = client.sendData(data);
		string call,tot;
		while((call=client.getData())!="")
			tot.append(call);
		HttpResponseParser parser(tot);
		client.closeConnection();

		map<string,string> mapsd;
		vector<string> temp;
		string conte = parser.getContent();
		StringUtil::split(temp, conte, ("&"));
		//cout << conte << flush;
		for(unsigned int i=0;i<temp.size();i++)
		{
			vector<string> temp1;
			StringUtil::split(temp1, temp.at(i), ("="));
			mapsd[temp1.at(0)] = temp1.at(1);
			cout << temp1.at(0) << " = " << temp1.at(1) << endl;
		}
		if(mapsd["oauth_token"]!="" && mapsd["oauth_token_secret"]!="" && mapsd["tusername"]!="")
		{
			string filen = req.getCntxt_root()+"/"+reqParams["tusername"]+"-tokens-secrets";
			ofstream ofs(filen.c_str());
			string wrf = mapsd["tusername"] + ":" + mapsd["oauth_token"] + ":" + mapsd["oauth_token_secret"]+"\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Acquired request token");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Could not get request token");
		}
	}
	else if(req.getFile()=="authorizeUser.auth")
	{
		FileAuthController fauthu(req.getCntxt_root()+"/"+reqParams["tusername"]+"-tokens-secrets",":");
		string key1 = "sumeet&",key;
		bool flag = fauthu.getPassword(reqParams["tusername"],key);
		if(reqParams["tusername"]!="" && flag)
		{
			string tok = key.substr(0,key.find(":"));
			key = key1 + key.substr(key.find(":")+1);

			string data = "http://"+hostp+"/login.oauth?";
			data += "oauth_token="+tok+"&username="+reqParams["tusername"];
			cout << data << endl;

			res.setStatusCode("303");
			res.setStatusMsg("Moved Permanently\r\nLocation: "+data+"\r\n\r\n");
			cout << "redirecting to third party url" << endl;
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Invalid user");

		}

	}
	else if(req.getFile()=="calledback.auth")
	{
		FileAuthController fauthu(req.getCntxt_root()+"/"+reqParams["tusername"]+"-tokens-secrets",":");
		string key1 = "sumeet&",key;
		key = fauthu.get(reqParams["tusername"],2);
		string tok = fauthu.get(reqParams["tusername"],1);
		bool flag = (tok!="" && key!="");
		if(reqParams["tusername"]!="" && flag && reqParams["access"]=="true")
		{
			key = key1 + key;

			Client client;
			client.connection("localhost",8080);
			string data = "GET /access.oauth?";
			data += "oauth_consumer_key=sumeet&oauth_nonce=&oauth_timestamp=0&oauth_token="+tok+"&oauth_signature_method=HMAC-SHA1&oauth_version=1.0&tusername="+reqParams["tusername"]+"&";
			string sig = "GET&" + CryptoHandler::urlEncode("http://"+hostp+"/access.oauth") + "&";
			sig += CryptoHandler::urlEncode("oauth_consumer_key=sumeet&oauth_nonce=&oauth_signature_method=HMAC-SHA1&oauth_timestamp=0&oauth_token="+tok+"&oauth_version=1.0&tusername="+reqParams["tusername"]);

			cout << sig << endl;
			cout << key << endl;
			char* resst = (char*)CryptoHandler::hmac_sha1((char*)sig.c_str(),(char*)key.c_str(),true);
			data += "oauth_signature=";
			string sign(resst);
			data.append(CryptoHandler::urlEncode(sign));

			data += " HTTP/1.1\r\nHost: "+hostp+"\r\nUser-Agent: Program\r\n\r\n";

			cout << data << endl;

			int bytes = client.sendData(data);
			string call,tot;
			while((call=client.getData())!="")
				tot.append(call);
			HttpResponseParser parser(tot);
			client.closeConnection();

			map<string,string> mapsd;
			vector<string> temp;
			string conte = parser.getContent();
			StringUtil::split(temp, conte, ("&"));
			//cout << conte << flush;
			for(unsigned int i=0;i<temp.size();i++)
			{
				vector<string> temp1;
				StringUtil::split(temp1, temp.at(i), ("="));
				mapsd[temp1.at(0)] = temp1.at(1);
				cout << temp1.at(0) << " = " << temp1.at(1) << endl;
			}
			if(mapsd["oauth_token"]!="" && mapsd["oauth_token_secret"]!="" && mapsd["tusername"]!="")
			{
				string filen = req.getCntxt_root()+"/"+reqParams["tusername"]+"-access-secrets";
				ofstream ofs(filen.c_str());
				string wrf = mapsd["tusername"] + ":" + mapsd["oauth_token"] + ":" + mapsd["oauth_token_secret"]+"\n";
				ofs.write(wrf.c_str(),wrf.length());
				ofs.close();
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type("text/html");
				string conte = "<html><head><script type='text/javascript' src='public/json2.js'></script><script type='text/javascript' src='public/prototype.js'></script><script type='text/javascript' src='public/oauth.js'></script></head>";
				conte += "File Name: <input id='resource' type='text'/><input type='submit' onclick='getResource(\"resource\",\""+reqParams["tusername"]+"\")'/></body>";
				conte += "</html>";
				res.setContent_str(conte);
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type("text/plain");
				res.setContent_str("Could not get access token");
			}
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type("text/plain");
			res.setContent_str("Invalid user");
		}
	}
	else if(req.getFile()=="accessToken.auth")
	{

	}
	else if(req.getFile()=="getResource.auth")
	{
		FileAuthController fauthu(req.getCntxt_root()+"/"+reqParams["tusername"]+"-access-secrets",":");
		string key1 = "sumeet&",key;
		key = fauthu.get(reqParams["tusername"],2);
		string tok = fauthu.get(reqParams["tusername"],1);
		bool flag = (tok!="" && key!="");
		if(reqParams["tusername"]!="" && flag && reqParams["file"]!="")
		{
			key = key1 + key;

			Client client;
			client.connection("localhost",8080);
			string data = "GET /getResource.oauth?file="+reqParams["file"];
			data += "&oauth_consumer_key=sumeet&oauth_nonce=&oauth_timestamp=0&oauth_token="+tok+"&oauth_signature_method=HMAC-SHA1&oauth_version=1.0&tusername="+reqParams["tusername"]+"&";
			string sig = "GET&" + CryptoHandler::urlEncode("http://"+hostp+"/getResource.oauth") + "&";
			sig += CryptoHandler::urlEncode("file="+reqParams["file"]+"&oauth_consumer_key=sumeet&oauth_nonce=&oauth_signature_method=HMAC-SHA1&oauth_timestamp=0&oauth_token="+tok+"&oauth_version=1.0&tusername="+reqParams["tusername"]);

			cout << sig << endl;
			cout << key << endl;
			char* resst = (char*)CryptoHandler::hmac_sha1((char*)sig.c_str(),(char*)key.c_str(),true);
			data += "oauth_signature=";
			string sign(resst);
			data.append(CryptoHandler::urlEncode(sign));

			data += " HTTP/1.1\r\nHost: "+hostp+"\r\nUser-Agent: Program\r\n\r\n";

			cout << data << endl;

			int bytes = client.sendData(data);
			string call,tot;
			while((call=client.getData())!="")
				tot.append(call);
			HttpResponseParser parser(tot);
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

