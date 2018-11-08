extern "C" {
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
}
#include "HttpRequest.h"
#include "PropFileReader.h"
#include "cstdlib"
#include "dlfcn.h"
#include "WsUtil.h"
#include "sstream"
#include "StringUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <queue>
#include "ComponentHandler.h"
#include "AppContext.h"
#include "Logger.h"
#include "ConfigurationHandler.h"
#include "ServiceTask.h"
#include "PropFileReader.h"
#include "XmlParseException.h"
#include "MessageHandler.h"
#include "MethodInvoc.h"
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



static Logger logger;
static bool doneOnce = false;

extern "C" {
static char *ngx_http_ffeadcpp(ngx_conf_t *cf, void *post, void *data);
static ngx_conf_post_handler_pt ngx_http_ffeadcpp_module_p = ngx_http_ffeadcpp;
static ngx_int_t exit_process(ngx_cycle_t *cycle);
static ngx_int_t init_worker_process(ngx_cycle_t *cycle);
static ngx_int_t init_module(ngx_cycle_t *cycle);
static ngx_int_t ngx_http_ffeadcpp_module_handler(ngx_http_request_t *r);
static ngx_str_t ffeadcpp_path;
/*
 * The structure will holds the value of the
 * module directive hello
 */
typedef struct {
    ngx_str_t   name;
} ngx_http_ffeadcpp_module_loc_conf_t;

/* The function which initializes memory for the module configuration structure
 */
static void *
ngx_http_ffeadcpp_module_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_ffeadcpp_module_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_ffeadcpp_module_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

/*
 * The command array or array, which holds one subarray for each module
 * directive along with a function which validates the value of the
 * directive and also initializes the main handler of this module
 */
static ngx_command_t ngx_http_ffeadcpp_module_commands[] = {
    { ngx_string("ffeadcpp_path"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ffeadcpp_module_loc_conf_t, name),
      &ngx_http_ffeadcpp_module_p },

    ngx_null_command
};


/*
 * The module context has hooks , here we have a hook for creating
 * location configuration
 */
static ngx_http_module_t ngx_http_ffeadcpp_module_module_ctx = {
    NULL,                          /* preconfiguration */
    NULL,                          /* postconfiguration */

    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */

    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */

    ngx_http_ffeadcpp_module_create_loc_conf, /* create location configuration */
    NULL                           /* merge location configuration */
};


/*
 * The module which binds the context and commands
 *
 */
ngx_module_t ngx_http_ffeadcpp_module = {
    NGX_MODULE_V1,
    &ngx_http_ffeadcpp_module_module_ctx,    /* module context */
    ngx_http_ffeadcpp_module_commands,       /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
	&init_module,                  /* init module */
    &init_worker_process,          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    &exit_process,                /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

static char * ngx_http_ffeadcpp(ngx_conf_t *cf, void *post, void *data)
{
    ngx_http_core_loc_conf_t *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_ffeadcpp_module_handler;

    ngx_str_t  *name = data; // i.e., first field of ngx_http_ffeadcpp_module_loc_conf_t

    if (ngx_strcmp(name->data, "") == 0) {
        return NGX_CONF_ERROR;
    }
    ffeadcpp_path.data = name->data;
    ffeadcpp_path.len = ngx_strlen(ffeadcpp_path.data);
	
	std::cerr << "FFEAD in ngx_http_ffeadcpp " << name->data << std::endl;

    return NGX_CONF_OK;
}
}
/*
 * Main handler function of the module.
 */

static ngx_int_t ngx_http_ffeadcpp_module_handler(ngx_http_request_t *r)
{
	std::cerr << "FFEAD in ngx_http_ffeadcpp_module_handler" << std::endl;
    ngx_int_t    rc;
    ngx_buf_t   *b;
    ngx_chain_t  out;

    /* we response to 'GET' and 'HEAD' requests only */
    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    /* discard request body, since we don't need it here */
    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK) {
        return rc;
    }

    /* set the 'Content-type' header */
    r->headers_out.content_type_len = sizeof("text/html") - 1;
    r->headers_out.content_type.len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char *) "text/html";

    /* send the header only, if the request type is http 'HEAD' */
    if (r->method == NGX_HTTP_HEAD) {
        r->headers_out.status = NGX_HTTP_OK;
        r->headers_out.content_length_n = ffeadcpp_path.len;

        return ngx_http_send_header(r);
    }

    /* allocate a buffer for your response body */
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    /* attach this buffer to the buffer chain */
    out.buf = b;
    out.next = NULL;

    /* adjust the pointers of the buffer */
    b->pos = ffeadcpp_path.data;
    b->last = ffeadcpp_path.data + ffeadcpp_path.len;
    b->memory = 1;    /* this buffer is in memory */
    b->last_buf = 1;  /* this is the last buffer in the buffer chain */

    /* set the status line */
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = ffeadcpp_path.len;

    /* send the headers of your response */
    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    /* send the buffer chain of your response */
    return ngx_http_output_filter(r, &out);
}

