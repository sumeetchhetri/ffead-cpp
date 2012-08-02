/*
	Copyright 2012, Sumeet Chhetri

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


#include "Cibernate.h"
#include "algorithm"
#include "AfcUtil.h"

#include "Controller.h"

#include "PropFileReader.h"
#include "TemplateEngine.h"
#include "DCPGenerator.h"
#include "Reflection.h"
#include "cstdlib"
#include "dlfcn.h"
#include "WsUtil.h"
#include "sstream"

#include "ClassInfo.h"
#include <boost/lexical_cast.hpp>
#include "View.h"
#include "XmlParser.h"
#include "TemplateHandler.h"
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
#include "DynamicView.h"
#include "HttpRequest.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include "ApplicationUtil.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <boost/thread/recursive_mutex.hpp>
#include <queue>
#include <sys/uio.h>
#include <sys/un.h>
#include "Cibernate.h"
#include "ComponentGen.h"
#include "ComponentHandler.h"
#include "Reflector.h"
#include "AppContext.h"
#include "Logger.h"
#include "Filter.h"
#include "FileAuthController.h"
#include "RestController.h"
#include "ConfigurationHandler.h"
#include "SSLHandler.h"
#include "ServiceTask.h"
#include "PropFileReader.h"
#include "XmlParseException.h"
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
typedef bool (*FunPtr1) (void *);
typedef ClassInfo (*FunPtr) ();
typedef string (*DCPPtr) ();
typedef void (*ReceiveTask1)(int);
typedef void* (*toVoidP) (string);

using namespace std;

map<string,HttpSession> sessionMap;
boost::mutex m_mutex;
boost::mutex p_mutex;
int nor=0;
ofstream logfile;
static propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap;
static strVec dcpsss;
static propMap params;
static bool isSSLEnabled,isThreadprq,processforcekilled,processgendone,sessatserv,isCompileEnabled;
void* dlib;
static Logger logger;
ConfigurationData configurationData;
SSLHandler sSLHandler;
static long sessionTimeout;
static string ip_address;

typedef map<string, RestFunction> resFuncMap;
static resFuncMap rstCntMap;
static map<string, string> handoffs;
static map<string, Element> formMap;
static map<string, vector<string> > filterMap;
static string resourcePath, serverRootDirectory;;

extern "C" module AP_MODULE_DECLARE_DATA ffead_ccp_module;

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
	ServiceTask *task = new ServiceTask(0,serverRootDirectory,NULL,
			isSSLEnabled, NULL, sSLHandler, configurationData, dlib);
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
    int stat;
    char *line;
    MyReq *requ = (MyReq*)req;
    request_rec *r = (request_rec *)requ->r;
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
	//string host = "" + apr_table_get(r->headers_in, "Host");
	apr_table_get((const apr_table_t *)r->headers_in, "User-Agent");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Language");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Encoding");
	apr_table_get((const apr_table_t *)r->headers_in, "Accept-Charset");
	apr_table_get((const apr_table_t *)r->headers_in, "Keep-Alive");
	apr_table_get((const apr_table_t *)r->headers_in, "Connection");
	apr_table_get((const apr_table_t *)r->headers_in, "Cache-Control");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-Type");
	apr_table_get((const apr_table_t *)r->headers_in, "Content-Length");
	apr_table_get((const apr_table_t *)r->headers_in, "Referer");
	apr_table_get((const apr_table_t *)r->headers_in, "Pragma");
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
	string port = boost::lexical_cast<string>(r->server->port);
	if(ip_address=="")
	{
		ip_address = r->server->server_hostname;
		ip_address += ":";
		ip_address += port;
		configurationData.ip_address = ip_address;
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
	hreq->setHttpVersion(r->protocol);

	HttpResponse respo = service(hreq);
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
	fprintf(stderr, respo.generateResponse().c_str());
	fflush(stderr);

    if(respo.getContent_type()!="")
	{
    	r->content_type = respo.getContent_type().c_str();
	}
    if(respo.getContent_str()!="")
    {
    	ap_rprintf (r, respo.getContent_str().c_str());
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

static void one_time_init()
{
	PropFileReader pread;
	serverRootDirectory = get_env_var("FFEAD_CPP_PATH");
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	resourcePath = respath;

	string logf = serverRootDirectory+"/server.log";
	string logp = respath+"/log.prop";
	Logger::init(logp);
	logger = Logger::getLogger("FfeadCppApacheModule");

	propMap srprps = pread.getProperties(respath+"server.prop");

	strVec webdirs,webdirs1,pubfiles;
	ConfigurationHandler::listi(webpath,"/",true,webdirs);
	ConfigurationHandler::listi(webpath,"/",false,webdirs1);
	ConfigurationHandler::listi(pubpath,".js",false,pubfiles);

	strVec cmpnames;
	try
	{
		configurationData = ConfigurationHandler::handle(webdirs, webdirs1, incpath, rtdcfpath, pubpath, respath, isSSLEnabled);
	}
	catch(XmlParseException *p)
	{
		logger << p->getMessage() << endl;
	}
	if(srprps["SESS_STATE"]=="server")
		sessatserv = true;
	if(srprps["SESS_TIME_OUT"]!="")
	{
		try {
			sessionTimeout = boost::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch (...) {
			sessionTimeout = 3600;
			logger << "\nInvalid session timeout value defined, defaulting to 1hour/3600sec";
		}
	}
	configurationData.sessionTimeout = sessionTimeout;
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
	string compres = respath+"run.sh";
	if(!libpresent)
	{
		vector<string> argss;
		string output;
		bool passed = ScriptHandler::execute(compres, argss, output);
		logger << "Intermediate code generation pass = " << passed << endl;
	}

	dlib = dlopen(Constants::INTER_LIB_FILE.c_str(), RTLD_NOW|RTLD_GLOBAL);
	if(dlib==NULL)
	{
		cout << dlerror() << endl;
		logger.info("Could not load Library");
	}
	else
		logger.info("Library loaded successfully");
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
static bool doneOnce;
static void mod_ffeadcpp_register_hooks (apr_pool_t *p)
{

	//ap_hook_insert_filter(myModuleInsertFilters, NULL, NULL, APR_HOOK_MIDDLE) ;

	//ap_register_input_filter(myInputFilterName, myInputFilter, NULL,AP_FTYPE_RESOURCE);


	/*ap_register_output_filter(myOutputFilterName, myOutputFilter,
		NULL, AP_FTYPE_RESOURCE) ;*/

	ap_hook_handler(mod_ffeadcpp_method_handler, NULL, NULL, APR_HOOK_LAST);
	if(doneOnce==false)
	{
		fprintf(stderr, "init once and once only.........");
		fflush(stderr);
		one_time_init();
	}
	doneOnce = true;
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
	  NULL,
	  NULL,
	  NULL,
	  mod_ffeadcpp_register_hooks,      /* callback for registering hooks */
	};
};

