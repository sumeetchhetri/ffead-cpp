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

using namespace std;

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
	
	cerr << "FFEAD in ngx_http_ffeadcpp " << name->data << endl;

    return NGX_CONF_OK;
}
}
/*
 * Main handler function of the module.
 */

static ngx_http_read_input_data(ngx_http_request_t *r, string& data)
{
	if(NULL == r->request_body->temp_file)
    {
        /*
         * The entire request body is available in the list of buffers pointed by r->request_body->bufs.
         *
         * The list can have a maixmum of two buffers. One buffer contains the request body that was pre-read along with the request headers.
         * The other buffer contains the rest of the request body. The maximum size of the buffer is controlled by 'client_body_buffer_size' directive.
         * If the request body cannot be contained within these two buffers, the entire body  is writtin to the temp file and the buffers are cleared.
         */
        ngx_buf_t    *buf;
        ngx_chain_t  *cl;

        cl = r->request_body->bufs;
        for( ;NULL != cl; cl = cl->next )
        {
            buf = cl->buf;
			data.append((const char*)buf->pos, buf->last-buf->pos);
        }
    }
    else
    {
        /**
         * The entire request body is available in the temporary file.
         *
         */
        size_t ret;
        size_t offset = 0;
        unsigned char buff[4096];

        while(  (ret = ngx_read_file(&r->request_body->temp_file->file, buff, 4096, offset)) > 0)
        {
            data.append((const char*)buff, ret);
			offset = offset + ret;
        }
    }
}

static ngx_int_t ngx_http_ffeadcpp_module_handler_post_read(ngx_http_request_t *r);
static ngx_int_t ngx_http_ffeadcpp_module_handler(ngx_http_request_t *r)
{
    ngx_int_t rc = ngx_http_read_client_request_body(r, ngx_http_ffeadcpp_module_handler_post_read);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        return rc;
    }

    return NGX_DONE;
}

ngx_int_t set_custom_header_in_headers_out(ngx_http_request_t *r, const string& key, const string& value) {
    ngx_table_elt_t   *h;

    /*
    All we have to do is just to allocate the header...
    */
    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    /*
    ... setup the header key ...
    */
    h->key.len = key.length();
    h->key.data = key.c_str();

    /*
    ... and the value.
    */
    h->value.len = value.length();
    h->value.data = value.c_str();

    /*
    Mark the header as not deleted.
    */
    h->hash = 1;

    return NGX_OK;
}

