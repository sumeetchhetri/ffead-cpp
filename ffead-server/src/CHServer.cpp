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
 * CHServer.cpp
 *
 *  Created on: Aug 25, 2009
 *      Author: sumeet
 */

#include "CHServer.h"


CHServer::CHServer()
{}

CHServer::~CHServer() {
	// TODO Auto-generated destructor stub
}
SharedData* SharedData::shared_instance = NULL;
string servd;
static propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap;
static map<string, vector<string> > filterMap;
static string resourcePath;
static strVec dcpsss;
static bool isSSLEnabled,isThreadprq,processforcekilled,processgendone,sessatserv,isCompileEnabled;
static int thrdpsiz,shmid;
static SSL_CTX *ctx;
static int s_server_session_id_context = 1;
static int s_server_auth_session_id_context = 2;
static int client_auth=0;
static char *ciphers=0;
static BIO *bio_err=0;
static char *pass;
map<int,pid_t> pds;
static pid_t parid;
void *dlib = NULL;
static string key_file,dh_file,ca_list,rand_file,sec_password,srv_auth_prvd,srv_auth_mode,srv_auth_file;
typedef map<string,string> sessionMap;
static boost::mutex m_mutex,p_mutex;

void writeToSharedMemeory(string sessionId, string value,bool napp)
{
	string filen = servd+"/tmp/"+sessionId+".sess";
	ofstream ofs;
	if(napp)
		ofs.open(filen.c_str());
	else
		ofs.open(filen.c_str(),ios_base::app);
	ofs.write(value.c_str(),value.length());
	ofs.close();
}

map<string,string> readFromSharedMemeory(string sessionId)
{
	map<string,string> valss;
	string filen = servd+"/tmp/"+sessionId+".sess";
	ifstream ifs(filen.c_str());
	string tem,all;
	while(getline(ifs,tem))
		all.append(tem+"\n");
	strVec results;
	boost::iter_split(results, all, boost::first_finder("; "));
	for(unsigned j=0;j<(int)results.size()-1;j++)
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
	}
	return valss;
}

/*The password code is not thread safe*/
static int password_cb(char *buf,int num,
  int rwflag,void *userdata)
  {
    if(num<strlen(pass)+1)
      return(0);

    strcpy(buf,pass);
    return(strlen(pass));
  }

void load_dh_params(SSL_CTX *ctx,char *file)
{
    DH *ret=0;
    BIO *bio;

    if ((bio=BIO_new_file(file,"r")) == NULL)
    	cout << "Couldn't open DH file" << flush;

    ret=PEM_read_bio_DHparams(bio,NULL,NULL,
      NULL);
    BIO_free(bio);
    if(SSL_CTX_set_tmp_dh(ctx,ret)<0)
    	cout << "Couldn't set DH parameters" << flush;
  }

static void sigpipe_handle(int x){
}
SSL_CTX *initialize_ctx(char *keyfile,char *password)
  {
    SSL_METHOD *meth;
    SSL_CTX *ctx;

    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();

      /* An error write context */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    signal(SIGPIPE,sigpipe_handle);

    /* Create our context*/
    meth=(SSL_METHOD*)SSLv23_method();
    ctx=SSL_CTX_new(meth);

    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_chain_file(ctx,
      keyfile)))
    	cout << "Can't read certificate file" << flush;

    pass=password;
    SSL_CTX_set_default_passwd_cb(ctx,
      password_cb);
    if(!(SSL_CTX_use_PrivateKey_file(ctx,
      keyfile,SSL_FILETYPE_PEM)))
    	cout << "Can't read key file" << flush;

    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx,
      ca_list.c_str(),0)))
    	cout << "Can't read CA list" << flush;
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx,1);
#endif

    return ctx;
  }

void destroy_ctx(SSL_CTX *ctx)
  {
    SSL_CTX_free(ctx);
  }

void error_occurred(char *error,int fd,SSL *ssl)
{
	cout << error << flush;
	close(fd);
	int r=SSL_shutdown(ssl);
	if(!r){
	  /* If we called SSL_shutdown() first then
		 we always get return value of '0'. In
		 this case, try again, but first send a
		 TCP FIN to trigger the other side's
		 close_notify*/
	  shutdown(fd,1);
	  r=SSL_shutdown(ssl);
	}
	switch(r){
	  case 1:
		break; /* Success */
	  case 0:
	  case -1:
	  default:
		  cout << "shutdown failed" << flush;
	}
	SSL_free(ssl);
}

void closeSSL(int fd,SSL *ssl,BIO* bio)
{
	BIO_free(bio);
	int r=SSL_shutdown(ssl);
	if(!r){
	  /* If we called SSL_shutdown() first then
		 we always get return value of '0'. In
		 this case, try again, but first send a
		 TCP FIN to trigger the other side's
		 close_notify*/
	  shutdown(fd,1);
	  r=SSL_shutdown(ssl);
	}
	switch(r){
	  case 1:
		break; /* Success */
	  case 0:
	  case -1:
	  default:
		  cout << "shutdown failed" << flush;
	}
	SSL_free(ssl);
}

char *unbase64(unsigned char *input, int length)
{
  BIO *b64, *bmem;

  char *buffer = (char *)malloc(length);
  memset(buffer, 0, length);

  b64 = BIO_new(BIO_f_base64());
  bmem = BIO_new_mem_buf(input, length);
  bmem = BIO_push(b64, bmem);

  BIO_read(bmem, buffer, length);

  BIO_free_all(bmem);

  return buffer;
}

char *base64(const unsigned char *input, int length)
{
  BIO *bmem, *b64;
  BUF_MEM *bptr;

  b64 = BIO_new(BIO_f_base64());
  bmem = BIO_new(BIO_s_mem());
  b64 = BIO_push(b64, bmem);
  BIO_write(b64, input, length);
  BIO_flush(b64);
  BIO_get_mem_ptr(b64, &bptr);

  char *buff = (char *)malloc(bptr->length);
  memcpy(buff, bptr->data, bptr->length-1);
  buff[bptr->length-1] = 0;

  BIO_free_all(b64);

  return buff;
}

string createResponse(string htm,string url)
{
	string data;
	stringstream ss;
	ss << htm.length();

	string temp;
	ss >> temp;
	size_t tes;
	if((tes=url.find(".html"))!=string::npos)
	{
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: "+temp+"\n\n"+htm+"\n";
		//cout << "HTML DATA" <<flush;
	}
	else if(htm.size()!=0)
	{
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: image/jpeg\nContent-Length: "+temp+"\n\n"+htm+"\n";
		//cout << "other DATA" <<flush;
	}
	return data;
}

string getFileExtension(const string& file)
{
	string str = file;
	string ext = "";
	for(unsigned int i=0; i<str.length(); i++)
	{
		if(str[i] == '.')
		{
			for(unsigned int j = i; j<str.length(); j++)
			{
				ext += str[j];
			}
			return ext;
		}
	}
	return ext;
}


string createResponseVec(string len,string url)
{
	string data;
	//size_t tes;
	string ext = getFileExtension(url);
	PropFileReader pf;
	propMap props = pf.getProperties("resources/mime-types.prop");
	string contype = props[ext];

	//cout << props.size() << flush;
	/*if((tes=url.find(".html"))!=string::npos)
	{
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\n\nContent-Length: "+len+"\n\n";
		cout << "HTML DATA" <<flush;
	}
	else if((tes=url.find(".jpg"))!=string::npos)
	{
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: image/jpeg\nContent-Length: "+len+"\n\n";
		cout << "other DATA" <<flush;
	}
	else if((tes=url.find(".txt"))!=string::npos)
	{
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/plain\nContent-Length: "+len+"\n\n";
		cout << "other DATA" <<flush;
	}
	else if((tes=url.find(".xml"))!=string::npos)
	{
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/xml\nContent-Length: "+len+"\n\n";
		cout << "other DATA" <<flush;
	}
	else if((tes=url.find(".jar"))!=string::npos)
	{*/
		data = "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nConnection: close\nContent-Type: "+contype+"\nContent-Length: "+len+"\n\n";
		//cout << "other DATA" <<flush;
	//}
	return data;
}

string getContent(string url)
{
	string all;
	string line;
	string fname = "/home/sumeet/"+url;

	if (url=="/")
	{
		//cout << "\nURL is" << fname << flush;
		return all;
	}
	//Magick::Image image("/home/sumeet/test123.jpg");
    ifstream myfile (&fname[0],ios::in | ios::binary);
    stringstream ss;
    if (myfile.is_open())
    {
	  while (!myfile.eof())
	  {
		  getline(myfile,line);
		  //char c = getchar(myfile);
		  //ss << c;
		  all += line;
	  }
	  myfile.close();
    }
    else
    {
    	//cout << "Unable to open file";
    }
    //ss >> all;
    return all;
}

