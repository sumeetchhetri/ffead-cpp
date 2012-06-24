/*
	Copyright 2010, Sumeet Chhetri

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
 * ServiceTask.cpp
 *
 *  Created on: 20-Jun-2012
 *      Author: sumeetc
 */

#include "ServiceTask.h"

ServiceTask::ServiceTask(int fd,string serverRootDirectory,map<string,string> *params,
		bool isSSLEnabled, SSL_CTX *ctx, SSLHandler sslHandler, ConfigurationData configData, void* dlib) {
	this->fd=fd;this->serverRootDirectory=serverRootDirectory;
	this->params= params;
	this->isSSLEnabled = isSSLEnabled;
	this->ctx = ctx;
	this->sslHandler = sslHandler;
	this->configData = configData;
	this->dlib = dlib;
}

ServiceTask::~ServiceTask() {
	// TODO Auto-generated destructor stub
}

void ServiceTask::writeToSharedMemeory(string sessionId, string value,bool napp)
{
	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
	cout << "saving session to file " << filen << endl;
	ofstream ofs;
	if(napp)
		ofs.open(filen.c_str());
	else
		ofs.open(filen.c_str(),ios_base::app);
	ofs.write(value.c_str(),value.length());
	ofs.close();
}

map<string,string> ServiceTask::readFromSharedMemeory(string sessionId)
{
	map<string,string> valss;
	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
	ifstream ifs(filen.c_str());
	string tem,all;
	while(getline(ifs,tem))
		all.append(tem+"\n");
	strVec results;
	boost::iter_split(results, all, boost::first_finder("; "));
	for(int j=0;j<(int)results.size()-1;j++)
	{
		if(results.at(j)=="")continue;
		strVec results1;
		boost::replace_all(results.at(j),"%3B%20","; ");
		boost::iter_split(results1, results.at(j), boost::first_finder("="));
		if(results1.size()==2)
		{
			boost::replace_all(results1.at(0),"%3D","=");
			boost::replace_all(results1.at(1),"%3D","=");
			valss[results1.at(0)] = results1.at(1);
		}
		else
		{
			boost::replace_all(results1.at(0),"%3D","=");
			valss[results1.at(0)] = "true";
		}
		cout << "read key/value pair " << results1.at(0) << " = " << valss[results1.at(0)] << endl;
	}
	return valss;
}

string ServiceTask::getFileExtension(string file)
{
	if(file.find_last_of(".")!=string::npos)return file.substr(file.find_last_of("."));
	return file;
}

