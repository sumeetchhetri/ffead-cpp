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

static std::string servd, serverCntrlFileNm;
static bool isSSLEnabled = false, isThreadprq = false, processforcekilled = false,
		processgendone = false, isCompileEnabled = false;
static int preForked = 5;
static std::map<int,pid_t> pds;
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
		CHServer::getLogger() << "Exiting, Got errono " << sig << std::endl;
		exit(0);
	}
}

void signalSIGSEGV(int sig)
{
	signal(SIGSEGV,signalSIGSEGV);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Segmentation fault occurred for process" << getpid() << "\n" << std::endl;
	abort();
}
void signalSIGCHLD(int sig)
{
	signal(SIGCHLD,signalSIGCHLD);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Child process got killed " << getpid() << "\n"  << std::endl;
	//abort();
}
void signalSIGABRT(int sig)
{
	signal(SIGABRT,signalSIGABRT);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Abort signal occurred for process" << getpid() << "\n" << std::endl;
	abort();
}
void signalSIGTERM(int sig)
{
	signal(SIGTERM,signalSIGTERM);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Termination signal occurred for process" << getpid() << "\n" << std::endl;
	abort();
}

void signalSIGKILL(int sig)
{
	signal(SIGKILL,signalSIGKILL);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Kill signal occurred for process" << getpid() << "\n" << std::endl;
	abort();
}

void signalSIGINT(int sig)
{
	signal(SIGINT,signalSIGINT);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Interrupt signal occurred for process" << getpid() << "\n" << std::endl;
	//abort();
}

void signalSIGFPE(int sig)
{
	signal(SIGFPE,signalSIGFPE);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Floating point Exception occurred for process" << getpid() << "\n" << std::endl;
	abort();
}

void signalSIGPIPE(int sig)
{
	signal(SIGPIPE,signalSIGPIPE);
	/*string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());*/
	handler(sig);
	CHServer::getLogger() << "Broken pipe ignore it" << getpid() << "\n" << std::endl;
	//abort();
}

