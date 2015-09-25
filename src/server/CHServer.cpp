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
 * CHServer.cpp
 *
 *  Created on: Aug 25, 2009
 *      Author: sumeet
 */

#include "CHServer.h"

static string servd, serverCntrlFileNm;
static bool isSSLEnabled = false, isThreadprq = false, processforcekilled = false,
		processgendone = false, isCompileEnabled = false;
static int thrdpsiz, preForked = 5;
static map<int,pid_t> pds;
static Mutex m_mutex, p_mutex;

Logger CHServer::logger;

void sigchld_handler_server(int s)
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

#if !defined(OS_MINGW) && !defined(OS_DARWIN)
int send_connection(int fd,int descriptor)
{
	struct msghdr msg;
	struct iovec  iov [1];
	int           n;

	/* need to send some data otherwise client can't distinguish between
	* EOF and "just sending a file descriptor"
	*/
	iov [0].iov_base = (void *)"x";
	iov [0].iov_len  = 1;

	/* not relevant for connected sockets */
	msg.msg_name    = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov     = iov;
	msg.msg_iovlen  = 1;
	msg.msg_flags   = 0;

	msg.msg_control = NULL;
	msg.msg_controllen = 0;

	/* put the descriptor in the ancillary data */
	{
		/* using a union to ensure its correctly aligned */
		union
		{
		  struct cmsghdr cmsg;
		  char           control [CMSG_SPACE (sizeof (int))];
		} msg_control;
		struct cmsghdr *cmsg;

		msg.msg_control    = &msg_control;
		msg.msg_controllen = sizeof (msg_control);

		cmsg = CMSG_FIRSTHDR (&msg);

		cmsg->cmsg_len   = CMSG_LEN (sizeof (int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type  = SCM_RIGHTS;

		*((int *) CMSG_DATA (cmsg)) = descriptor;
	}

	if((n= sendmsg(fd, &msg, 0)) < 0 )
	{
	  perror("sendmsg()");
	  exit(1);
	}
	close(descriptor);
	return n;
}

int receive_fd(int fd)
{
	union
	{
		struct cmsghdr cmsg;
		char           control [CMSG_SPACE (sizeof (int))];
	} msg_control;

	struct msghdr   msg;
	struct iovec    iov [1];
	struct cmsghdr *cmsg;
	char            buf [192];
	int             n;

	iov [0].iov_base = buf;
	iov [0].iov_len  = sizeof (buf);

	/* not relevant for connected sockets */
	msg.msg_name       = NULL;
	msg.msg_namelen    = 0;
	msg.msg_iov        = iov;
	msg.msg_iovlen     = 1;
	msg.msg_control    = &msg_control;
	msg.msg_controllen = sizeof (msg_control);
	msg.msg_flags      = 0;

	n = recvmsg (fd, &msg, 0);
	if(n == 1)
	{
		for (cmsg = CMSG_FIRSTHDR (&msg); cmsg; cmsg = CMSG_NXTHDR (&msg, cmsg))
		{
			int descriptor;

			if (cmsg->cmsg_len   != CMSG_LEN (sizeof (int)) ||
					cmsg->cmsg_level != SOL_SOCKET              ||
					cmsg->cmsg_type  != SCM_RIGHTS)
				continue;

			descriptor = *((int *) CMSG_DATA (cmsg));
			return descriptor;
		}
	}
	return -1;
}
#endif

void handler(int sig)
{
#if defined(OS_LINUX) || defined(OS_SOLARIS)
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
#endif
}

void siginthandler(int sig)
{
	if(errno!=EINTR) {
		CHServer::getLogger() << "Exiting, Got errono " << sig << endl;
		exit(0);
	}
}

void signalSIGSEGV(int sig)
{
	signal(SIGSEGV,signalSIGSEGV);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Segmentation fault occurred for process" << getpid() << "\n" << endl;
	abort();
}
void signalSIGCHLD(int sig)
{
	signal(SIGCHLD,signalSIGCHLD);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Child process got killed " << getpid() << "\n"  << endl;
	//abort();
}
void signalSIGABRT(int sig)
{
	signal(SIGABRT,signalSIGABRT);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Abort signal occurred for process" << getpid() << "\n" << endl;
	abort();
}
void signalSIGTERM(int sig)
{
	signal(SIGTERM,signalSIGTERM);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Termination signal occurred for process" << getpid() << "\n" << endl;
	abort();
}

void signalSIGKILL(int sig)
{
	signal(SIGKILL,signalSIGKILL);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Kill signal occurred for process" << getpid() << "\n" << endl;
	abort();
}

void signalSIGINT(int sig)
{
	signal(SIGINT,signalSIGINT);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Interrupt signal occurred for process" << getpid() << "\n" << endl;
	//abort();
}

void signalSIGFPE(int sig)
{
	signal(SIGFPE,signalSIGFPE);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Floating point Exception occurred for process" << getpid() << "\n" << endl;
	abort();
}

void signalSIGPIPE(int sig)
{
	signal(SIGPIPE,signalSIGPIPE);
	/*string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());*/
	handler(sig);
	CHServer::getLogger() << "Broken pipe ignore it" << getpid() << "\n" << endl;
	//abort();
}

void signalSIGILL(int sig)
{
	signal(SIGILL,signalSIGILL);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Floating point Exception occurred for process" << getpid() << "\n" << endl;
	abort();
}

void* service(void* arg)
{
	ServiceTask *task = (ServiceTask*)arg;
	//task->run();
	delete task;
	return NULL;
}

#if !defined(OS_MINGW) && !defined(OS_DARWIN) && !defined(OS_CYGWIN)
pid_t createChildProcess(string serverRootDirectory,int sp[],int sockfd)
{
	pid_t pid;
	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sp) == -1)
	{
		perror("socketpair");
		exit(1);
	}
	if((pid=fork())==0)
	{
		char pidStr[10];
		memset(pidStr, 0, 10);
		sprintf(pidStr, "%ld", (long)getpid());
		string lgname = "CHServer-";
		lgname.append(pidStr);
		Logger plogger = LoggerFactory::getLogger(lgname);

		servd = serverRootDirectory;
		string filename;
		stringstream ss;
		ss << serverRootDirectory;
		ss << getpid();
		ss >> filename;
		filename.append(".cntrl");
		plogger << ("generated file " + filename) << endl;
		ofstream cntrlfile;
		cntrlfile.open(filename.c_str());
		cntrlfile << "Process Running" << endl;
		cntrlfile.close();

		close(sockfd);

		//SelEpolKqEvPrt selEpolKqEvPrtHandler;
		//selEpolKqEvPrtHandler.initialize(sp[1]);
		ThreadPool pool;
		if(!isThreadprq)
		{
			pool.init(30);
		}

		struct stat buffer;
		while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
		{
			/*int nfds = selEpolKqEvPrtHandler.getEvents();
			if (nfds == -1)
			{
				perror("poller wait child process");
				plogger << "\n----------poller child process----" << endl;
			}
			else*/
			{
				int fd = receive_fd(sp[1]);
				//selEpolKqEvPrtHandler.reRegisterServerSock();
				#ifdef OS_MINGW
					u_long bMode = 0;
					ioctlsocket(fd, FIONBIO, &bMode);
				#else
					fcntl(fd, F_SETFL, O_SYNC);
				#endif

				char buf[10];
				memset(buf, 0, 10);
				int err;
				if((err=recv(fd,buf,10,MSG_PEEK))==0)
				{
					close(fd);
					plogger << "Socket conn closed before being serviced" << endl;
					continue;
				}

				try
				{
					if(isThreadprq)
					{
						ServiceTask *task = new ServiceTask(fd,serverRootDirectory);
						Thread pthread(&service, task);
						pthread.execute();
					}
					else
					{
						ServiceTask *task = new ServiceTask(fd,serverRootDirectory);
						task->setCleanUp(true);
						pool.submit(task);
					}
				}
				catch(const char* err)
				{
					plogger << "Exception occurred while processing ServiceTask request - " << err << endl;
				}
				catch(...)
				{
					plogger << "Standard exception occurred while processing ServiceTask request " << endl;
				}
			}
		}
	}
	return pid;
}
#endif