vector<unsigned char> getContentVec(string url,string locale,string ext)
{
	vector<unsigned char> all;
	string fname = ""+url;

	if (url=="/")
	{
		//cout << "\nURL is" << fname << flush;
		return all;
	}
	ifstream myfile;
	//Magick::Image image("/home/sumeet/test123.jpg");
	if(locale.find("english")==string::npos && (ext==".html" || ext==".htm"))
	{
		string fnj = fname;
		boost::replace_first(fnj,".",("_" + locale+"."));
		myfile.open(&fnj[0],ios::in | ios::binary);
		//cout << fnj << flush;
	}
	if(!myfile.is_open())
		myfile.open(&fname[0],ios::in | ios::binary);
    unsigned char byte = '\0';
    if (myfile.is_open())
    {
      while (myfile.read((char*)&byte, sizeof(byte)))
	  {
		  all.push_back(byte);
	  }
	  myfile.close();
    }
    else
    {
    	//cout << "Unable to open file";
    }
    //ss >> all;
    return all;
}

string getContentStr(string url,string locale,string ext)
{
	string all;
	string fname = ""+url;

	if (url=="/")
	{
		//cout << "\nURL is" << fname << flush;
		return all;
	}
	ifstream myfile;
	//Magick::Image image("/home/sumeet/test123.jpg");
	if(locale.find("english")==string::npos && (ext==".html" || ext==".htm"))
	{
		string fnj = fname;
		boost::replace_first(fnj,".",("_" + locale+"."));
		myfile.open(&fnj[0],ios::in | ios::binary);
		//cout << fnj << flush;
	}
	if(!myfile.is_open())
		myfile.open(&fname[0],ios::in | ios::binary);
    unsigned char byte = '\0';
    if (myfile.is_open())
    {
      while (myfile.read((char*)&byte, sizeof(byte)))
	  {
		  all.push_back(byte);
	  }
	  myfile.close();
    }
    else
    {
    	//cout << "Unable to open file";
    }
    //ss >> all;
    return all;
}