void signalSIGILL(int sig)
{
	signal(SIGILL,signalSIGILL);
	std::string filename;
	std::stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	handler(sig);
	CHServer::getLogger() << "Floating point Exception occurred for process" << getpid() << "\n" << std::endl;
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
pid_t createChildProcess(std::string serverRootDirectory,int sp[],int sockfd)
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
		std::string lgname = "CHServer-";
		lgname.append(pidStr);
		Logger plogger = LoggerFactory::getLogger(lgname);

		servd = serverRootDirectory;
		std::string filename;
		std::stringstream ss;
		ss << serverRootDirectory;
		ss << getpid();
		ss >> filename;
		filename.append(".cntrl");
		plogger << ("generated file " + filename) << std::endl;
		std::ofstream cntrlfile;
		cntrlfile.open(filename.c_str());
		cntrlfile << "Process Running" << std::endl;
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
				plogger << "\n----------poller child process----" << std::endl;
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
					plogger << "Socket conn closed before being serviced" << std::endl;
					continue;
				}

				try
				{
					if(isThreadprq)
					{
						ServiceTask *task = new ServiceTask(fd,serverRootDirectory);
						Thread* pthread = new Thread(&service, task);
						pthread->execute();
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
					plogger << "Exception occurred while processing ServiceTask request - " << err << std::endl;
				}
				catch(...)
				{
					plogger << "Standard exception occurred while processing ServiceTask request " << std::endl;
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
		std::map<std::string,bool> stat;
		while(1)
		{
			char buf[10];
			memset(pidStr, 0, 10);
			if(read(sp[1], buf, sizeof buf) < 0)
			{
				std::string temp = buf;
				strVec tempv;
				StringUtil::split(tempv, temp, ":");
				if(tempv.size()==2)
				{
					if(tempv.at(0)=="R")
					{
						std::string h = "0";
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
	std::string* ipaddr = (std::string*)args;
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(1);
	}
	std::string ip = ipaddr->substr(0, ipaddr->find(":"));
	std::string port = ipaddr->substr(ipaddr->find(":")+1);

	if(isSSLEnabled) {
		SSLClient sc;
		sc.connectionUnresolv(ip,CastUtil::lexical_cast<int>(port));
		sc.closeConnection();
	} else {
		Client sc;
		sc.connectionUnresolv(ip,CastUtil::lexical_cast<int>(port));
		sc.closeConnection();
	}

	return NULL;
}

#ifdef INC_DCP
void* CHServer::dynamic_page_monitor(void* arg)
{
	std::string serverRootDirectory = *(std::string*)arg;
	struct stat statbuf;
	std::map<std::string, std::string> dcpsss = ConfigurationData::getInstance()->dynamicCppPagesMap;
	std::map<std::string, std::string> tpes = ConfigurationData::getInstance()->templateFilesMap;
	std::map<std::string, std::string> dcspstpes = dcpsss;
	dcspstpes.insert(tpes.begin(), tpes.end());
	std::map<std::string,long> statsinf;
	std::map<std::string, std::string>::iterator it;
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
				std::string rtdcfpath = serverRootDirectory + "rtdcf/";
				std::string respath = serverRootDirectory + "resources/";

				logger << "started generating dcp code" <<std::endl;
				std::string ret = DCPGenerator::generateDCPAll();
				AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
				logger << "done generating dcp code" <<std::endl;
				logger << "started generating template code" <<std::endl;
				ret = TemplateGenerator::generateTempCdAll(servd);
				AfcUtil::writeTofile(rtdcfpath+"TemplateInterface.cpp",ret,true);
				logger << "done generating template code" <<std::endl;

				std::string compres;
//#if BUILT_WITH_CONFGURE == 1
				compres = respath+"rundyn-automake_dinter.sh "+serverRootDirectory;
//#else
				//compres = respath+"rundyn_dinter.sh "+serverRootDirectory;
//#endif
				std::string output = ScriptHandler::execute(compres, true);
				//int i=system(compres.c_str()); 
				//if(!i)
				{
					logger << output << std::endl;
					logger << "regenerating intermediate code-----Done" << std::endl;
					logger.info("Done generating intermediate code");
				}
				m_mutex.lock();
				#if !defined(OS_MINGW) && !defined(OS_DARWIN)
				if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
				{
					std::map<int,pid_t>::iterator it;
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
		//cout << "No Server root directory specified, quitting..." << std::endl;
		return 0;
	}

	std::string serverRootDirectory = argv[1];
	serverRootDirectory += "/";
	if(serverRootDirectory.find("//")==0)
	{
		RegexUtil::replace(serverRootDirectory,"[/]+","/");
	}

	std::string port = "";
	std::string ipaddr = "";
	std::string servingAppNames = "";
	std::vector<std::string> servedAppNames;
	bool isMain = true;

	PropFileReader pread;
	std::string respath = serverRootDirectory + "resources/";
	propMap srprps = pread.getProperties(respath+"server.prop");

	std::ofstream local("/dev/null");
	if(StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])!="true") {
		std::streambuf* cout_buff = std::cout.rdbuf();
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
		servedAppNames = StringUtil::splitAndReturn<std::vector<std::string> >(servingAppNames, ",");
	}
	int vhostNum = 0;
	if(argc > 5)
	{
		vhostNum = CastUtil::lexical_cast<int>(argv[5]);
	}

	try {
		return CHServer::entryPoint(vhostNum, isMain, serverRootDirectory, port, ipaddr, servedAppNames);
	} catch (const char* e) {
		std::cout << e << std::endl;
	} catch (const XmlParseException& e) {
		std::cout << e.getMessage() << std::endl;
	} catch (...) {
		std::cout << "Error Occurred in serve" << std::endl;
	}
	return 0;
}


int CHServer::entryPoint(int vhostNum, bool isMain, std::string serverRootDirectory, std::string port, std::string ipaddr, std::vector<std::string> servedAppNames)
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

	std::string incpath = serverRootDirectory + "include/";
	std::string rtdcfpath = serverRootDirectory + "rtdcf/";
	std::string pubpath = serverRootDirectory + "public/";
	std::string respath = serverRootDirectory + "resources/";
	std::string webpath = serverRootDirectory + "web/";
	std::string logpath = serverRootDirectory + "logs/";
	std::string resourcePath = respath;

	PropFileReader pread;
	propMap srprps = pread.getProperties(respath+"server.prop");

	servd = serverRootDirectory;
	std::string logp = respath+"/logging.xml";
	LoggerFactory::init(logp, serverRootDirectory, "", StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true");

	std::string name;
	if(isMain)
	{
		serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
		name = "CHServer(Main)";
		LoggerFactory::instance->setVhostNumber(0);
	}
	else
	{
		name = "CHServer(VHost-" + CastUtil::lexical_cast<std::string>(vhostNum) + ")";
		serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." + CastUtil::lexical_cast<std::string>(vhostNum);
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
			logger << "Invalid number for worker processes defined" << std::endl;
			preForked = 5;
		}
    }
    std::string sslEnabled = srprps["SSL_ENAB"];
   	if(sslEnabled=="true" || sslEnabled=="TRUE")
   		isSSLEnabled = true;

   	int thrdpsiz = 0, wthrdpsiz = 2;
   	std::string thrdpreq = srprps["THRD_PREQ"];
   	if(thrdpreq=="true" || thrdpreq=="TRUE")
   	{
   		isThreadprq = true;
   	}
   	else
	{
		thrdpreq = srprps["THRD_PSIZ"];
		if(thrdpreq=="")
			thrdpsiz = CommonUtils::getProcessorCount();
		else
		{
			try
			{
				thrdpsiz = CastUtil::lexical_cast<int>(thrdpreq);
			}
			catch(...)
			{
				logger << "Invalid service thread pool size defined" << std::endl;
				thrdpsiz = CommonUtils::getProcessorCount();
			}
		}
	}
   	thrdpreq = srprps["W_THRD_PSIZ"];
	if(thrdpreq!="")
	{
		try
		{
			wthrdpsiz = CastUtil::lexical_cast<int>(thrdpreq);
		}
		catch(...)
		{
			logger << "Invalid writer thread pool size defined" << std::endl;
			wthrdpsiz = 2;
		}
	}
   	std::string compileEnabled = srprps["DEV_MODE"];
	if(compileEnabled=="true" || compileEnabled=="TRUE")
		isCompileEnabled = true;

	/*if(srprps["SCRIPT_ERRS"]=="true" || srprps["SCRIPT_ERRS"]=="TRUE")
	{
		SCRIPT_EXEC_SHOW_ERRS = true;
	}*/
	bool sessatserv = false;
   	if(srprps["SESS_STATE"]=="server")
   		sessatserv = true;
   	long sessionTimeout = 3600;
   	if(srprps["SESS_TIME_OUT"]!="")
   	{
   		try {
   			sessionTimeout = CastUtil::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch (...) {
			logger << "Invalid session timeout value defined, defaulting to 1hour/3600sec" << std::endl;
		}
   	}

   	if(port=="")
   	{
   		port = srprps["PORT_NO"];
   	}

	std::string ipport;

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
    	logger << p.getMessage() << std::endl;
    }
    catch(const char* msg)
	{
		logger << msg << std::endl;
	}

    SSLHandler::initInstance(ConfigurationData::getInstance()->securityProperties);

    logger << INTER_LIB_FILE << std::endl;

    bool libpresent = true;
    void *dlibtemp = dlopen(INTER_LIB_FILE, RTLD_NOW);
	//logger << endl <<dlibtemp << std::endl;
	if(dlibtemp==NULL)
	{
		libpresent = false;
		logger << dlerror() << std::endl;
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
			std::string configureFilePath = rtdcfpath+"/autotools/configure";
			if (access( configureFilePath.c_str(), F_OK ) == -1 )
			{
				std::string compres = rtdcfpath+"/autotools/autogen.sh "+serverRootDirectory;
				std::string output = ScriptHandler::execute(compres, true);
				logger << "Set up configure for intermediate libraries\n\n" << std::endl;
			}

			if (access( configureFilePath.c_str(), F_OK ) != -1 )
			{
				std::string compres = respath+"rundyn-configure.sh "+serverRootDirectory;
			#ifdef DEBUG
				compres += " --enable-debug=yes";
			#endif
				std::string output = ScriptHandler::execute(compres, true);
				logger << "Set up makefiles for intermediate libraries\n\n" << std::endl;
				logger << output << std::endl;

				compres = respath+"rundyn-automake.sh "+serverRootDirectory;
				output = ScriptHandler::execute(compres, true);
				logger << "Intermediate code generation task\n\n" << std::endl;
				logger << output << std::endl;
			}
		}

		void* checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
		void* checkddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
		if(checkdlib==NULL || checkddlib==NULL)
		{
			std::string compres = rtdcfpath+"/autotools/autogen-noreconf.sh "+serverRootDirectory;
			std::string output = ScriptHandler::execute(compres, true);
			logger << "Set up configure for intermediate libraries\n\n" << std::endl;

			compres = respath+"rundyn-configure.sh "+serverRootDirectory;
			#ifdef DEBUG
				compres += " --enable-debug=yes";
			#endif
			output = ScriptHandler::execute(compres, true);
			logger << "Set up makefiles for intermediate libraries\n\n" << std::endl;
			logger << output << std::endl;

			compres = respath+"rundyn-automake.sh "+serverRootDirectory;
			if(!libpresent)
			{
				std::string output = ScriptHandler::execute(compres, true);
				logger << "Rerunning Intermediate code generation task\n\n" << std::endl;
				logger << output << std::endl;
			}
			checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
			checkddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
		}

		if(checkdlib==NULL || checkddlib==NULL)
		{
			logger << dlerror() << std::endl;
			logger.info("Could not load Library");
			exit(0);
		}
		else
		{
			dlclose(checkdlib);
			dlclose(checkddlib);
			logger.info("Library generated successfully");
		}

	#ifdef INC_COMP
		for (unsigned int var1 = 0;var1<ConfigurationData::getInstance()->componentNames.size();var1++)
		{
			std::string name = ConfigurationData::getInstance()->componentNames.at(var1);
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
			logger << ("Invalid poolsize specified for distocache") << std::endl;
		}

		try {
			if(srprps["DISTOCACHE_PORT_NO"]!="")
			{
				CastUtil::lexical_cast<int>(srprps["DISTOCACHE_PORT_NO"]);
				DistoCacheHandler::trigger(srprps["DISTOCACHE_PORT_NO"], distocachepoolsize);
				logger << ("Session store is set to distocache store") << std::endl;
				distocache = true;
			}
		} catch(...) {
			logger << ("Invalid port specified for distocache") << std::endl;
		}

		if(!distocache) {
			logger << ("Session store is set to file store") << std::endl;
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
			logger << ("Component Handler Services are disabled") << std::endl;
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
			logger << ("Messaging Handler Services are disabled") << std::endl;
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
			logger << ("Method Invoker Services are disabled") << std::endl;
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
			std::vector<std::string> vhosts = mpmap["VHOST_ENTRY"];
			for(int vhi=0;vhi<vhosts.size();vhi++)
			{
				std::vector<std::string> vhostprops = StringUtil::splitAndReturn<std::vector<std::string> >(vhosts.at(vhi), ";");
				if(vhostprops.size()==3)
				{
					std::string vhostname = StringUtil::trimCopy(vhostprops.at(0));
					std::string vhostport = StringUtil::trimCopy(vhostprops.at(1));
					std::string vhostapps = StringUtil::trimCopy(vhostprops.at(2));
					bool valid = true;
					if(vhostname=="")
					{
						valid = false;
						logger << ("No host specified for Virtual-Host") << std::endl;
					}
					if(vhostport=="")
					{
						valid = false;
						logger << ("No port specified for Virtual-Host") << std::endl;
					}
					if(vhostapps=="")
					{
						valid = false;
						logger << ("No apps specified for Virtual-Host") << std::endl;
					}

					if(valid)
					{
						std::vector<std::string> spns = StringUtil::splitAndReturn<std::vector<std::string> >(vhostapps, ",");
						std::map<std::string, bool> updatedcontextNames;
						std::map<std::string, std::string> updatedaliasNames;
						for (int spni = 0; spni < (int)spns.size(); ++spni) {
							StringUtil::trim(spns.at(spni));
							std::string vapnm = spns.at(spni);
							std::string valias = vapnm;
							if(vapnm.find(":")!=std::string::npos) {
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
						std::map<std::string, bool>::iterator ucit;
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

							std::string lnm = "CHServer(VHost-" +
									CastUtil::lexical_cast<std::string>(vhi+1) + ")";
							serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." +
									CastUtil::lexical_cast<std::string>(vhi+1);
							serve(vhostport, vhostname, thrdpsiz, wthrdpsiz, serverRootDirectory, srprps, vhi+1);
						}
					#else
						std::string vhostcmd = "./vhost-server.sh " + serverRootDirectory + " " + vhostname + " " + vhostport
								+ " " + vhostapps + " " + CastUtil::lexical_cast<std::string>(vhi+1);
						std::string vhostcmdo = ScriptHandler::chdirExecute(vhostcmd, serverRootDirectory, true);
						logger.info("Starting new Virtual-Host at " + (vhostname + ":" + vhostport));
						logger << vhostcmdo << std::endl;
					#endif
					}
				}
			}
		}

		try {
			serve(port, ipaddr, thrdpsiz, wthrdpsiz, serverRootDirectory, srprps, vhostNum);
		} catch (const char* e) {
			logger << e << std::endl;
		} catch (...) {
			logger << "Error Occurred in serve" << std::endl;
		}
	}

	return 0;
}

void CHServer::serve(std::string port, std::string ipaddr, int thrdpsiz, int wthrdpsiz, std::string serverRootDirectory, propMap sprops, int vhostNumber)
{
	std::string ipport;

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
			logger << ("No context to be served by Server (" +  ipport  + ")") << std::endl;
		else
			logger << ("No context to be served by Virtual-Host (" +  ipport  + ")") << std::endl;
		return;
	}
	else
	{
		if(vhostNumber==0)
		{
			std::map<std::string, bool>::iterator it;
			for (it=ConfigurationData::getInstance()->servingContexts.begin();it!=ConfigurationData::getInstance()->servingContexts.end();++it)
				logger << ("Server (" +  ipport  + ") serves context (" +  it->first  + ")") << std::endl;
		}
		else
		{
			std::map<std::string, bool>::iterator it;
			for (it=ConfigurationData::getInstance()->servingContexts.begin();it!=ConfigurationData::getInstance()->servingContexts.end();++it)
				logger << ("Virtual-Host (" +  ipport  + ") serves context (" +  it->first  + ")") << std::endl;
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
			logger << "Invalid number for worker processes defined" << std::endl;
			preForked = 5;
		}
	}

	SOCKET sockfd, new_fd;  // listen on sock_fd, new connection on new_fd

	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

	sockfd = Server::createListener(ipaddr, CastUtil::lexical_cast<int>(port), true);

	if(sockfd==-1)
	{
		logger << "Unable to start the server on the specified ip/port..." << std::endl;
		return;
	}

	logger << ("Initializing WSDL files....") << std::endl;
	ConfigurationHandler::initializeWsdls();
	logger << ("Initializing WSDL files done....") << std::endl;

#ifdef INC_SDORM
	logger << ("Initializing DataSources....") << std::endl;
	ConfigurationHandler::initializeDataSources();
	logger << ("Initializing DataSources done....") << std::endl;
#endif

	logger << ("Initializing Caches....") << std::endl;
	ConfigurationHandler::initializeCaches();
	logger << ("Initializing Caches done....") << std::endl;

	std::vector<std::string> files;
	int sp[preForked][2];

	/*TODO if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
	{
		#if !defined(OS_MINGW) && !defined(OS_DARWIN)
		for(int j=0;j<preForked;j++)
		{
			pid_t pid = createChildProcess(serverRootDirectory,sp[j],sockfd);
			pds[j] = pid;
			std::stringstream ss;
			std::string filename;
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
		/*TODO if(!isThreadprq)
		{
			pool = new ThreadPool(thrdpsiz/2,thrdpsiz,true);
			pool->start();
		}*/
	}

