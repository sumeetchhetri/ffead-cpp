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
	map<string,string> reqparams = req->getAllParams();
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

	string key,tokk,resu,csec;
	bool isreqtype = false;
	bool flag = true;
	string conss;
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
		cout << allpars << allparst << endl;
		allpars += CryptoHandler::urlEncode(allparst);
		cout << allpars << endl;
		cout << "key = " << key << endl;
		char* resst = (char*)CryptoHandler::hmac_sha1((char*)allpars.c_str(),(char*)key.c_str(),true);
		cout << "done crypto " << resst << endl;
		resu.append(resst);
	}

	cout << "encoded sig = " << reqparams["oauth_signature"] << endl;
	string signature = CryptoHandler::urlDecode(reqparams["oauth_signature"]);
	cout << "decoded sig = " << signature << endl;
	if(flag && resu==signature && signature!="" && resu!="")
	{
		res->setStatusCode("200");
		res->setStatusMsg("OK");
		res->setContent_type("text/plain");
		string filen;
		if(tokk=="" && req->getFile()=="request.oauth")
		{
			filen = req->getCntxt_root()+"/tokens";
			ofstream ofs(filen.c_str());
			string oauthtok,oauthsec;
			oauthtok = boost::lexical_cast<string>(Timer::getCurrentTime());
			oauthsec = boost::lexical_cast<string>(rand()%1000 + 123453) + oauthtok + boost::lexical_cast<string>(rand()%1000 + 12353);
			string wrf = oauthtok + ":" + key + oauthsec + "\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			string parsc;
			for(it=reqparams.begin();it!=reqparams.end();it++)
			{
				if(it->first!="realm" && it->first.find("oauth_")==string::npos && it->first!="Method")
				{
					parsc.append(it->first + "=" + it->second + "&");
				}
			}
			if(reqparams["oauth_callback"]!="")
			{
				filen = req->getCntxt_root()+"/callbacks";
				ofstream ofs1(filen.c_str());
				string oauth_ver = oauthtok + boost::lexical_cast<string>(rand()%1000);
				wrf = CryptoHandler::urlDecode(reqparams["oauth_callback"])+"?oauth_verifier="+oauth_ver+"&"+parsc;
				ofs1.write(wrf.c_str(),wrf.length());
				ofs1.close();
			}
			string cont = ("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec+"&")+parsc;
			if(cont[cont.length()-1]=='&')
				cont = cont.substr(0,cont.length()-1);
			res->setContent_str(cont);
			cout << "verified initial request signature is valid" << endl;
			cout << "provided a request token" << endl;
		}
		else if(isreqtype && reqparams["oauth_token"]!="" && reqparams["oauth_consumer_key"]!=""
				&& req->getFile()=="access.oauth")
		{
			filen = req->getCntxt_root()+"/access_tokens";
			ofstream ofs(filen.c_str());
			string oauthtok,oauthsec;
			oauthtok = boost::lexical_cast<string>(Timer::getCurrentTime());
			oauthsec = boost::lexical_cast<string>(rand()%1000 + 123453) + oauthtok + boost::lexical_cast<string>(rand()%1000 + 12353);
			string wrf = oauthtok + ":" + csec + oauthsec + "\n";
			ofs.write(wrf.c_str(),wrf.length());
			ofs.close();
			string cont = ("oauth_token="+oauthtok+"&oauth_token_secret="+oauthsec+"&");
			for(it=reqparams.begin();it!=reqparams.end();it++)
			{
				if(it->first!="realm" && it->first.find("oauth_")==string::npos && it->first!="Method")
				{
					cont.append(it->first + "=" + it->second + "&");
				}
			}
			if(cont[cont.length()-1]=='&')
				cont = cont.substr(0,cont.length()-1);
			res->setContent_str(cont);
			cout << "verified request token signature is valid" << endl;
			cout << "provided an access token" << endl;
		}
		else if(!isreqtype && req->getFile()=="getResource.oauth" && reqparams["file"]!="")
		{
			cout << "resource access granted for " << reqparams["file"] << endl;
			res->setStatusCode("");
			res->setStatusMsg("");
			res->setContent_type("");
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
			res->setStatusCode("401");
			res->setStatusMsg("Unauthorized\r\nWWW-Authenticate: OAuth realm=\""+url+"\"");
			res->setContent_type("text/plain");
			cout << "invalid username/password" << endl;
		}
		else if(key==reqparams["password"])
		{
			cout << "valid username/password" << endl;
			if(reqparams["oauthparms"]!="")
			{
				res->setStatusCode("303");
				res->setStatusMsg("Moved Permanently");
				res->setLocation(CryptoHandler::urlDecode(reqparams["oauthparms"])+"&access=true");
				cout << "redirecting to callback url" << endl;
			}
			else
			{
				res->setContent_str(reqparams["oauthparms"]+"&access=true");
				cout << "no callback url specified sending access info in content" << endl;
			}
		}
	}
	else if(req->getFile()=="login.oauth")
	{
		string filen = req->getCntxt_root()+"/callbacks";
		ifstream ifs(filen.c_str());
		string wrf;
		getline(ifs,wrf);
		ifs.close();
		if(reqparams["oauth_token"]!="")
			wrf = wrf+"oauth_token="+reqparams["oauth_token"];
		string html = "<html><head></head><body><form name=\"name1\" method=\"POST\" action=\"/login.oauth\">";
		html += "<input name='oauthparms' type='hidden' value='"+wrf+"'/>";
		html += "Username:<input name='username' type='text'/>";
		html += "Password:<input name='password' type='password'/>";
		html += "<input type='submit' value='Submit'/>";
		html += "</form></body></html>";
		res->setStatusCode("200");
		res->setStatusMsg("OK");
		res->setContent_type("text/html");
		res->setContent_str(html);
		cout << "Login page display" << endl;
	}
	else
	{
		res->setStatusCode("401");
		res->setStatusMsg("Unauthorized\r\nWWW-Authenticate: OAuth realm=\""+url+"\"");
		cout << "verified request token signature is invalid" << endl;
		return true;
	}
}