/*pid_t createChildMonitProcess(int sp[])
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
			memset(pidStr, 0, 10);
			if(read(sp[1], buf, sizeof buf) < 0)
			{
				string temp = buf;
				strVec tempv;
				StringUtil::split(tempv, temp, ":");
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
}*/

void* gracefullShutdown_monitor(void* args)
{
	string* ipaddr = (string*)args;
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(1);
	}
	string ip = ipaddr->substr(0, ipaddr->find(":"));
	string port = ipaddr->substr(ipaddr->find(":")+1);
	ClientInterface* client;
	if(isSSLEnabled)
		client = new SSLClient;
	else
		client = new Client;
	client->connectionUnresolv(ip,CastUtil::lexical_cast<int>(port));
	client->closeConnection();
	delete client;

	return NULL;
}

#ifdef INC_DCP
void* CHServer::dynamic_page_monitor(void* arg)
{
	string serverRootDirectory = *(string*)arg;
	struct stat statbuf;
	map<string, string> dcpsss = ConfigurationData::getInstance()->dynamicCppPagesMap;
	map<string, string> tpes = ConfigurationData::getInstance()->templateFilesMap;
	map<string, string> dcspstpes = dcpsss;
	dcspstpes.insert(tpes.begin(), tpes.end());
	map<string,long> statsinf;
	map<string, string>::iterator it;
	for(it=dcspstpes.begin();it!=dcspstpes.end();++it)
	{
		stat(it->first.c_str(), &statbuf);
		time_t tm = statbuf.st_mtime;
		long tim = (uintmax_t)tm;
		statsinf[it->first] = tim;
	}
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(5);
		bool flag = false;
		if(processgendone)
			continue;
		for(it=dcspstpes.begin();it!=dcspstpes.end();++it)
		{
			stat(it->first.c_str(), &statbuf);
			time_t tm = statbuf.st_mtime;
			long tim = (uintmax_t)tm;
			if(tim!=statsinf[it->first])
			{
				string rtdcfpath = serverRootDirectory + "rtdcf/";
				string respath = serverRootDirectory + "resources/";

				logger << "started generating dcp code" <<endl;
				string ret = DCPGenerator::generateDCPAll();
				AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
				logger << "done generating dcp code" <<endl;
				logger << "started generating template code" <<endl;
				ret = TemplateGenerator::generateTempCdAll(servd);
				AfcUtil::writeTofile(rtdcfpath+"TemplateInterface.cpp",ret,true);
				logger << "done generating template code" <<endl;

				string compres;
//#if BUILT_WITH_CONFGURE == 1
				compres = respath+"rundyn-automake_dinter.sh "+serverRootDirectory;
//#else
				//compres = respath+"rundyn_dinter.sh "+serverRootDirectory;
//#endif
				string output = ScriptHandler::execute(compres, true);
				//int i=system(compres.c_str()); 
				//if(!i)
				{
					logger << output << endl;
					logger << "regenerating intermediate code-----Done" << endl;
					logger.info("Done generating intermediate code");
				}
				m_mutex.lock();
				#if !defined(OS_MINGW) && !defined(OS_DARWIN)
				if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
				{
					map<int,pid_t>::iterator it;
					for(it=pds.begin();it!=pds.end();it++)
					{
						kill(it->second,9);
					}
				}
				#endif
				m_mutex.unlock();
				processforcekilled = true;
				flag = true;
				break;
			}
		}
		if(flag)
		{
			for(it=dcspstpes.begin();it!=dcspstpes.end();++it)
			{
				stat(it->first.c_str(), &statbuf);
				time_t tm = statbuf.st_mtime;
				long tim = (uintmax_t)tm;
				statsinf[it->first] = tim;
			}
		}
	}
	return NULL;
}
#endif