void ServiceTask::createResponse(HttpResponse &res,bool flag,map<string,string> vals,string prevcookid, long sessionTimeout, bool sessatserv)
{
	if(flag)
	{
		string values;
		cout << "session object modified " << vals.size() << endl;
		Date date;
		string id = boost::lexical_cast<string>(Timer::getCurrentTime());
		int seconds = sessionTimeout;
		date = date.addSeconds(sessionTimeout);
		DateFormat dformat("ddd, dd-mmm-yyyy hh:mi:ss");
		map<string,string>::iterator it;
		for(it=vals.begin();it!=vals.end();it++)
		{
			string key = it->first;
			string value = it->second;
			boost::replace_all(key,"; ","%3B%20");
			boost::replace_all(key,"=","%3D");
			boost::replace_all(value,"; ","%3B%20");
			boost::replace_all(value,"=","%3D");
			cout << it->first << " = " << it->second << endl;
			if(!sessatserv)
				res.addCookie(key + "=" + value + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
			else
			{
				values += key + "=" + value + "; ";
			}
		}
		if(sessatserv)
		{
			if(values!="")
			{
				if(prevcookid!="")
					writeToSharedMemeory(prevcookid,values,true);
				else
				{
					writeToSharedMemeory(id,values,false);
					res.addCookie("FFEADID=" + id + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
				}
			}
		}
	}
}

string ServiceTask::getContentStr(string url,string locale,string ext)
{
	cout << "content request for -- " << url << " " << ext << endl;
	string all;
    string fname = url;
	if (url=="/")
    {
       return all;
    }
    ifstream myfile;
    if(locale.find("english")==string::npos && (ext==".html" || ext==".htm"))
    {
            string fnj = fname;
            boost::replace_first(fnj,".",("_" + locale+"."));
            myfile.open(fnj.c_str(),ios::in | ios::binary);
			if (myfile.is_open())
			{
				string line;
				while(getline(myfile,line)){all.append(line+"\n");}
				myfile.close();
				return all;
			}
    }
	ifstream myfile1;
    myfile1.open(fname.c_str(),ios::in | ios::binary);
    if (myfile1.is_open())
    {
		string line;
		while(getline(myfile1,line)){all.append(line+"\n");}
        myfile1.close();
    }
    return all;
}

void ServiceTask::run()
{
	//cout << dlib << endl;
	string ip = "invalid session";
	string alldatlg = "\ngot fd from parent";
	SSL *ssl=NULL;
	BIO *sbio=NULL;
	BIO *io=NULL,*ssl_bio=NULL;
	try
	{
		int cntlen = 0;
		char buf[MAXBUFLENM];
		strVec results;
		stringstream ss;
		string temp;
		//int bytes = -1;
		if(isSSLEnabled)
		{
			sbio=BIO_new_socket(fd,BIO_CLOSE);
			//cout << "\nBefore = " << ssl << flush;
			ssl=SSL_new(ctx);
			//cout << "\nAfter = " << ssl << flush;
			SSL_set_bio(ssl,sbio,sbio);
			int r;
			if((r=SSL_accept(ssl)<=0))
			{
				sslHandler.error_occurred((char*)"SSL accept error",fd,ssl);
				return;
			}

			io=BIO_new(BIO_f_buffer());
			ssl_bio=BIO_new(BIO_f_ssl());
			BIO_set_ssl(ssl_bio,ssl,BIO_CLOSE);
			BIO_push(io,ssl_bio);
			int er=-1;
			bool flag = true;
			while(flag)
			{
				er = BIO_gets(io,buf,BUFSIZZ-1);
				switch(SSL_get_error(ssl,er))
				{
					case SSL_ERROR_NONE:
						break;
					case SSL_ERROR_ZERO_RETURN:
					{
						sslHandler.error_occurred((char*)"SSL error problem",fd,ssl);
						if(io!=NULL)BIO_free(io);
						return;
					}
					default:
					{
						sslHandler.error_occurred((char*)"SSL read problem",fd,ssl);
						if(io!=NULL)BIO_free(io);
						return;
					}
				}
				ss << buf;
				//cout <<buf <<endl;
				if(!strcmp(buf,"\r\n") || !strcmp(buf,"\n"))
					break;
				string temp(buf);
				temp = temp.substr(0,temp.length()-1);
				results.push_back(temp);
				//cout << temp <<endl;
				if(temp.find("Content-Length:")!=string::npos)
				{
					std::string cntle = temp.substr(temp.find(": ")+2);
					cntle = cntle.substr(0,cntle.length()-1);
					//cout << "contne-length="<<cntle <<endl;
					try
					{
						cntlen = boost::lexical_cast<int>(cntle);
					}
					catch(boost::bad_lexical_cast&)
					{
						cout << "bad lexical cast" <<endl;
					}
				}
				memset(&buf[0], 0, sizeof(buf));
			}
		}
		else
		{
			int er=-1;
			bool flag = true;
			sbio=BIO_new_socket(fd,BIO_CLOSE);
			io=BIO_new(BIO_f_buffer());
			BIO_push(io,sbio);
			cout << "into run method" << endl;
			while(flag)
			{
				er = BIO_gets(io,buf,BUFSIZZ-1);
				if(er==0)
				{
					close(fd);
					cout << "\nsocket closed before being serviced" <<flush;
					return;
				}
				ss << buf;
				if(!strcmp(buf,"\r\n") || !strcmp(buf,"\n") || er<0)
					break;
				string temp(buf);
				temp = temp.substr(0,temp.length()-1);
				results.push_back(temp);
				//cout << temp <<endl;
				if(temp.find("Content-Length:")!=string::npos)
				{
					std::string cntle = temp.substr(temp.find(": ")+2);
					cntle = cntle.substr(0,cntle.length()-1);
					//cout << "contne-length="<<cntle <<endl;
					try
					{
						cntlen = boost::lexical_cast<int>(cntle);
					}
					catch(boost::bad_lexical_cast&)
					{
						cout << "bad lexical cast" <<endl;
					}
				}
				memset(&buf[0], 0, sizeof(buf));
			}
		}

		ss.clear();
		if(isSSLEnabled && cntlen>0)
		{
			int er=-1;
			if(cntlen>0)
			{
				//cout << "reading conetnt " << cntlen << endl;
				er = BIO_read(io,buf,cntlen);
				switch(SSL_get_error(ssl,er))
				{
					case SSL_ERROR_NONE:
						cntlen -= er;
						break;
					case SSL_ERROR_ZERO_RETURN:
					{
						sslHandler.error_occurred((char*)"SSL error problem",fd,ssl);
						if(io!=NULL)BIO_free(io);
						return;
					}
					default:
					{
						sslHandler.error_occurred((char*)"SSL read problem",fd,ssl);
						if(io!=NULL)BIO_free(io);
						return;
					}
				}
				string temp(buf);
				results.push_back("\r");
				results.push_back(temp);
				//cout <<buf <<endl;
				memset(&buf[0], 0, sizeof(buf));
			}
		}
		else if(cntlen>0)
		{
			int er=-1;
			if(cntlen>0)
			{
				//cout << "reading conetnt " << cntlen << endl;
				er = BIO_read(io,buf,cntlen);
				if(er==0)
				{
					close(fd);
					cout << "\nsocket closed before being serviced" <<flush;
					return;
				}
				else if(er>0)
				{
					string temp(buf);
					results.push_back("\r");
					results.push_back(temp);
					//cout << temp <<endl;
					memset(&buf[0], 0, sizeof(buf));
				}
			}
		}
		alldatlg += "--read data";
		map<string,string> params1 = *params;
		string webpath = serverRootDirectory + "web/";
		HttpRequest* req= new HttpRequest(results,webpath);

		if(req->getFile()=="")
		{
			cout << req->getFile() << endl;
			req->setFile("index.html");
		}
		if(req->hasCookie())
		{
			cout << "has the session id" << endl;
			if(!configData.sessatserv)
				req->getSession()->setSessionAttributes(req->getCookieInfo());
			else
			{
				string id = req->getCookieInfoAttribute("FFEADID");
				cout << id << endl;
				map<string,string> values = readFromSharedMemeory(id);
				req->getSession()->setSessionAttributes(values);
			}
		}

		if(configData.cntMap[req->getCntxt_name()]!="true")
		{
			req->setCntxt_name("default");
			req->setCntxt_root(webpath+"default");
			req->setUrl(webpath+"default"+req->getActUrl());
		}
		//cout << req->getCntxt_name() << req->getCntxt_root() << req->getUrl() << endl;

		if(configData.appMap[req->getCntxt_name()]!="false")
		{
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string meth1 = (req->getCntxt_name()+"checkRules");
			string path1;
			void *mkr1 = dlsym(dlib, meth1.c_str());
			if(mkr1!=NULL)
			{
				typedef string (*DCPPtr1) (string,HttpSession);
				DCPPtr1 f =  (DCPPtr1)mkr1;
				path1 = f(req->getUrl(),*(req->getSession()));
				//cout << path1 << flush;
				if(path1=="FAILED")
				{
					req->setUrl("");
				}
				else if(path1!="" && path1!=req->getUrl())
				{
					req->setUrl(path1);
				}
			}
		}

		HttpResponse res;
		string ext = getFileExtension(req->getUrl());
		vector<unsigned char> test;
		string content;
		string claz;
		bool isoAuthRes = false;
		long sessionTimeoutVar = configData.sessionTimeout;
		bool isContrl = securityHandler.handle(configData.ip_address, req, res, configData.securityObjectMap, sessionTimeoutVar, dlib, configData.cntMap);

		filterHandler.handleIn(req, res, configData.filterMap, dlib, ext);

		if(!isContrl)
		{
			isContrl = authHandler.handle(configData.autMap, configData.autpattMap, req, res, configData.filterMap, dlib, ext);
		}
		string pthwofile = req->getCntxt_name()+req->getActUrl();
		if(req->getCntxt_name()!="default" && configData.cntMap[req->getCntxt_name()]=="true")
		{
			pthwofile = req->getActUrl();
		}
		if(!isContrl)
		{
			isContrl = controllerHandler.handle(req, res, configData.urlpattMap, configData.mappattMap, dlib, ext,
					configData.rstCntMap, configData.mapMap, configData.urlMap, pthwofile);
		}

		/*After going through the controller the response might be blank, just set the HTTP version*/
		res.setHttpVersion(req->getHttpVersion());
		//cout << req->toString() << endl;
		if(isContrl)
		{

		}
		else if(ext==".form")
		{
			formHandler.handle(req, res, configData.formMap, dlib);
		}
		else if((req->getContent_type().find("application/soap+xml")!=string::npos || req->getContent_type().find("text/xml")!=string::npos)
				&& (req->getContent().find("<soap:Envelope")!=string::npos || req->getContent().find("<soapenv:Envelope")!=string::npos)
				&& configData.wsdlmap[req->getFile()]==req->getCntxt_name())
		{
			soapHandler.handle(req, res, dlib, configData.props[".xml"]);
		}
		else
		{
			bool cntrlit = scriptHandler.handle(req, res, configData.handoffs, dlib, ext, configData.props);
			cout << "html page requested" <<endl;
			if(cntrlit)
			{

			}
			else
			{
				cntrlit = extHandler.handle(req, res, dlib, configData.resourcePath, configData.tmplMap, configData.vwMap, ext, configData.props);
			}
			if(!cntrlit && ext==".fview")
			{
				content = fviewHandler.handle(req, res, configData.fviewmap);
			}
			else
			{
				if(res.getContent_str()=="")
					content = getContentStr(req->getUrl(),configData.lprops[req->getDefaultLocale()],ext);
				else
					content = res.getContent_str();
			}
			if(content.length()==0)
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				//res.setContent_len(boost::lexical_cast<string>(0));
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				if(res.getContent_type()=="")res.setContent_type(configData.props[ext]);
				res.setContent_str(content);
				//res.setContent_len(boost::lexical_cast<string>(content.length()));
				//sess.setAttribute("CURR",req->getUrl());
			}
		}

		filterHandler.handleOut(req, res, configData.filterMap, dlib, ext);

		alldatlg += "--processed data";
		string h1;
		bool sessionchanged = !req->hasCookie();
		sessionchanged |= req->getSession()->isDirty();
		if(req->getConnection()!="")
			res.setConnection("close");
		createResponse(res,sessionchanged,req->getSession()->getSessionAttributes(),req->getCookieInfoAttribute("FFEADID"), sessionTimeoutVar, configData.sessatserv);
		h1 = res.generateResponse();
		//cout << h1 << endl;
		if(isSSLEnabled)
		{
			int r;
			/* Now perform renegotiation if requested */
			if(configData.client_auth==CLIENT_AUTH_REHANDSHAKE){
			  SSL_set_verify(ssl,SSL_VERIFY_PEER |
				SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);

			  /* Stop the client from just resuming the
				 un-authenticated session */
			  SSL_set_session_id_context(ssl,
				(const unsigned char*)&SSLHandler::s_server_auth_session_id_context,
				sizeof(SSLHandler::s_server_auth_session_id_context));

			  if(SSL_renegotiate(ssl)<=0)
			  {
				  sslHandler.error_occurred((char*)"SSL renegotiation error",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			  }
			  if(SSL_do_handshake(ssl)<=0)
			  {
				  sslHandler.error_occurred((char*)"SSL renegotiation error",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			  }
			  ssl->state=SSL_ST_ACCEPT;
			  if(SSL_do_handshake(ssl)<=0)
			  {
				  sslHandler.error_occurred((char*)"SSL renegotiation error",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			  }
			}
			if((r=BIO_puts(io,h1.c_str()))<=0)
			{
				  sslHandler.error_occurred((char*)"send failed",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			}
			if((r=BIO_flush(io))<0)
			{
				  sslHandler.error_occurred((char*)"Error flushing BIO",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			}
			sslHandler.closeSSL(fd,ssl,io);
		}
		else
		{
			int size;
			if ((size=send(fd,&h1[0] , h1.length(), 0)) == -1)
				cout << "send failed" << flush;
			else if(size==0)
			{
				close(fd);
				memset(&buf[0], 0, sizeof(buf));
				cout << "socket closed for writing" << flush;
				return;
			}

			if(io!=NULL)BIO_free_all(io);
		}
		close(fd);
		memset(&buf[0], 0, sizeof(buf));
		ss.clear();

		//Logger::info("got new connection to process\n"+req->getFile()+" :: " + res.getStatusCode() + "\n"+req->getCntxt_name() + "\n"+req->getCntxt_root() + "\n"+req->getUrl());
		delete req;
		cout << alldatlg << "--sent data--DONE" << flush;
		//sessionMap[sessId] = sess;
	}
	catch(...)
	{
		cout << "Standard exception: " << endl;
	}
}


