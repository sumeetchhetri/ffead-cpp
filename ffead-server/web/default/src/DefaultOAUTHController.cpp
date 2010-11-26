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


bool DefaultOAUTHController::handle(HttpRequest* req,HttpResponse* res)
{
	string method = req->getMethod();
	boost::to_upper(method);

	map<string,string>::iterator it;
	map<string,string> reqparams = req->getRequestParams();
	map<string,string> oauthparams = req->getAuthinfo();
	for(it=oauthparams.begin();it!=oauthparams.end();it++)
	{
		reqparams[it->first] = it->second;
	}

	string url = reqparams["realm"];
	if(url=="")
		url = "http://" + req->getHost() + req->getActUrl();

	string allpars = (method + "&" + CryptoHandler::urlEncode(url) + "&");
	string allparst;
	cout << "init" <<endl;

	vector<string> temps;
	for(it=reqparams.begin();it!=reqparams.end();it++)
	{
		if(it->first!="realm" && it->first!="oauth_signature" && it->first!="Method")
		{
			temps.push_back((it->first + "=" + it->second));
		}
	}
	for (int var = 0; var < temps.size(); ++var)
	{
		allparst += temps.at(var);
		if(var!=temps.size()-1)
			allparst += "&";
	}
	cout << "req and oauth params" <<endl;

	FileAuthController fauthu(req->getCntxt_root()+"/users",":");
	FileAuthController fautht(req->getCntxt_root()+"/tokens",":");
	FileAuthController fauthta(req->getCntxt_root()+"/access_tokens",":");

	string key,tokk,resu;
	bool isreqtype;
	bool flag = fauthu.getPassword(reqparams["oauth_consumer_key"],key);

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
	}
	key += ("&" + tokk);
	cout << "key = " << key << endl;

	if(flag)
	{
		cout << allpars << allparst << endl;
		allpars += CryptoHandler::urlEncode(allparst);
		cout << allpars << endl;
		char* resst = (char*)CryptoHandler::hmac_sha1((char*)allpars.c_str(),(char*)key.c_str(),true);
		cout << "done crypto " << resst << endl;
		resu.append(resst);
	}

	cout << "encoded sig = " << reqparams["oauth_signature"] << endl;
	string signature = CryptoHandler::urlDecode(reqparams["oauth_signature"]);
	cout << "decoded sig = " << signature << endl;
	if(resu==signature)
	{
		res->setStatusCode("200");
		res->setStatusMsg("OK");
		res->setContent_type("text/plain");
		string filen;
		if(tokk=="")
		{
			filen = req->getCntxt_root()+"/tokens";
			ofstream ofs(filen.c_str(),ios_base::app);
			string oauthtok,oauthsec;
			oauthtok = boost::lexical_cast<string>(Timer::getCurrentTime());
			oauthsec = boost::lexical_cast<string>(rand()%1000 + 123453) + oauthtok + boost::lexical_cast<string>(rand()%1000 + 12353);
			string wrf = oauthtok + ":" + oauthsec + "\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			res->setContent("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec);
			cout << "verified initial request signature is valid" << endl;
			cout << "provided a request token" << endl;
		}
		else if(reqparams["username"]!="" && reqparams["password"]!="")
		{
			flag = fauthu.getPassword(reqparams["username"],key);
			if(!flag)
			{
				res->setStatusCode("401");
				res->setStatusMsg("Unauthorized\r\nWWW-Authenticate: OAuth realm=\""+url+"\"");
				res->setContent_type("text/plain");
				cout << "invalid username/password" << endl;
			}
			else if(key==reqparams["password"])
			{
				cout << "valid username/password" << endl;
				if(reqparams["oauth_callback"]!="")
				{
					res->setStatusCode("303");
					res->setStatusMsg("Moved Permanently\r\nLocation: "+CryptoHandler::urlDecode(reqparams["oauth_callback"])+"?"+allparst+"&access=true");
					cout << "redirecting to callback url" << endl;
				}
				else
				{
					res->setContent(allparst+"&access=true");
					cout << "no callback url specified sending access info in content" << endl;
				}
			}
		}
		else if(isreqtype && reqparams["oauth_token"]!="" && reqparams["oauth_consumer_key"]!="")
		{
			filen = req->getCntxt_root()+"/access_tokens";
			ofstream ofs(filen.c_str(),ios_base::app);
			string oauthtok,oauthsec;
			oauthtok = boost::lexical_cast<string>(Timer::getCurrentTime());
			oauthsec = boost::lexical_cast<string>(rand()%1000 + 123453) + oauthtok + boost::lexical_cast<string>(rand()%1000 + 12353);
			string wrf = oauthtok + ":" + oauthsec + "\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			res->setContent("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec);
			cout << "verified request token signature is valid" << endl;
			cout << "provided an access token" << endl;
		}
		else if(!isreqtype)
		{
			cout << "resource access granted" << endl;
			return false;
		}
		return true;
	}
	else
	{
		res->setStatusCode("401");
		res->setStatusMsg("Unauthorized\r\nWWW-Authenticate: OAuth realm=\""+url+"\"");
		cout << "verified request token signature is invalid" << endl;
		return true;
	}
}