int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		//cout << "No Server root directory specified, quitting..." << endl;
		return 0;
	}

	string serverRootDirectory = argv[1];
	serverRootDirectory += "/";
	if(serverRootDirectory.find("//")==0)
	{
		RegexUtil::replace(serverRootDirectory,"[/]+","/");
	}

	string port = "";
	string ipaddr = "";
	string servingAppNames = "";
	vector<string> servedAppNames;
	bool isMain = true;

	PropFileReader pread;
	string respath = serverRootDirectory + "resources/";
	propMap srprps = pread.getProperties(respath+"server.prop");

	std::ofstream local("/dev/null");
	if(StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])!="true") {
		streambuf* cout_buff = std::cout.rdbuf();
		std::cout.rdbuf(local.rdbuf());
	}

	if(argc > 2)
	{
		port = argv[2];
		isMain = false;
	}
	if(port=="")
	{
		port = srprps["IP_ADDR"];
	}
	if(argc > 3)
	{
		ipaddr = argv[3];
	}
	if(ipaddr=="")
	{
		ipaddr = srprps["IP_ADDR"];
	}
	if(argc > 4)
	{
		servingAppNames = argv[4];
		servedAppNames = StringUtil::splitAndReturn<vector<string> >(servingAppNames, ",");
	}
	int vhostNum = 0;
	if(argc > 5)
	{
		vhostNum = CastUtil::lexical_cast<int>(argv[5]);
	}

	try {
		return CHServer::entryPoint(vhostNum, isMain, serverRootDirectory, port, ipaddr, servedAppNames);
	} catch (const char* e) {
		cout << e << endl;
	} catch (const XmlParseException& e) {
		cout << e.getMessage() << endl;
	} catch (...) {
		cout << "Error Occurred in serve" << endl;
	}
	return 0;
}