#ifdef INC_DCP
	if(isCompileEnabled)
	{
		Thread* pthread = new Thread(&CHServer::dynamic_page_monitor, &serverRootDirectory);
		pthread->execute();
	}
#endif

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	logger << ("Initializing ffeadContext....") << std::endl;
	ConfigurationData::getInstance()->initializeAllSingletonBeans();
	logger << ("Initializing ffeadContext done....") << std::endl;

	//printf("server: waiting for connections...\n");
	logger.info("Server: waiting for connections on " + ipport);

	std::ofstream serverCntrlFileo;
	serverCntrlFileo.open(serverCntrlFileNm.c_str());
	serverCntrlFileo << "Server Running" << std::endl;
	serverCntrlFileo.close();

	//Sleep for some time so as to make sure all the new child processes are set correctly
	//and all init is complete...
	sleep(5);

	//Thread* pthread = new Thread(&gracefullShutdown_monitor, &ipport);
	//pthread->execute();

	std::string cntEnc = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["CONTENT_ENCODING"]);
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

	HTTPResponseStatus::getStatusByCode(200);

	ServiceHandler* handler = new HttpServiceHandler(cntEnc, &CHServer::httpServiceFactoryMethod, thrdpsiz, wthrdpsiz);
	handler->start();

	RequestReaderHandler reader(handler, sockfd);
	reader.registerSocketInterfaceFactory(&CHServer::createSocketInterface);
	reader.start();

	bool stats = false;

	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(5);
	}

	handler->stop();

	std::string ip = ipport.substr(0, ipport.find(":"));
	reader.stop(ip, CastUtil::lexical_cast<int>(port), isSSLEnabled);

	close(sockfd);

	delete (HttpServiceHandler*)handler;

