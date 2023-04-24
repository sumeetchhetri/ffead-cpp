/*
	Copyright 2009-2020, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
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
 * ServerInitUtil.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "ServerInitUtil.h"

Logger ServerInitUtil::loggerIB;
cb_into_pv PicoVWriter::cb;
cb_into_pv_for_date PicoVWriter::cdt;
moodycamel::ConcurrentQueue<PicoVWriter*> PicoVWriter::toBeClosedConns;

void ServerInitUtil::closeConnection(void* pcwr) {
	PicoVWriter* si = (PicoVWriter*)pcwr;
	si->cqat = Timer::getTimestamp();
	PicoVWriter::toBeClosedConns.enqueue(si);
}

void ServerInitUtil::closeConnections() {
	PicoVWriter* si;
	Timer t;
	t.start();
	while(PicoVWriter::toBeClosedConns.try_dequeue(si)) {
		if((Timer::getTimestamp()-si->cqat)>10 && si->useCounter==0) {
			delete si;
		} else {
			PicoVWriter::toBeClosedConns.enqueue(si);
		}
		if(t.elapsedMilliSeconds()>900) {
			PicoVWriter::cdt();
			t.start();
		}
	}
}

void ServerInitUtil::bootstrapIB(std::string serverRootDirectory, SERVER_BACKEND type) {
	if(type==V_PICO) {
		Writer::isPicoEvAsyncBackendMode = true;
	}
	bootstrap(serverRootDirectory, loggerIB, type);
}

void ServerInitUtil::bootstrap(std::string serverRootDirectory, Logger& logger, SERVER_BACKEND type) {
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

	std::string servd = serverRootDirectory;
	std::string logp = respath+"/logging.xml";
	LoggerFactory::init(logp, serverRootDirectory, "", StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true");

	logger = LoggerFactory::getLogger("MOD_FFEADCPP");

	logger << "FFEAD in bootstrap " << serverRootDirectory << std::endl;

	bool isCompileEnabled = false;
	std::string compileEnabled = srprps["DEV_MODE"];
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
			sessionTimeout = CastUtil::toLong(srprps["SESS_TIME_OUT"]);
		} catch(const std::exception& e) {
			logger << "Invalid session timeout value defined, defaulting to 1hour/3600sec" << std::endl;
		}
	}

	ConfigurationData::getInstance();

#ifdef HAVE_SSLINC
	SSLHandler::setIsSSL(false);
#endif

	strVec webdirs,webdirs1,pubfiles;
	//ConfigurationHandler::listi(webpath,"/",true,webdirs,false);
	CommonUtils::listFiles(webdirs, webpath, "/");
	//ConfigurationHandler::listi(webpath,"/",false,webdirs1,false);
	CommonUtils::listFiles(webdirs1, webpath, "/", false);

	CommonUtils::loadMimeTypes(respath+"mime-types.prop");
	CommonUtils::loadLocales(respath+"locale.prop");

	RegexUtil::replace(serverRootDirectory,"[/]+","/");
	RegexUtil::replace(webpath,"[/]+","/");

	CoreServerProperties csp(serverRootDirectory, respath, webpath, srprps, sessionTimeout, sessatserv);
	ConfigurationData::getInstance()->setCoreServerProperties(csp);

	bool enableCors = StringUtil::toLowerCopy(srprps["ENABLE_CRS"])=="true";
	bool enableSecurity = StringUtil::toLowerCopy(srprps["ENABLE_SEC"])=="true";
	bool enableFilters = StringUtil::toLowerCopy(srprps["ENABLE_FLT"])=="true";
	bool enableControllers = StringUtil::toLowerCopy(srprps["ENABLE_CNT"])=="true";
	bool enableContMpg = StringUtil::toLowerCopy(srprps["ENABLE_CNT_MPG"])=="true";
	bool enableContPath = StringUtil::toLowerCopy(srprps["ENABLE_CNT_PTH"])=="true";
	bool enableContExt = StringUtil::toLowerCopy(srprps["ENABLE_CNT_EXT"])=="true";
	bool enableContRst = StringUtil::toLowerCopy(srprps["ENABLE_CNT_RST"])=="true";
	bool enableExtra = StringUtil::toLowerCopy(srprps["ENABLE_EXT"])=="true";
	bool enableScripts = StringUtil::toLowerCopy(srprps["ENABLE_SCR"])=="true";
	bool enableSoap = StringUtil::toLowerCopy(srprps["ENABLE_SWS"])=="true";
	bool enableLogging = StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true";
	bool enableExtControllers = StringUtil::toLowerCopy(srprps["ENABLE_EXT_CNT"])=="true";
	bool enableJobs = StringUtil::toLowerCopy(srprps["ENABLE_JOBS"])=="true";
	bool enableStaticResponses = StringUtil::toLowerCopy(srprps["ENABLE_STATIC_RESP"])=="true";
	ConfigurationData::enableFeatures(enableCors, enableSecurity, enableFilters, enableControllers,
			enableContMpg, enableContPath, enableContExt,enableContRst, enableExtra, enableScripts,
			enableSoap, enableLogging, enableExtControllers, enableJobs, enableStaticResponses);

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

	logger << INTER_LIB_FILE << std::endl;

	bool libpresent = true;
	void *dlibtemp = dlopen(INTER_LIB_FILE, RTLD_NOW);
	//logger << std::endl <<dlibtemp << std::endl;
	if(dlibtemp==NULL)
	{
		libpresent = false;
		logger << dlerror() << std::endl;
		logger.info("Could not load Library");
	}
	else
		dlclose(dlibtemp);

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
			logger << "Invalid Build Type specified, only autotools, cmake, xmake, meson, scons, shellb, bazel and buck2 supported...\n" << std::endl;
#endif
			std::string output = ScriptHandler::execute(compres, true);
			logger << "Intermediate code generation task\n\n" << std::endl;
			logger << output << std::endl;
		}

		void* checkdlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
		void* checkddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
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

	/*#ifdef INC_DSTC
	bool distocache = false;
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

	logger << ("Initializing WSDL files....") << std::endl;
	ConfigurationHandler::initializeWsdls();
	logger << ("Initializing WSDL files done....") << std::endl;

	void* dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	//logger << std::endl <<dlib << std::endl;
	if(dlib==NULL)
	{
		logger << dlerror() << std::endl;
		logger.info("Could not load Library");
		exit(0);
	}
	else
	{
		logger.info("Library loaded successfully");
		dlclose(dlib);
	}

	void* ddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
	//logger << std::endl <<dlib << std::endl;
	if(ddlib==NULL)
	{
		logger << dlerror() << std::endl;
		logger.info("Could not load dynamic Library");
		exit(0);
	}
	else
	{
		logger.info("Dynamic Library loaded successfully");
		dlclose(ddlib);
	}

	ddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
	//logger << std::endl <<dlib << std::endl;
	if(ddlib==NULL)
	{
		logger << dlerror() << std::endl;
		logger.info("Could not load dynamic Library");
		exit(0);
	}
	else
	{
		logger.info("Second Dynamic Library loaded successfully");
		dlclose(ddlib);
	}

	ConfigurationData::getInstance()->serverType = type;
}

void ServerInitUtil::initIB() {
	init(loggerIB);
}

void ServerInitUtil::initIB(cb_reg_ext_fd_pv pvregfd, cb_into_pv cb, cb_into_pv_for_date cdt) {
	Writer::pvregfd = pvregfd;
	PicoVWriter::cb = cb;
	PicoVWriter::cdt = cdt;
	init(loggerIB);
}

void ServerInitUtil::init(Logger& logger) {
	logger << "FFEAD in init" << std::endl;
	logger << "Initializing ffead-cpp....." << std::endl;
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

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	logger << ("Initializing ffeadContext....") << std::endl;
	ConfigurationData::getInstance()->initializeAllSingletonBeans();
	GenericObject::init(ConfigurationData::getReflector());
	logger << ("Initializing ffeadContext done....") << std::endl;

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

#ifdef INC_JOBS
	if(ConfigurationData::getInstance()->isJobsEnabled()) {
		JobScheduler::start();
	}
#endif

	bool isLazyHeaderParsing = StringUtil::toLowerCopy(ConfigurationData::getInstance()->coreServerProperties.sprops["LAZY_HEADER_PARSE"])=="true";

	HTTPResponseStatus::init();

	HttpRequest::init(isLazyHeaderParsing);

	HttpResponse::init();

	MultipartContent::init();
}

void ServerInitUtil::cleanUp() {
#ifdef INC_SDORM
	ConfigurationHandler::destroyDataSources();
#endif

	ConfigurationHandler::destroyCaches();

	ConfigurationHandler::destroySearches();

	ConfigurationData::getInstance()->clearAllSingletonBeans();

#ifdef INC_JOBS
	if(ConfigurationData::getInstance()->isJobsEnabled()) {
		JobScheduler::stop();
	}
#endif

	RegexUtil::flushCache();

	HttpClient::cleanup();

	LoggerFactory::clear();

	CommonUtils::clearInstance();
}