static ngx_int_t ngx_http_ffeadcpp_module_handler_post_read(ngx_http_request_t *r)
{
	string cntpath = "";
	cntpath.append(ffeadcpp_path.data, ffeadcpp_path.len);
	cntpath += "/web/";
	HttpRequest* req = new HttpRequest(cntpath);

	cerr << "FFEAD in ngx_http_ffeadcpp_module_handler" << endl;
    ngx_int_t    rc;
    ngx_buf_t   *b;
    ngx_chain_t  out;

	ngx_list_part_t            *part;
    ngx_table_elt_t            *h;
    ngx_uint_t                  i;

    /*
    Get the first part of the list. There is usual only one part.
    */
    part = &r->headers_in.headers.part;
    h = part->elts;

    /*
    Headers list array may consist of more than one part,
    so loop through all of it
    */
    for (i = 0; /* void */ ; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) {
                /* The last part, search is done. */
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        req->buildRequest(string(h[i]->key.data, h[i]->key.len), string(h[i]->value.data, h[i]->value.len));
    }

    string content;
	ngx_http_read_input_data(r, content);
	cerr << "Input Request Data\n " << content << "\n======================\n" << endl;

	if(content!="")
	{
		req->buildRequest("Content", content.c_str());
	}
	req->buildRequest("URL", r->uri.data);
	req->buildRequest("Method", r->main->method_name.data);
	if(r->args.len > 0)
	{
		req->buildRequest("GetArguments", r->args.data);
	}
	req->buildRequest("HttpVersion", CastUtil::lexical_cast<string>(r->http_version));

	HttpResponse* respo = new HttpResponse;
	ServiceTask* task = new ServiceTask;
	task->handle(req, respo);
	delete task;

	if(respo->isDone()) {
		for (int var = 0; var < (int)respo->getCookies().size(); var++)
		{
			set_custom_header_in_headers_out(r, string("Set-Cookie"), respo->getCookies().at(var));
		}

		/* allocate a buffer for your response body */
		b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
		if (b == NULL) {
			return NGX_HTTP_INTERNAL_SERVER_ERROR;
		}

		/* attach this buffer to the buffer chain */
		out.buf = b;
		out.next = NULL;

		string data = respo->generateResponse(false);
		map<string,string>::const_iterator it;
		for(it=respo->getHeaders().begin();it!=respo->getHeaders().end();it++) {
			if(StringUtil::toLowerCopy(it->first)=="content-length") {
				r->headers_out.content_length_n = CastUtil::lexical_cast<int>(it->second);
			} else if(StringUtil::toLowerCopy(it->first)!="server") {
				set_custom_header_in_headers_out(r, it->first, it->second);
			}
		}
		/* adjust the pointers of the buffer */
		b->pos = data.c_str();
		b->last = data.length();
		b->memory = 1;    /* this buffer is in memory */
		b->last_buf = 1;  /* this is the last buffer in the buffer chain */

		/* set the status line */
		r->headers_out.status = CastUtil::lexical_cast<int>(respo->getStatusCode());

		/* send the headers of your response */
		rc = ngx_http_send_header(r);

		if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
	        return rc;
	    }

	    /* send the buffer chain of your response */
	    return ngx_http_output_filter(r, &out);
	} else {
		return NGX_DONE;
	}
}

/*
 * Function for the directive hello , it validates its value
 * and copies it to a static variable to be printed later
 */

static ngx_int_t init_module(ngx_cycle_t *cycle)
{
	string serverRootDirectory;
	serverRootDirectory.append(ffeadcpp_path.data, ffeadcpp_path.len);
	
	cerr << "FFEAD in init_module " << serverRootDirectory << endl;
	//if(serverRootDirectory=="") {
	//	serverRootDirectory = fconfig.defpath;
	//}

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

	string servd = serverRootDirectory;
	string logp = respath+"/logging.xml";
	LoggerFactory::init(logp, serverRootDirectory, "", StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true");

	logger = LoggerFactory::getLogger("MOD_FFEADCPP");

	bool isCompileEnabled = false;
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
    	logger << p.getMessage() << endl;
    }
    catch(const char* msg)
	{
		logger << msg << endl;
	}

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


#ifdef INC_JOBS
	JobScheduler::start();
#endif

	logger << ("Initializing WSDL files....") << endl;
	ConfigurationHandler::initializeWsdls();
	logger << ("Initializing WSDL files done....") << endl;

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
	return NGX_OK;
}

static ngx_int_t init_worker_process(ngx_cycle_t *cycle)
{
	cerr << "FFEAD in init_worker_process" << endl;
	cerr << "Initializing ffead-cpp....." << endl;
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

#ifdef INC_SDORM
	logger << ("Initializing DataSources....") << endl;
	ConfigurationHandler::initializeDataSources();
	logger << ("Initializing DataSources done....") << endl;
#endif

	logger << ("Initializing Caches....") << endl;
	ConfigurationHandler::initializeCaches();
	logger << ("Initializing Caches done....") << endl;

	//Load all the FFEADContext beans so that the same copy is shared by all process
	//We need singleton beans so only initialize singletons(controllers,authhandlers,formhandlers..)
	logger << ("Initializing ffeadContext....") << endl;
	ConfigurationData::getInstance()->initializeAllSingletonBeans();
	logger << ("Initializing ffeadContext done....") << endl;
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