int CHServer::entryPoint(int vhostNum, bool isMain, string serverRootDirectory, string port, string ipaddr, vector<string> servedAppNames)
{
	pid_t parid = getpid();

	#ifndef OS_MINGW
	/*struct sigaction act;
	memset (&act, '\0', sizeof(act));
	act.sa_handler = siginthandler;
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}*/
	//signal(SIGSEGV,signalSIGSEGV);
	//signal(SIGFPE,signalSIGFPE);
	(void) sigignore(SIGPIPE);
	//(void) sigignore(SIGINT);
	#else
		// startup WinSock in Windows
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(1,1), &wsa_data);
	#endif
	
	#ifdef OS_MINGW
	SOCKET sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	#else
	int sockfd, new_fd;
	#endif
	
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

    //int yes=1,rv;
    int nfds;

    serverRootDirectory += "/";
	if(serverRootDirectory.find("//")==0)
	{
		RegexUtil::replace(serverRootDirectory,"[/]+","/");
	}

	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	string logpath = serverRootDirectory + "logs/";
	string resourcePath = respath;

	PropFileReader pread;
	propMap srprps = pread.getProperties(respath+"server.prop");

	servd = serverRootDirectory;
	string logp = respath+"/logging.xml";
	LoggerFactory::init(logp, serverRootDirectory, "", StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true");

	string name;
	if(isMain)
	{
		serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
		name = "CHServer(Main)";
		LoggerFactory::instance->setVhostNumber(0);
	}
	else
	{
		name = "CHServer(VHost-" + CastUtil::lexical_cast<string>(vhostNum) + ")";
		serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." + CastUtil::lexical_cast<string>(vhostNum);
	}

	logger = LoggerFactory::getLogger(name);

    if(srprps["NUM_PROC"]!="")
    {
    	try
		{
    		preForked = CastUtil::lexical_cast<int>(srprps["NUM_PROC"]);
		}
		catch(...)
		{
			logger << "Invalid number for worker processes defined" << endl;
			preForked = 5;
		}
    }
    string sslEnabled = srprps["SSL_ENAB"];
   	if(sslEnabled=="true" || sslEnabled=="TRUE")
   		isSSLEnabled = true;
   	string thrdpreq = srprps["THRD_PREQ"];
   	if(thrdpreq=="true" || thrdpreq=="TRUE")
   		isThreadprq = true;
   	else
	{
		thrdpreq = srprps["THRD_PSIZ"];
		if(thrdpreq=="")
			thrdpsiz = 10;
		else
		{
			try
			{
				thrdpsiz = CastUtil::lexical_cast<int>(thrdpreq);
			}
			catch(...)
			{
				logger << "Invalid thread pool size defined" << endl;
				thrdpsiz = 10;
			}
		}
	}
   	string compileEnabled = srprps["DEV_MODE"];
	if(compileEnabled=="true" || compileEnabled=="TRUE")
		isCompileEnabled = true;

	/*if(srprps["SCRIPT_ERRS"]=="true" || srprps["SCRIPT_ERRS"]=="TRUE")
	{
		SCRIPT_EXEC_SHOW_ERRS = true;
	}*/
	bool sessatserv = true;
   	if(srprps["SESS_STATE"]=="server")
   		sessatserv = true;
   	long sessionTimeout = 3600;
   	if(srprps["SESS_TIME_OUT"]!="")
   	{
   		try {
   			sessionTimeout = CastUtil::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch (...) {
			logger << "Invalid session timeout value defined, defaulting to 1hour/3600sec" << endl;
		}
   	}

   	if(port=="")
   	{
   		port = srprps["PORT_NO"];
   	}

	string ipport;

	if(ipaddr!="")
	{
		if(port=="80")
			ipport = ipaddr;
		else
			ipport = ipaddr + ":" + port;
	}
	else
	{
		if(port=="80")
			ipport = "localhost";
		else
			ipport = "localhost:" + port;
	}

	ConfigurationData::getInstance();
	SSLHandler::setIsSSL(isSSLEnabled);

	strVec webdirs,webdirs1,pubfiles;
	//ConfigurationHandler::listi(webpath,"/",true,webdirs,false);
	CommonUtils::listFiles(webdirs, webpath, "/");
    //ConfigurationHandler::listi(webpath,"/",false,webdirs1,false);
	CommonUtils::listFiles(webdirs1, webpath, "/", false);

    CommonUtils::loadMimeTypes(respath+"mime-types.prop");
	CommonUtils::loadLocales(respath+"locale.prop");

	RegexUtil::replace(serverRootDirectory,"[/]+","/");
	ConfigurationData::getInstance()->coreServerProperties.serverRootDirectory = serverRootDirectory;
	ConfigurationData::getInstance()->coreServerProperties.resourcePath = respath;
	RegexUtil::replace(webpath,"[/]+","/");
	ConfigurationData::getInstance()->coreServerProperties.webPath = webpath;
    ConfigurationData::getInstance()->coreServerProperties.ip_address = ipport;
	ConfigurationData::getInstance()->coreServerProperties.sprops = srprps;
	ConfigurationData::getInstance()->coreServerProperties.sessionTimeout = sessionTimeout;
	ConfigurationData::getInstance()->coreServerProperties.sessatserv = sessatserv;
	ConfigurationData::getInstance()->coreServerProperties.isMainServerProcess = isMain;
	ConfigurationData::getInstance()->coreServerProperties.sessservdistocache = false;

    strVec cmpnames;
    try
    {
    	ConfigurationHandler::handle(webdirs, webdirs1, incpath, rtdcfpath, serverRootDirectory, respath);
    }
    catch(const XmlParseException& p)
    {
    	logger << p.getMessage() << endl;
    }
    catch(const char* msg)
	{
		logger << msg << endl;
	}

    SSLHandler::initInstance(ConfigurationData::getInstance()->securityProperties);

    logger << INTER_LIB_FILE << endl;

    bool libpresent = true;
    void *dlibtemp = dlopen(INTER_LIB_FILE, RTLD_NOW);
	//logger << endl <<dlibtemp << endl;
	if(dlibtemp==NULL)
	{
		libpresent = false;
		logger << dlerror() << endl;
		logger.info("Could not load Library");
	}
	else
		dlclose(dlibtemp);

	if(isMain)
	{
		//Generate library if dev mode = true or the library is not found in prod mode
		if(isCompileEnabled || !libpresent)
			libpresent = false;

		if(!libpresent)
		{
			string configureFilePath = rtdcfpath+"/autotools/configure";
			if (access( configureFilePath.c_str(), F_OK ) == -1 )
			{
				string compres = rtdcfpath+"/autotools/autogen.sh "+serverRootDirectory;
				string output = ScriptHandler::execute(compres, true);
				logger << "Set up configure for intermediate libraries\n\n" << endl;
			}

			if (access( configureFilePath.c_str(), F_OK ) != -1 )
			{
				string compres = respath+"rundyn-configure.sh "+serverRootDirectory;
			#ifdef DEBUG
				compres += " --enable-debug=yes";
			#endif
				string output = ScriptHandler::execute(compres, true);
				logger << "Set up makefiles for intermediate libraries\n\n" << endl;
				logger << output << endl;

				compres = respath+"rundyn-automake.sh "+serverRootDirectory;
				output = ScriptHandler::execute(compres, true);
				logger << "Intermediate code generation task\n\n" << endl;
				logger << output << endl;
			}
		}

		void* checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
		if(checkdlib==NULL)
		{
			string compres = rtdcfpath+"/autotools/autogen-noreconf.sh "+serverRootDirectory;
			string output = ScriptHandler::execute(compres, true);
			logger << "Set up configure for intermediate libraries\n\n" << endl;

			compres = respath+"rundyn-configure.sh "+serverRootDirectory;
			#ifdef DEBUG
				compres += " --enable-debug=yes";
			#endif
			output = ScriptHandler::execute(compres, true);
			logger << "Set up makefiles for intermediate libraries\n\n" << endl;
			logger << output << endl;

			compres = respath+"rundyn-automake.sh "+serverRootDirectory;
			if(!libpresent)
			{
				string output = ScriptHandler::execute(compres, true);
				logger << "Rerunning Intermediate code generation task\n\n" << endl;
				logger << output << endl;
			}
			checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
		}

		if(checkdlib==NULL)
		{
			logger << dlerror() << endl;
			logger.info("Could not load Library");
			exit(0);
		}
		else
		{
			dlclose(checkdlib);
			logger.info("Library generated successfully");
		}

	#ifdef INC_COMP
		for (unsigned int var1 = 0;var1<ConfigurationData::getInstance()->componentNames.size();var1++)
		{
			string name = ConfigurationData::getInstance()->componentNames.at(var1);
			StringUtil::replaceFirst(name,"Component_","");
			ComponentHandler::registerComponent(name);
			AppContext::registerComponent(name);
		}
	#endif

		bool distocache = false;
	/*#ifdef INC_DSTC
		int distocachepoolsize = 20;
		try {
			if(srprps["DISTOCACHE_POOL_SIZE"]!="")
			{
				distocachepoolsize = CastUtil::lexical_cast<int>(srprps["DISTOCACHE_POOL_SIZE"]);
			}
		} catch(...) {
			logger << ("Invalid poolsize specified for distocache") << endl;
		}

		try {
			if(srprps["DISTOCACHE_PORT_NO"]!="")
			{
				CastUtil::lexical_cast<int>(srprps["DISTOCACHE_PORT_NO"]);
				DistoCacheHandler::trigger(srprps["DISTOCACHE_PORT_NO"], distocachepoolsize);
				logger << ("Session store is set to distocache store") << endl;
				distocache = true;
			}
		} catch(...) {
			logger << ("Invalid port specified for distocache") << endl;
		}

		if(!distocache) {
			logger << ("Session store is set to file store") << endl;
		}
	#endif*/


	#ifdef INC_COMP
		try {
			if(srprps["CMP_PORT"]!="")
			{
				int port = CastUtil::lexical_cast<int>(srprps["CMP_PORT"]);
				if(port>0)
				{
					ComponentHandler::trigger(srprps["CMP_PORT"]);
				}
			}
		} catch(...) {
			logger << ("Component Handler Services are disabled") << endl;
		}
	#endif

	#ifdef INC_MSGH
		try {
			if(srprps["MESS_PORT"]!="")
			{
				int port = CastUtil::lexical_cast<int>(srprps["MESS_PORT"]);
				if(port>0)
				{
					MessageHandler::trigger(srprps["MESS_PORT"],resourcePath);
				}
			}
		} catch(...) {
			logger << ("Messaging Handler Services are disabled") << endl;
		}
	#endif

	#ifdef INC_MI
		try {
			if(srprps["MI_PORT"]!="")
			{
				int port = CastUtil::lexical_cast<int>(srprps["MI_PORT"]);
				if(port>0)
				{
					MethodInvoc::trigger(srprps["MI_PORT"]);
				}
			}
		} catch(...) {
			logger << ("Method Invoker Services are disabled") << endl;
		}
	#endif

	#ifdef INC_JOBS
		JobScheduler::start();
	#endif
	}

	pid_t pid;

	if(isMain)
	{
		propMultiMap mpmap = pread.getPropertiesMultiMap(respath+"server.prop");
		if(mpmap.find("VHOST_ENTRY")!=mpmap.end() && mpmap["VHOST_ENTRY"].size()>0)
		{
			vector<string> vhosts = mpmap["VHOST_ENTRY"];
			for(int vhi=0;vhi<vhosts.size();vhi++)
			{
				vector<string> vhostprops = StringUtil::splitAndReturn<vector<string> >(vhosts.at(vhi), ";");
				if(vhostprops.size()==3)
				{
					string vhostname = StringUtil::trimCopy(vhostprops.at(0));
					string vhostport = StringUtil::trimCopy(vhostprops.at(1));
					string vhostapps = StringUtil::trimCopy(vhostprops.at(2));
					bool valid = true;
					if(vhostname=="")
					{
						valid = false;
						logger << ("No host specified for Virtual-Host") << endl;
					}
					if(vhostport=="")
					{
						valid = false;
						logger << ("No port specified for Virtual-Host") << endl;
					}
					if(vhostapps=="")
					{
						valid = false;
						logger << ("No apps specified for Virtual-Host") << endl;
					}

					if(valid)
					{
						vector<string> spns = StringUtil::splitAndReturn<vector<string> >(vhostapps, ",");
						map<string, bool> updatedcontextNames;
						map<string, string> updatedaliasNames;
						for (int spni = 0; spni < (int)spns.size(); ++spni) {
							StringUtil::trim(spns.at(spni));
							string vapnm = spns.at(spni);
							string valias = vapnm;
							if(vapnm.find(":")!=string::npos) {
								valias = vapnm.substr(vapnm.find(":")+1);
								vapnm = vapnm.substr(0, vapnm.find(":"));
							}
							if(vapnm!="" && ConfigurationData::getInstance()->servingContexts.find(vapnm)!=
									ConfigurationData::getInstance()->servingContexts.end())
							{
								updatedcontextNames[vapnm] = true;
								if(valias!=vapnm) {
									updatedaliasNames[valias] = vapnm;
								}
							}
						}
						map<string, bool>::iterator ucit;
						for(ucit=updatedcontextNames.begin();ucit!=updatedcontextNames.end();ucit++)
						{
							if(ConfigurationData::getInstance()->servingContexts.find(ucit->first)!=
									ConfigurationData::getInstance()->servingContexts.end())
							{
								ConfigurationData::getInstance()->servingContexts.erase(ucit->first);
							}
							if(ConfigurationData::getInstance()->appAliases.find(ucit->first)!=
									ConfigurationData::getInstance()->appAliases.end())
							{
								ConfigurationData::getInstance()->appAliases.erase(ucit->first);
							}
						}

					#if !defined(OS_MINGW)
						pid = fork();
						if(pid == 0)
						{
							LoggerFactory::instance->setVhostNumber(vhi+1);
							ConfigurationData::getInstance()->servingContexts = updatedcontextNames;
							ConfigurationData::getInstance()->appAliases = updatedaliasNames;

							string lnm = "CHServer(VHost-" +
									CastUtil::lexical_cast<string>(vhi+1) + ")";
							serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." +
									CastUtil::lexical_cast<string>(vhi+1);
							serve(vhostport, vhostname, thrdpsiz, serverRootDirectory, srprps, vhi+1);
						}
					#else
						string vhostcmd = "./vhost-server.sh " + serverRootDirectory + " " + vhostname + " " + vhostport
								+ " " + vhostapps + " " + CastUtil::lexical_cast<string>(vhi+1);
						string vhostcmdo = ScriptHandler::chdirExecute(vhostcmd, serverRootDirectory, true);
						logger.info("Starting new Virtual-Host at " + (vhostname + ":" + vhostport));
						logger << vhostcmdo << endl;
					#endif
					}
				}
			}
		}

		try {
			serve(port, ipaddr, thrdpsiz, serverRootDirectory, srprps, vhostNum);
		} catch (const char* e) {
			logger << e << endl;
		} catch (...) {
			logger << "Error Occurred in serve" << endl;
		}

	#ifdef INC_COMP
		ComponentHandler::stop();
	#endif

	#ifdef INC_MSGH
		MessageHandler::stop();
	#endif

	#ifdef INC_MI
		MethodInvoc::stop();
	#endif

	#ifdef INC_JOBS
		JobScheduler::stop();
	#endif
	}

	return 0;
}

HttpServiceTask* CHServer::httpServiceFactoryMethod() {
	return new ServiceTask();
}

void CHServer::serve(string port, string ipaddr, int thrdpsiz, string serverRootDirectory, propMap sprops, int vhostNumber)
{
	string ipport;

	if(ipaddr!="")
	{
		if(port=="80")
			ipport = ipaddr;
		else
			ipport = ipaddr + ":" + port;
	}
	else
	{
		if(port=="80")
			ipport = "localhost";
		else
			ipport = "localhost:" + port;
	}

	ConfigurationData::getInstance()->coreServerProperties.ip_address = ipport;

	if(ConfigurationData::getInstance()->servingContexts.size()==0)
	{
		if(vhostNumber==0)
			logger << ("No context to be served by Server (" +  ipport  + ")") << endl;
		else
			logger << ("No context to be served by Virtual-Host (" +  ipport  + ")") << endl;
		return;
	}
	else
	{
		if(vhostNumber==0)
		{
			map<string, bool>::iterator it;
			for (it=ConfigurationData::getInstance()->servingContexts.begin();it!=ConfigurationData::getInstance()->servingContexts.end();++it)
				logger << ("Server (" +  ipport  + ") serves context (" +  it->first  + ")") << endl;
		}
		else
		{
			map<string, bool>::iterator it;
			for (it=ConfigurationData::getInstance()->servingContexts.begin();it!=ConfigurationData::getInstance()->servingContexts.end();++it)
				logger << ("Virtual-Host (" +  ipport  + ") serves context (" +  it->first  + ")") << endl;
		}
	}

	int nfds;
	if(sprops["NUM_PROC"]!="")
	{
		try
		{
			preForked = CastUtil::lexical_cast<int>(sprops["NUM_PROC"]);
		}
		catch(...)
		{
			logger << "Invalid number for worker processes defined" << endl;
			preForked = 5;
		}
	}

	SOCKET sockfd, new_fd;  // listen on sock_fd, new connection on new_fd

	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

	sockfd = Server::createListener(ipaddr, CastUtil::lexical_cast<int>(port), true);

	if(sockfd==-1)
	{
		logger << "Unable to start the server on the specified ip/port..." << endl;
		return;
	}

	logger << ("Initializing WSDL files....") << endl;
	ConfigurationHandler::initializeWsdls();
	logger << ("Initializing WSDL files done....") << endl;

#ifdef INC_SDORM
	logger << ("Initializing DataSources....") << endl;
	ConfigurationHandler::initializeDataSources();
	logger << ("Initializing DataSources done....") << endl;
#endif

	logger << ("Initializing Caches....") << endl;
	ConfigurationHandler::initializeCaches();
	logger << ("Initializing Caches done....") << endl;

	vector<string> files;
	int sp[preForked][2];
	ThreadPool *pool = NULL;

	/*TODO if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
	{
		#if !defined(OS_MINGW) && !defined(OS_DARWIN)
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
		#endif
	}
	else*/
	{
		void* dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
		//logger << endl <<dlib << endl;
		if(dlib==NULL)
		{
			logger << dlerror() << endl;
			logger.info("Could not load Library");
			exit(0);
		}
		else
		{
			logger.info("Library loaded successfully");
			dlclose(dlib);
		}

		void* ddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
		//logger << endl <<dlib << endl;
		if(ddlib==NULL)
		{
			logger << dlerror() << endl;
			logger.info("Could not load dynamic Library");
			exit(0);
		}
		else
		{
			logger.info("Dynamic Library loaded successfully");
			dlclose(ddlib);
		}

		/*TODO if(!isThreadprq)
		{
			pool = new ThreadPool(thrdpsiz/2,thrdpsiz,true);
			pool->start();
		}*/
	}

#ifdef INC_DCP
	if(isCompileEnabled)
	{
		Thread pthread(&CHServer::dynamic_page_monitor, &serverRootDirectory);
		pthread.execute();
	}
#endif

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	logger << ("Initializing ffeadContext....") << endl;
	ConfigurationData::getInstance()->ffeadContext.initializeAllSingletonBeans(ConfigurationData::getInstance()->servingContexts);
	logger << ("Initializing ffeadContext done....") << endl;

	//printf("server: waiting for connections...\n");
	logger.info("Server: waiting for connections on " + ipport);

	ofstream serverCntrlFileo;
	serverCntrlFileo.open(serverCntrlFileNm.c_str());
	serverCntrlFileo << "Server Running" << endl;
	serverCntrlFileo.close();

	//Sleep for some time so as to make sure all the new child processes are set correctly
	//and all init is complete...
	sleep(5);

	Thread gsthread(&gracefullShutdown_monitor, &ipport);
	gsthread.execute();

	string cntEnc = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["CONTENT_ENCODING"]);
	try {
		techunkSiz = CastUtil::lexical_cast<int>(ConfigurationData::getInstance()->coreServerProperties.sprops["TRANSFER_ENCODING_CHUNK_SIZE"]);
	} catch (...) {
	}
	try {
		connKeepAlive = CastUtil::lexical_cast<int>(ConfigurationData::getInstance()->coreServerProperties.sprops["KEEP_ALIVE_SECONDS"]);
	} catch (...) {
	}
	try {
		maxReqHdrCnt = CastUtil::lexical_cast<int>(ConfigurationData::getInstance()->coreServerProperties.sprops["MAX_REQUEST_HEADERS_COUNT"]);
	} catch (...) {
	}
	try {
		maxEntitySize = CastUtil::lexical_cast<int>(ConfigurationData::getInstance()->coreServerProperties.sprops["MAX_REQUEST_ENTITY_SIZE"]);
	} catch (...) {
	}

	ServiceHandler* handler = new HttpServiceHandler(cntEnc, &CHServer::httpServiceFactoryMethod, thrdpsiz);
	handler->start();

	RequestReaderHandler reader(handler, sockfd);
	reader.registerSocketInterfaceFactory(&CHServer::createSocketInterface);
	reader.start();

	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(1);
	}
	reader.stop();

	sleep(2);


	/*SelEpolKqEvPrt selEpolKqEvPrtHandler;
	selEpolKqEvPrtHandler.initialize(sockfd);
	int childNo = 0;
	//if(fork()==0)
	//{
		//start  of hotdeployment process

	//}

	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		if(childNo>=preForked)
			childNo = 0;
		errno = 0;
		nfds = selEpolKqEvPrtHandler.getEvents();
		if (nfds == -1)
		{
			//perror("poll_wait main process");
			if(errno==EBADF)
			{
				logger << "Inavlid fd" <<endl;
			}
			else if(errno==EFAULT)
			{
				logger << "The memory area pointed to by events is not accessible" <<endl;
			}
			else if(errno==EINTR)
			{
				//logger << "call was interrupted by a signal handler before any of the requested events occurred" <<endl;
			}
			else if(errno==EINVAL)
			{
				//logger << "not a poll file descriptor, or maxevents is less than or equal to zero" << endl;
			}
			else
			{
				logger << "not an epoll file descriptor" <<endl;
			}
			continue;
		}
		processgendone = false;
		if(processforcekilled)
		{
			if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
			{
				#if !defined(OS_MINGW) && !defined(OS_DARWIN)
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
				#endif
			}
			else
			{
				delete pool;
				pool = new ThreadPool;
				pool->init(thrdpsiz,thrdpsiz+30,true);
			}
			processforcekilled = false;
			processgendone = true;
		}
		for(int n=0;n<nfds;n++)
		{
			if(childNo>=preForked)
				childNo = 0;
			SOCKET descriptor = selEpolKqEvPrtHandler.getDescriptor(n);
			if (descriptor == sockfd)
			{
				new_fd = -1;
				sin_size = sizeof their_addr;
				new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
				if (new_fd == -1)
				{
					perror("accept");
					continue;
				}
				else
				{
					selEpolKqEvPrtHandler.reRegisterServerSock();
					selEpolKqEvPrtHandler.registerForEvent(new_fd);
				}
			}
			else if (descriptor != -1)
			{
				logger << ("got new connection " + CastUtil::lexical_cast<string>(descriptor)) << endl;
				selEpolKqEvPrtHandler.unRegisterForEvent(descriptor);
				if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
				{
					errno = 0;
					ifstream cntrlfile;
					cntrlfile.open(files.at(childNo).c_str());
					if(cntrlfile.is_open())
					{
						#if !defined(OS_MINGW) && !defined(OS_DARWIN)
						send_connection(sp[childNo][0], descriptor);
						string cno = CastUtil::lexical_cast<string>(childNo);
						childNo++;
						#endif
					}
					else
					{
						logger.info("Child Process got killed exiting...\n");
						break;
					}
					cntrlfile.close();
					if(errno!=0) {
						logger << ("Send socket failed with errno = " + CastUtil::lexical_cast<string>(errno)) << endl;
					}
				}
				else
				{
					#ifdef OS_MINGW
						u_long bMode = 0;
						ioctlsocket(descriptor, FIONBIO, &bMode);
					#else
						fcntl(descriptor, F_SETFL, O_SYNC);
					#endif
					try
					{
						if(isThreadprq)
						{
							ServiceTask *task = new ServiceTask(descriptor,serverRootDirectory);
							Thread pthread(&service, task);
							pthread.execute();
						}
						else
						{
							ServiceTask *task = new ServiceTask(descriptor,serverRootDirectory);
							task->setCleanUp(true);
							pool->submit(task);
						}
					}
					catch(const char* err)
					{
						logger << "Exception occurred while processing ServiceTask request - " << err << endl;
					}
					catch(...)
					{
						logger << "Standard exception occurred while processing ServiceTask request " << endl;
					}
				}
			}
		}
	}*/

	close(sockfd);

