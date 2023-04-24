/*
	Copyright 2009-2020, Sumeet Chhetri

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

//For http2 use https://github.com/nghttp2/nghttp2/blob/master/examples/asio-sv2.cc
//For http3 use https://github.com/litespeedtech/lsquic/blob/master/bin/http_server.c

#include "CHServer.h"

std::string CHServer::serverCntrlFileNm;

static std::string servd;
static bool isSSLEnabled = false, isThreadprq = false, processforcekilled = false,
		processgendone = false, isCompileEnabled = false;
static int preForked = 5;
static std::map<int,pid_t> pds;
static Mutex m_mutex, p_mutex;
static bool isrHandler1 = true;

static Logger logger;

#ifndef OS_MINGW
void sigchld_handler_server(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}
#endif

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
#if defined(HAVE_EXECINFOINC) && !defined(OS_ANDROID)
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
		logger << "Exiting, Got errono " << sig << std::endl;
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
	logger << "Segmentation fault occurred for process" << getpid() << "\n" << std::endl;
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
	logger << "Child process got killed " << getpid() << "\n"  << std::endl;
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
	logger << "Abort signal occurred for process" << getpid() << "\n" << std::endl;
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
	logger << "Termination signal occurred for process" << getpid() << "\n" << std::endl;
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
	logger << "Kill signal occurred for process" << getpid() << "\n" << std::endl;
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
	logger << "Interrupt signal occurred for process" << getpid() << "\n" << std::endl;
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
	logger << "Floating point Exception occurred for process" << getpid() << "\n" << std::endl;
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
	logger << "Broken pipe ignore it" << getpid() << "\n" << std::endl;
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
	logger << "Floating point Exception occurred for process" << getpid() << "\n" << std::endl;
	abort();
}

void* CHServer::gracefullShutdown_monitor(void* args)
{
	std::string* ipaddr = (std::string*)args;
	struct stat buffer;
	while(stat (CHServer::serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(1);
	}
	std::string ip = ipaddr->substr(0, ipaddr->find(":"));
	std::string port = ipaddr->substr(ipaddr->find(":")+1);

	if(isSSLEnabled) {
#ifdef HAVE_SSLINC
		SSLClient sc;
		sc.connection(ip, CastUtil::toInt(port));
		sc.closeConnection();
#endif
	} else {
		Client sc;
		sc.connection(ip, CastUtil::toInt(port));
		sc.closeConnection();
	}

	return NULL;
}

#ifdef INC_DCP
void* CHServer::dynamic_page_monitor(void* arg)
{
	std::string serverRootDirectory = *(std::string*)arg;
	struct stat statbuf;
	std::map<std::string, std::string, std::less<> > dcpsss = ConfigurationData::getInstance()->dynamicCppPagesMap;
	std::map<std::string, std::string, std::less<> > tpes = ConfigurationData::getInstance()->templateFilesMap;
	std::map<std::string, std::string, std::less<> > dcspstpes = dcpsss;
	dcspstpes.insert(tpes.begin(), tpes.end());
	std::map<std::string,long> statsinf;
	std::map<std::string, std::string, std::less<>>::iterator it;
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

/*
void * operator new(size_t size) throw(std::bad_alloc)
{
	ConfigurationData::counter++;
    return malloc(size);
}

void operator delete(void * p) throw()
{
	ConfigurationData::counter--;
    free(p);
}

void *operator new[](std::size_t size) throw(std::bad_alloc)
{
	ConfigurationData::counter++;
    return malloc(size);
}
void operator delete[](void *p) throw()
{
	ConfigurationData::counter--;
    free(p);
}
*/


