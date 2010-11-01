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

//int nor=0;
//ofstream logfile;
CHServer::CHServer()
{}

CHServer::~CHServer() {
	// TODO Auto-generated destructor stub
}

//static map<pid_t,bool> CHServer::get()->getststs();
static Logger *logger;
static propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap;
static string resourcePath;
static string libName = "libinter.so";
static void *dlib;
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
		resp += ("Content-Length: "+res.getContent_len() + "\n");
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
	command = ("ls -F1 "+cwd+"|grep '"+type+"$'");
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
			if(apDir)
				folders.push_back(cwd+folderName);
			else
				folders.push_back(folderName);
			folderName = "";
		}
	}
	pclose(pipe_fp);
}

void service(int fd,string serverRootDirectory,string ip)
{
	try{
	Reflector ref;
	char buf[MAXBUFLENM];
	strVec results;
	stringstream ss;
	string temp;
	int bytes = -1;
	while((bytes=recv(fd, buf, sizeof(buf), MSG_DONTWAIT))>0)
	{
		ss << buf;
		memset(&buf[0], 0, sizeof(buf));
		//cout << "\n" << bytes << flush;
		bytes = -1;
	}
	/*while((bytes=recv(fd, buf, sizeof buf, 0))!=-1)
	{
		ss << buf;
		buf[0] = '\0';
	}*/
	while(getline(ss,temp,'\n'))
	{
		results.push_back(temp);
	}
	string webpath = serverRootDirectory + "web/";
	HttpRequest* req= new HttpRequest(results,webpath);
	string sessId = (ip + req->getUser_agent());
	HttpSession sess = sessionMap[sessId];
	/*if(req->getCntxt_name()=="public" && pubMap[req->getFile()]=="true")
	{
		req->setUrl(serverRootDirectory+req->getActUrl());
	}
	else */if(cntMap[req->getCntxt_name()]!="true")
	{
		req->setCntxt_name("default");
		req->setCntxt_root(webpath+"default");
		req->setUrl(webpath+"default"+req->getActUrl());
	}
	if(appMap[req->getCntxt_name()]!="false")
	{
		//void *dlib1 = dlopen("libinter.so", RTLD_NOW);
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
			path1 = f(req->getUrl(),sess);
			cout << path1 << flush;
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
	res.setHttpVersion(req->getHttpVersion());

	if(sess.getSessionId()!="")
		req->setSession(sess);
	else
	{
		sess = req->getSession();
		sess.setSessionId(sessId);
	}
	string treee = "sumeet";
	sess.setAttribute("USER",treee);
	sess.setAttribute("PASS",treee);
	string ext = getFileExtension(req->getUrl());
	vector<unsigned char> test;

	if(ext!=".dcp" && ext!=".view" && ext!=".tpe" && ext!=".wsdl")
		test = getContentVec(req->getUrl(),lprops[req->getDefaultLocale()],ext);

	string claz;
	cout << urlpattMap["*.*"] << flush;
	if(urlpattMap["*.*"]!="" || urlMap[ext]!="")
	{
		if(urlpattMap["*.*"]!="")
			claz = "getReflectionCIFor" + urlpattMap["*.*"];
		else
			claz = "getReflectionCIFor" + urlMap[ext];
		string libName = "libinter.so";
		//void *dlib = dlopen(libName.c_str(), RTLD_NOW);
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
			void *_temp = ref.newInstanceGVP(ctor);
			Controller *thrd = (Controller *)_temp;
			try{
				 cout << "Controller called" << endl;
			thrd->service(*req);
			}catch(...){ cout << "Controller exception" << endl;}
			cout << "Controller called\n" << flush;
		}
	}
	else if(req->getMethod()=="POST" && req->getRequestParam("claz")!="" && req->getRequestParam("method")!="")
	{
		string con = AfcUtil::execute(*req);
		res.setStatusCode("200");
		res.setStatusMsg("OK");
		res.setContent_type(props[".txt"]);
		res.setContent(con);
		res.setContent_len(boost::lexical_cast<string>(con.size()));
	}
	else if(ext==".dcp")
	{
		string libName = "libinter.so";
		//void *dlib = dlopen(libName.c_str(), RTLD_NOW);
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
			DCPPtr f =  (DCPPtr)mkr;
			f();
			string patf;
			patf = req->getCntxt_root() + "/dcp/_" + file + ".html";
			test = getContentVec(patf,lprops[req->getDefaultLocale()],ext);
		}
		ext = ".html";
		if(ext!="" && test.size()==0)
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			res.setContent(test);
			res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent(test);
			res.setContent_len(boost::lexical_cast<string>(test.size()));
		}
	}
	else if(ext==".view" && vwMap[req->getFile()]!="")
	{
		string libName = "libinter.so";
		//void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		claz = "getReflectionCIFor" + vwMap[req->getFile()];
		void *mkr = dlsym(dlib, claz.c_str());
		if(mkr!=NULL)
		{
			FunPtr f =  (FunPtr)mkr;
			ClassInfo srv = f();
			args argus;
			Constructor ctor = srv.getConstructor(argus);
			void *_temp = ref.newInstanceGVP(ctor);
			DynamicView *thrd = (DynamicView *)_temp;
			Document doc = thrd->getDocument();
			View view;
			string t = view.generateDocument(doc);
			Cont test1(t.begin(),t.end());
			test = test1;
		}
		ext = ".html";
		if(ext!="" && test.size()==0)
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			res.setContent(test);
			res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent(test);
			res.setContent_len(boost::lexical_cast<string>(test.size()));
		}
	}
	else if(ext==".tpe" && tmplMap[req->getFile()]!="")
	{
		string xml = "<html><head><script src=\"${dat}\"></script></head><body><input type=\"text\"/><input type=\"submit\"/></body></html>";
		TemplateEngine te;
		ext = ".html";
		string libName = "libinter.so";
		//void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		claz = "getReflectionCIFor" + tmplMap[req->getFile()];
		void *mkr = dlsym(dlib, claz.c_str());
		if(mkr!=NULL)
		{
			FunPtr f =  (FunPtr)mkr;
			ClassInfo srv = f();
			args argus;
			Constructor ctor = srv.getConstructor(argus);
			void *_temp = ref.newInstanceGVP(ctor);
			TemplateHandler *thrd = (TemplateHandler *)_temp;
			Context cnt = thrd->getContext();
			string t = te.evaluate(req->getUrl(),cnt);
			Cont test1(t.begin(),t.end());
			test = test1;
		}
		if(ext!="" && test.size()==0)
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			res.setContent(test);
			res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent(test);
			res.setContent_len(boost::lexical_cast<string>(test.size()));
		}
	}
	else if((req->getContent_type().find("application/soap+xml")!=string::npos || req->getContent_type().find("text/xml")!=string::npos)
			&& (req->getContent().find("<soap:Envelope")!=string::npos || req->getContent().find("<soapenv:Envelope")!=string::npos))
	{
		string libName = "libinter.so";
		//void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string meth,ws_name;
		ws_name = req->getFile();
		try
		{
			XmlParser parser("Parser");
			//cout << req->getContent() << flush;
			Document doc = parser.getDocument(req->getContent());
			Element soapenv = doc.getRootElement();
			cout << soapenv.getTagName() << "----\n" << flush;
			Element soapbody;
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
				outpt = "<" + method.getNameSpc() + ":" + meth + "Response>" + outpt + "</" + method.getNameSpc() + ":" + meth + "Response>";
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
				string env = "<" + soapenv.getTagNameSpc();
				for(it=attl.begin();it!=attl.end();it++)
				{
					env.append(" " + it->first + "=\"" + it->second + "\" ");
				}
				env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
				res.setStatusCode("200");
				res.setStatusMsg("OK");
				res.setContent_type(props[".xml"]);
				res.setContent(env);
				res.setContent_len(boost::lexical_cast<string>(env.size()));
			}
			cout << methodname << "end----\n" << flush;
		}
		catch(Exception *e)
		{
			cout << e->what() << flush;
		}
	}
	else if(ext==".wsdl")
	{
		test = getContentVec(resourcePath+req->getFile(),"english",ext);
		if(test.size()==0)
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			res.setContent(test);
			res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent(test);
			res.setContent_len(boost::lexical_cast<string>(test.size()));
			sess.setAttribute("CURR",req->getUrl());
		}
	}
	else
	{
		if(ext!="" && test.size()==0)
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			res.setContent(test);
			res.setContent_len(boost::lexical_cast<string>(0));
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent(test);
			res.setContent_len(boost::lexical_cast<string>(test.size()));
			sess.setAttribute("CURR",req->getUrl());
		}
	}

	string h1;
	h1 = createResponse(res);
	if (send(fd,&h1[0] , h1.length(), 0) == -1)
		cout << "send failed" << flush;

	int size;
	if(res.getStatusCode()!="404" && res.getContent().size()>0)
	{
		if ((size=send(fd,&(res.getContent())[0],res.getContent().size(),0)) == -1)
			cout << "send failed" << flush;
	}

	memset(&buf[0], 0, sizeof(buf));
	close(fd);

	ss.clear();
	//boost::mutex::scoped_lock lock(m_mutex);
	//string h;
	//ss << (++nor);
	//ss >> h;
	boost::mutex::scoped_lock lock(m_mutex);
	//h = "\nRequest Number: " + h + ": " + res.getStatusCode();
	/*logfile << ("got new connection to process\n") << flush;
	logfile << (req->getFile()+" :: " + res.getStatusCode() + "\n") << flush;
	logfile << (req->getCntxt_name() + "\n") << flush;
	logfile << (req->getCntxt_root() + "\n") << flush;
	logfile << (req->getUrl() + "\n") << flush;*/
	logger->info("got new connection to process\n"+req->getFile()+" :: " + res.getStatusCode() + "\n"+req->getCntxt_name() + "\n"+req->getCntxt_root() + "\n"+req->getUrl());
	delete req;
	sessionMap[sessId] = sess;}catch(...){ cout << "Standard exception: " << endl;}
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
string servd;
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