#ifdef INC_SDORM
	ConfigurationHandler::destroyDataSources();
#endif

	SSLHandler::clear();

	ConfigurationHandler::destroyCaches();

	ConfigurationData::clearInstance();

	logger << "Destructed SSLHandler" << endl;

	if(pool!=NULL) {
		delete pool;
		logger << "Destructed Thread pool" << endl;
	}

	#if !defined(OS_MINGW) && !defined(OS_DARWIN)
	if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
	{
		map<int,pid_t>::iterator it;
		for(it=pds.begin();it!=pds.end();++it)
		{
			kill(it->second, SIGKILL);
			logger << ("Killed child process ") << it->second << endl;
		}
		logger << "Destructed child processes" << endl;
	}
	#endif

	LoggerFactory::clear();

	#ifdef OS_MINGW
		WSACleanup();
	#endif
}

int CHServer::techunkSiz = 0;
int CHServer::connKeepAlive = 10;
int CHServer::maxReqHdrCnt = 100, CHServer::maxEntitySize = 2147483647;

SocketInterface* CHServer::createSocketInterface(SocketUtil* sockUtil) {
	SocketInterface* sockIntf = NULL;
	if(SSLHandler::getInstance()->getIsSSL() && sockUtil->getAlpnProto().find("h2")==0)
	{
		sockIntf = new Http2Handler(true, sockUtil, ConfigurationData::getInstance()->coreServerProperties.webPath);
	}
	else
	{
		sockIntf = new Http11Handler(sockUtil, ConfigurationData::getInstance()->coreServerProperties.webPath,
				techunkSiz, connKeepAlive*1000, maxReqHdrCnt, maxEntitySize);
	}

	return sockIntf;
}

Logger& CHServer::getLogger()
{
	return logger;
}