#ifdef INC_SDORM
	ConfigurationHandler::destroyDataSources();
#endif

	SSLHandler::clear();

	ConfigurationHandler::destroyCaches();

	ConfigurationData::getInstance()->clearAllSingletonBeans();

	ConfigurationData::clearInstance();

	logger << "Destructed SSLHandler" << std::endl;

	#if !defined(OS_MINGW) && !defined(OS_DARWIN)
	if(preForked>0 && IS_FILE_DESC_PASSING_AVAIL)
	{
		std::map<int,pid_t>::iterator it;
		for(it=pds.begin();it!=pds.end();++it)
		{
			kill(it->second, SIGKILL);
			logger << ("Killed child process ") << it->second << std::endl;
		}
		logger << "Destructed child processes" << std::endl;
	}
	#endif

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

	RegexUtil::flushCache();

	LoggerFactory::clear();

	#ifdef OS_MINGW
		WSACleanup();
	#endif
}

int CHServer::techunkSiz = 0;
int CHServer::connKeepAlive = 10;
int CHServer::maxReqHdrCnt = 100, CHServer::maxEntitySize = 2147483647;


HttpServiceTask* CHServer::httpServiceFactoryMethod() {
	return new ServiceTask();
}

SocketInterface* CHServer::createSocketInterface(SOCKET fd) {
	SocketInterface* sockIntf = NULL;
	/*if(SSLHandler::getInstance()->getIsSSL() && sockUtil->getAlpnProto().find("h2")==0)
	{
		sockIntf = new Http2Handler(true, fd, ConfigurationData::getInstance()->coreServerProperties.webPath);
	}
	else*/
	//{
		sockIntf = new Http11Handler(fd, ConfigurationData::getInstance()->coreServerProperties.webPath,
				techunkSiz, connKeepAlive*1000, maxReqHdrCnt, maxEntitySize);
	//}

	return sockIntf;
}

Logger& CHServer::getLogger()
{
	return logger;
}