/*
 * Function for the directive hello , it validates its value
 * and copies it to a static variable to be printed later
 */

static ngx_int_t init_module(ngx_cycle_t *cycle)
{
	std::string serverRootDirectory;
	serverRootDirectory.append(ffeadcpp_path.data, ffeadcpp_path.len);
	
	std::cerr << "FFEAD in init_module " << serverRootDirectory << std::endl;
	//if(serverRootDirectory=="") {
	//	serverRootDirectory = fconfig.defpath;
	//}

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

	std::string servd = serverRootDirectory;
	std::string logp = respath+"/logging.xml";
	LoggerFactory::init(logp, serverRootDirectory, "", StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true");

	logger = LoggerFactory::getLogger("MOD_FFEADCPP");

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
   			sessionTimeout = CastUtil::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch(const std::exception& e) {
			logger << "Invalid session timeout value defined, defaulting to 1hour/3600sec" << std::endl;
		}
   	}

	ConfigurationData::getInstance();
	SSLHandler::setIsSSL(false);

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
		logger << msg << std::endl;
	}

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
	if(checkdlib==NULL)
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
	}

	if(checkdlib==NULL)
	{
		logger << dlerror() << std::endl;
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
	} catch(const std::exception& e) {
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
	} catch(const std::exception& e) {
		logger << ("Invalid port specified for distocache") << std::endl;
	}

	if(!distocache) {
		logger << ("Session store is set to file store") << std::endl;
	}
#endif*/


#ifdef INC_JOBS
	JobScheduler::start();
#endif

	logger << ("Initializing WSDL files....") << std::endl;
	ConfigurationHandler::initializeWsdls();
	logger << ("Initializing WSDL files done....") << std::endl;

	void* dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	//logger << endl <<dlib << std::endl;
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
	//logger << endl <<dlib << std::endl;
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
	return NGX_OK;
}

static ngx_int_t init_worker_process(ngx_cycle_t *cycle)
{
	std::cerr << "FFEAD in init_worker_process" << std::endl;
	std::cerr << "Initializing ffead-cpp....." << std::endl;
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
	} catch(const std::exception& e) {
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
	} catch(const std::exception& e) {
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
	} catch(const std::exception& e) {
		logger << ("Method Invoker Services are disabled") << std::endl;
	}
#endif

#ifdef INC_SDORM
	logger << ("Initializing DataSources....") << std::endl;
	ConfigurationHandler::initializeDataSources();
	logger << ("Initializing DataSources done....") << std::endl;
#endif

	logger << ("Initializing Caches....") << std::endl;
	ConfigurationHandler::initializeCaches();
	logger << ("Initializing Caches done....") << std::endl;

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	logger << ("Initializing ffeadContext....") << std::endl;
	ConfigurationData::getInstance()->initializeAllSingletonBeans();
	logger << ("Initializing ffeadContext done....") << std::endl;
}

static ngx_int_t exit_process(ngx_cycle_t *cycle)
{
#ifdef INC_SDORM
	ConfigurationHandler::destroyDataSources();
#endif

	ConfigurationHandler::destroyCaches();

	ConfigurationData::getInstance()->clearAllSingletonBeans();
	return NGX_OK;
}