void createResponse(HttpResponse &res,bool flag,map<string,string> vals,string prevcookid)
{
	if(flag)
	{
		string values;
		cout << "session object modified " << vals.size() << endl;
		Date date;
		string id = boost::lexical_cast<string>(Timer::getCurrentTime());
		date.setHh(date.getHh()+6);
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

void listi(string cwd,string type,bool apDir,strVec &folders)
{
	FILE *pipe_fp;
	string command;
	command = ("ls -F1 "+cwd+"|grep '"+type+"'");
	cout << "\nCommand:" << command << flush;
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
	}
	int t_char;
	string folderName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			stringstream ss;
			ss << (char)t_char;
			string temp;
			ss >> temp;
			folderName.append(temp);
		}
		else if(folderName!="")
		{
			boost::replace_first(folderName,"*","");
			if(folderName.find("~")==string::npos)
			{
				cout << "\nlist for file" << (cwd+"/"+folderName) << "\n" << flush;
				if(apDir)
					folders.push_back(cwd+folderName);
				else
					folders.push_back(folderName);
			}
			folderName = "";
		}
	}
	pclose(pipe_fp);
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
		int bytes = -1;
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
				error_occurred("SSL accept error",fd,ssl);
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
						error_occurred("SSL error problem",fd,ssl);
						if(io!=NULL)BIO_free(io);
						return;
					}
					default:
					{
						error_occurred("SSL read problem",fd,ssl);
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
						error_occurred("SSL error problem",fd,ssl);
						if(io!=NULL)BIO_free(io);
						return;
					}
					default:
					{
						error_occurred("SSL read problem",fd,ssl);
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
			req->setFile("index.html");
		}
		if(req->hasCookie())
		{
			if(!sessatserv)
				req->getSession()->setSessionAttributes(req->getCookieInfo());
			else
			{
				string id = req->getCookieInfo()["FFEADID"];
				map<string,string> values = readFromSharedMemeory(id);
				req->getSession()->setSessionAttributes(values);
			}
		}

		if(cntMap[req->getCntxt_name()]!="true")
		{
			req->setCntxt_name("default");
			req->setCntxt_root(webpath+"default");
			req->setUrl(webpath+"default"+req->getActUrl());
		}
		//cout << req->getCntxt_name() << req->getCntxt_root() << req->getUrl() << endl;

		if(appMap[req->getCntxt_name()]!="false")
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
		//if(ext!=".dcp" && ext!=".view" && ext!=".tpe" && ext!=".wsdl")
		//	test = getContentVec(req->getUrl(),lprops[req->getDefaultLocale()],ext);

		string claz;
		//cout << urlpattMap["*.*"] << flush;
		bool isAuthenticated = false;
		bool isoAuthRes = false;
		bool isContrl = false;

		if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"in")!=filterMap.end())
		{
			vector<string> tempp;
			if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end())
				tempp = filterMap[req->getCntxt_name()+"*.*in"];
			else
				tempp = filterMap[req->getCntxt_name()+ext+"in"];

			for (int var = 0; var < tempp.size(); ++var)
			{
				string clasz = tempp.at(var);
				clasz = "getReflectionCIFor" + clasz;
				cout << "filter handled by class " << clasz << endl;
				if(dlib == NULL)
				{
					cerr << dlerror() << endl;
					exit(-1);
				}
				void *mkr = dlsym(dlib, clasz.c_str());
				if(mkr!=NULL)
				{
					FunPtr f =  (FunPtr)mkr;
					ClassInfo srv = f();
					args argus;
					Constructor ctor = srv.getConstructor(argus);
					Reflector ref;
					void *_temp = ref.newInstanceGVP(ctor);
					Filter *filter = (Filter*)_temp;
					filter->doInputFilter(req);
					cout << "filter called" << endl;
					delete _temp;
				}
			}
		}
		if(autpattMap[req->getCntxt_name()+"*.*"]!="" || autMap[req->getCntxt_name()+ext]!="")
		{
			if(autpattMap[req->getCntxt_name()+"*.*"]!="")
			{
				claz = autpattMap[req->getCntxt_name()+"*.*"];
			}
			else
			{
				claz = autMap[req->getCntxt_name()+ext];
			}
			AuthController *authc;
			cout << "OAUTH/HTTP Authorization requested " <<  claz << endl;
			map<string,string>::iterator it;
			map<string,string> tempmap = req->getAuthinfo();
			for(it=tempmap.begin();it!=tempmap.end();it++)
			{
				cout << it->first << " = " << it->second << endl;
			}
			map<string,string> tempmap1 = req->getRequestParams();
			for(it=tempmap1.begin();it!=tempmap1.end();it++)
			{
				cout << it->first << " = " << it->second << endl;
			}
			if(claz.find("file:")==0)
			{
				claz = req->getCntxt_root()+"/"+claz.substr(claz.find(":")+1);
				cout << "auth handled by file " << claz << endl;
				authc = new FileAuthController(claz,":");
				if(authc->isInitialized())
				{
					if(authc->authenticate(req->getAuthinfo()["Username"],req->getAuthinfo()["Password"]))
					{
						cout << "valid user" << endl;
					}
					else
					{
						cout << "invalid user" << endl;
						res.setStatusCode("401");
						res.setStatusMsg("Unauthorized\r\nWWW-Authenticate: Invalid authentication details");
						isContrl = true;
						cout << "verified request token signature is invalid" << endl;
					}
				}
				else
				{
					cout << "invalid user repo defined" << endl;
				}
			}
			else if(claz.find("class:")==0)
			{
				claz = claz.substr(claz.find(":")+1);
				claz = "getReflectionCIFor" + claz;
				cout << "auth handled by class " << claz << endl;
				if(dlib == NULL)
				{
					cerr << dlerror() << endl;
					exit(-1);
				}
				void *mkr = dlsym(dlib, claz.c_str());
				if(mkr!=NULL)
				{
					FunPtr f =  (FunPtr)mkr;
					ClassInfo srv = f();
					args argus;
					Constructor ctor = srv.getConstructor(argus);
					Reflector ref;
					void *_temp = ref.newInstanceGVP(ctor);
					authc = (AuthController*)_temp;
					isoAuthRes = authc->handle(req,&res);
					if(res.getStatusCode()!="")
						isContrl = true;
					cout << "authhandler called" << endl;
					ext = getFileExtension(req->getUrl());
					delete authc;
				}
			}
		}

		if(!isContrl && (urlpattMap[req->getCntxt_name()+"*.*"]!="" || urlMap[req->getCntxt_name()+ext]!=""))
		{
			//cout << "Controller requested for " << req->getCntxt_name() << " name " << urlMap[req->getCntxt_name()+ext] << endl;
			if(urlpattMap[req->getCntxt_name()+"*.*"]!="")
				claz = "getReflectionCIFor" + urlpattMap[req->getCntxt_name()+"*.*"];
			else
				claz = "getReflectionCIFor" + urlMap[req->getCntxt_name()+ext];
			string libName = "libinter.so";
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			void *mkr = dlsym(dlib, claz.c_str());
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Constructor ctor = srv.getConstructor(argus);
				Reflector ref;
				void *_temp = ref.newInstanceGVP(ctor);
				Controller *thrd = (Controller *)_temp;
				try{
					 cout << "Controller called" << endl;
					 res = thrd->service(*req);
					 cout << res.getStatusCode() << endl;
					 cout << res.getContent_type() << endl;
					 cout << res.getContent_len() << endl;
					 if(res.getStatusCode()!="")
						 isContrl = true;
					 ext = getFileExtension(req->getUrl());
					 //delete mkr;
				}catch(...){ cout << "Controller exception" << endl;}
				cout << "Controller called\n" << flush;
			}
		}
		else if(!isContrl && (mappattMap[req->getCntxt_name()+"*.*"]!="" || mapMap[req->getCntxt_name()+ext]!=""))
		{
			string file = req->getFile();
			string fili = file.substr(0,file.find_last_of("."));
			if(mappattMap[req->getCntxt_name()+"*.*"]!="")
			{
				req->setFile(fili+mappattMap[req->getCntxt_name()+"*.*"]);
				cout << "URL mapped from * to " << mappattMap[req->getCntxt_name()+"*.*"] << endl;
			}
			else
			{
				req->setFile(fili+mapMap[req->getCntxt_name()+ext]);
				cout << "URL mapped from " << ext << " to " << mapMap[req->getCntxt_name()+ext] << endl;
			}

		}

		/*After going through the controller the response might be blank, just set the HTTP version*/
		res.setHttpVersion(req->getHttpVersion());
		//cout << req->toString() << endl;
		if(isContrl)
		{

		}
		else if(req->getMethod()=="POST" && req->getRequestParam("claz")!="" && req->getRequestParam("method")!="")
		{
			content = AfcUtil::execute(*req);
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".txt"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
		else if(ext==".dcp")
		{
			string libName = "libinter.so";
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			int s = req->getUrl().find_last_of("/")+1;
			int en = req->getUrl().find_last_of(".");
			string meth,file;
			file = req->getUrl().substr(s,en-s);
			meth = "_" + file + "emittHTML";

			void *mkr = dlsym(dlib, meth.c_str());
			if(mkr!=NULL)
			{
				cout << endl << "inside dcp " << meth << endl;
				DCPPtr f =  (DCPPtr)mkr;
				content = f();
				//string patf;
				//patf = req->getCntxt_root() + "/dcp_" + file + ".html";
				//content = getContentStr(patf,lprops[req->getDefaultLocale()],ext);
				//delete mkr;
			}
			ext = ".html";
			if(ext!="" && content.length()==0)
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				//res.setContent_len("0");
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type(props[ext]);
				res.setContent_str(content);
				//res.setContent_len(boost::lexical_cast<string>(content.length()));
			}
		}
		else if(ext==".view" && vwMap[req->getCntxt_name()+req->getFile()]!="")
		{
			string libName = "libinter.so";
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			claz = "getReflectionCIFor" + vwMap[req->getCntxt_name()+req->getFile()];
			void *mkr = dlsym(dlib, claz.c_str());
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Reflector ref;
				Constructor ctor = srv.getConstructor(argus);
				void *_temp = ref.newInstanceGVP(ctor);
				DynamicView *thrd = (DynamicView *)_temp;
				Document doc = thrd->getDocument();
				View view;
				string t = view.generateDocument(doc);
				content = t;
			}
			ext = ".html";
			if(ext!="" && (content.length()==0))
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				//res.setContent_len("0");
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type(props[ext]);
				res.setContent_str(content);
				//res.setContent_len(boost::lexical_cast<string>(content.length()));
			}
		}
		else if(ext==".tpe" && tmplMap[req->getCntxt_name()+req->getFile()]!="")
		{
			TemplateEngine te;
			ext = ".html";
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			claz = "getReflectionCIFor" + tmplMap[req->getCntxt_name()+req->getFile()];
			void *mkr = dlsym(dlib, claz.c_str());
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Constructor ctor = srv.getConstructor(argus);
				Reflector ref;
				void *_temp = ref.newInstanceGVP(ctor);
				TemplateHandler *thrd = (TemplateHandler *)_temp;
				Context cnt = thrd->getContext();
				string t = te.evaluate(req->getUrl(),cnt);
				content = t;
			}
			if(ext!="" && (content.length()==0))
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				//res.setContent_len("0");
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type(props[ext]);
				res.setContent_str(content);
				//res.setContent_len(boost::lexical_cast<string>(content.length()));
			}
		}
		else if((req->getContent_type().find("application/soap+xml")!=string::npos || req->getContent_type().find("text/xml")!=string::npos)
				&& (req->getContent().find("<soap:Envelope")!=string::npos || req->getContent().find("<soapenv:Envelope")!=string::npos)
				&& wsdlmap[req->getFile()]==req->getCntxt_name())
		{
			string meth,ws_name,env;
			ws_name = req->getFile();
			Element soapenv;
			Logger::info("request => "+req->getContent());
			Element soapbody;
			try
			{
				XmlParser parser("Parser");
				Document doc = parser.getDocument(req->getContent());
				soapenv = doc.getRootElement();
				cout << soapenv.getTagName() << "----\n" << flush;

				if(soapenv.getChildElements().size()==1
						&& soapenv.getChildElements().at(0).getTagName()=="Body")
					soapbody = soapenv.getChildElements().at(0);
				else if(soapenv.getChildElements().size()==2
						&& soapenv.getChildElements().at(1).getTagName()=="Body")
					soapbody = soapenv.getChildElements().at(1);
				cout << soapbody.getTagName() << "----\n" << flush;
				Element method = soapbody.getChildElements().at(0);
				cout << method.getTagName() << "----\n" << flush;
				meth = method.getTagName();
				string methodname = meth + ws_name;
				cout << methodname << "----\n" << flush;
				void *mkr = dlsym(dlib, methodname.c_str());
				if(mkr!=NULL)
				{
					typedef string (*WsPtr) (Element);
					WsPtr f =  (WsPtr)mkr;
					string outpt = f(method);
					typedef map<string,string> AttributeList;
					AttributeList attl = soapbody.getAttributes();
					AttributeList::iterator it;
					string bod = "<" + soapbody.getTagNameSpc();
					for(it=attl.begin();it!=attl.end();it++)
					{
						bod.append(" " + it->first + "=\"" + it->second + "\" ");
					}
					bod.append(">"+outpt + "</" + soapbody.getTagNameSpc()+">");
					attl = soapenv.getAttributes();
					env = "<" + soapenv.getTagNameSpc();
					for(it=attl.begin();it!=attl.end();it++)
					{
						env.append(" " + it->first + "=\"" + it->second + "\" ");
					}
					env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
					//delete mkr;
				}
				else
				{
					typedef map<string,string> AttributeList;
					AttributeList attl = soapbody.getAttributes();
					AttributeList::iterator it;
					string bod = "<" + soapbody.getTagNameSpc();
					for(it=attl.begin();it!=attl.end();it++)
					{
						bod.append(" " + it->first + "=\"" + it->second + "\" ");
					}
					bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>Operation not supported</faultstring><faultactor/><detail>No such method error</detail><soap-fault></" + soapbody.getTagNameSpc()+">");
					attl = soapenv.getAttributes();
					env = "<" + soapenv.getTagNameSpc();
					for(it=attl.begin();it!=attl.end();it++)
					{
						env.append(" " + it->first + "=\"" + it->second + "\" ");
					}
					env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
				}
				cout << "\n----------------------------------------------------------------------------\n" << flush;
				cout << env << "\n----------------------------------------------------------------------------\n" << flush;
			}
			catch(string &fault)
			{
				typedef map<string,string> AttributeList;
				AttributeList attl = soapbody.getAttributes();
				AttributeList::iterator it;
				string bod = "<" + soapbody.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					bod.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+fault+"</faultstring><detail></detail><soap-fault></" + soapbody.getTagNameSpc()+">");
				attl = soapenv.getAttributes();
				env = "<" + soapenv.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					env.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
				cout << fault << flush;
			}
			catch(Exception *e)
			{
				typedef map<string,string> AttributeList;
				AttributeList attl = soapbody.getAttributes();
				AttributeList::iterator it;
				string bod = "<" + soapbody.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					bod.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+e->what()+"</faultstring><detail></detail><soap-fault></" + soapbody.getTagNameSpc()+">");
				attl = soapenv.getAttributes();
				env = "<" + soapenv.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					env.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
				cout << e->what() << flush;
			}
			catch(...)
			{
				typedef map<string,string> AttributeList;
				AttributeList attl = soapbody.getAttributes();
				AttributeList::iterator it;
				string bod = "<" + soapbody.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					bod.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>Standard Exception</faultstring><detail></detail><soap-fault></" + soapbody.getTagNameSpc()+">");
				attl = soapenv.getAttributes();
				env = "<" + soapenv.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					env.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
				cout << "Standard Exception" << flush;
			}
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".xml"]);
			res.setContent_str(env);
			//res.setContent_len(boost::lexical_cast<string>(env.length()));
		}
		else if(ext==".wsdl")
		{
			content = getContentStr(resourcePath+req->getFile(),"english",ext);
			if((content.length()==0))
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				//res.setContent_len("0");
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type(props[ext]);
				res.setContent_str(content);
				//res.setContent_len(boost::lexical_cast<string>(content.length()));
			}
		}
		else
		{
			if(ext==".fview")
			{
				cout << "inside fview " << req->getFile() << endl;
				string file = req->getFile();
				boost::replace_first(file,"fview","html");
				string ffile = req->getCntxt_root()+"/fviews/"+file;
				cout << ffile << endl;
				ifstream infile(ffile.c_str());
				string temp;
				if(infile.is_open())
				{
					content = "";
					while(getline(infile, temp))
					{
						if(temp.find("<?")==string::npos && temp.find("?>")==string::npos)
							content.append(temp);
					}
					int h = content.find("</head>");
					int ht = content.find("<html>");
					if(h!=string::npos)
					{
						string st = content.substr(0,h-1);
						string en = content.substr(h);
						content = st + "<script type=\"text/javascript\" src=\"public/json2.js\"></script>";
						content += "<script type=\"text/javascript\" src=\"public/prototype.js\"></script>";
						content += "<script type=\"text/javascript\" src=\"public/afc.js\"></script>";
						content += "<script type=\"text/javascript\" src=\"public/_afc_Objects.js\"></script>";
						content += "<script type=\"text/javascript\" src=\"public/_afc_Interfaces.js\"></script>";
						content += "<script type=\"text/javascript\" src=\"public/"+fviewmap[file]+".js\"></script>" + en;
					}
					else
					{
						if(ht!=string::npos)
						{
							string st = content.substr(0,ht+6);
							string en = content.substr(ht+6);
							content = st + "<script type=\"text/javascript\" src=\"public/json2.js\"></script>";
							content += "<script type=\"text/javascript\" src=\"public/prototype.js\"></script>";
							content += "<script type=\"text/javascript\" src=\"public/afc.js\"></script>";
							content += "<script type=\"text/javascript\" src=\"public/_afc_Objects.js\"></script>";
							content += "<script type=\"text/javascript\" src=\"public/_afc_Interfaces.js\"></script>";
							content += "<script type=\"text/javascript\" src=\"public/"+fviewmap[file]+".js\"></script>" + en;
						}
					}
				}
				infile.close();
				cout << content << flush;
			}
			else
				content = getContentStr(req->getUrl(),lprops[req->getDefaultLocale()],ext);
			if(ext!="" && (content.length()==0))
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				//res.setContent_len(boost::lexical_cast<string>(0));
			}
			else
			{
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type(props[ext]);
				res.setContent_str(content);
				//res.setContent_len(boost::lexical_cast<string>(content.length()));
				//sess.setAttribute("CURR",req->getUrl());
			}
		}

		if(filterMap.find(req->getCntxt_name()+"*.*out")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"out")!=filterMap.end())
		{
			vector<string> tempp;
			if(filterMap.find(req->getCntxt_name()+"*.*out")!=filterMap.end())
				tempp = filterMap[req->getCntxt_name()+"*.*out"];
			else
				tempp = filterMap[req->getCntxt_name()+ext+"out"];

			for (int var = 0; var < tempp.size(); ++var)
			{
				string clasz = tempp.at(var);
				clasz = "getReflectionCIFor" + clasz;
				cout << "filter handled by class " << clasz << endl;
				if(dlib == NULL)
				{
					cerr << dlerror() << endl;
					exit(-1);
				}
				void *mkr = dlsym(dlib, clasz.c_str());
				if(mkr!=NULL)
				{
					FunPtr f =  (FunPtr)mkr;
					ClassInfo srv = f();
					args argus;
					Constructor ctor = srv.getConstructor(argus);
					Reflector ref;
					void *_temp = ref.newInstanceGVP(ctor);
					Filter *filter = (Filter*)_temp;
					filter->doOutputFilter(&res);
					cout << "filter called" << endl;
					delete _temp;
				}
			}
		}

		alldatlg += "--processed data";
		string h1;
		bool sessionchanged = !req->hasCookie();
		sessionchanged |= req->getSession()->isDirty();

		createResponse(res,sessionchanged,req->getSession()->getSessionAttributes(),req->getCookieInfo()["FFEADID"]);
		h1 = res.generateResponse();
		//cout << h1 << endl;
		if(isSSLEnabled)
		{
			int r;
			/* Now perform renegotiation if requested */
			if(client_auth==CLIENT_AUTH_REHANDSHAKE){
			  SSL_set_verify(ssl,SSL_VERIFY_PEER |
				SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);

			  /* Stop the client from just resuming the
				 un-authenticated session */
			  SSL_set_session_id_context(ssl,
				(const unsigned char*)&s_server_auth_session_id_context,
				sizeof(s_server_auth_session_id_context));

			  if(SSL_renegotiate(ssl)<=0)
			  {
				  error_occurred("SSL renegotiation error",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			  }
			  if(SSL_do_handshake(ssl)<=0)
			  {
				  error_occurred("SSL renegotiation error",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			  }
			  ssl->state=SSL_ST_ACCEPT;
			  if(SSL_do_handshake(ssl)<=0)
			  {
				  error_occurred("SSL renegotiation error",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			  }
			}
			if((r=BIO_puts(io,h1.c_str()))<=0)
			{
				  error_occurred("send failed",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			}
			/*int size;
			if(res.getContent_str().length()>0)
			{
				if ((size=BIO_puts(io,res.getContent_str().c_str()))<=0)
				{
					  error_occurred("send failed",fd,ssl);
					  if(io!=NULL)BIO_free(io);
					  return;
				}
			}*/
			if((r=BIO_flush(io))<0)
			{
				  error_occurred("Error flushing BIO",fd,ssl);
				  if(io!=NULL)BIO_free(io);
				  return;
			}
			//cout << h1 << flush;
			//if(io!=NULL)BIO_free_all(io);
			closeSSL(fd,ssl,io);
		}
		else
		{
			int size;
			if ((size=send(fd,&h1[0] , h1.length(), 0)) == -1)
				cout << "send failed" << flush;
			else if(size==0)
			{
				close(fd);memset(&buf[0], 0, sizeof(buf));
				cout << "socket closed for writing" <<flush;return;
			}


			/*if(res.getContent_str().length()>0)
			{
				if ((size=send(fd,res.getContent_str().c_str(), res.getContent_str().length(),0)) == -1)
					cout << "send failed" << flush;
				else if(size==0)
				{
					close(fd);memset(&buf[0], 0, sizeof(buf));
					cout << "socket closed for writing" <<flush;return;
				}
			}*/
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


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int send_connection(int fd,int unix_socket_fd)
{
	//struct sockaddr_un unix_socket_name = {0};
	struct msghdr msg;
	char ccmsg[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;
	struct iovec vec;  /* stupidity: must send/receive at least one byte */
	char *str = (char *)"x";
	int rv;

	//msg.msg_name = (struct sockaddr*)&unix_socket_name;
	//msg.msg_namelen = sizeof(unix_socket_name);
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	vec.iov_base = str;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	/* old BSD implementations should use msg_accrights instead of
	* msg_control; the interface is different. */
	msg.msg_control = ccmsg;
	msg.msg_controllen = sizeof(ccmsg);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	*(int*)CMSG_DATA(cmsg) = fd;
	msg.msg_controllen = cmsg->cmsg_len;

	msg.msg_flags = 0;
	//boost::mutex::scoped_lock lock(p_mutex);
	if((rv= sendmsg(unix_socket_fd, &msg, 0)) < 0 )
	{
	  perror("sendmsg()");
	  exit(1);
	}
	close(fd);
	return rv;
}

int receive_fd(int fd)
{
  struct msghdr msg;
  struct iovec iov;
  char buf[1];
  int rv;
  int connfd = -1;
  char ccmsg[CMSG_SPACE(sizeof(connfd))];
  struct cmsghdr *cmsg;

  iov.iov_base = buf;
  iov.iov_len = 1;

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  /* old BSD implementations should use msg_accrights instead of
   * msg_control; the interface is different. */
  msg.msg_control = ccmsg;
  msg.msg_controllen = sizeof(ccmsg); /* ? seems to work... */

  rv = recvmsg(fd, &msg, 0);
  if (rv == -1) {
    perror("recvmsg");
    return -1;
  }

  cmsg = CMSG_FIRSTHDR(&msg);
  if (!cmsg->cmsg_type == SCM_RIGHTS) {
    fprintf(stderr, "got control message of unknown type %d\n",
	    cmsg->cmsg_type);
    return -1;
  }
  return *(int*)CMSG_DATA(cmsg);
}

void signalSIGSEGV(int dummy)
{
	signal(SIGSEGV,signalSIGSEGV);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Segmentation fault occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGCHLD(int dummy)
{
	signal(SIGCHLD,signalSIGCHLD);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Child process got killed " << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGABRT(int dummy)
{
	signal(SIGABRT,signalSIGABRT);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Abort signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGTERM(int dummy)
{
	signal(SIGKILL,signalSIGTERM);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Termination signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void signalSIGKILL(int dummy)
{
	signal(SIGKILL,signalSIGKILL);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Kill signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void signalSIGINT(int dummy)
{
	signal(SIGINT,signalSIGINT);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Interrupt signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void signalSIGFPE(int dummy)
{
	signal(SIGFPE,signalSIGFPE);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGPIPE(int dummy)
{
	signal(SIGPIPE,signalSIGPIPE);
	/*string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());*/
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Broken pipe ignore it" << getpid() << "\n" << tempo << flush;
	//abort();
}

void signalSIGILL(int dummy)
{
	signal(SIGILL,signalSIGILL);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void cleanUpRoutine(string tempo)
{
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void service(int fd,string serverRootDirectory,map<string,string> *params)

{
	ServiceTask *task = new ServiceTask(fd,serverRootDirectory,params);
	task->run();
	delete task;
	//cout << "\nDestroyed task" << flush;
}

pid_t createChildProcess(string serverRootDirectory,int sp[],int sockfd)
{
	pid_t pid;
	struct epoll_event ev;
	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sp) == -1)
	{
		perror("socketpair");
		exit(1);
	}
	if((pid=fork())==0)
	{
		dlib = dlopen("libinter.so", RTLD_NOW|RTLD_GLOBAL);
		cout << endl <<dlib << endl;
		if(dlib==NULL)
		{
			cout << dlerror() << endl;
			Logger::info("Could not load Library");
		}
		else
			Logger::info("Library loaded successfully");
		if(isSSLEnabled)
		{
			/*HTTPS related*/
			//client_auth=CLIENT_AUTH_REQUIRE;
			/* Build our SSL context*/
			ctx=initialize_ctx((char*)key_file.c_str(),(char*)sec_password.c_str());
			load_dh_params(ctx,(char*)dh_file.c_str());

			SSL_CTX_set_session_id_context(ctx,
			  (const unsigned char*)&s_server_session_id_context,
			  sizeof s_server_session_id_context);

			/* Set our cipher list */
			if(ciphers){
			  SSL_CTX_set_cipher_list(ctx,ciphers);
			}
			if(client_auth==2)
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);
			else
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,0);
		}
		servd = serverRootDirectory;
		string filename;
		stringstream ss;
		ss << serverRootDirectory;
		ss << getpid();
		ss >> filename;
		filename.append(".cntrl");
		cout << "generated file " << filename << flush;
		ofstream cntrlfile;
		cntrlfile.open(filename.c_str());
		cntrlfile << "Process Running" << flush;
		cntrlfile.close();
		struct msghdr msg;
		struct iovec iov;
		char buf[1];
		int rv;
		int connfd = -1;
		char ccmsg[CMSG_SPACE(sizeof(connfd))];
		struct cmsghdr *cmsg;

		iov.iov_base = buf;
		iov.iov_len = 1;

		msg.msg_name = 0;
		msg.msg_namelen = 0;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		/* old BSD implementations should use msg_accrights instead of
		* msg_control; the interface is different. */
		msg.msg_control = ccmsg;
		msg.msg_controllen = sizeof(ccmsg); /* ? seems to work... */
		close(sockfd);
		struct epoll_event events[1];
		//printf("parent side--%d child side---%d child pid--%ld\n",sp[j][0],sp[j][1],(long)getpid());
		int epoll_handle = epoll_create(1);
		ev.events = EPOLLIN | EPOLLPRI;
		ev.data.fd = sp[1];
		if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, sp[1], &ev) < 0)
		{
			fprintf(stderr, "epoll set insertion error: fd=%d\n", sp[1]);
			return -1;
		}
		ThreadPool pool;
		if(!isThreadprq)
		{
			pool.init(thrdpsiz,30,true);
		}
		PropFileReader pread;
		propMap params = pread.getProperties(serverRootDirectory+"resources/security.prop");

		cout << params.size() <<endl;
		while(1)
		{
			int nfds = epoll_wait(epoll_handle, events, 1,-1);
			if (nfds == -1)
			{
				perror("epoll_wait child process");
				cout << "\n----------epoll_wait child process----" << flush;
				//break;
			}
			else
			{
				//int fd = receive_fd(sp[j][1]);
				rv = recvmsg(sp[1], &msg, 0);
				if (rv == -1)
				{
					perror("recvmsg");
					cout << "\n----------error occurred----" << flush;
					exit(1);
				}

				cmsg = CMSG_FIRSTHDR(&msg);
				if (!cmsg->cmsg_type == SCM_RIGHTS)
				{
					fprintf(stderr, "got control message of unknown type %d\n",cmsg->cmsg_type);
					exit(1);
				}
				int fd = *(int*)CMSG_DATA(cmsg);
				fcntl(fd, F_SETFL,O_SYNC);

				char buf[10];
				int err;
				if((err=recv(fd,buf,10,MSG_PEEK))==0)
				{
					close(fd);
					cout << "\nsocket conn closed before being serviced" << flush;
					continue;
				}

				if(isThreadprq)
					boost::thread m_thread(boost::bind(&service,fd,serverRootDirectory,&params));
				else
				{
					ServiceTask *task = new ServiceTask(fd,serverRootDirectory,&params);
					pool.execute(*task);
				}
			}
		}
	}
	return pid;
}

pid_t createChildMonitProcess(int sp[])
{
	pid_t pid;
	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sp) == -1)
	{
		perror("socketpair");
		exit(1);
	}
	if((pid=fork())==0)
	{
		map<string,bool> stat;
		while(1)
		{
			char buf[10];
			if(read(sp[1], buf, sizeof buf) < 0)
			{
				string temp = buf;
				strVec tempv;
				boost::iter_split(tempv, temp, boost::first_finder(":"));
				if(tempv.size()==2)
				{
					if(tempv.at(0)=="R")
					{
						string h = "0";
						if(stat[tempv.at(1)])
							h = "1";
						write(sp[0], h.c_str() , sizeof(h));
					}
					else if(tempv.at(0)=="W")
					{
						stat[tempv.at(1)] = false;
					}
				}
			}
		}
	}
	return pid;
}


strVec temporaray(strVec webdirs,strVec webdirs1,string incpath,string rtdcfpath,string pubpath,string respath)
{
	strVec all,dcps,afcd,appf,wspath,compnts,cmpnames;
	string includeRef;
	TemplateEngine templ;
	Context cntxt;
	string libs,ilibs,isrcs,iobjs,ideps;
	Reflection ref;
	vector<bool> stat;
	strVec vecvp,pathvec;
	propMap srp;
	PropFileReader pread;
	XmlParser parser("Parser");
	ComponentGen gen;
	if(isSSLEnabled)
	{
		propMap sslsec = pread.getProperties(respath+"/security.prop");
		if(sslsec.size()>0)
		{
			key_file = sslsec["KEYFILE"];
			dh_file = sslsec["DHFILE"];
			ca_list = sslsec["CA_LIST"];
			rand_file = sslsec["RANDOM"];
			sec_password = sslsec["PASSWORD"];
			string tempcl = sslsec["CLIENT_SEC_LEVEL"];
			srv_auth_prvd = sslsec["SRV_AUTH_PRVD"];
			srv_auth_mode = sslsec["SRV_AUTH_MODE"];
			srv_auth_file = sslsec["SRV_AUTH_FILE"];
			if(tempcl!="")
			{
				try
				{
					client_auth = boost::lexical_cast<int>(tempcl);
				}
				catch(...)
				{
					cout << "\nInvalid client auth level defined" << flush;
					client_auth = 1;
				}
			}
		}
	}
	for(unsigned int var=0;var<webdirs.size();var++)
	{
		//cout <<  webdirs.at(0) << flush;
		string defpath = webdirs.at(var);
		string dcppath = defpath + "dcp/";
		string cmppath = defpath + "components/";
		string usrincludes = defpath + "include/";
		//propMap srp = pread.getProperties(defpath+"config/app.prop");

		string name = webdirs1.at(var);
		boost::replace_all(name,"/","");
		cntMap[name] = "true";
		listi(dcppath,".dcp",true,dcps);
		listi(cmppath,".cmp",true,compnts);
		all.push_back(usrincludes);
		appf.push_back(defpath+"app.xml");

		libs += ("-l"+ name+" ");
		ilibs += ("-I" + usrincludes+" ");
		wspath.push_back(name);

		Element root = parser.getDocument(defpath+"config/application.xml").getRootElement();
		if(root.getTagName()=="app" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="controllers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="controller")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string clas = cntrls.at(cntn).getAttribute("class");
							if(url!="" && clas!="")
							{
								if(cntrls.at(cntn).getAttribute("url").find("*")!=string::npos)
								{
									if(url=="*.*")
										urlpattMap[name+url] = clas;
									else
									{
										url = url.substr(url.find("*")+1);
										urlMap[name+url] = clas;
									}
								}
								else if(clas!="")
									urlMap[name+url] = clas;
								cout << "adding controller => " << name << url << " :: " << clas << endl;
							}
							else
							{
								string from = cntrls.at(cntn).getAttribute("from");
								string to = cntrls.at(cntn).getAttribute("to");
								if(to.find("*")!=string::npos && to!="")
									to = to.substr(to.find("*")+1);
								if(from.find("*")!=string::npos && to!="")
								{
									if(from=="*.*")
										mappattMap[name+from] = to;
									else
									{
										from = from.substr(from.find("*")+1);
										mapMap[name+from] = to;
									}
								}
								else if(to!="")
								{
									mapMap[name+from] = to;
								}
								cout << "adding mapping => " << name << from << " :: " << to << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="authhandlers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="authhandler")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string provider = cntrls.at(cntn).getAttribute("provider");
							if(url!="" && provider!="")
							{
								if(url.find("*")!=string::npos)
								{
									if(url=="*.*")
										autpattMap[name+url] = provider;
									else
									{
										url = url.substr(url.find("*")+1);
										autMap[name+url] = provider;
									}
								}
								else if(provider!="")
									autMap[name+url] = provider;
								cout << "adding authhandler => " << name << url << " :: " << provider << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="filters")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="filter")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string clas = cntrls.at(cntn).getAttribute("class");
							string type = cntrls.at(cntn).getAttribute("type");
							if(clas!="" && (type=="in" || type=="out"))
							{
								if(url=="")url="*.*";
								if(url.find("*")!=string::npos)
								{
									if(url=="*.*")
									{
										filterMap[name+url+type].push_back(clas);
									}
									else
									{
										url = url.substr(url.find("*")+1);
										filterMap[name+url+type].push_back(clas);
									}
								}
								cout << "adding filter => " << name << url << type << " :: " << clas << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="templates")
				{
					ElementList tmplts = eles.at(apps).getChildElements();
					for (unsigned int tmpn = 0; tmpn < tmplts.size(); tmpn++)
					{
						if(tmplts.at(tmpn).getTagName()=="template")
						{
							tmplMap[name+tmplts.at(tmpn).getAttribute("file")] = tmplts.at(tmpn).getAttribute("class");
							//cout << tmplts.at(tmpn).getAttribute("file") << " :: " << tmplts.at(tmpn).getAttribute("class") << flush;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="dviews")
				{
					ElementList dvs = eles.at(apps).getChildElements();
					for (unsigned int dn = 0; dn < dvs.size(); dn++)
					{
						if(dvs.at(dn).getTagName()=="dview")
						{
							vwMap[name+dvs.at(dn).getAttribute("path")] = dvs.at(dn).getAttribute("class");
							//cout << dvs.at(dn).getAttribute("path") << " :: " << dvs.at(dn).getAttribute("class") << flush;
						}
					}
				}
			}
		}
		Mapping* mapping = new Mapping;
		smstrMap appTableColMapping;
		strMap maptc,maptcl;
		relMap appTableRelMapping;
		Element dbroot = parser.getDocument(defpath+"config/cibernate.xml").getRootElement();
		if(dbroot.getTagName()=="cibernate")
		{
			ElementList dbeles = dbroot.getChildElements();
			for (unsigned int dbs = 0; dbs < dbeles.size(); dbs++)
			{
				if(dbeles.at(dbs).getTagName()=="config")
				{
					ElementList confs = dbeles.at(dbs).getChildElements();
					string uid,pwd,dsn;
					int psize= 2;
					for (unsigned int cns = 0; cns < confs.size(); cns++)
					{
						if(confs.at(cns).getTagName()=="uid")
						{
							uid = confs.at(cns).getText();
						}
						else if(confs.at(cns).getTagName()=="pwd")
						{
							pwd = confs.at(cns).getText();
						}
						else if(confs.at(cns).getTagName()=="dsn")
						{
							dsn = confs.at(cns).getText();
						}
						else if(confs.at(cns).getTagName()=="pool-size")
						{
							if(confs.at(cns).getText()!="")
								psize = boost::lexical_cast<int>(confs.at(cns).getText());
						}
					}
					CibernateConnPools::addPool(psize,uid,pwd,dsn,name);
				}
				else if(dbeles.at(dbs).getTagName()=="tables")
				{
					ElementList tabs = dbeles.at(dbs).getChildElements();
					for (unsigned int dn = 0; dn < tabs.size(); dn++)
					{
						if(tabs.at(dn).getTagName()=="table")
						{
							vector<DBRel> relv;
							/*DBRel relation;
							relation.type = (tabs.at(dn).getAttribute("hasOne")!="")?1:((tabs.at(dn).getAttribute("hasMany")!="")?2:((tabs.at(dn).getAttribute("many")!="")?3:0));
							if(relation.type==1)
								relation.clsName = tabs.at(dn).getAttribute("hasOne");
							else if(relation.type==2)
								relation.clsName = tabs.at(dn).getAttribute("hasMany");
							else if(relation.type==3)
								relation.clsName = tabs.at(dn).getAttribute("many");
							relation.fk = tabs.at(dn).getAttribute("fk");
							relation.pk_rel = tabs.at(dn).getAttribute("pk");*/
							maptcl[tabs.at(dn).getAttribute("class")] = tabs.at(dn).getAttribute("name");
							ElementList cols = tabs.at(dn).getChildElements();
							for (unsigned int cn = 0; cn < cols.size(); cn++)
							{
								if(cols.at(cn).getTagName()=="hasOne")
								{
									DBRel relation;
									relation.clsName = cols.at(cn).getText();
									relation.type = 1;
									relation.fk = cols.at(cn).getAttribute("fk");
									relation.pk = cols.at(cn).getAttribute("pk");
									relv.push_back(relation);
								}
								else if(cols.at(cn).getTagName()=="hasMany")
								{
									DBRel relation;
									relation.clsName = cols.at(cn).getText();
									relation.type = 2;
									relation.fk = cols.at(cn).getAttribute("fk");
									relation.pk = cols.at(cn).getAttribute("pk");
									relation.field = cols.at(cn).getAttribute("field");
									relv.push_back(relation);
								}
								else if(cols.at(cn).getTagName()=="many")
								{
									DBRel relation;
									relation.clsName = cols.at(cn).getText();
									relation.type = 3;
									relation.fk = cols.at(cn).getAttribute("fk");
									relation.pk = cols.at(cn).getAttribute("pk");
									relv.push_back(relation);
								}
								else if(cols.at(cn).getTagName()=="col")
								{
									maptc[cols.at(cn).getAttribute("obf")] = cols.at(cn).getAttribute("dbf");
								}
							}
							appTableColMapping[tabs.at(dn).getAttribute("class")] = maptc;
							appTableRelMapping[tabs.at(dn).getAttribute("class")] = relv;
						}
					}
				}
			}
		}
		mapping->setAppTableColMapping(appTableColMapping);
		mapping->setAppTableClassMapping(maptcl);
		mapping->setAppTableRelMapping(appTableRelMapping);
		CibernateConnPools::addMapping(name,mapping);
		//cout << (defpath+"config/app.prop") << flush;
		propMap afc = pread.getProperties(defpath+"config/afc.prop");

		string filepath;
		if(afc.size()>0)
		{
			string objs = afc["PROP"];
			strVec objv;
			boost::iter_split(objv, objs, boost::first_finder(","));
			for (unsigned int var1 = 0;var1<objv.size();var1++)
			{
				if(objv.at(var1)!="")
				{
					//strVec info = ref.getAfcObjectData(usrincludes+objv.at(var)+".h", true);
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(true);
					afcd.push_back(objv.at(var1));
				}
			}
			objs = afc["INTF"];
			objv.clear();
			boost::iter_split(objv, objs, boost::first_finder(","));
			for (unsigned int var1 = 0;var1<objv.size();var1++)
			{
				if(objv.at(var1)!="")
				{
					//strVec info = ref.getAfcObjectData(usrincludes+objv.at(var)+".h", false);
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(false);
					afcd.push_back(objv.at(var1));
				}
			}
		}
		root = parser.getDocument(defpath+"config/fviews.xml").getRootElement();
		if(root.getTagName()=="fview" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="page")
				{
					string fvw = eles.at(apps).getAttribute("htm");
					boost::replace_first(fvw,".html",".fview");
					fviewmap[eles.at(apps).getAttribute("htm")] = eles.at(apps).getAttribute("class");
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(false);
					afcd.push_back(eles.at(apps).getAttribute("class"));
					ElementList elese = eles.at(apps).getChildElements();
					string js = "window.onload = function(){";
					for (unsigned int appse = 0; appse < elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="event")
						{
							js += "\ndocument.getElementById('"+elese.at(appse).getAttribute("eid")+"').";
							js += elese.at(appse).getAttribute("type") + " = function(){";
							js += eles.at(apps).getAttribute("class")+"."+elese.at(appse).getAttribute("func")+"(";
							string args = elese.at(appse).getAttribute("args");
							if(args!="")
								args += ",";
							js += args + "\""+elese.at(appse).getAttribute("cb")+"\",\"/"+name+"/"+fvw+"\");}";
						}
					}
					js += "\n}\n\n";
					for (unsigned int appse = 0; appse < elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="functions")
						{
							js += elese.at(appse).getText();
						}
					}
					AfcUtil::writeTofile(pubpath+eles.at(apps).getAttribute("class")+".js",js,true);
				}
			}
		}
	}
	cout << endl<< "started generating compoenent code" <<endl;
	for (unsigned int var1 = 0;var1<compnts.size();var1++)
	{
		string cudata,cuheader,curemote,curemoteheaders;
		string file = gen.generateComponentCU(compnts.at(var1),cudata,cuheader,curemote,curemoteheaders);
		AfcUtil::writeTofile(rtdcfpath+file+".h",cuheader,true);
		AfcUtil::writeTofile(rtdcfpath+file+".cpp",cudata,true);
		AfcUtil::writeTofile(rtdcfpath+file+"_Remote.h",curemoteheaders,true);
		AfcUtil::writeTofile(rtdcfpath+file+"_Remote.cpp",curemote,true);
		isrcs += "./"+file+".cpp \\\n"+"./"+file+"_Remote.cpp \\\n";
		iobjs += "./"+file+".o \\\n"+"./"+file+"_Remote.o \\\n";
		ideps += "./"+file+".d \\\n"+"./"+file+"_Remote.d \\\n";
		cmpnames.push_back(file);
		cout << endl<< compnts.at(var1) <<endl;
	}
	cout << endl<< "done generating compoenent code" <<endl;
	string ret = ref.generateClassDefinitionsAll(all,includeRef);
	AfcUtil::writeTofile(rtdcfpath+"ReflectorInterface.cpp",ret,true);
	ret = ref.generateSerDefinitionAll(all,includeRef);
	AfcUtil::writeTofile(rtdcfpath+"SerializeInterface.cpp",ret,true);
	cntxt["RUNTIME_LIBRARIES"] = libs;
	ret = templ.evaluate(rtdcfpath+"objects.mk.template",cntxt);
	AfcUtil::writeTofile(rtdcfpath+"objects.mk",ret,true);
	cntxt.clear();
	cntxt["USER_DEFINED_INC"] = ilibs;
	cntxt["RUNTIME_COMP_SRCS"] = isrcs;
	cntxt["RUNTIME_COMP_OBJS"] = iobjs;
	cntxt["RUNTIME_COMP_DEPS"] = ideps;
	ret = templ.evaluate(rtdcfpath+"subdir.mk.template",cntxt);
	AfcUtil::writeTofile(rtdcfpath+"subdir.mk",ret,true);
	dcpsss = dcps;
	ret = DCPGenerator::generateDCPAll(dcps);
	AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
	string headers,objs,infjs;
	ret = AfcUtil::generateJsObjectsAll(vecvp,afcd,stat,headers,objs,infjs,pathvec);
	AfcUtil::writeTofile(rtdcfpath+"AjaxInterface.cpp",ret,true);
	AfcUtil::writeTofile(pubpath+"_afc_Objects.js",objs,true);
	AfcUtil::writeTofile(pubpath+"_afc_Interfaces.js",infjs,true);
	AfcUtil::writeTofile(incpath+"AfcInclude.h",headers,true);
	ApplicationUtil apputil;
	webdirs.clear();
	ret = apputil.buildAllApplications(appf,webdirs1,appMap);
	AfcUtil::writeTofile(rtdcfpath+"ApplicationInterface.cpp",ret,true);
	WsUtil wsu;
	ret = wsu.generateAllWSDL(wspath,respath,wsdlmap);
	AfcUtil::writeTofile(rtdcfpath+"WsInterface.cpp",ret,true);
	return cmpnames;
}

void dynamic_page_monitor(string serverRootDirectory)
{
	struct stat statbuf;
	map<string,long> statsinf;
	for(int i=0;i<(int)dcpsss.size();i++)
	{
		stat(dcpsss.at(i).c_str(), &statbuf);
		time_t tm = statbuf.st_mtime;
		long tim = (uintmax_t)tm;
		statsinf[dcpsss.at(i)] = tim;
	}
	while(true)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(5));
		bool flag = false;
		if(processgendone)
			continue;
		for(int i=0;i<(int)dcpsss.size();i++)
		{
			stat(dcpsss.at(i).c_str(), &statbuf);
			time_t tm = statbuf.st_mtime;
			long tim = (uintmax_t)tm;
			if(tim!=statsinf[dcpsss.at(i)])
			{
				string rtdcfpath = serverRootDirectory + "rtdcf/";
				string respath = serverRootDirectory + "resources/";
				string ret = DCPGenerator::generateDCPAll(dcpsss);
				AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
				string compres = "/"+respath+"rundyn.sh";
				int i=system(compres.c_str());
				if(!i)
				{
					cout << "regenarting intermediate code-----Done" << endl;
					Logger::info("Done generating intermediate code");
				}
				m_mutex.lock();
				map<int,pid_t>::iterator it;
				for(it=pds.begin();it!=pds.end();it++)
				{
					kill(it->second,9);
				}
				m_mutex.unlock();
				processforcekilled = true;
				flag = true;
				break;
			}
		}
		if(flag)
		{
			for(int ii=0;ii<(int)dcpsss.size();ii++)
			{
				stat(dcpsss.at(ii).c_str(), &statbuf);
				time_t tm = statbuf.st_mtime;
				long tim = (uintmax_t)tm;
				statsinf[dcpsss.at(ii)] = tim;
			}
		}
	}
}


int main(int argc, char* argv[])
{
	parid = getpid();
	signal(SIGSEGV,signalSIGSEGV);
	signal(SIGFPE,signalSIGFPE);

	//signal(SIGILL,signalSIGILL);
	signal(SIGPIPE,signalSIGPIPE);
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;


    struct epoll_event ev;
	//struct rlimit rt;
    int yes=1;
    //char s[INET6_ADDRSTRLEN];
    int rv,nfds,preForked=5;

    /*char path[1024];
	pwd(path,sizeof path);
	string serverRootDirectory;
	stringstream sf;
	sf << path;
	sf >> serverRootDirectory;*/
    string serverRootDirectory = argv[1];

	serverRootDirectory += "/";//serverRootDirectory = "/home/sumeet/server/";
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	resourcePath = respath;

	//string logf = serverRootDirectory+"/server.log";
	//logfile.open(logf.c_str());
	string logp = respath+"/log.prop";
	Logger::init(logp);

    PropFileReader pread;
    propMap srprps = pread.getProperties(respath+"server.prop");
    if(srprps["NUM_PROC"]!="")
    	preForked = boost::lexical_cast<int>(srprps["NUM_PROC"]);
    string sslEnabled = srprps["SSL_ENAB"];
   	if(sslEnabled=="true" || sslEnabled=="TRUE")
   		isSSLEnabled = true;
   	string thrdpreq = srprps["THRD_PREQ"];
   	if(thrdpreq=="true" || thrdpreq=="TRUE")
   		isThreadprq = true;
   	string compileEnabled = srprps["COMPILE_ENABLED"];
	if(compileEnabled=="true" || compileEnabled=="TRUE")
		isCompileEnabled = true;
   	else
   	{
   		thrdpreq = srprps["THRD_PSIZ"];
   		if(thrdpreq=="")
   			thrdpsiz = 30;
   		else
   		{
   			try
   			{
   				thrdpsiz = boost::lexical_cast<int>(thrdpreq);
   			}
   			catch(...)
   			{
   				cout << "\nInvalid thread pool size defined" <<flush;
   				thrdpsiz = 30;
   			}
   		}
   	}
   	if(srprps["SESS_STATE"]=="server")
   		sessatserv = true;
    /* Set each process to allow the maximum number of files to open */
	/*rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
	if(setrlimit(RLIMIT_NOFILE, &rt) == -1)
	{
		perror("setrlimit");
		exit(1);
	}
	else
		printf("set the parameters of system resources, the success!\n");*/

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    string PORT = srprps["PORT_NO"];
    if ((rv = getaddrinfo(NULL, &PORT[0], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (listen(sockfd, BACKLOGM) == -1) {
        perror("listen");
        exit(1);
    }
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    strVec webdirs,webdirs1,pubfiles;
    listi(webpath,"/",true,webdirs);
    listi(webpath,"/",false,webdirs1);
    listi(pubpath,".js",false,pubfiles);
    for(unsigned int var=0;var<pubfiles.size();var++)
    {
    	pubMap[pubfiles.at(var)] = "true";
    }
    strVec cmpnames;
    try
    {
    	cmpnames = temporaray(webdirs,webdirs1,incpath,rtdcfpath,pubpath,respath);
    }
    catch(XmlParseException *p)
    {
    	cout << p->getMessage() << endl;
    }

    bool libpresent = true;
    void *dlibtemp = dlopen("libinter.so", RTLD_NOW|RTLD_GLOBAL);
	cout << endl <<dlibtemp << endl;
	if(dlibtemp==NULL)
	{
		libpresent = false;
		cout << dlerror() << endl;
		Logger::info("Could not load Library");
	}
	else
		dlclose(dlibtemp);
    if(isCompileEnabled)
    	libpresent = false;

	props = pread.getProperties(respath+"mime-types.prop");
	lprops = pread.getProperties(respath+"locale.prop");
	string compres = "/"+respath+"run.sh";
	if(!libpresent)
	{
		int i=system(compres.c_str());
		if(!i)
		{
			cout << "Done" << flush;
			Logger::info("Done generating intermediate code");
			//logfile << "Done generating intermediate code\n" << flush;
		}
	}

	for (unsigned int var1 = 0;var1<cmpnames.size();var1++)
	{
		string name = cmpnames.at(var1);
		boost::replace_first(name,"Component_","");
		ComponentHandler::registerComponent(name);
		AppContext::registerComponent(name);
	}
	if(srprps["CMP_PORT"]=="")
	{
		srprps["CMP_PORT"] = "7001";
	}
	ComponentHandler::trigger(srprps["CMP_PORT"]);
	if(srprps["MESS_PORT"]=="")
	{
		srprps["MESS_PORT"] = "7002";
	}
	MessageHandler::trigger(srprps["MESS_PORT"],resourcePath);
	if(srprps["MI_PORT"]=="")
	{
		srprps["MI_PORT"] = "7003";
	}
	MethodInvoc::trigger(srprps["MI_PORT"]);
	int sp[preForked][2]; /* the pair of socket descriptors */
	printf("server: waiting for connections...\n");
	//logfile << "Server: waiting for connections on port " << PORT << "\n" << flush;
	Logger::info("Server: waiting for connections on port "+PORT);

	vector<string> files;
	for(int j=0;j<preForked;j++)
	{
		pid_t pid = createChildProcess(serverRootDirectory,sp[j],sockfd);
		pds[j] = pid;
		stringstream ss;
		string filename;
		ss << serverRootDirectory;
		ss << pds[j];
		ss >> filename;
		filename.append(".cntrl");
		files.push_back(filename);
	}
	if(isCompileEnabled)boost::thread m_thread(boost::bind(&dynamic_page_monitor ,serverRootDirectory));
	struct epoll_event events[MAXEPOLLSIZE];
	int epoll_handle = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN | EPOLLPRI;
	ev.data.fd = sockfd;
	if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, sockfd, &ev) < 0)
	{
		fprintf(stderr, "epoll set insertion error: fd=%d\n", sockfd);
		return -1;
	}
	else
		printf("listener socket to join epoll success!\n");
	int childNo = 0;
	/*if(fork()==0)
	{
		//start  of hotdeployment process

	}*/
	//cout << "Done" << flush;
	struct msghdr msg;
	char ccmsg[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;
	struct iovec vec;  /* stupidity: must send/receive at least one byte */
	char *str = (char *)"x";
	//msg.msg_name = (struct sockaddr*)&unix_socket_name;
	//msg.msg_namelen = sizeof(unix_socket_name);
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	vec.iov_base = str;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	/* old BSD implementations should use msg_accrights instead of
	* msg_control; the interface is different. */
	msg.msg_control = ccmsg;
	msg.msg_controllen = sizeof(ccmsg);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	msg.msg_flags = 0;
	int curfds = 1;
	ifstream cntrlfile;
	ThreadPool pool;
	if(!isThreadprq)
	{
		pool.init(thrdpsiz,30,true);
	}
	while(1)
	{

		if(childNo>=preForked)
			childNo = 0;
		nfds = epoll_wait(epoll_handle, events, curfds,-1);
		if (nfds == -1)
		{
			perror("epoll_wait main process");
			//logfile << "Interruption Signal Received\n" << flush;
			Logger::info("Interruption Signal Received\n");
			curfds = 1;
			if(errno==EBADF)
				cout << "\nInavlid fd" <<flush;
			else if(errno==EFAULT)
				cout << "\nThe memory area pointed to by events is not accessible" <<flush;
			else if(errno==EINTR)
				cout << "\ncall was interrupted by a signal handler before any of the requested events occurred" <<flush;
			else
				cout << "\nnot an epoll file descriptor" <<flush;
			//break;
		}
		if(processforcekilled)
		{
			files.clear();
			for(int j=0;j<preForked;j++)
			{
				pid_t pid = createChildProcess(serverRootDirectory,sp[j],sockfd);
				pds[j] = pid;
				stringstream ss;
				string filename;
				ss << serverRootDirectory;
				ss << pds[j];
				ss >> filename;
				filename.append(".cntrl");
				files.push_back(filename);
			}
			processforcekilled = false;
			processgendone = true;
		}
		for(int n=0;n<nfds;n++)
		{
			if(childNo>=preForked)
				childNo = 0;
			if (events[n].data.fd == sockfd)
			{
				new_fd = -1;
				sin_size = sizeof their_addr;
				new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
				//cout << "\nnew http request" <<flush;
				//logfile << "Interruption Signal Received\n" << flush;
				if (new_fd == -1)
				{
					perror("accept");
					continue;
				}
				else
				{
					curfds++;
					fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFD, 0) | O_NONBLOCK);
					ev.events = EPOLLIN | EPOLLPRI;
					ev.data.fd = new_fd;
					if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, new_fd, &ev) < 0)
					{
						perror("epoll");
						cout << "\nerror adding to epoll cntl list" << flush;
						return -1;
					}
				}
			}
			else
			{
				epoll_ctl(epoll_handle, EPOLL_CTL_DEL, events[n].data.fd,&ev);
				curfds--;
				cntrlfile.open(files.at(childNo).c_str());
				if(cntrlfile.is_open())
				{
					*(int*)CMSG_DATA(cmsg) = events[n].data.fd;
					msg.msg_controllen = cmsg->cmsg_len;
					if((rv= sendmsg(sp[childNo][0], &msg, 0)) < 0)
					{
					  perror("sendmsg()");
					  exit(1);
					}
					string cno = boost::lexical_cast<string>(childNo);
					close(events[n].data.fd);
					childNo++;
				}
				else
				{
					int tcn = childNo;
					for(int o=0;o<preForked;o++)
					{
						cntrlfile.open(files.at(o).c_str());
						if(cntrlfile.is_open())
						{
							*(int*)CMSG_DATA(cmsg) = events[n].data.fd;
							msg.msg_controllen = cmsg->cmsg_len;
							if((rv= sendmsg(sp[o][0], &msg, 0)) < 0)
							{
							  perror("sendmsg()");
							  exit(1);
							}
							string cno = boost::lexical_cast<string>(o);
							//logfile << ("sent socket to process "+cno+"\n") << flush;
							close(events[n].data.fd);
							childNo = o+1;
							break;
						}
					}
					close(sp[tcn][0]);
					close(sp[tcn][1]);
					cout << "Process got killed" << flush;
					pid_t pid = createChildProcess(serverRootDirectory,sp[tcn],sockfd);
					pds[tcn] = pid;
					stringstream ss;
					string filename;
					ss << serverRootDirectory;
					ss << pid;
					ss >> filename;
					filename.append(".cntrl");
					files[tcn] = filename;
					cout << "created a new Process" << flush;
					//logfile << "Process got killed hence created a new Process " << pid << flush;
					Logger::info("Process got killed hence created a new Process\n");
				}
				cntrlfile.close();
			}
		}
	}
	return 0;
}