int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		//cout << "No Server root directory specified, quitting..." << std::endl;
		return 0;
	}

	std::string serverRootDirectory = argv[1];
	serverRootDirectory += "/";
	RegexUtil::replace(serverRootDirectory,"[/]+","/");

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
		//std::streambuf* cout_buff = std::cout.rdbuf();
		std::cout.rdbuf(local.rdbuf());
	}

	if(argc > 2)
	{
		port = argv[2];
		isMain = false;
	}
	if(port=="")
	{
		port = srprps["PORT_NO"];
	}
	if(argc > 3)
	{
		ipaddr = argv[3];
	}
	if(ipaddr=="")
	{
		ipaddr = srprps["IP_ADDR"];
	}
	int vhostNum = 0;
	if(argc > 4)
	{
		vhostNum = CastUtil::toInt(argv[4]);
	}
	if(argc > 5)
	{
		servingAppNames = argv[5];
		servedAppNames = StringUtil::splitAndReturn<std::vector<std::string> >(servingAppNames, ",");
	}

	try {
		return CHServer::entryPoint(vhostNum, isMain, serverRootDirectory, port, ipaddr, servedAppNames);
	} catch (const XmlParseException& e) {
		std::cout << e.getMessage() << std::endl;
	} catch(const std::exception& e) {
		std::cout << "Error Occurred in serve" << std::endl;
	}
	return 0;
}