void cleanUpRoutine(string tempo)
{
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
ExceptionHandler g_objExceptionHandler();
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
		if(dlib==NULL)
		{
			dlib = dlopen("libinter.so", RTLD_NOW|RTLD_GLOBAL);
			if(dlib==NULL)
			{
				cout << dlerror() << endl;
				logger->info("Could not load Library");
			}
			else
				logger->info("Library loaded successfully");
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
		while(1)
		{
			int nfds = epoll_wait(epoll_handle, events, 1,-1);
			if (nfds == -1)
			{
				perror("epoll_wait child process");
				//break;
			}
			else
			{
				//int fd = receive_fd(sp[j][1]);
				rv = recvmsg(sp[1], &msg, 0);
				if (rv == -1)
				{
					perror("recvmsg");
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
				//printf("child: read %d for %d child pid--%ld\n", fd,events[nfds].data.fd,(long)getpid());
				string jh;

				boost::thread m_thread(boost::bind(&service,fd,serverRootDirectory,jh));
				//cout << "child got connection\n" << flush;
				//close(fd);
				//m_thread.join();
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

int main(int argc, char* argv[])
{
	signal(SIGSEGV,signalSIGSEGV);
	signal(SIGFPE,signalSIGFPE);
	//std::cout << argv[0] << std::endl;
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
	logger = new Logger(logp);

    PropFileReader pread;
    propMap srprps = pread.getProperties(respath+"server.prop");
    if(srprps["NUM_PROC"]!="")
    	preForked = boost::lexical_cast<int>(srprps["NUM_PROC"]);

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
    strVec all,dcps,afcd,appf,wspath,compnts,cmpnames;
    string includeRef;
    TemplateEngine templ;
	Context cntxt;
	string libs,ilibs,isrcs,iobjs,ideps;
	Reflection ref;
	vector<string> vecvec;
	vector<bool> stat;
	propMap srp;

	XmlParser parser("Parser");
	ComponentGen gen;

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
		wspath.push_back(defpath);

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
							if(cntrls.at(cntn).getAttribute("url").find("*")!=string::npos)
							{
								if(url=="*.*")
									urlpattMap[url] = cntrls.at(cntn).getAttribute("class");
								else
								{
									url = url.substr(url.find("*")+1);
									urlMap[url] = cntrls.at(cntn).getAttribute("class");
								}
							}
							else
								urlMap[url] = cntrls.at(cntn).getAttribute("class");
							//cout << url << " :: " << cntrls.at(cntn).getAttribute("class") << flush;
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
							tmplMap[tmplts.at(tmpn).getAttribute("file")] = tmplts.at(tmpn).getAttribute("class");
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
							vwMap[dvs.at(dn).getAttribute("path")] = dvs.at(dn).getAttribute("class");
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
					vecvec.push_back(usrincludes);
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
					vecvec.push_back(usrincludes);
					stat.push_back(false);
					afcd.push_back(objv.at(var1));
				}
			}
		}
    }

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
	}

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
	ret = DCPGenerator::generateDCPAll(dcps);
	AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
	string headers,objs,infjs;
	ret = AfcUtil::generateJsObjectsAll(vecvec,afcd,stat,headers,objs,infjs);
	AfcUtil::writeTofile(rtdcfpath+"AjaxInterface.cpp",ret,true);
	AfcUtil::writeTofile(pubpath+"_afc_Objects.js",objs,true);
	AfcUtil::writeTofile(pubpath+"_afc_Interfaces.js",infjs,true);
	AfcUtil::writeTofile(incpath+"AfcInclude.h",headers,true);
	ApplicationUtil apputil;
	webdirs.clear();
	ret = apputil.buildAllApplications(appf,webdirs1,appMap);
	AfcUtil::writeTofile(rtdcfpath+"ApplicationInterface.cpp",ret,true);
	WsUtil wsu;
	ret = wsu.generateAllWSDL(wspath,respath);
	AfcUtil::writeTofile(rtdcfpath+"WsInterface.cpp",ret,true);

	props = pread.getProperties(respath+"mime-types.prop");
	lprops = pread.getProperties(respath+"locale.prop");
	string compres = "/"+respath+"run.sh";
	int i=system(compres.c_str());
	if(!i)
	{
		cout << "Done" << flush;
		logger->info("Done generating intermediate code");
		//logfile << "Done generating intermediate code\n" << flush;
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
	logger->info("Server: waiting for connections on port "+PORT);
	map<int,pid_t> pds;
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

	while(1)
	{

		if(childNo>=preForked)
			childNo = 0;
		nfds = epoll_wait(epoll_handle, events, curfds,-1);
		if (nfds == -1)
		{
			perror("epoll_wait main process");
			//logfile << "Interruption Signal Received\n" << flush;
			logger->info("Interruption Signal Received\n");
			//break;
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
				printf("new http request\n");
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
						return -1;
					}
				}
			}
			else
			{
				epoll_ctl(epoll_handle, EPOLL_CTL_DEL, events[n].data.fd,&ev);
				curfds--;
				//send_connection(events[n].data.fd,sp[childNo++][0]);

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
					//logfile << ("sent socket to process "+cno+"\n") << flush;
					close(events[n].data.fd);
					childNo++;
					//boost::thread m_thread(boost::bind(&send_connection,events[n].data.fd,sp[childNo++][0]));
					//m_thread.join();
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
					logger->info("Process got killed hence created a new Process\n");
				}
				cntrlfile.close();
			}
		}
	}
	return 0;
}
