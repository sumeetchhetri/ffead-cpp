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


CHServer::CHServer()
{}

CHServer::~CHServer() {
	// TODO Auto-generated destructor stub
}
//SharedData* SharedData::shared_instance = NULL;
string servd, serverCntrlFileNm;
static bool isSSLEnabled = false,isThreadprq = false,processforcekilled = false,processgendone = false,sessatserv = false,isCompileEnabled = false;
static long sessionTimeout;
static int thrdpsiz/*,shmid*/;
map<int,pid_t> pds;
static pid_t parid;
typedef map<string,string> sessionMap;
static Mutex m_mutex,p_mutex;

Logger logger;

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
		cout << "Exiting, Got errono " << sig << endl;
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
	logger << "Segmentation fault occurred for process" << getpid() << "\n" << endl;
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
	logger << "Child process got killed " << getpid() << "\n"  << endl;
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
	logger << "Abort signal occurred for process" << getpid() << "\n" << endl;
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
	logger << "Termination signal occurred for process" << getpid() << "\n" << endl;
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
	logger << "Kill signal occurred for process" << getpid() << "\n" << endl;
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
	logger << "Interrupt signal occurred for process" << getpid() << "\n" << endl;
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
	logger << "Floating point Exception occurred for process" << getpid() << "\n" << endl;
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
	logger << "Broken pipe ignore it" << getpid() << "\n" << endl;
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
	logger << "Floating point Exception occurred for process" << getpid() << "\n" << endl;
	abort();
}

