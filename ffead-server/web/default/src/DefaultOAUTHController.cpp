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

	string allpars = (method + "&" + CryptoHandler::urlEncode(oauthparams["realm"]) + "&");
	string allparst;
	cout << "init" <<endl;

	int cnt = 0;
	for(it=reqparams.begin();it!=reqparams.end();it++)
	{
		allparst += (it->first + "=" + it->second);
		if(cnt++!=reqparams.size()-1)
			allparst += "&";
	}
	cout << "req params" <<endl;
	cnt = 0;
	vector<string> temps;
	for(it=oauthparams.begin();it!=oauthparams.end();it++)
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
	cout << "oauth params" <<endl;

	FileAuthController fauthu(req->getCntxt_root()+"/users",":");
	FileAuthController fautht(req->getCntxt_root()+"/tokens",":");
	cout << req->getCntxt_root()+"/users" << endl;

	string key,tokk,resu;
	bool flag = fauthu.getPassword(oauthparams["oauth_consumer_key"],key);

	if(oauthparams.find("oauth_token")!=oauthparams.end()
			&& oauthparams["oauth_token"]!="")
	{
		flag &= fautht.getPassword(oauthparams["oauth_token"],tokk);
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

	cout << "encoded sig = " << oauthparams["oauth_signature"] << endl;
	string signature = CryptoHandler::urlDecode(oauthparams["oauth_signature"]);
	cout << "decoded sig = " << signature << endl;
	if(resu==signature)
	{
		res->setStatusCode("200");
		res->setStatusMsg("OK");
		res->setContent_type("text/plain");
		string filen = req->getCntxt_root()+"/tokens";
		ofstream ofs(filen.c_str(),ios_base::app);
		string oauthtok,oauthsec;
		oauthtok = boost::lexical_cast<string>(Timer::getCurrentTime());
		oauthsec = boost::lexical_cast<string>(rand()%1000 + 123453) + oauthtok + boost::lexical_cast<string>(rand()%1000 + 12353);
		string wrf = oauthtok + ":" + oauthsec;
		ofs.write(wrf.c_str(),wrf.length());
		ofs.close();
		res->setContent("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec);
		cout << "verified request token signature is valid" << endl;
		return true;
	}
	else
	{
		res->setStatusCode("401");
		res->setStatusMsg("Unauthorized\r\nWWW-Authenticate: OAuth realm=\""+oauthparams["realm"]+"\"");
		res->setContent_type("text/plain");
		res->setContent("oauth_token=ab3cd9j4ks73hf7g&oauth_token_secret=xyz4992k83j47x0b");
		cout << "verified request token signature is invalid" << endl;
		return true;
	}
}
