/*
	Copyright 2012, Sumeet Chhetri

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

// Apache specific header files
#include <httpd.h>
#include <http_config.h>
#include <http_protocol.h>
#include <http_main.h>



#include <http_request.h>
#include <apr_strings.h>
#include <http_core.h>
#include <http_log.h>
#include <apr_pools.h>
#include "util_script.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "apr_general.h"
#include "util_filter.h"
#include "apr_buckets.h"


#include "Cibernate.h"
#include "algorithm"
#include "AfcUtil.h"

#include "Controller.h"

#include "PropFileReader.h"
#include "TemplateEngine.h"
#include "DCPGenerator.h"
#include "Reflection.h"
#include "cstdlib"
#include "dlfcn.h"
#include "WsUtil.h"
#include "sstream"

#include "ClassInfo.h"
#include <boost/lexical_cast.hpp>
#include "View.h"
#include "XmlParser.h"
#include "TemplateHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "DynamicView.h"
#include "HttpRequest.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include "ApplicationUtil.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <boost/thread/recursive_mutex.hpp>
#include <queue>
#include <sys/uio.h>
#include <sys/un.h>
#include "Cibernate.h"
#include "ComponentGen.h"
#include "ComponentHandler.h"
#include "Reflector.h"
#include "AppContext.h"
#include "Logger.h"
#include "Filter.h"
#include "FileAuthController.h"
#include "RestController.h"
#undef strtoul
#ifdef WINDOWS
    #include <direct.h>
    #define pwd _getcwd
#else
    #include <unistd.h>
    #define pwd getcwd
 #endif
#define MAXEPOLLSIZE 100
#define BACKLOG 500
#define MAXBUFLEN 1024
typedef bool (*FunPtr1) (void *);
typedef ClassInfo (*FunPtr) ();
typedef string (*DCPPtr) ();
typedef void (*ReceiveTask1)(int);
typedef void* (*toVoidP) (string);

using namespace std;

map<string,HttpSession> sessionMap;
boost::mutex m_mutex;
boost::mutex p_mutex;
int nor=0;
ofstream logfile;
static propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap;
static strVec dcpsss;
static propMap params;
static bool isSSLEnabled,isThreadprq,processforcekilled,processgendone,sessatserv,isCompileEnabled;
void* dlib;
class RestFunctionParams
{
public:
	int pos;
	string type;
};

class RestFunction
{
public:
	string name;
	string alias;
	string clas;
	string meth;
	string baseUrl;
	vector<RestFunctionParams> params;
};
typedef map<string, RestFunction> resFuncMap;
static resFuncMap rstCntMap;
static map<string, string> handoffs;
static map<string, Element> formMap;
static map<string, vector<string> > filterMap;
static string resourcePath, serverRootDirectory;;

extern "C" module AP_MODULE_DECLARE_DATA ffead_ccp_module;

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

vector<unsigned char> getContentVec(string url,string locale)
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
	if(locale.find("english")==string::npos)
	{
		string fnj = fname;
		boost::replace_first(fnj,".",("_" + locale+"."));
		myfile.open(&fnj[0],ios::in | ios::binary);
		cout << fnj << flush;
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

string createResponse(HttpResponse res)
{
	string resp;
	resp = (res.getHttpVersion() + " " + res.getStatusCode() + " " + res.getStatusMsg() + "\n");
	if(res.getContent().size()>0)
	{
		resp += ("Content-Length: "+boost::lexical_cast<string>(res.getContent_len()) + "\n");
		resp += ("Content-Type: "+res.getContent_type() + "\n\n");
	}
	else
		resp += "\n";
	return resp;
}

void listi(string cwd,string type,bool apDir,strVec &folders)
{
	FILE *pipe_fp;
	string command;

	command = ("ls -F1 "+cwd+"|grep '"+type+"'");
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
	}
	char t_char;
	string folderName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			stringstream ss;
			ss << t_char;
			string temp;
			ss >> temp;
			folderName.append(temp);
		}
		else if(folderName!="")
		{
			boost::replace_first(folderName,"*","");
			cout << "\n" << folderName << flush;
			if(folderName.find("~")==string::npos)
			{
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

void writeToSharedMemeory(string sessionId, string value,bool napp)
{
	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
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
	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
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

static int popenRWE(int *rwepipe, const char *exe, const char *const argv[],string tmpf)
{
	int in[2];
	int out[2];
	int err[2];
	int pid;
	int rc;

	rc = pipe(in);
	if (rc<0)
		goto error_in;

	rc = pipe(out);
	if (rc<0)
		goto error_out;

	rc = pipe(err);
	if (rc<0)
		goto error_err;

	pid = fork();
	if (pid > 0) { // parent
		close(in[0]);
		close(out[1]);
		close(err[1]);
		rwepipe[0] = in[1];
		rwepipe[1] = out[0];
		rwepipe[2] = err[0];
		return pid;
	} else if (pid == 0) { // child
		close(in[1]);
		close(out[0]);
		close(err[0]);
		close(0);
		dup(in[0]);
		close(1);
		dup(out[1]);
		close(2);
		dup(err[1]);cout << tmpf << endl;
		chdir(tmpf.c_str());
		execvp(exe, (char**)argv);
		exit(1);
	} else
		goto error_fork;

	return pid;

error_fork:
	close(err[0]);
	close(err[1]);
error_err:
	close(out[0]);
	close(out[1]);
error_out:
	close(in[0]);
	close(in[1]);
error_in:
	return -1;
}

static int pcloseRWE(int pid, int *rwepipe)
{
	int rc, status;
	close(rwepipe[0]);
	close(rwepipe[1]);
	close(rwepipe[2]);
	rc = waitpid(pid, &status, 0);
	return status;
}

HttpResponse service(HttpRequest* req)
{
	string ip;
	Reflector ref;
	map<string,string> params1 = params;
	string webpath = serverRootDirectory + "web/";
	//HttpRequest* req= new HttpRequest(results,webpath);
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
	//bool isAuthenticated = false;
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
		map<string,string> tempmap1 = req->getAllParams();
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
	string pthwofile = req->getCntxt_name()+req->getActUrl();
	//pthwofile = pthwofile.substr(0, pthwofile.find_last_of("/")+1);
	if(handoffs.find(req->getCntxt_name())!=handoffs.end())
	{
		ext = handoffs[req->getCntxt_name()+"extension"];
	}
	else if(!isContrl && (urlpattMap[req->getCntxt_name()+"*.*"]!="" || urlMap[req->getCntxt_name()+ext]!=""))
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
	else if(!isContrl)
	{
		resFuncMap::iterator it;
		RestFunction rft;
		bool flag = false;
		vector<string> valss;
		int prsiz = 0;
		//cout << pthwofile << endl;
		for (it=rstCntMap.begin();it!=rstCntMap.end();it++)
		{
			valss.clear();
			//cout << it->first << endl;
			//if(pthwofile.find(it->first)!=string::npos)
			{
				RestFunction ft = it->second;
				prsiz = ft.params.size();
				string pthwofiletemp(pthwofile);
				if(ft.baseUrl=="")
				{
					cout << "checking url : " << pthwofiletemp << ",param size: " << prsiz <<
							", against url: " << it->first << endl;
					for (int var = 0; var < prsiz; var++)
					{
						cout << "loop - " << pthwofiletemp << endl;
						string valsvv(pthwofiletemp.substr(pthwofiletemp.find_last_of("/")+1));
						pthwofiletemp = pthwofiletemp.substr(0, pthwofiletemp.find_last_of("/"));
						valss.push_back(valsvv);
					}
					reverse(valss.begin(),valss.end());
					//cout << "after - " << pthwofiletemp << endl;
					/*if(pthwofiletemp.at(pthwofiletemp.length()-1)=='/')
					{
						pthwofiletemp = pthwofiletemp.substr(0, pthwofiletemp.length()-1);
					}*/
					//cout << "after - " << pthwofiletemp << endl;
					cout << "checking url : " << pthwofiletemp << ",param size: " << prsiz << ",vals: " << valss.size() <<
							", against url: " << it->first << endl;
					if(it->first==pthwofiletemp)
					{
						string lhs = boost::to_upper_copy(ft.meth);
						string rhs = boost::to_upper_copy(req->getMethod());
						cout << lhs << " <> " << rhs << endl;
						if(prsiz==(int)valss.size() && lhs==rhs)
						{
							cout << "got correct url -- restcontroller " << endl;
							rft = ft;
							flag = true;
						}
						else
						{
							res.setStatusCode("404");
							res.setStatusMsg("Not Found");
							//res.setContent_type("text/plain");
							/*if(prsiz==valss.size())
								res.setContent_str("Invalid number of arguments");
							else
								res.setContent_str("Invalid HTTPMethod used");*/
							cout << "Rest Controller Param/Method Error" << endl;
						}
						break;
					}
				}
				else
				{
					string baseUrl(req->getCntxt_name()+ft.baseUrl);
					cout << "checking url : " << pthwofiletemp << ",param size: " << prsiz <<
							", against url: " << baseUrl << endl;
					for (int var = 1; var <= prsiz; var++)
					{
						strVec vemp;
						stringstream ss;
						ss << "{";
						ss << var;
						ss << "}";
						string param;
						ss >> param;
						boost::iter_split(vemp, baseUrl, boost::first_finder(param));
						if(vemp.size()==2 && pthwofiletemp.find(vemp.at(0))!=string::npos)
						{
							string temp = pthwofiletemp;
							boost::replace_first(temp, vemp.at(0), "");
							if(temp.find("/")!=string::npos)
							{
								pthwofiletemp = temp.substr(temp.find("/"));
								temp = temp.substr(0, temp.find("/"));
							}
							valss.push_back(temp);
							baseUrl = vemp.at(1);
							cout << "variable at " << param << " mapped to " << temp << " from URL" << endl;
							cout << baseUrl << endl;
							cout << pthwofiletemp << endl;
						}
						else
						{
							flag = false;
							break;
						}
					}
					string lhs = boost::to_upper_copy(ft.meth);
					string rhs = boost::to_upper_copy(req->getMethod());
					cout << lhs << " <> " << rhs << endl;
					if(prsiz==(int)valss.size() && lhs==rhs)
					{

						cout << "got correct url -- restcontroller " << endl;
						rft = ft;
						flag = true;
						break;
					}
					else
					{
						res.setStatusCode("404");
						res.setStatusMsg("Not Found");
						//res.setContent_type("text/plain");
						/*if(prsiz==valss.size())
							res.setContent_str("Invalid number of arguments");
						else
							res.setContent_str("Invalid HTTPMethod used");*/
						cout << "Rest Controller Param/Method Error" << endl;
					}
				}
			}
		}
		if(flag)
		{
			//cout << "inside restcontroller logic ..." << endl;
			string libName = "libinter.so";
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string clasnam("getReflectionCIFor"+rft.clas);
			void *mkr = dlsym(dlib, clasnam.c_str());
			cout << mkr << endl;
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Constructor ctor = srv.getConstructor(argus);
				Reflector ref;
				void *_temp = ref.newInstanceGVP(ctor);
				RestController* rstcnt = (RestController*)_temp;
				rstcnt->request = req;
				rstcnt->response = &res;

				vals valus;
				bool invValue = false;
				for (int var = 0; var < prsiz; var++)
				{
					try
					{
						argus.push_back(rft.params.at(var).type);
						if(rft.params.at(var).type=="int")
						{
							int* ival = new int(boost::lexical_cast<int>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="long")
						{
							long* ival = new long(boost::lexical_cast<long>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="double")
						{
							double* ival = new double(boost::lexical_cast<double>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="float")
						{
							float* ival = new float(boost::lexical_cast<float>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="bool")
						{
							bool* ival = new bool(boost::lexical_cast<bool>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="string")
						{
							string* sval = new string(valss.at(var));
							valus.push_back(sval);
						}
					} catch (...) {
						invValue= true;
						break;
					}
				}

				Method meth = srv.getMethod(rft.name, argus);
				if(meth.getMethodName()!="")
				{
					ref.invokeMethodUnknownReturn(_temp,meth,valus);
					cout << "successfully called restcontroller" << endl;
					//return;
				}
				else
				{
					res.setStatusCode("404");
					res.setStatusMsg("Not Found");
					//res.setContent_type("text/plain");
					//res.setContent_str("Rest Controller Method Not Found");
					cout << "Rest Controller Method Not Found" << endl;
					//return;
				}
			}
		}
	}

	/*After going through the controller the response might be blank, just set the HTTP version*/
	res.setHttpVersion(req->getHttpVersion());
	//cout << req->toString() << endl;
	if(isContrl)
	{

	}
	else if(ext==".form")
	{
		Reflector ref;
		Element ele = formMap[req->getFile()];
		cout << ele.getTagName() << endl;
		cout << ele.render() << endl;
		ClassInfo binfo = ref.getClassInfo(ele.getAttribute("bean"));
		ElementList eles = ele.getChildElements();
		string json = "{";
		for (unsigned int apps = 0; apps < eles.size(); apps++)
		{
			if(eles.at(apps).getTagName()=="field")
			{
				string name = eles.at(apps).getAttribute("name");
				Field fld = binfo.getField(eles.at(apps).getAttribute("prop"));
				if(fld.getType()=="string")
					json += "\""+eles.at(apps).getAttribute("prop")+"\": \"" + req->getRequestParam(name) + "\",";
				else
				{
					if(fld.getType()=="int" || fld.getType()=="long")
					{
						if(req->getRequestParam(name)=="")
							json += "\""+eles.at(apps).getAttribute("prop")+"\": 0,";
						else
							json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getRequestParam(name) + ",";
					}
					else if(fld.getType()=="double" || fld.getType()=="float")
					{
						if(req->getRequestParam(name)=="")
							json += "\""+eles.at(apps).getAttribute("prop")+"\": 0.0,";
						else
							json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getRequestParam(name) + ",";
					}
					else if(fld.getType()=="bool")
					{
						if(req->getRequestParam(name)=="")
							json += "\""+eles.at(apps).getAttribute("prop")+"\": false,";
						else
							json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getRequestParam(name) + ",";
					}
				}
			}
		}
		if(json.find(",")!=string::npos)
		{
			json = json.substr(0,json.length()-1);
		}
		json += "}";
		cout << json << endl;
		string libName = "libinter.so";
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string meth = "toVoidP" + ele.getAttribute("bean");
		cout << meth << endl;
		void *mkr = dlsym(dlib, meth.c_str());
		if(mkr!=NULL)
		{
			toVoidP f1 = (toVoidP)mkr;
			void *_beaninst = f1(json);
			//FunPtr f =  (FunPtr)mkr;
			ClassInfo srv = ref.getClassInfo(ele.getAttribute("controller"));
			args argus;
			vals valus;
			Constructor ctor = srv.getConstructor(argus);
			void *_temp = ref.newInstanceGVP(ctor);
			argus.push_back("void*");
			argus.push_back("HttpResponse*");
			valus.push_back(_beaninst);
			valus.push_back(&res);
			Method meth = srv.getMethod("onSubmit",argus);
			if(meth.getMethodName()!="")
			{
				ref.invokeMethodUnknownReturn(_temp,meth,valus);
				cout << "successfully called formcontroller" << endl;
				//return;
			}
			else
			{
				res.setStatusCode("404");
				res.setStatusMsg("Not Found");
				res.setContent_type("text/plain");
				res.setContent_str("Controller Method Not Found");
				cout << "Controller Method Not Found" << endl;
				//return;
			}
		}
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
			res.setContent_type(props[".html"]);
			cout << content << flush;
		}
		else
		{
			if(res.getContent_str()=="")
				content = getContentStr(req->getUrl(),lprops[req->getDefaultLocale()],ext);
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
			if(res.getContent_type()=="")res.setContent_type(props[ext]);
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

		for (int var = 0; var < (int)tempp.size(); ++var)
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


	boost::mutex::scoped_lock lock(m_mutex);
	logfile << ("got new connection to process\n") << flush;
	logfile << (req->getFile()+" :: " + res.getStatusCode() + "\n") << flush;
	logfile << (req->getCntxt_name() + "\n") << flush;
	logfile << (req->getCntxt_root() + "\n") << flush;
	logfile << (req->getUrl() + "\n") << flush;
	//sessionMap[sessId] = sess;
	return res;
}

/*static int util_read(request_rec *r, const char **rbuf)
{
   int rc;

    if ((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)) != OK) {
       return rc;
   }

    if (ap_should_client_block(r)) {
       char argsbuffer[HUGE_STRING_LEN];
      int rsize, len_read, rpos=0;
      long length = r->remaining;
      *rbuf = apr_pcalloc(r->pool, length + 1);

      while ((len_read = ap_get_client_block(r, argsbuffer, sizeof(argsbuffer))) > 0) {

          if ((rpos + len_read) > length) {
              rsize = length - rpos;
          }
          else {
              rsize = len_read;
          }
          memcpy((char*)*rbuf + rpos, argsbuffer, rsize);
          rpos += rsize;
      }
    }
   return rc;
}*/
class MyReq
{
public:
	request_rec* r;
	HttpRequest* htreq;
};
int iterate_func(void *req, const char *key, const char *value)
{
    int stat;
    char *line;
    MyReq *requ = (MyReq*)req;
    request_rec *r = (request_rec *)requ->r;
    HttpRequest* hreq = (HttpRequest*)requ->htreq;
    if (key == NULL || value == NULL || value[0] == '\0')
        return 1;
    hreq->buildRequest(key,value);
    stringstream ss;
    string d,df;
    ss << "<br/>";
    ss >> d;
    df += d;
    ss.clear();
    ss << key;
    ss >> d;
    df += d;
    ss.clear();
    ss << " : ";
    ss >> d;
    df += d;
    ss.clear();
    ss << value;
    ss >> d;
    df += d;
    //ap_rprintf(r, df.c_str());
    return 1;
}


static void getRequestStructure(request_rec *r,HttpRequest* req)
{
	//string host = "" + apr_table_get(r->headers_in, "Host");
	apr_table_get((const apr_table_t *)r->headers_in, "User-Agent");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Language");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Encoding");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Charset");
	apr_table_get((const apr_table_t *)r->headers_in, "Keep-Alive");
	apr_table_get((const apr_table_t *)r->headers_in, "Connection");
	apr_table_get((const apr_table_t *)r->headers_in, "Cache-Control");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-Type");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-Length");
	apr_table_get((const apr_table_t *)r->headers_in, "Referer");
	apr_table_get((const apr_table_t *)r->headers_in, "Pragma");
	/*HttpRequest* req= new HttpRequest();
	this->setHost(temp.at(1));
	this->setUser_agent(temp.at(1));
	this->setAccept(temp.at(1));
	if(temp.at(0)=="Accept-Language")
	{
		strVec lemp;
		this->setAccept_lang(temp.at(1));
		boost::iter_split(lemp, temp.at(1), boost::first_finder(","));
		for(unsigned int li=0;li<lemp.size();li++)
		{
			if(lemp.at(li).find(";")==string::npos && lemp.at(li)!="")
			{
				string t = lemp.at(li);
				size_t s = t.find_first_not_of(" ");
				size_t e = t.find_last_not_of(" ")+1;
				t = t.substr(s,e-s);
				this->localeInfo.push_back(t);
			}
			else if(lemp.at(li)!="")
			{
				string t = lemp.at(li);
				size_t s = t.find_first_not_of(" ");
				size_t e = t.find(";");
				t = t.substr(s,e-s);
				e = t.find_last_not_of(" ")+1;
				t = t.substr(0,e);
				this->localeInfo.push_back(t);
			}
		}
		//cout << temp.at(1) << flush;
	}
	this->setAccept_encod(temp.at(1));
	this->setAccept_lang(temp.at(1));
	this->setKeep_alive(temp.at(1));
	this->setConnection(temp.at(1));
	this->setCache_ctrl(temp.at(1));
	this->setContent_type(temp.at(1));
	this->setContent_len(temp.at(1));
	this->setReferer(temp.at(1));
	this->setPragma(temp.at(1));*/


}

void signalSIGSEGV(int dummy)
{
	signal(SIGSEGV,signalSIGSEGV);
	string filename;
	stringstream ss;
	ss << serverRootDirectory;
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
	fprintf(stderr, tempo.c_str());
	fflush(stderr);
	cout << "Segmentation fault occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

static apr_bucket* get_file_bucket(request_rec* r, const char* fname)
{
  apr_file_t* file = NULL ;
  apr_finfo_t finfo ;
  if ( apr_stat(&finfo, fname, APR_FINFO_SIZE, r->pool) != APR_SUCCESS ) {
    return NULL ;
  }
  if ( apr_file_open(&file, fname, APR_READ|APR_SHARELOCK|APR_SENDFILE_ENABLED,
	APR_OS_DEFAULT, r->pool ) != APR_SUCCESS ) {
    return NULL ;
  }
  if ( ! file ) {
    return NULL ;
  }
  return apr_bucket_file_create(file, 0, finfo.size, r->pool,
	r->connection->bucket_alloc) ;
}

static int mod_ffeadcpp_method_handler (request_rec *r)
{
	signal(SIGSEGV,signalSIGSEGV);
	/*ap_rprintf (r, "<HTML>\n");
	ap_rprintf (r, "<HEAD><TITLE>My handler</TITLE></HEAD>\n");
	ap_rprintf (r, "<BODY>\n");
	ap_rprintf (r, "Hello, world! My first C++ Module for my frameworkj<BR>\n");
	ap_rprintf (r, "<form name=\"input\" action=\"/\" method=\"post\">Username:<input type=\"text\" name=\"user\" /><input type=\"submit\" value=\"Submit\" /></form>");
	ap_rprintf (r, "Request Data is: ");*/
	apr_bucket_brigade *bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
	apr_status_t status = ap_get_brigade(r->input_filters, bb,AP_MODE_READBYTES, APR_BLOCK_READ,HUGE_STRING_LEN);
	if (status != APR_SUCCESS) {
		return status;
	}
	apr_bucket* b ;
	string content;
	stringstream ss;
	string temp;
	for ( b = APR_BRIGADE_FIRST(bb);b != APR_BRIGADE_SENTINEL(bb);b = APR_BUCKET_NEXT(b) )
	{
		size_t bytes ;
		const char* buf = "\0";
		if ( APR_BUCKET_IS_EOS(b) )
		{
		}
		else if (apr_bucket_read(b, &buf, &bytes, APR_BLOCK_READ)== APR_SUCCESS )
		{
			content += buf;
		}
	}
	HttpRequest* hreq= new HttpRequest();
	string webpath = "URL" + serverRootDirectory + "web/";
	MyReq* req = new MyReq();
	req->r = r;
	req->htreq = hreq;

	apr_table_do(iterate_func, req, r->headers_in, NULL);

	/*ss << r->content_type;
	ss >> temp;
	hreq->setContent_type(temp);

	ss << r->content_encoding;
	ss >> temp;
	hreq->setAccept_encod(temp);

	ss << r->uri;
	ss >> temp;
	hreq->setUrl(temp);

	ap_rprintf (r, "<br/><br/><br/><br/>URI: ");
	ap_rprintf (r, r->uri);

	ap_rprintf (r, "<br/>FileName: ");
	ap_rprintf (r, r->filename);

	ap_rprintf (r, "<br/>Unparsed URI:");
	ap_rprintf (r, r->unparsed_uri);

	ss << r->filename;
	ss >> temp;
	hreq->setUrl(temp);

	ap_rprintf (r, "<br/>Path Info");
	ap_rprintf (r, r->path_info);*/
	//hreq->buildRequest("Method",r->method);
	//hreq->buildRequest("Method",r->method);
	string contret;
	if(content!="")
	{
		/*size_t rn = content.find_first_of("\r\n");
		string h = boost::lexical_cast<string>(rn);

		string boundary = content.substr(0,rn);
		fprintf(stderr,boundary.c_str());
		fflush(stderr);

		string retval;
		string delb = boundary+"\r\n";
		string delend = boundary+"--\r\n";
		size_t stb = content.find_first_of(delb)+delb.length();
		size_t enb = content.find_last_not_of(delend);
		h = boost::lexical_cast<string>(stb)+" "+boost::lexical_cast<string>(enb);
		fprintf(stderr,h.c_str());
		fflush(stderr);
		string param_conts = content.substr(stb,enb-stb);
		strVec parameters;
		boost::iter_split(parameters, param_conts, boost::first_finder(delb));
		retval =  "Boundary: "+ boundary + "\nLength: " ;
		retval += boost::lexical_cast<string>(content.length()) +"\nStart End: "
		+ boost::lexical_cast<string>(stb) + " " + boost::lexical_cast<string>(enb) +"\n";

		for(unsigned j=0;j<parameters.size();j++)
		{
			if(parameters.at(j)=="" || parameters.at(j).find_first_not_of(" ")==string::npos
					|| parameters.at(j).find_first_not_of("\r\n")==string::npos)
				continue;
			//FormData datf;
			string parm = parameters.at(j);
			retval+= parm + "\nparm";
			size_t dis = parm.find("Content-Disposition: ");
			string cont_disp,cont_type;
			if(dis!=string::npos)
			{
				size_t dist = parm.find("Content-Type: ");
				size_t dise;
				if(dist==string::npos)
				{
					dist = parm.find("\r\n\r\n");
					dise = dist + 4;
				}
				else
				{
					//parm = parm.substr(dist+14);
					cont_type = parm.substr(dist+14,parm.find("\r\n\r\n")-(dist+14));
					dise = parm.find("\r\n\r\n") + 4;
					cout << "\nctype = " << cont_type << flush;
					//dist = dist-12;
				}
				cont_disp = parm.substr(dis+21,dist-(dis+21));
				boost::replace_first(cont_disp,"\r\n","");
				retval+=  "\ncdisp = " + cont_disp;
				retval+= "\ndise = " + boost::lexical_cast<string>(dise);
				parm = parm.substr(dise);
			}
			//dis = parm.find("Content-Type: ");
			//cout << dis << " dis\n" << flush;
			//if(dis!=string::npos)
			//{
				//string cont_type = parm.substr(dis+14);
				//parm = parm.substr(dis+4);
				strVec parmdef;
				boost::iter_split(parmdef, cont_disp, boost::first_finder(";"));
				string key;
				for(unsigned k=0;k<parmdef.size();k++)
				{
					if(parmdef.at(k)!="" && parmdef.at(k).find("=")!=string::npos)
					{
						size_t stpd = parmdef.at(k).find_first_not_of(" ");
						size_t enpd = parmdef.at(k).find_last_not_of(" ");
						retval+=  "\nparmdef = " + parmdef.at(k) ;
						retval+=  "\nst en = " + boost::lexical_cast<string>(stpd)  + " " + boost::lexical_cast<string>(enpd);
						string propert = parmdef.at(k).substr(stpd,enpd-stpd+1);
						strVec proplr;
						boost::iter_split(proplr, propert, boost::first_finder("="));
						if(proplr.size()==2)
						{
							if(proplr.at(0)=="name" && proplr.at(1)!="\"\"")
							{
								key = proplr.at(1);
								key = key.substr(key.find_first_not_of("\""),key.find_last_not_of("\"")-key.find_first_not_of("\"")+1);
								//datf.type = cont_type;
								//datf.value = parm;
							}
							else if(proplr.at(0)=="filename" && proplr.at(1)!="\"\"")
							{
								string fna = proplr.at(1);
								fna = fna.substr(fna.find_first_not_of("\""),fna.find_last_not_of("\"")-fna.find_first_not_of("\"")+1);
								//datf.fileName = fna;
							}
						}
					}
				}
			//}
		}
		fprintf(stderr,retval.c_str());
		fflush(stderr);*/
		contret = hreq->buildRequest("Content",content.c_str());
		fprintf(stderr,contret.c_str());
		fflush(stderr);
	}
	/*if(r->boundary != NULL && r->boundary[0] != '\0')
	{
		hreq->buildRequest("Boundary",r->boundary);
	}*/
	//hreq->buildRequest(webpath.c_str(),r->the_request);
	hreq->buildRequest(webpath.c_str(),r->uri);
	hreq->buildRequest("Method",r->method);
	if(r->args != NULL && r->args[0] != '\0')
	{
		/*ap_rprintf (r, "<br/>Args");
		ap_rprintf (r, r->args);*/
		hreq->buildRequest("GetArguments",r->args);
	}
	hreq->setHttpVersion(r->protocol);

	/*ap_rprintf (r, "<br/>Canonical FileName");
	ap_rprintf (r, r->canonical_filename);

	ap_rprintf (r, "<br/>FileName: ");
	ap_rprintf (r, r->filename);

	ap_rprintf (r, "<br/>Unparsed URI:");
	ap_rprintf (r, r->unparsed_uri);

	ap_rprintf (r, "<br/>parsed URI Path:");
	ap_rprintf (r, r->parsed_uri.path);

	string star =  hreq->toString();
	char sta[star.length()+1];
	ss.clear();
	ss << star;
	ss >> sta;
	ap_rprintf (r, star.c_str());*/

	HttpResponse respo = service(hreq);
	fprintf(stderr,"\n\n\n\n--------------------------------------------\n");
	fprintf(stderr,hreq->getUrl().c_str());
	fprintf(stderr,"\n");
	string star =  hreq->toString();
	fprintf(stderr,star.c_str());
	fprintf(stderr,"\n--------------------------------------------\n\n\n\n");
	fprintf(stderr,contret.c_str());
	fflush(stderr);
	fprintf(stderr,"\n\n");
	fflush(stderr);
	fprintf(stderr, respo.generateResponse().c_str());
	fflush(stderr);

    if(respo.getContent_type()!="")
	{
    	r->content_type = respo.getContent_type().c_str();
	}
    if(respo.getContent_str()!="")
    {
    	ap_rprintf (r, respo.getContent_str().c_str());
    	return OK;
    }
	/*r->filename = (char*)hreq->getUrl().c_str();
	r->canonical_filename = (char*)hreq->getUrl().c_str();
	stringstream sd;string zz;
	sd << r->unparsed_uri;
	sd >> zz;
	string tesf = "/server/web/default"+zz;
	r->unparsed_uri = (char*)tesf.c_str();
	r->parsed_uri.path = (char*)tesf.c_str();

	ap_rprintf (r, "<br/>filename:");
	ap_rprintf (r, r->filename);
	ap_rprintf (r, "<br/>canonical_filename:");
	ap_rprintf (r, r->canonical_filename);
	ap_rprintf (r, "</BODY>\n");
	ap_rprintf (r, "</HTML>\n");*/

	/*apr_bucket* filebuck = get_file_bucket(r,(char*)hreq->getUrl().c_str());
	apr_bucket_brigade* tmpbb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
	APR_BRIGADE_INSERT_TAIL(tmpbb, filebuck);
	ap_pass_brigade(r->output_filters, tmpbb);
	apr_brigade_cleanup(tmpbb);*/
	return OK;
}


//Every module must declare it's data structure as shown above. Since this module does not require any configuration most of the callback locations have been left blank, except for the last one - that one is invoked by the HTTPD core so that the module can declare other functions that should be invoked to handle various events (like an HTTP request).

/*
 * This function is a callback and it declares what
 * other functions should be called for request
 * processing and configuration requests. This
 * callback function declares the Handlers for
 * other events.
 */

static void one_time_init()
{
	char path[1024];
	pwd(path,sizeof path);
	ffprintf(path);

	stringstream sf;
	sf << path;
	sf >> serverRootDirectory;
	PropFileReader pread;
	serverRootDirectory += "/";
	serverRootDirectory = "/var/www/html/server/";
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	resourcePath = respath;
	string logf = serverRootDirectory+"/server.log";
	logfile.open(logf.c_str());
	string logp = respath+"/log.prop";
	Logger::init(logp);
	strVec webdirs,webdirs1,pubfiles;
	listi(webpath,"/",true,webdirs);
	listi(webpath,"/",false,webdirs1);
	listi(pubpath,".js",false,pubfiles);
	for(unsigned int var=0;var<pubfiles.size();var++)
	{
		pubMap[pubfiles.at(var)] = "true";
	}
	strVec all,dcps,afcd,appf,wspath,compnts,cmpnames,handoffVec;
	string includeRef;
	TemplateEngine templ;
	Context cntxt;
	string libs,ilibs,isrcs,iobjs,ideps;
	Reflection ref;
	vector<bool> stat;
	strVec vecvp,pathvec;
	propMap srp;
	XmlParser parser("Parser");
	ComponentGen gen;
	params = pread.getProperties(serverRootDirectory+"resources/security.prop");

	cout << params.size() <<endl;
	string rundyncontent;
	for(unsigned int var=0;var<webdirs.size();var++)
	{
		//cout <<  webdirs.at(0) << flush;
		string defpath = webdirs.at(var);
		string compres = "mkdir "+defpath + "temp";
		int i=system(compres.c_str());
		if(!i)
		{
			cout << "creating temp folder..." << endl;
			Logger::info("Done creating temp folder...");
		}
		/*compres = "mkdir "+defpath + "scripts";
		i=system(compres.c_str());
		if(!i)
		{
			cout << "creating scripts folder..." << endl;
			Logger::info("Done creating scripts folder...");
		}*/
		string dcppath = defpath + "dcp/";
		string cmppath = defpath + "components/";
		string usrincludes = defpath + "include/";
		//propMap srp = pread.getProperties(defpath+"config/app.prop");

		string name = webdirs1.at(var);
		boost::replace_all(name,"/","");
		rundyncontent += "cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/"+name+"/public/\n";
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
				else if(eles.at(apps).getTagName()=="restcontrollers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="restcontroller")
						{
							string url = cntrls.at(cntn).getAttribute("urlpath");
							string clas = cntrls.at(cntn).getAttribute("class");
							string rname = cntrls.at(cntn).getAttribute("name");
							ElementList resfuncs = cntrls.at(cntn).getChildElements();
							for (unsigned int cntn1 = 0; cntn1 < resfuncs.size(); cntn1++)
							{
								if(resfuncs.at(cntn1).getTagName()=="restfunction")
								{
									RestFunction restfunction;
									restfunction.name = resfuncs.at(cntn1).getAttribute("name");
									restfunction.alias = resfuncs.at(cntn1).getAttribute("alias");
									restfunction.clas = clas;
									restfunction.meth = resfuncs.at(cntn1).getAttribute("meth");
									restfunction.baseUrl = resfuncs.at(cntn1).getAttribute("baseUrl");
									ElementList resfuncparams = resfuncs.at(cntn1).getChildElements();
									for (unsigned int cntn2 = 0; cntn2 < resfuncparams.size(); cntn2++)
									{
										if(resfuncparams.at(cntn2).getTagName()=="param")
										{
											RestFunctionParams param;
											/*try
											{
												param.pos = boost::lexical_cast<int>(resfuncparams.at(cntn2).getAttribute("pos"));
											} catch(...) {
												cout << "CONFIGURATION_ERROR-> Invalid pos attribute specified for function "
														<< restfunction.name << ",pos value should be an integer." << endl;
											}*/
											param.type = resfuncparams.at(cntn2).getAttribute("type");
											restfunction.params.push_back(param);
										}
									}
									//if(restfunction.params.size()==0)
									//	continue;
									if(clas!="")
									{
										if(url.find("*")==string::npos)
										{
											if(url=="")
												url = "/";
											string urlmpp;
											if(rname!="")
											{
												if(restfunction.alias!="")
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+rname+"/"+restfunction.alias;
													else
														urlmpp = name+restfunction.baseUrl;
													rstCntMap[urlmpp] = restfunction;
												}
												else
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+rname+"/"+restfunction.name;
													else
														urlmpp = name+restfunction.baseUrl;
													rstCntMap[name+url+rname+"/"+restfunction.name] = restfunction;
												}
											}
											else
											{
												if(restfunction.alias!="")
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+clas+"/"+restfunction.alias;
													else
														urlmpp = name+restfunction.baseUrl;
													rstCntMap[urlmpp] = restfunction;
												}
												else
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+clas+"/"+restfunction.name;
													else
														urlmpp = name+restfunction.baseUrl;
													rstCntMap[urlmpp] = restfunction;
												}
											}
											cout << "adding rest-controller => " << urlmpp  << " , class => " << clas << endl;
										}
									}
								}
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="handoffs")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="handoff")
						{
							string app = cntrls.at(cntn).getAttribute("app");
							string def = cntrls.at(cntn).getAttribute("default");
							string ext = cntrls.at(cntn).getAttribute("ext");
							handoffs[app] = def;
							handoffs[app+"extension"] = ext;
							handoffVec.push_back("-l"+ app+" ");
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
					string nsfns = "\nvar _fview_namespace = {";
					string js = "\n\nwindow.onload = function(){";
					for (unsigned int appse = 0; appse < elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="event")
						{
							nsfns += "\n\"_fview_cntxt_global_js_callback"+boost::lexical_cast<string>(appse)+"\" : function(response){" + elese.at(appse).getAttribute("cb") + "},";
							js += "\ndocument.getElementById('"+elese.at(appse).getAttribute("eid")+"').";
							js += elese.at(appse).getAttribute("type") + " = function(){";
							js += eles.at(apps).getAttribute("class")+"."+elese.at(appse).getAttribute("func")+"(";
							string args = elese.at(appse).getAttribute("args");
							if(args!="")
								args += ",";
							js += args + "\"_fview_cntxt_global_js_callback"+boost::lexical_cast<string>(appse)+"\",\"/"+name+"/"+fvw+"\",_fview_namespace);}";
						}
						else if(elese.at(appse).getTagName()=="form")
						{
							pathvec.push_back(name);
							vecvp.push_back(usrincludes);
							stat.push_back(true);
							afcd.push_back(elese.at(appse).getAttribute("bean"));
							formMap[elese.at(appse).getAttribute("name")] = elese.at(appse);
						}
					}
					js += "\n}\n\n";
					nsfns = nsfns.substr(0,nsfns.length()-1) + "\n}\n";
					js = nsfns + js;
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
	for (unsigned int cntn = 0; cntn < handoffVec.size(); cntn++)
	{
		boost::replace_first(libs, handoffVec.at(cntn), "");
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
	TemplateEngine engine;
	cntxt.clear();
	cntxt["Dynamic_Public_Folder_Copy"] = rundyncontent;
	string cont = engine.evaluate(respath+"/rundyn_template.sh", cntxt);
	AfcUtil::writeTofile(respath+"/rundyn.sh", cont, true);

	props = pread.getProperties(respath+"mime-types.prop");
	lprops = pread.getProperties(respath+"locale.prop");
	string compres = "bash "+respath+"/run.sh";
	int i=system(compres.c_str());
	if(!i)
	{
		cout << "Done" << flush;
	}

	dlib = dlopen("libinter.so", RTLD_NOW|RTLD_GLOBAL);
	if(dlib==NULL)
	{
		cout << dlerror() << endl;
		Logger::info("Could not load Library");
	}
	else
		Logger::info("Library loaded successfully");
}



static int myInputFilter(ap_filter_t *f,apr_bucket_brigade *bb,ap_input_mode_t mode,apr_read_type_e block,apr_off_t readbytes)
{
    apr_bucket* b ;
	for ( b = APR_BRIGADE_FIRST(bb);b != APR_BRIGADE_SENTINEL(bb);b = APR_BUCKET_NEXT(b) )
	{
		const char* buf ;
		size_t bytes ;
		if ( APR_BUCKET_IS_EOS(b) )
		{
		}
		else if (apr_bucket_read(b, &buf, &bytes, APR_BLOCK_READ)== APR_SUCCESS )
		{
			//size_t count = 0 ;
			//const char* p = buf ;
			ofstream myfile1;
			myfile1.open("/home/sumeet/bucket_data.tmp");
			if (myfile1.is_open())
			{
				myfile1.write(buf,sizeof buf);
				myfile1.close();
			}
		}
	}
    return ap_pass_brigade(f->next, bb) ;
}

static const char *myInputFilterName = "MyFilter";
static bool doneOnce;
static void mod_ffeadcpp_register_hooks (apr_pool_t *p)
{

	//ap_hook_insert_filter(myModuleInsertFilters, NULL, NULL, APR_HOOK_MIDDLE) ;

	//ap_register_input_filter(myInputFilterName, myInputFilter, NULL,AP_FTYPE_RESOURCE);


	/*ap_register_output_filter(myOutputFilterName, myOutputFilter,
		NULL, AP_FTYPE_RESOURCE) ;*/

	ap_hook_handler(mod_ffeadcpp_method_handler, NULL, NULL, APR_HOOK_LAST);
	if(doneOnce==false)
	{
		fprintf(stderr, "init once and once only.........");
		fflush(stderr);
		one_time_init();
	}
	doneOnce = true;
}
extern "C"
{

	//When this function is called by the HTTPD core, it registers a handler that should be invoked for all HTTP requests.

	/*
	 * This function is registered as a handler for HTTP methods and will
	 * therefore be invoked for all GET requests (and others).  Regardless
	 * of the request type, this function simply sends a message to
	 * STDERR (which httpd redirects to logs/error_log).  A real module
	 * would do *alot* more at this point.
	 */



	//Obviously an Apache module will require information about structures, macros and functions from Apache's core. These two header files are all that is required for this module, but real modules will need to include other header files relating to request handling, logging, protocols, etc.

	/*
	 * Declare and populate the module's data structure.  The
	 * name of this structure ('ffead_ccp_module') is important - it
	 * must match the name of the module.  This structure is the
	 * only "glue" between the httpd core and the module.
	 */
	module AP_MODULE_DECLARE_DATA ffead_ccp_module =
	{
	  // Only one callback function is provided.  Real
	  // modules will need to declare callback functions for
	  // server/directory configuration, configuration merging
	  // and other tasks.
	  STANDARD20_MODULE_STUFF,
	  NULL,
	  NULL,
	  NULL,
	  NULL,
	  NULL,
	  mod_ffeadcpp_register_hooks,      /* callback for registering hooks */
	};
};

