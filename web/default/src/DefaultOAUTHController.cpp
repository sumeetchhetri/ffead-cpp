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
 * DefaultOAUTHController.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: sumeet
 */

#include "DefaultOAUTHController.h"

DefaultOAUTHController::DefaultOAUTHController() {
	// TODO Auto-generated constructor stub

}

DefaultOAUTHController::~DefaultOAUTHController() {
	// TODO Auto-generated destructor stub
}


bool DefaultOAUTHController::service(HttpRequest* req, HttpResponse* res)
{
	std::string method = std::string(req->getMethod());
	StringUtil::toUpper(method);

	RMap::iterator it;
	RMap reqparams = req->getAllParams();
	RMap oauthparams = req->getAuthinfo();
	for(it=oauthparams.begin();it!=oauthparams.end();it++)
	{
		reqparams[it->first] = it->second;
	}

	std::string url = reqparams["realm"];
	if(url=="")
		url = "http://" + req->getHeader(HttpRequest::Host) + req->getActUrl();

	std::string allpars = (method + "&" + CryptoHandler::urlEncode(url) + "&");
	std::string allparst;
	std::cout << "init" <<std::endl;

	std::vector<std::string> temps;
	for(it=reqparams.begin();it!=reqparams.end();it++)
	{
		if(it->first!="realm" && it->first!="oauth_signature" && it->first!="Method")
		{
			temps.push_back((it->first + "=" + it->second));
		}
	}
	for (int var = 0; var < (int)temps.size(); ++var)
	{
		allparst += temps.at(var);
		if(var!=(int)temps.size()-1)
			allparst += "&";
	}
	std::cout << "req and oauth params" <<std::endl;

	FileAuthController fauthu(req->getCntxt_root()+"/users",":");
	FileAuthController fautht(req->getCntxt_root()+"/tokens",":");
	FileAuthController fauthta(req->getCntxt_root()+"/access_tokens",":");

	std::string key,tokk,resu,csec;
	bool isreqtype = false;
	bool flag = true;
	std::string conss;
	if(reqparams["oauth_consumer_key"]!="")
	{
		flag = fauthu.getPassword(reqparams["oauth_consumer_key"],key);
		key += ("&");
		csec = key;
	}

	if(reqparams.find("oauth_token")!=reqparams.end()
			&& reqparams["oauth_token"]!="")
	{
		bool tempfl = fautht.getPassword(reqparams["oauth_token"],tokk);
		if(tempfl)
			isreqtype = true;
		else
		{
			tempfl = fauthta.getPassword(reqparams["oauth_token"],tokk);
			isreqtype = false;
		}
		flag &= tempfl;
		key = tokk;
	}

	if(flag)
	{
		std::cout << allpars << allparst << std::endl;
		allpars += CryptoHandler::urlEncode(allparst);
		std::cout << allpars << std::endl;
		std::cout << "key = " << key << std::endl;
		char* resst = (char*)CryptoHandler::hmac_sha1((char*)allpars.c_str(),(char*)key.c_str(),true);
		std::cout << "done crypto " << resst << std::endl;
		resu.append(resst);
	}

	std::cout << "encoded sig = " << reqparams["oauth_signature"] << std::endl;
	std::string signature = CryptoHandler::urlDecode(reqparams["oauth_signature"]);
	std::cout << "decoded sig = " << signature << std::endl;
	if(flag && resu==signature && signature!="" && resu!="")
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
		std::string filen;
		if(tokk=="" && req->getFile()=="request.oauth")
		{
			filen = req->getCntxt_root()+"/tokens";
			std::ofstream ofs(filen.c_str());
			std::string oauthtok,oauthsec;
			oauthtok = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
			oauthsec = CastUtil::lexical_cast<std::string>(rand()%1000 + 123453) + oauthtok + CastUtil::lexical_cast<std::string>(rand()%1000 + 12353);
			std::string wrf = oauthtok + ":" + key + oauthsec + "\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			std::string parsc;
			for(it=reqparams.begin();it!=reqparams.end();it++)
			{
				if(it->first!="realm" && it->first.find("oauth_")==std::string::npos && it->first!="Method")
				{
					parsc.append(it->first + "=" + it->second + "&");
				}
			}
			if(reqparams["oauth_callback"]!="")
			{
				filen = req->getCntxt_root()+"/callbacks";
				std::ofstream ofs1(filen.c_str());
				std::string oauth_ver = oauthtok + CastUtil::lexical_cast<std::string>(rand()%1000);
				wrf = CryptoHandler::urlDecode(reqparams["oauth_callback"])+"?oauth_verifier="+oauth_ver+"&"+parsc;
				ofs1.write(wrf.c_str(),wrf.length());
				ofs1.close();
			}
			std::string cont = ("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec+"&")+parsc;
			if(cont[cont.length()-1]=='&')
				cont = cont.substr(0,cont.length()-1);
			res->setContent(cont);
			std::cout << "verified initial request signature is valid" << std::endl;
			std::cout << "provided a request token" << std::endl;
		}
		else if(isreqtype && reqparams["oauth_token"]!="" && reqparams["oauth_consumer_key"]!=""
				&& req->getFile()=="access.oauth")
		{
			filen = req->getCntxt_root()+"/access_tokens";
			std::ofstream ofs(filen.c_str());
			std::string oauthtok,oauthsec;
			oauthtok = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
			oauthsec = CastUtil::lexical_cast<std::string>(rand()%1000 + 123453) + oauthtok + CastUtil::lexical_cast<std::string>(rand()%1000 + 12353);
			std::string wrf = oauthtok + ":" + csec + oauthsec + "\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			std::string cont = ("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec+"&");
			for(it=reqparams.begin();it!=reqparams.end();it++)
			{
				if(it->first!="realm" && it->first.find("oauth_")==std::string::npos && it->first!="Method")
				{
					cont.append(it->first + "=" + it->second + "&");
				}
			}
			if(cont[cont.length()-1]=='&')
				cont = cont.substr(0,cont.length()-1);
			res->setContent(cont);
			std::cout << "verified request token signature is valid" << std::endl;
			std::cout << "provided an access token" << std::endl;
		}
		else if(!isreqtype && req->getFile()=="getResource.oauth" && reqparams["file"]!="")
		{
			std::cout << "resource access granted for " << reqparams["file"] << std::endl;
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			req->setFile(reqparams["file"]);
			return false;
		}
		return true;
	}
	else if(req->getFile()=="login.oauth" && reqparams["username"]!="" && reqparams["password"]!="" && reqparams["oauthparms"]!="")
	{
		flag = fauthu.getPassword(reqparams["username"],key);
		if(!flag)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
			res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			std::cout << "invalid username/password" << std::endl;
		}
		else if(key==reqparams["password"])
		{
			std::cout << "valid username/password" << std::endl;
			if(reqparams["oauthparms"]!="")
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::MovedPermanently);
				res->addHeaderValue(HttpResponse::Location, CryptoHandler::urlDecode(reqparams["oauthparms"])+"&access=true");
				std::cout << "redirecting to callback url" << std::endl;
			}
			else
			{
				res->setContent(reqparams["oauthparms"]+"&access=true");
				std::cout << "no callback url specified sending access info in content" << std::endl;
			}
		}
		return true;
	}
	else if(req->getFile()=="login.oauth")
	{
		std::string filen = req->getCntxt_root()+"/callbacks";
		std::ifstream ifs(filen.c_str());
		std::string wrf;
		getline(ifs,wrf);
		ifs.close();
		if(reqparams["oauth_token"]!="")
			wrf = wrf+"oauth_token="+reqparams["oauth_token"];
		std::string html = "<html><head></head><body><form name=\"name1\" method=\"POST\" action=\"/login.oauth\">";
		html += "<input name='oauthparms' type='hidden' value='"+wrf+"'/>";
		html += "Username:<input name='username' type='text'/>";
		html += "Password:<input name='password' type='password'/>";
		html += "<input type='submit' value='Submit'/>";
		html += "</form></body></html>";
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_SHTML);
		res->setContent(html);
		std::cout << "Login page display" << std::endl;
		return true;
	}
	else
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
		//res->setStatusMsg("Unauthorized\r\nWWW-Authenticate: OAuth realm=\""+url+"\"");
		std::cout << "verified request token signature is invalid" << std::endl;
		return true;
	}
}
