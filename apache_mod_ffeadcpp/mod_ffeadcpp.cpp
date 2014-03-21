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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "HttpRequest.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <queue>
#include <sys/uio.h>
#include <sys/un.h>
#include "ComponentHandler.h"
#include "AppContext.h"
#include "Logger.h"
#include "ConfigurationHandler.h"
#include "ServiceTask.h"
#include "PropFileReader.h"
#include "XmlParseException.h"
#include "DistoCacheHandler.h"
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

map<string,HttpSession> sessionMap;

int nor=0;
ofstream logfile;
static propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap;
static strVec dcpsss;
static propMap params;
static bool sessatserv,isCompileEnabled;
void* dlib;
static Logger logger;
static long sessionTimeout;
static string ip_address;

typedef map<string, RestFunction> resFuncMap;
static resFuncMap rstCntMap;
static map<string, string> handoffs;
static map<string, Element> formMap;
static map<string, vector<string> > filterMap;
static string resourcePath, serverRootDirectory;

extern "C" module AP_MODULE_DECLARE_DATA ffead_ccp_module;
static bool doneOnce;

typedef struct {
	char *path;
	char *defpath;
} ffead_ccp_module_config;

static void *create_modffeapcpp_config(apr_pool_t *p, server_rec *s)
{
	// This module's configuration structure.
	ffead_ccp_module_config *newcfg;

	// Allocate memory from the provided pool.
	newcfg = (ffead_ccp_module_config *) apr_pcalloc(p, sizeof(ffead_ccp_module_config));

	// Set the string to a default value.
	newcfg->path = "/";
	newcfg->defpath = "/";

	// Return the created configuration struct.
	return (void *) newcfg;
}

const char *set_modffeapcpp_path(cmd_parms *parms,
		void *mconfig, const char *arg)
{
	ffead_ccp_module_config *s_cfg = (ffead_ccp_module_config*)ap_get_module_config(
			parms->server->module_config, &ffead_ccp_module);
	s_cfg->path = (char *) arg;
	return NULL;
}

const char *set_modffeapcpp_defpath(cmd_parms *parms,
		void *mconfig, const char *arg)
{
	ffead_ccp_module_config *s_cfg = (ffead_ccp_module_config*)ap_get_module_config(
			parms->server->module_config, &ffead_ccp_module);
	s_cfg->defpath = (char *) arg;
	return NULL;
}

static const command_rec mod_ffeapcpp_cmds[] =
{
		AP_INIT_TAKE1(
				"FFEAD_CPP_PATH",
				set_modffeapcpp_path,
				NULL,
				RSRC_CONF,
				"FFEAD_CPP_PATH, the path to the ffead-server"
		),
		AP_INIT_TAKE1(
				"DocumentRoot",
				set_modffeapcpp_defpath,
				NULL,
				RSRC_CONF,
				"DocumentRoot"
		),
		{NULL}
};

string get_env_var(string key) {
	char* val;
	val = getenv(key.c_str());
	string retval = "";
	if (val != NULL) {
		retval = val;
	}
	return retval;
}

HttpResponse service(HttpRequest* req)
{
	logger << "service method " << endl;
	ServiceTask *task = new ServiceTask(0, serverRootDirectory);
	HttpResponse res = task->apacheRun(req);
	delete task;
	return res;
}

class MyReq
{
public:
	request_rec* r;
	HttpRequest* htreq;
};

int iterate_func(void *req, const char *key, const char *value)
{
	//int stat;
	//char *line;
	MyReq *requ = (MyReq*)req;
	//request_rec *r = (request_rec *)requ->r;
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
	apr_table_get((const apr_table_t *)r->headers_in, "Accept");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Charset");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Encoding");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Language");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Datetime");
	apr_table_get((const apr_table_t *)r->headers_in, "Access-Control-Request-Headers");
	apr_table_get((const apr_table_t *)r->headers_in, "Access-Control-Request-Method");
	apr_table_get((const apr_table_t *)r->headers_in, "Authorization");
	apr_table_get((const apr_table_t *)r->headers_in, "Cache-Control");
	apr_table_get((const apr_table_t *)r->headers_in, "Connection");
	apr_table_get((const apr_table_t *)r->headers_in, "Cookie");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-Length");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-MD5");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-Type");
	apr_table_get((const apr_table_t *)r->headers_in, "Date");
	apr_table_get((const apr_table_t *)r->headers_in, "Expect");
	apr_table_get((const apr_table_t *)r->headers_in, "From");
	apr_table_get((const apr_table_t *)r->headers_in, "Host");
	apr_table_get((const apr_table_t *)r->headers_in, "If-Match");
	apr_table_get((const apr_table_t *)r->headers_in, "If-Modified-Since");
	apr_table_get((const apr_table_t *)r->headers_in, "If-None-Match");
	apr_table_get((const apr_table_t *)r->headers_in, "If-Range");
	apr_table_get((const apr_table_t *)r->headers_in, "If-Unmodified-Since");
	apr_table_get((const apr_table_t *)r->headers_in, "Max-Forwards");
	apr_table_get((const apr_table_t *)r->headers_in, "Origin");
	apr_table_get((const apr_table_t *)r->headers_in, "Pragma");
	apr_table_get((const apr_table_t *)r->headers_in, "Proxy-Authorization");
	apr_table_get((const apr_table_t *)r->headers_in, "Range");
	apr_table_get((const apr_table_t *)r->headers_in, "Referer");
	apr_table_get((const apr_table_t *)r->headers_in, "TE");
	apr_table_get((const apr_table_t *)r->headers_in, "Upgrade");
	apr_table_get((const apr_table_t *)r->headers_in, "User-Agent");
	apr_table_get((const apr_table_t *)r->headers_in, "Via");
	apr_table_get((const apr_table_t *)r->headers_in, "Warning");
}

