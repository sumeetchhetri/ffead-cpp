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
string servd;
static bool isSSLEnabled = false,isThreadprq = false,processforcekilled = false,processgendone = false,sessatserv = false,isCompileEnabled = false;
static long sessionTimeout;
static int thrdpsiz/*,shmid*/;
static SSL_CTX *ctx;
static char *ciphers=0;
map<int,pid_t> pds;
static pid_t parid;
void *dlib = NULL;
typedef map<string,string> sessionMap;
static Mutex m_mutex,p_mutex;
ConfigurationData configurationData;

Logger logger;

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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Segmentation fault occurred for process" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Child process got killed " << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Abort signal occurred for process" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Termination signal occurred for process" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Kill signal occurred for process" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Interrupt signal occurred for process" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Broken pipe ignore it" << getpid() << "\n" << tempo << flush;
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
	string tempo;
	/*void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);*/
	logger << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
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
		SSLHandler sSLHandler;
		dlib = dlopen(Constants::INTER_LIB_FILE.c_str(), RTLD_NOW);
		logger << endl <<dlib << endl;
		if(dlib==NULL)
		{
			logger << dlerror() << endl;
			logger.info("Could not load Library");
		}
		else
			logger.info("Library loaded successfully");
		if(isSSLEnabled)
		{
			/*HTTPS related*/
			//client_auth=CLIENT_AUTH_REQUIRE;
			/* Build our SSL context*/
			ctx = sSLHandler.initialize_ctx((char*)configurationData.key_file.c_str(),(char*)configurationData.sec_password.c_str(),
					configurationData.ca_list);
			sSLHandler.load_dh_params(ctx,(char*)configurationData.dh_file.c_str());

			SSL_CTX_set_session_id_context(ctx,
			  (const unsigned char*)&SSLHandler::s_server_session_id_context,
			  sizeof SSLHandler::s_server_session_id_context);

			/* Set our cipher list */
			if(ciphers){
			  SSL_CTX_set_cipher_list(ctx,ciphers);
			}
			/*if(configurationData.client_auth==2)
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);
			else if(configurationData.client_auth==1)
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,0);*/
		}
		servd = serverRootDirectory;
		string filename;
		stringstream ss;
		ss << serverRootDirectory;
		ss << getpid();
		ss >> filename;
		filename.append(".cntrl");
		logger << "generated file " << filename << flush;
		ofstream cntrlfile;
		cntrlfile.open(filename.c_str());
		cntrlfile << "Process Running" << flush;
		cntrlfile.close();

		close(sockfd);

		SelEpolKqEvPrt selEpolKqEvPrtHandler;
		selEpolKqEvPrtHandler.initialize(sp[1]);
		ThreadPool pool;
		if(!isThreadprq)
		{
			pool.init(thrdpsiz,30,true);
		}
		PropFileReader pread;
		propMap params = pread.getProperties(serverRootDirectory+"resources/security.prop");

		logger << params.size() <<endl;
		while(1)
		{
			int nfds = selEpolKqEvPrtHandler.getEvents();
			if (nfds == -1)
			{
				perror("poller wait child process");
				logger << "\n----------poller child process----" << flush;
			}
			else
			{
				int fd = receive_fd(sp[1]);
				selEpolKqEvPrtHandler.reRegisterServerSock();
				fcntl(fd, F_SETFL,O_SYNC);

				char buf[10];
				int err;
				if((err=recv(fd,buf,10,MSG_PEEK))==0)
				{
					close(fd);
					logger << "\nsocket conn closed before being serviced" << flush;
					continue;
				}

				if(isThreadprq)
				{
					ServiceTask *task = new ServiceTask(fd,serverRootDirectory,&params,
								isSSLEnabled, ctx, sSLHandler, configurationData, dlib);
					Thread pthread(&service, task);
					pthread.execute();
				}
				else
				{
					ServiceTask *task = new ServiceTask(fd,serverRootDirectory,&params,
							isSSLEnabled, ctx, sSLHandler, configurationData, dlib);
					pool.execute(*task);
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


void* dynamic_page_monitor(void* arg)
{
	string serverRootDirectory = *(string*)arg;
	struct stat statbuf;
	strVec dcpsss = configurationData.dcpsss;
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
		Thread::sSleep(5);
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
				string compres = respath+"rundyn.sh";
				int i=system(compres.c_str());
				if(!i)
				{
					logger << "regenarting intermediate code-----Done" << endl;
					logger.info("Done generating intermediate code");
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
	return NULL;
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
    int yes=1;
    int rv,nfds,preForked=5;
    string serverRootDirectory = argv[1];

	serverRootDirectory += "/";//serverRootDirectory = "/home/sumeet/server/";
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	string resourcePath = respath;

	servd = serverRootDirectory;
	string logp = respath+"/log.prop";
	Logger::init(logp);

	logger = Logger::getLogger("CHServer");

    PropFileReader pread;
    propMap srprps = pread.getProperties(respath+"server.prop");
    if(srprps["NUM_PROC"]!="")
    	preForked = CastUtil::lexical_cast<int>(srprps["NUM_PROC"]);
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
				logger << "\nInvalid thread pool size defined" << flush;
				thrdpsiz = 30;
			}
		}
	}
   	string compileEnabled = srprps["DEV_MODE"];
	if(compileEnabled=="true" || compileEnabled=="TRUE")
		isCompileEnabled = true;

	if(srprps["SCRIPT_ERRS"]=="true" || srprps["SCRIPT_ERRS"]=="TRUE")
	{
		Constants::SCRIPT_EXEC_SHOW_ERRS = true;
	}
   	if(srprps["SESS_STATE"]=="server")
   		sessatserv = true;
   	if(srprps["SESS_TIME_OUT"]!="")
   	{
   		try {
   			sessionTimeout = CastUtil::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch (...) {
			sessionTimeout = 3600;
			logger << "\nInvalid session timeout value defined, defaulting to 1hour/3600sec";
		}
   	}
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    string PORT = srprps["PORT_NO"];
    string IP_ADDRES = srprps["IP_ADDR"];
    string IP_ADDRESS;
    const char *ip_addr = NULL;
	if(IP_ADDRES!="")
		ip_addr = IP_ADDRES.c_str();
    if ((rv = getaddrinfo(ip_addr, &PORT[0], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    if(IP_ADDRES!="")
    	IP_ADDRESS = IP_ADDRES + ":" + PORT;
    else
    	IP_ADDRESS = "localhost:" + PORT;
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
    ConfigurationHandler::listi(webpath,"/",true,webdirs);
    ConfigurationHandler::listi(webpath,"/",false,webdirs1);
    ConfigurationHandler::listi(pubpath,".js",false,pubfiles);


    strVec cmpnames;
    try
    {
    	configurationData = ConfigurationHandler::handle(webdirs, webdirs1, incpath, rtdcfpath, pubpath, respath, isSSLEnabled);
    }
    catch(XmlParseException p)
    {
    	logger << p.getMessage() << endl;
    }
    catch(const char* msg)
	{
		logger << msg << endl;
	}
    configurationData.sessionTimeout = sessionTimeout;
    configurationData.ip_address = IP_ADDRESS;
    configurationData.sessatserv = sessatserv;
    for(unsigned int var=0;var<pubfiles.size();var++)
	{
    	configurationData.pubMap[pubfiles.at(var)] = "true";
	}
    bool libpresent = true;
    void *dlibtemp = dlopen(Constants::INTER_LIB_FILE.c_str(), RTLD_NOW);
	logger << endl <<dlibtemp << endl;
	if(dlibtemp==NULL)
	{
		libpresent = false;
		logger << dlerror() << endl;
		logger.info("Could not load Library");
	}
	else
		dlclose(dlibtemp);
    if(isCompileEnabled)
    	libpresent = false;

    configurationData.props = pread.getProperties(respath+"mime-types.prop");
    configurationData.lprops = pread.getProperties(respath+"locale.prop");
	string compres = respath+"rundyn.sh";
	if(!libpresent)
	{
		string output = ScriptHandler::execute(compres, true);
		logger << "Intermediate code generation task " << endl;
		logger << output << endl;
	}

	for (unsigned int var1 = 0;var1<configurationData.cmpnames.size();var1++)
	{
		string name = configurationData.cmpnames.at(var1);
		StringUtil::replaceFirst(name,"Component_","");
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

	printf("server: waiting for connections...\n");
	logger.info("Server: waiting for connections on port "+PORT);

	vector<string> files;
	int sp[preForked][2];
	ThreadPool *pool;
	propMap params;
	SSLHandler sSLHandler;
	if(Constants::IS_FILE_DESC_PASSING_AVAIL)
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
		propMap params = pread.getProperties(serverRootDirectory+"resources/security.prop");
		SSLHandler sSLHandler;
		if(isSSLEnabled)
		{
			/*HTTPS related*/
			//client_auth=CLIENT_AUTH_REQUIRE;
			/* Build our SSL context*/
			ctx = sSLHandler.initialize_ctx((char*)configurationData.key_file.c_str(),(char*)configurationData.sec_password.c_str(),
					configurationData.ca_list);
			sSLHandler.load_dh_params(ctx,(char*)configurationData.dh_file.c_str());

			SSL_CTX_set_session_id_context(ctx,
			  (const unsigned char*)&SSLHandler::s_server_session_id_context,
			  sizeof SSLHandler::s_server_session_id_context);

			/* Set our cipher list */
			if(ciphers){
			  SSL_CTX_set_cipher_list(ctx,ciphers);
			}
			/*if(configurationData.client_auth==2)
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);
			else if(configurationData.client_auth==1)
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,0);*/
		}
		dlib = dlopen(Constants::INTER_LIB_FILE.c_str(), RTLD_NOW);
		logger << endl <<dlib << endl;
		if(dlib==NULL)
		{
			logger << dlerror() << endl;
			logger.info("Could not load Library");
		}
		else
			logger.info("Library loaded successfully");
		if(!isThreadprq)
		{
			pool = new ThreadPool(thrdpsiz,thrdpsiz+30,true);
			pool->start();
		}
	}

	if(isCompileEnabled)
	{
		Thread pthread(&dynamic_page_monitor, &serverRootDirectory);
		pthread.execute();
	}

	SelEpolKqEvPrt selEpolKqEvPrtHandler;
	selEpolKqEvPrtHandler.initialize(sockfd);
	int childNo = 0;
	/*if(fork()==0)
	{
		//start  of hotdeployment process

	}*/

	ifstream cntrlfile;
	while(1)
	{
		if(childNo>=preForked)
			childNo = 0;
		nfds = selEpolKqEvPrtHandler.getEvents();
		logger << "in while " << nfds <<flush;
		if (nfds == -1)
		{
			perror("poll_wait main process");
			logger.info("Interruption Signal Received\n");
			if(errno==EBADF)
				logger << "\nInavlid fd" <<flush;
			else if(errno==EFAULT)
				logger << "\nThe memory area pointed to by events is not accessible" <<flush;
			else if(errno==EINTR)
				logger << "\ncall was interrupted by a signal handler before any of the requested events occurred" <<flush;
			else if(errno==EINVAL)
				logger << "not a poll file descriptor, or maxevents is less than or equal to zero" << endl;
			else
				logger << "\nnot an epoll file descriptor" <<flush;
		}
		processgendone = false;
		if(processforcekilled)
		{
			if(Constants::IS_FILE_DESC_PASSING_AVAIL)
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
				logger << "got new connection "<< descriptor << endl;
				selEpolKqEvPrtHandler.unRegisterForEvent(descriptor);
				if(Constants::IS_FILE_DESC_PASSING_AVAIL)
				{
					cntrlfile.open(files.at(childNo).c_str());
					if(cntrlfile.is_open())
					{
						send_connection(sp[childNo][0], descriptor);
						string cno = CastUtil::lexical_cast<string>(childNo);
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
								send_connection(sp[childNo][0], descriptor);
								string cno = CastUtil::lexical_cast<string>(o);
								childNo = o+1;
								break;
							}
						}
						close(sp[tcn][0]);
						close(sp[tcn][1]);
						logger << "Process got killed" << flush;
						pid_t pid = createChildProcess(serverRootDirectory,sp[tcn],sockfd);
						pds[tcn] = pid;
						stringstream ss;
						string filename;
						ss << serverRootDirectory;
						ss << pid;
						ss >> filename;
						filename.append(".cntrl");
						files[tcn] = filename;
						logger << "created a new Process" << flush;
						logger.info("Process got killed hence created a new Process\n");
					}
					cntrlfile.close();
				}
				else
				{
					fcntl(descriptor, F_SETFL,O_SYNC);
					if(isThreadprq)
					{
						ServiceTask *task = new ServiceTask(descriptor,serverRootDirectory,&params,
									isSSLEnabled, ctx, sSLHandler, configurationData, dlib);
						Thread pthread(&service, task);
						pthread.execute();
					}
					else
					{
						ServiceTask *task = new ServiceTask(descriptor,serverRootDirectory,&params,
								isSSLEnabled, ctx, sSLHandler, configurationData, dlib);
						pool->execute(*task);
					}
				}
			}
		}
	}
	return 0;
}