int CHServer::entryPoint(int vhostNum, bool isMain, std::string serverRootDirectory, std::string port, std::string ipaddr, std::vector<std::string> servedAppNames)
{
	//pid_t parid = getpid();

	#ifndef OS_MINGW
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_handler = sigchld_handler_server;
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &act, NULL) < 0) {
		perror ("sigaction");
		exit(1);
	}
	//signal(SIGSEGV,signalSIGSEGV);
	//signal(SIGFPE,signalSIGFPE);
	#ifndef OS_ANDROID
	(void) sigignore(SIGPIPE);
	#endif
	//(void) sigignore(SIGINT);
	#else
		// startup WinSock in Windows
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(1,1), &wsa_data);
	#endif
	
	#ifdef OS_MINGW
	//SOCKET sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	#else
	//int sockfd, new_fd;
	#endif
	
	//struct sockaddr_storage their_addr; // connector's address information
	//socklen_t sin_size;

    //int yes=1,rv;
    //int nfds;

    serverRootDirectory += "/";
    RegexUtil::replace(serverRootDirectory,"[/]+","/");

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
	std::string logp = respath+"logging.xml";
	LoggerFactory::init(logp, serverRootDirectory, "", StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true");

	std::string name;
	if(isMain)
	{
		std::stringstream ss;
		ss << serverRootDirectory;
		ss << "ffead.";
		ss << getpid();
		ss << ".cntrl";
		ss >> serverCntrlFileNm;
		//serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
		name = "CHServer(Main)";
		LoggerFactory::instance->setVhostNumber(0);
	}
	else
	{
		name = "CHServer(VHost-" + CastUtil::fromNumber(vhostNum) + ")";
		serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." + CastUtil::fromNumber(vhostNum);
	}

	logger = LoggerFactory::getLogger(name);

    if(srprps["NUM_PROC"]!="")
    {
    	try
		{
    		preForked = CastUtil::toInt(srprps["NUM_PROC"]);
		}
		catch(const std::exception& e)
		{
			logger << "Invalid number for worker processes defined" << std::endl;
			preForked = 5;
		}
    }
    std::string sslEnabled = srprps["SSL_ENAB"];
   	if(sslEnabled=="true" || sslEnabled=="TRUE")
   		isSSLEnabled = true;

   	int thrdpsiz = 0;
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
				thrdpsiz = CastUtil::toInt(thrdpreq);
			}
			catch(const std::exception& e)
			{
				logger << "Invalid service thread pool size defined" << std::endl;
				thrdpsiz = CommonUtils::getProcessorCount();
			}
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
   			sessionTimeout = CastUtil::toLong(srprps["SESS_TIME_OUT"]);
		} catch(const std::exception& e) {
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
#ifdef HAVE_SSLINC
	SSLHandler::setIsSSL(isSSLEnabled);
#endif
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

	ConfigurationData::getInstance()->enableCors = StringUtil::toLowerCopy(srprps["ENABLE_CRS"])=="true";
	ConfigurationData::getInstance()->enableSecurity = StringUtil::toLowerCopy(srprps["ENABLE_SEC"])=="true";
	ConfigurationData::getInstance()->enableFilters = StringUtil::toLowerCopy(srprps["ENABLE_FLT"])=="true";
	ConfigurationData::getInstance()->enableControllers = StringUtil::toLowerCopy(srprps["ENABLE_CNT"])=="true";
	ConfigurationData::getInstance()->enableExtControllers = StringUtil::toLowerCopy(srprps["ENABLE_EXT_CNT"])=="true";
	ConfigurationData::getInstance()->enableContMpg = StringUtil::toLowerCopy(srprps["ENABLE_CNT_MPG"])=="true";
	ConfigurationData::getInstance()->enableContPath = StringUtil::toLowerCopy(srprps["ENABLE_CNT_PTH"])=="true";
	ConfigurationData::getInstance()->enableContExt = StringUtil::toLowerCopy(srprps["ENABLE_CNT_EXT"])=="true";
	ConfigurationData::getInstance()->enableContRst = StringUtil::toLowerCopy(srprps["ENABLE_CNT_RST"])=="true";
	ConfigurationData::getInstance()->enableExtra = StringUtil::toLowerCopy(srprps["ENABLE_EXT"])=="true";
	ConfigurationData::getInstance()->enableScripts = StringUtil::toLowerCopy(srprps["ENABLE_SCR"])=="true";
	ConfigurationData::getInstance()->enableSoap = StringUtil::toLowerCopy(srprps["ENABLE_SWS"])=="true";
	ConfigurationData::getInstance()->enableLogging = StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true";
	ConfigurationData::getInstance()->enableJobs = StringUtil::toLowerCopy(srprps["ENABLE_JOBS"])=="true";
	ConfigurationData::getInstance()->enableStaticResponses = StringUtil::toLowerCopy(srprps["ENABLE_STATIC_RESP"])=="true";

    strVec cmpnames;
    try
    {
    	ConfigurationHandler::handle(webdirs, webdirs1, incpath, rtdcfpath, serverRootDirectory, respath);
    }
    catch(const XmlParseException& p)
    {
    	logger << p.getMessage() << std::endl;
    }
    catch(const std::exception& msg)
	{
		logger << msg.what() << std::endl;
	}
#ifdef HAVE_SSLINC
    SSLHandler::initInstance(ConfigurationData::getInstance()->securityProperties);
#endif
    logger << INTER_LIB_FILE << std::endl;

    bool libpresent = true;
    void *ilibtemp = dlopen(INTER_LIB_FILE, RTLD_NOW);
    void *dlibtemp = dlopen(DINTER_LIB_FILE, RTLD_NOW);
	//logger << endl <<dlibtemp << std::endl;
	if(ilibtemp==NULL || dlibtemp==NULL)
	{
		libpresent = false;
		char *errstr = dlerror();
		if (errstr != NULL) {
			printf ("A dynamic linking error occurred: (%s)\n", errstr);
		}
		logger.info("Could not load Library");
	}
	else
	{
		dlclose(ilibtemp);
		dlclose(dlibtemp);
	}

	if(isMain)
	{
		//Generate library if dev mode = true or the library is not found in prod mode
		if(isCompileEnabled || !libpresent)
			libpresent = false;
#ifdef BUILD_AUTOCONF
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

				compres = respath+"rundyn-automake_autoconf.sh "+serverRootDirectory;
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

			compres = respath+"rundyn-automake_autoconf.sh "+serverRootDirectory;
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
			char *errstr = dlerror();
			if (errstr != NULL) {
				printf ("A dynamic linking error occurred: (%s)\n", errstr);
			}
			logger.info("Could not load Library");
			exit(0);
		}
		else
		{
			dlclose(checkdlib);
			dlclose(checkddlib);
			logger.info("Library generated successfully");
		}
#else
		if(!libpresent)
		{
#if defined(BUILD_CMAKE)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " cmake";
#elif defined(BUILD_XMAKE)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " xmake";
#elif defined(BUILD_MESON)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " meson";
#elif defined(BUILD_SCONS)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " scons";
#elif defined(BUILD_BAZEL)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " bazel";
#elif defined(BUILD_BUCK2)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " buck2";
#elif defined(BUILD_SHELLB)
			std::string compres = respath+"rundyn-automake.sh "+serverRootDirectory + " shellb";
#else
			logger << "Invalid Build Type specified, only cmake, xmake, meson, scons, shellb, bazel and buck2 supported...\n" << std::endl;
#endif
			std::string output = ScriptHandler::execute(compres, true);
			logger << "Intermediate code generation task\n\n" << std::endl;
			logger << output << std::endl;
		}

		void* checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
		void* checkddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
		if(checkdlib==NULL || checkddlib==NULL)
		{
			char *errstr = dlerror();
			if (errstr != NULL) {
				printf ("A dynamic linking error occurred: (%s)\n", errstr);
			}
			logger.info("Could not load Library");
			exit(0);
		}
		else
		{
			dlclose(checkdlib);
			dlclose(checkddlib);
			logger.info("Library generated successfully");
		}
#endif

	#ifdef INC_COMP
		for (unsigned int var1 = 0;var1<ConfigurationData::getInstance()->componentNames.size();var1++)
		{
			std::string name = ConfigurationData::getInstance()->componentNames.at(var1);
			StringUtil::replaceFirst(name,"Component_","");
			ComponentHandler::registerComponent(name);
			AppContext::registerComponent(name);
		}
	#endif

		//bool distocache = false;
	/*#ifdef INC_DSTC
		int distocachepoolsize = 20;
		try {
			if(srprps["DISTOCACHE_POOL_SIZE"]!="")
			{
				distocachepoolsize = CastUtil::toInt(srprps["DISTOCACHE_POOL_SIZE"]);
			}
		} catch(const std::exception& e) {
			logger << ("Invalid poolsize specified for distocache") << std::endl;
		}

		try {
			if(srprps["DISTOCACHE_PORT_NO"]!="")
			{
				CastUtil::toInt(srprps["DISTOCACHE_PORT_NO"]);
				DistoCacheHandler::trigger(srprps["DISTOCACHE_PORT_NO"], distocachepoolsize);
				logger << ("Session store is set to distocache store") << std::endl;
				distocache = true;
			}
		} catch(const std::exception& e) {
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
				int port = CastUtil::toInt(srprps["CMP_PORT"]);
				if(port>0)
				{
					ComponentHandler::trigger(srprps["CMP_PORT"]);
				}
			}
		} catch(const std::exception& e) {
			logger << ("Component Handler Services are disabled") << std::endl;
		}
	#endif

	#ifdef INC_MSGH
		try {
			if(srprps["MESS_PORT"]!="")
			{
				int port = CastUtil::toInt(srprps["MESS_PORT"]);
				if(port>0)
				{
					MessageHandler::trigger(srprps["MESS_PORT"],resourcePath);
				}
			}
		} catch(const std::exception& e) {
			logger << ("Messaging Handler Services are disabled") << std::endl;
		}
	#endif

	#ifdef INC_MI
		try {
			if(srprps["MI_PORT"]!="")
			{
				int port = CastUtil::toInt(srprps["MI_PORT"]);
				if(port>0)
				{
					MethodInvoc::trigger(srprps["MI_PORT"]);
				}
			}
		} catch(const std::exception& e) {
			logger << ("Method Invoker Services are disabled") << std::endl;
		}
	#endif
	}

	pid_t pid;

	if(isMain)
	{
		bool startedOne = false;
		if(StringUtil::toLowerCopy(srprps["PROC_PER_CORE"])=="true" || StringUtil::toLowerCopy(srprps["PROC_PER_CORE"])=="yes") {
			unsigned int nthreads = hardware_concurrency();
			for (int var = 0; var < (int)nthreads; ++var) {
				#if !defined(OS_MINGW)
					pid = fork();
					if(pid == 0)
					{
						LoggerFactory::instance->setVhostNumber(var+1);

						std::string lnm = "CHServer(VHost-" +
								CastUtil::fromNumber(var+1) + ")";
						serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." +
								CastUtil::fromNumber(var+1);
						serve(port, ipaddr, thrdpsiz, serverRootDirectory, srprps, var+1);
						exit(0);
					}
					else
					{
						serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." +
								CastUtil::fromNumber(var+1);
						struct stat buffer;
						while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
						{
							Thread::sSleep(10);
						}
					}
				#else
					std::string vhostcmd = "./vhost-server.sh " + serverRootDirectory + " \"" + ipaddr + "\" " + port
							+ " \"\" " + CastUtil::fromNumber(var+1);
					std::string vhostcmdo = ScriptHandler::chdirExecute(vhostcmd, serverRootDirectory, true);
					logger.info("Starting new Virtual-Host at " + (ipaddr + ":" + port));
					logger << vhostcmdo << std::endl;
				#endif
			}
			startedOne = true;
		} else {
			propMultiMap mpmap = pread.getPropertiesMultiMap(respath+"server.prop");
			if(mpmap.find("VHOST_ENTRY")!=mpmap.end() && mpmap["VHOST_ENTRY"].size()>0)
			{
				std::vector<std::string> vhosts = mpmap["VHOST_ENTRY"];
				for(int vhi=0;vhi<(int)vhosts.size();vhi++)
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
							//valid = false;
							//logger << ("No host specified for Virtual-Host") << std::endl;
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
							StringUtil::trim(vhostapps);
							std::map<std::string, bool, std::less<> > updatedcontextNames = ConfigurationData::getInstance()->servingContexts;
							std::map<std::string, std::string, std::less<> > updatedaliasNames = ConfigurationData::getInstance()->appAliases;
							if(vhostapps!="") {
								std::vector<std::string> spns = StringUtil::splitAndReturn<std::vector<std::string> >(vhostapps, ",");
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
								std::map<std::string, bool, std::less<>>::iterator ucit;
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
							}
							startedOne = true;
							#if !defined(OS_MINGW)
								pid = fork();
								if(pid == 0)
								{
									LoggerFactory::instance->setVhostNumber(vhi+1);
									ConfigurationData::getInstance()->servingContexts = updatedcontextNames;
									ConfigurationData::getInstance()->appAliases = updatedaliasNames;

									std::string lnm = "CHServer(VHost-" +
											CastUtil::fromNumber(vhi+1) + ")";
									serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." +
											CastUtil::fromNumber(vhi+1);
									serve(vhostport, vhostname, thrdpsiz, serverRootDirectory, srprps, vhi+1);
									exit(0);
								}
								else
								{
									serverCntrlFileNm = serverRootDirectory + "ffead.cntrl." +
											CastUtil::fromNumber(vhi+1);
									struct stat buffer;
									while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
									{
										Thread::sSleep(10);
									}
								}
							#else
								std::string vhostcmd = "./vhost-server.sh " + serverRootDirectory + " " + vhostname + " " + vhostport
										+ " " + vhostapps + " " + CastUtil::fromNumber(vhi+1);
								std::string vhostcmdo = ScriptHandler::chdirExecute(vhostcmd, serverRootDirectory, true);
								logger.info("Starting new Virtual-Host at " + (vhostname + ":" + vhostport));
								logger << vhostcmdo << std::endl;
							#endif
						}
					}
				}
				if(!startedOne) {
					logger << ("No valid Virtual-Hosts found, will run normal server") << std::endl;
				}
			}
		}

		if(!startedOne) {
			try {
				serve(port, ipaddr, thrdpsiz, serverRootDirectory, srprps, vhostNum);
			} catch(const std::exception& e) {
				logger << e.what() << std::endl;
			}
		}
	}

	return 0;
}

//Copied from https://github.com/awgn/speedcore/blob/master/speedcore.cpp
unsigned int CHServer::hardware_concurrency()
{
    auto proc = []() -> int {
        std::ifstream cpuinfo("/proc/cpuinfo");
        return std::count(std::istream_iterator<std::string>(cpuinfo),
                          std::istream_iterator<std::string>(),
                          std::string("processor"));
    };

    auto hc = std::thread::hardware_concurrency();
    return hc ? hc : proc();
}


void CHServer::serve(std::string port, std::string ipaddr, int thrdpsiz, std::string serverRootDirectory, propMap sprops, int vhostNumber)
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

	ConfigurationData::getInstance()->serverType = SERVER_BACKEND::EMBEDDED;
	ConfigurationData::getInstance()->coreServerProperties.ip_address = ipport;

	if(ConfigurationData::getInstance()->servingContexts.size()==0)
	{
		if(vhostNumber==0) {
			logger << ("No context to be served by Server (" +  ipport  + ")") << std::endl;
		} else {
			logger << ("No context to be served by Virtual-Host (" +  ipport  + ")") << std::endl;
		}
		return;
	}
	else
	{
		if(vhostNumber==0)
		{
			std::map<std::string, bool, std::less<>>::iterator it;
			for (it=ConfigurationData::getInstance()->servingContexts.begin();it!=ConfigurationData::getInstance()->servingContexts.end();++it)
				logger << ("Server (" +  ipport  + ") serves context (" +  it->first  + ")") << std::endl;
		}
		else
		{
			std::map<std::string, bool, std::less<>>::iterator it;
			for (it=ConfigurationData::getInstance()->servingContexts.begin();it!=ConfigurationData::getInstance()->servingContexts.end();++it)
				logger << ("Virtual-Host (" +  ipport  + ") serves context (" +  it->first  + ")") << std::endl;
		}
	}

	//int nfds;
	if(sprops["NUM_PROC"]!="")
	{
		try
		{
			preForked = CastUtil::toInt(sprops["NUM_PROC"]);
		}
		catch(const std::exception& e)
		{
			logger << "Invalid number for worker processes defined" << std::endl;
			preForked = 5;
		}
	}

	HttpClient::init();

	bool isSinglEVH = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["EVH_SINGLE"])=="true";

#ifdef USE_IO_URING
	if(!isSinglEVH) {
		logger << "Unable to start server with IO_URING in thread-pooled mode, please set EVH_SINGLE (Single process mode to true)" << std::endl;
		return;
	}
#endif

	std::string cntEnc = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["CONTENT_ENCODING"]);
	try {
		techunkSiz = CastUtil::toInt(ConfigurationData::getInstance()->coreServerProperties.sprops["TRANSFER_ENCODING_CHUNK_SIZE"]);
	} catch(const std::exception& e) {
	}
	try {
		connKeepAlive = CastUtil::toInt(ConfigurationData::getInstance()->coreServerProperties.sprops["KEEP_ALIVE_SECONDS"]);
	} catch(const std::exception& e) {
	}
	try {
		maxReqHdrCnt = CastUtil::toInt(ConfigurationData::getInstance()->coreServerProperties.sprops["MAX_REQUEST_HEADERS_COUNT"]);
	} catch(const std::exception& e) {
	}
	try {
		maxEntitySize = CastUtil::toInt(ConfigurationData::getInstance()->coreServerProperties.sprops["MAX_REQUEST_ENTITY_SIZE"]);
	} catch(const std::exception& e) {
	}

	std::string rHandler = "RequestReaderHandler";
	try {
		rHandler = ConfigurationData::getInstance()->coreServerProperties.sprops["REQUEST_HANDLER"];
	} catch(const std::exception& e) {
	}

	isrHandler1 = rHandler=="RequestReaderHandler";

	bool isLazyHeaderParsing = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["LAZY_HEADER_PARSE"])=="true";

	std::string qw = "false";
	try {
		qw = ConfigurationData::getInstance()->coreServerProperties.sprops["QUEUED_WRITES"];
	} catch(const std::exception& e) {
	}
	bool isQueuedWrites = StringUtil::toLowerCopy(qw)=="true";

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	logger << ("Initializing ffeadContext....") << std::endl;
	ConfigurationData::getInstance()->initializeAllSingletonBeans();
	GenericObject::init(&(ConfigurationData::getInstance()->reflector));
	logger << ("Initializing ffeadContext done....") << std::endl;

	logger << ("Initializing WSDL files....") << std::endl;
	ConfigurationHandler::initializeWsdls();
	logger << ("Initializing WSDL files done....") << std::endl;

	if(!isrHandler1) {
		isSinglEVH = true;
	}

	CommonUtils::setDate();

	unsigned int nthreads = hardware_concurrency();
	ServiceHandler* handler = new HttpServiceHandler(cntEnc, &ServiceTask::handleWebsockOpen, nthreads, isSinglEVH, &ServiceTask::handle);

	void* rHandleIns = NULL;

	if(isrHandler1) {
		RequestReaderHandler* reader = new RequestReaderHandler(handler, true, isSinglEVH);
		RequestReaderHandler::setInstance(reader);
		handler->start(&CHServer::clearSocketInterface);
		reader->registerSocketInterfaceFactory(&CHServer::createSocketInterface);
		reader->startNL(-1);
		rHandleIns = reader;
		logger << ("Initializing RequestReaderHandler....") << std::endl;
	} else {
		RequestHandler2* reader = new RequestHandler2(handler, true, isSSLEnabled, &ServiceTask::handle);
		RequestHandler2::setInstance(reader);
		handler->start(&CHServer::clearSocketInterface);
		reader->registerSocketInterfaceFactory(&CHServer::createSocketInterface);
		reader->startNL(-1, isQueuedWrites);
		rHandleIns = reader;
		logger << ("Initializing RequestHandler2....") << std::endl;
	}

	std::ofstream serverCntrlFileo;
	serverCntrlFileo.open(serverCntrlFileNm.c_str());
	serverCntrlFileo << "Server Running" << std::endl;
	serverCntrlFileo.close();

#ifdef INC_SDORM
	logger << ("Initializing DataSources....") << std::endl;
	ConfigurationHandler::initializeDataSources();
	logger << ("Initializing DataSources done....") << std::endl;
#endif

	logger << ("Initializing Caches....") << std::endl;
	ConfigurationHandler::initializeCaches();
	logger << ("Initializing Caches done....") << std::endl;

	logger << ("Initializing Searches....") << std::endl;
	ConfigurationHandler::initializeSearches();
	logger << ("Initializing Searches done....") << std::endl;

	//struct sockaddr_storage their_addr; // connector's address information
	//socklen_t sin_size;
	SOCKET sockfd = Server::createListener(ipaddr, CastUtil::toInt(port), true, isSinglEVH);
	if(sockfd==-1)
	{
		logger << "Unable to start the server on the specified ip/port..." << std::endl;
		return;
	}
#if defined(OS_LINUX) && !defined(OS_ANDROID) && !defined(DISABLE_BPF)
	Server::set_cbpf(sockfd, get_nprocs());
#endif
	if(isrHandler1) {
		RequestReaderHandler* rh = (RequestReaderHandler*)rHandleIns;
		rh->addListenerSocket(&doRegisterListenerFunc, sockfd);
	} else {
		RequestHandler2* rh = (RequestHandler2*)rHandleIns;
		rh->addListenerSocket(&doRegisterListenerFunc, sockfd);
	}

	bool enableJobs = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["ENABLE_JOBS"])=="true";
#ifdef INC_JOBS
	if(enableJobs) {
		JobScheduler::start();
	}
#endif

#ifdef INC_DCP
	if(isCompileEnabled)
	{
		Thread* pthread = new Thread(&CHServer::dynamic_page_monitor, &serverRootDirectory);
		pthread->execute();
	}
#endif

	//printf("server: waiting for connections...\n");
	logger.write("Server: waiting for connections on %s\n", ipport.c_str());

	//Sleep for some time so as to make sure all the new child processes are set correctly
	//and all init is complete...
	sleep(5);

	//Thread* pthread = new Thread(&gracefullShutdown_monitor, &ipport);
	//pthread->execute();


	HTTPResponseStatus::init();

	HttpRequest::init(isLazyHeaderParsing);

	HttpResponse::init();

	MultipartContent::init();

	int counter = 0;
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		CommonUtils::setDate();
		Thread::sSleep(1);
		if(counter++>=5) {
			handler->closeConnectionsInternal();
			counter = 0;
		}
		//CommonUtils::printStats();
	}

	std::string ip = ipport.substr(0, ipport.find(":"));
	if(isrHandler1) {
		RequestReaderHandler* rh = (RequestReaderHandler*)rHandleIns;
		rh->stop(ip, CastUtil::toInt(port), isSSLEnabled);
	} else {
		RequestHandler2* rh = (RequestHandler2*)rHandleIns;
		rh->stop(ip, CastUtil::toInt(port), isSSLEnabled);
	}

	if(isrHandler1) {
		delete (RequestReaderHandler*)rHandleIns;
	} else {
		delete (RequestHandler2*)rHandleIns;
	}

	close(sockfd);

	delete (HttpServiceHandler*)handler;
#ifdef HAVE_SSLINC
	SSLHandler::clear();
#endif
	ConfigurationHandler::destroyCaches();

	ConfigurationHandler::destroySearches();

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
	if(enableJobs) {
		JobScheduler::stop();
	}
#endif

	RegexUtil::flushCache();

	HttpClient::cleanup();

	#ifdef OS_MINGW
		WSACleanup();
	#endif

	//std::string lg = "Memory allocations waiting to be freed = " + CastUtil::fromNumber(ConfigurationData::counter);
	//logger <<  lg << std::endl;

#ifdef INC_SDORM
	ConfigurationHandler::destroyDataSources();
#endif

	LoggerFactory::clear();

	CommonUtils::clearInstance();
}

int CHServer::techunkSiz = 0;
int CHServer::connKeepAlive = 10;
int CHServer::maxReqHdrCnt = 100, CHServer::maxEntitySize = 2147483647;

BaseSocket* CHServer::createSocketInterface(SOCKET fd) {
	if(!isrHandler1) {
		return new Http11Socket(fd, techunkSiz, connKeepAlive, maxReqHdrCnt, maxEntitySize);
	}
#ifdef HAVE_SSLINC
	SSL* ssl = NULL;
	BIO* io = NULL;
	if(BaseSecureSocket::init(fd, ssl, io)) {
		return new Http2Handler(fd, ssl, io, true, ConfigurationData::getInstance()->coreServerProperties.webPath);
	} else {
		return new Http11Handler(fd, ssl, io, techunkSiz, connKeepAlive, maxReqHdrCnt, maxEntitySize);
	}
#else
	return new Http11Handler(fd, NULL, NULL, techunkSiz, connKeepAlive, maxReqHdrCnt, maxEntitySize);
#endif
}

void CHServer::clearSocketInterface(BaseSocket* bs) {
	if(!isrHandler1) {
		delete (Http11Socket*)bs;
	} else {
		SocketInterface* si = (SocketInterface*)bs;
		if(si->isHttp2()) {
			delete (Http2Handler*)si;
		} else {
			delete (Http11Handler*)si;
		}
	}
}

bool CHServer::doRegisterListenerFunc() {
	struct stat buffer;
	return stat (serverCntrlFileNm.c_str(), &buffer)==0 && CacheManager::isInitCompleted() && DataSourceManager::isInitCompleted() && SearchEngineManager::isInitCompleted();
}