void* service(void* arg)
{
	logger << "service method " << endl;
	ServiceTask *task = (ServiceTask*)arg;
	task->run();
	delete task;
	return NULL;
}

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
			pool.init(thrdpsiz,30,true);
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
				fcntl(fd, F_SETFL,O_SYNC);

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
void* dynamic_page_monitor(void* arg)
{
	string serverRootDirectory = *(string*)arg;
	struct stat statbuf;
	map<string, string> dcpsss = ConfigurationData::getInstance()->dcpsss;
	map<string, string> tpes = ConfigurationData::getInstance()->tpes;
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
				string ret = DCPGenerator::generateDCPAll(dcpsss);
				AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
				logger << "done generating dcp code" <<endl;
				logger << "started generating template code" <<endl;
				ret = TemplateGenerator::generateTempCdAll(tpes);
				AfcUtil::writeTofile(rtdcfpath+"TemplateInterface.cpp",ret,true);
				logger << "done generating template code" <<endl;

				string compres;
#if BUILT_WITH_CONFGURE == 1 && !defined(OS_CYGWIN)
				compres = respath+"rundyn-automake_dinter.sh";
#else
				compres = respath+"rundyn_dinter.sh";
#endif
				int i=system(compres.c_str());
				if(!i)
				{
					logger << "regenerating intermediate code-----Done" << endl;
					logger.info("Done generating intermediate code");
				}
				m_mutex.lock();
				if(IS_FILE_DESC_PASSING_AVAIL)
				{
					map<int,pid_t>::iterator it;
					for(it=pds.begin();it!=pds.end();it++)
					{
						kill(it->second,9);
					}
				}
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
	parid = getpid();

	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	act.sa_handler = siginthandler;
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	//signal(SIGSEGV,signalSIGSEGV);
	//signal(SIGFPE,signalSIGFPE);
	(void) sigignore(SIGPIPE);
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

    //int yes=1,rv;
    int nfds,preForked=5;
    string serverRootDirectory = argv[1];

	serverRootDirectory += "/";//serverRootDirectory = "/home/sumeet/server/";
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	string logpath = serverRootDirectory + "logs/";
	string resourcePath = respath;

	servd = serverRootDirectory;
	string logp = respath+"/logging.xml";
	LoggerFactory::init(logp, serverRootDirectory);

	serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";

	logger = LoggerFactory::getLogger("CHServer");

    PropFileReader pread;
    propMap srprps = pread.getProperties(respath+"server.prop");
    if(srprps["NUM_PROC"]!="")
    {
    	try
		{
    		preForked = CastUtil::lexical_cast<int>(srprps["NUM_PROC"]);
		}
		catch(...)
		{
			logger << "\nInvalid number for worker processes defined" << endl;
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
			thrdpsiz = 30;
		else
		{
			try
			{
				thrdpsiz = CastUtil::lexical_cast<int>(thrdpreq);
			}
			catch(...)
			{
				logger << "\nInvalid thread pool size defined" << endl;
				thrdpsiz = 30;
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
   	if(srprps["SESS_STATE"]=="server")
   		sessatserv = true;
   	if(srprps["SESS_TIME_OUT"]!="")
   	{
   		try {
   			sessionTimeout = CastUtil::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch (...) {
			sessionTimeout = 3600;
			logger << "\nInvalid session timeout value defined, defaulting to 1hour/3600sec" << endl;
		}
   	}

   	string PORT = srprps["PORT_NO"];
	string IP_ADDRES = srprps["IP_ADDR"];
	string IP_ADDRESS;

	if(IP_ADDRES!="")
		IP_ADDRESS = IP_ADDRES + ":" + PORT;
	else
		IP_ADDRESS = "localhost:" + PORT;

	ConfigurationData::getInstance();
	SSLHandler::setIsSSL(isSSLEnabled);

	ConfigurationData::getInstance()->ip_address = IP_ADDRESS;
	ConfigurationData::getInstance()->sprops = srprps;
	ConfigurationData::getInstance()->sessionTimeout = sessionTimeout;
	ConfigurationData::getInstance()->sessatserv = sessatserv;

	sockfd = Server::createListener(IP_ADDRES, PORT, false);

    strVec webdirs,webdirs1,pubfiles;
    ConfigurationHandler::listi(webpath,"/",true,webdirs,false);
    ConfigurationHandler::listi(webpath,"/",false,webdirs1,false);
    ConfigurationHandler::listi(pubpath,".js",false,pubfiles,false);

    for(unsigned int var=0;var<pubfiles.size();var++)
	{
		ConfigurationData::getInstance()->pubMap[pubfiles.at(var)] = "true";
	}

    ConfigurationData::getInstance()->props = pread.getProperties(respath+"mime-types.prop");
    ConfigurationData::getInstance()->lprops = pread.getProperties(respath+"locale.prop");

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

    SSLHandler::initInstance();

    cout << INTER_LIB_FILE << endl;

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

	//Generate library if dev mode = true or the library is not found in prod mode
    if(isCompileEnabled || !libpresent)
    	libpresent = false;

    string compres;
#if BUILT_WITH_CONFGURE == 1 && !defined(OS_CYGWIN)
    compres = respath+"rundyn-automake.sh";
#else
	compres = respath+"rundyn.sh";
#endif
	if(!libpresent)
	{
		string output = ScriptHandler::execute(compres, true);
		logger << "Intermediate code generation task\n\n" << endl;
		logger << output << endl;
	}
	void* checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
#if BUILT_WITH_CONFGURE == 1 && !defined(OS_CYGWIN)
	if(checkdlib==NULL)
	{
		compres = respath+"rundyn-configure.sh reconf";
		string output = ScriptHandler::execute(compres, true);
		logger << output << endl;

		compres = respath+"rundyn-automake.sh";
		if(!libpresent)
		{
			string output = ScriptHandler::execute(compres, true);
			logger << "Rerunning Intermediate code generation task\n\n" << endl;
			logger << output << endl;
		}
		checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	}
#endif
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
	for (unsigned int var1 = 0;var1<ConfigurationData::getInstance()->cmpnames.size();var1++)
	{
		string name = ConfigurationData::getInstance()->cmpnames.at(var1);
		StringUtil::replaceFirst(name,"Component_","");
		ComponentHandler::registerComponent(name);
		AppContext::registerComponent(name);
	}
#endif

	bool distocache = false;
#ifdef INC_DSTC
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
#endif

	ConfigurationData::getInstance()->sessservdistocache = distocache;

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

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	ConfigurationData::getInstance()->ffeadContext.initializeAllSingletonBeans();

	//printf("server: waiting for connections...\n");
	logger.info("Server: waiting for connections on " + ConfigurationData::getInstance()->ip_address);

	ofstream serverCntrlFileo;
	serverCntrlFileo.open(serverCntrlFileNm.c_str());
	serverCntrlFileo << "Server Running" << endl;
	serverCntrlFileo.close();

	vector<string> files;
	int sp[preForked][2];
	ThreadPool *pool = NULL;

	if(IS_FILE_DESC_PASSING_AVAIL)
	{
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
	}
	else
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

		if(!isThreadprq)
		{
			pool = new ThreadPool(thrdpsiz,thrdpsiz+30,true);
			pool->start();
		}
	}

#ifdef INC_DCP
	if(isCompileEnabled)
	{
		Thread pthread(&dynamic_page_monitor, &serverRootDirectory);
		pthread.execute();
	}
#endif

	//Sleep for some time so as to make sure all the new child processes are set correctly
	//and all init is complete...
	sleep(5);

	Thread gsthread(&gracefullShutdown_monitor, &IP_ADDRESS);
	gsthread.execute();

	SelEpolKqEvPrt selEpolKqEvPrtHandler;
	selEpolKqEvPrtHandler.initialize(sockfd);
	int childNo = 0;
	/*if(fork()==0)
	{
		//start  of hotdeployment process

	}*/

	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		if(childNo>=preForked)
			childNo = 0;
		errno = 0;
		nfds = selEpolKqEvPrtHandler.getEvents();
		if (nfds == -1)
		{
			perror("poll_wait main process");
			if(errno==EBADF)
				logger << "\nInavlid fd" <<endl;
			else if(errno==EFAULT)
				logger << "\nThe memory area pointed to by events is not accessible" <<endl;
			else if(errno==EINTR)
				logger << "\ncall was interrupted by a signal handler before any of the requested events occurred" <<endl;
			else if(errno==EINVAL)
				logger << "not a poll file descriptor, or maxevents is less than or equal to zero" << endl;
			else
				logger << "\nnot an epoll file descriptor" <<endl;
		}
		processgendone = false;
		if(processforcekilled)
		{
			if(IS_FILE_DESC_PASSING_AVAIL)
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
			int descriptor = selEpolKqEvPrtHandler.getDescriptor(n);
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
				if(IS_FILE_DESC_PASSING_AVAIL)
				{
					errno = 0;
					ifstream cntrlfile;
					cntrlfile.open(files.at(childNo).c_str());
					if(cntrlfile.is_open())
					{
						send_connection(sp[childNo][0], descriptor);
						string cno = CastUtil::lexical_cast<string>(childNo);
						childNo++;
					}
					else
					{
						/*int tcn = childNo;
						for(int o=0;o<preForked;o++)
						{
							ifstream cntrlfileT;
							cntrlfileT.open(files.at(o).c_str());
							if(cntrlfileT.is_open())
							{
								send_connection(sp[childNo][0], descriptor);
								string cno = CastUtil::lexical_cast<string>(o);
								childNo = o+1;
								cntrlfileT.close();
								break;
							}
						}
						close(sp[tcn][0]);
						close(sp[tcn][1]);
						logger << "Process got killed" << endl;
						pid_t pid = createChildProcess(serverRootDirectory,sp[tcn],sockfd);
						pds[tcn] = pid;
						stringstream ss;
						string filename;
						ss << serverRootDirectory;
						ss << pid;
						ss >> filename;
						filename.append(".cntrl");
						files[tcn] = filename;
						logger << "created a new Process" << endl;*/
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
					fcntl(descriptor, F_SETFL, O_SYNC);
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
	}

	close(sockfd);

#ifdef INC_COMP
	ComponentHandler::stop();
#endif

#ifdef INC_MSGH
	MessageHandler::stop();
#endif

#ifdef INC_MI
	MethodInvoc::stop();
#endif

#ifdef INC_CIB
	ConfigurationHandler::destroyCibernate();
#endif

#ifdef INC_JOBS
	JobScheduler::stop();
#endif
	SSLHandler::clear();

	ConfigurationData::clearInstance();

	logger << "Destructed SSLHandler" << endl;

	if(pool!=NULL) {
		delete pool;
		logger << "Destructed Thread pool" << endl;
	}

	if(IS_FILE_DESC_PASSING_AVAIL)
	{
		map<int,pid_t>::iterator it;
		for(it=pds.begin();it!=pds.end();++it)
		{
			kill(it->second, SIGKILL);
			logger << ("Killed child process ") << it->second << endl;
		}
		logger << "Destructed child processes" << endl;
	}

	LoggerFactory::clear();

	return 0;
}