void signalSIGSEGV(int dummy)
{
	signal(SIGSEGV,signalSIGSEGV);
	/*string filename;
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
	fflush(stderr);*/
	cout << "Segmentation fault occurred for process" << getpid() << "\n" << flush;
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
	string port = CastUtil::lexical_cast<string>(r->server->port);
	if(ip_address=="")
	{
		ip_address = r->server->server_hostname;
		ip_address += ":";
		ip_address += port;
		ConfigurationData::getInstance()->ip_address = ip_address;
	}
	signal(SIGSEGV,signalSIGSEGV);
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

	string contret;
	if(content!="")
	{
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
	//hreq->setHttpVersion(r->protocol);

	HttpResponse respo = service(hreq);
	string h1 = respo.generateResponse(hreq->getMethod(), hreq);
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
	fprintf(stderr, h1.c_str());
	fflush(stderr);
	delete hreq;
	delete req;
	if(respo.getHeader(HttpResponse::ContentType)!="")
	{
		r->content_type = respo.getHeader(HttpResponse::ContentType).c_str();
	}
	if(h1!="")
	{
		ap_rprintf (r, h1.c_str());
		return OK;
	}
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

void one_time_init(ffead_ccp_module_config *s_cf)
{
	PropFileReader pread;
	serverRootDirectory.append(s_cf->path);
	if(serverRootDirectory=="") {
		serverRootDirectory = s_cf->defpath;
	}
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	resourcePath = respath;

	ConfigurationData::getInstance();

	string logf = serverRootDirectory+"/server.log";
	string logp = respath+"/log.prop";
	LoggerFactory::init(logp, serverRootDirectory);
	logger = LoggerFactory::getLogger("FfeadCppApacheModule");

	propMap srprps = pread.getProperties(respath+"server.prop");

	strVec webdirs,webdirs1,pubfiles;
	ConfigurationHandler::listi(webpath,"/",true,webdirs,false);
	ConfigurationHandler::listi(webpath,"/",false,webdirs1,false);
	ConfigurationHandler::listi(pubpath,".js",false,pubfiles,false);

	strVec cmpnames;
	try
	{
		ConfigurationHandler::handle(webdirs, webdirs1, incpath, rtdcfpath, serverRootDirectory, respath);
	}
	catch(const XmlParseException &p)
	{
		logger << p.getMessage() << endl;
	}
	catch(const char* msg)
	{
		logger << msg << endl;
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
	ConfigurationData::getInstance()->sessionTimeout = sessionTimeout;
	ConfigurationData::getInstance()->sessatserv = sessatserv;
	for(unsigned int var=0;var<pubfiles.size();var++)
	{
		ConfigurationData::getInstance()->pubMap[pubfiles.at(var)] = "true";
	}
	bool libpresent = true;
	void *dlibtemp = dlopen(INTER_LIB_FILE, RTLD_NOW);
	logger << endl <<dlibtemp << endl;
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

	ConfigurationData::getInstance()->props = pread.getProperties(respath+"mime-types.prop");
	ConfigurationData::getInstance()->lprops = pread.getProperties(respath+"locale.prop");

	string compres;
#if BUILT_WITH_CONFGURE == 1
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
#if BUILT_WITH_CONFGURE == 1
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
}

/*
 * This routine is called to perform any module-specific fixing of header
 * fields, et cetera.  It is invoked just before any content-handler.
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  If we return OK, the
 * server will still call any remaining modules with an handler for this
 * phase.
 */
static int mod_ffeadcp_post_config_hanlder(apr_pool_t *pconf, apr_pool_t *plog,
                          apr_pool_t *ptemp, server_rec *s)
{
	// Get the module configuration
	ffead_ccp_module_config *s_cfg = (ffead_ccp_module_config*)
			ap_get_module_config(s->module_config, &ffead_ccp_module);
	one_time_init(s_cfg);
	doneOnce = true;
    return OK;
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
static void mod_ffeadcpp_register_hooks (apr_pool_t *p)
{

	//ap_hook_insert_filter(myModuleInsertFilters, NULL, NULL, APR_HOOK_MIDDLE) ;

	//ap_register_input_filter(myInputFilterName, myInputFilter, NULL,AP_FTYPE_RESOURCE);


	/*ap_register_output_filter(myOutputFilterName, myOutputFilter,
		NULL, AP_FTYPE_RESOURCE) ;*/

	ap_hook_handler(mod_ffeadcpp_method_handler, NULL, NULL, APR_HOOK_LAST);
	ap_hook_post_config(mod_ffeadcp_post_config_hanlder, NULL, NULL, APR_HOOK_MIDDLE);
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
		create_modffeapcpp_config,
		NULL,
		mod_ffeapcpp_cmds,
		mod_ffeadcpp_register_hooks,      /* callback for registering hooks */
};
};

