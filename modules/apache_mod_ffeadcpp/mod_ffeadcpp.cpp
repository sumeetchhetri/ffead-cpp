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
#include "ServerInitUtil.h"
#define MAXEPOLLSIZE 100
#define BACKLOG 500
#define MAXBUFLEN 1024



static Logger logger;

extern "C" module AP_MODULE_DECLARE_DATA ffead_cpp_module;
static bool doneOnce = false;

typedef struct {
	const char* path;
	const char* defpath;
} ffead_cpp_module_config;

static ffead_cpp_module_config fconfig;

static void *create_modffeadcpp_config(apr_pool_t *p, server_rec *s)
{
	// This module's configuration structure.
	ffead_cpp_module_config *newcfg;

	// Allocate memory from the provided pool.
	newcfg = (ffead_cpp_module_config *) apr_pcalloc(p, sizeof(ffead_cpp_module_config));

	// Set the std::string to a default value.
	newcfg->path = "/";
	newcfg->defpath = "/";

	// Return the created configuration struct.
	return (void *) newcfg;
}

const char *set_modffeadcpp_path(cmd_parms *parms, void *mconfig, const char *arg)
{
	//ffead_cpp_module_config *s_cfg = (ffead_cpp_module_config*)ap_get_module_config(
	//		parms->server->module_config, &ffead_cpp_module);
	std::cout << "path = " << arg << std::endl;
	fconfig.path = arg;
	return NULL;
}

const char *set_modffeadcpp_defpath(cmd_parms *parms, void *mconfig, const char *arg)
{
	//ffead_cpp_module_config *s_cfg = (ffead_cpp_module_config*)ap_get_module_config(
	//		parms->server->module_config, &ffead_cpp_module);
	std::cout << "defpath = " << arg << std::endl;
	fconfig.defpath = arg;
	return NULL;
}

static const command_rec mod_ffeadcpp_cmds[] =
{
		AP_INIT_TAKE1(
				"FFEAD_CPP_PATH",
				(cmd_func)set_modffeadcpp_path,
				NULL,
				RSRC_CONF,
				"FFEAD_CPP_PATH, the path to the ffead-server"
		),
		AP_INIT_TAKE1(
				"DocumentRoot",
				(cmd_func)set_modffeadcpp_defpath,
				NULL,
				RSRC_CONF,
				"DocumentRoot"
		),
		{NULL}
};

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
	return apr_bucket_file_create(file, 0, finfo.size, r->pool, r->connection->bucket_alloc) ;
}

static bool ignoreHeader(const std::string& hdr)
{
	std::string hdr1 = StringUtil::toLowerCopy(hdr);
	if(hdr1==StringUtil::toLowerCopy(HttpResponse::Server)
		|| hdr1==StringUtil::toLowerCopy(HttpResponse::DateHeader)
		|| hdr1==StringUtil::toLowerCopy(HttpResponse::AcceptRanges)
		|| hdr1==StringUtil::toLowerCopy(HttpResponse::ContentType)
		|| hdr1==StringUtil::toLowerCopy(HttpResponse::ContentLength))
	{
		return true;
	}
	return false;
}

static int mod_ffeadcpp_method_handler (request_rec *r)
{
	std::string content;
	apr_bucket_brigade *bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
	for ( ; ; ) {
		apr_bucket* b ;
		bool done = false;
		ap_get_brigade(r->input_filters, bb, AP_MODE_READBYTES, APR_BLOCK_READ, HUGE_STRING_LEN);
		for ( b = APR_BRIGADE_FIRST(bb);b != APR_BRIGADE_SENTINEL(bb);b = APR_BUCKET_NEXT(b) )
		{
			size_t bytes ;
			const char* buf = "\0";
			if ( APR_BUCKET_IS_EOS(b) )
			{
				done = true;
			}
			else if (apr_bucket_read(b, &buf, &bytes, APR_BLOCK_READ)== APR_SUCCESS )
			{
				content.append(buf, bytes);
			}
		}

		if (done)
		  break;
		else
		  apr_brigade_cleanup(bb) ;
	}
	apr_brigade_destroy(bb) ;

	HttpRequest req((const char*)r->uri, strlen(r->uri), (const char*)r->args, strlen(r->args),
					(const char*)r->method, strlen(r->method), std::move(content), r->proto_num);

	const apr_array_header_t* fields = apr_table_elts(r->headers_in);
	apr_table_entry_t* e = (apr_table_entry_t *) fields->elts;
	for(int i = 0; i < fields->nelts; i++) {
		req.addNginxApacheHeader((const char*)e[i].key, strlen(e[i].key), (const char*)e[i].val, strlen(e[i].val));
	}

	HttpResponse respo;
	ServiceTask task;
	task.handle(&req, &respo);

	for (int var = 0; var < (int)respo.getCookies().size(); var++)
	{
		apr_table_set(r->headers_out, "Set-Cookie", respo.getCookies().at(var).c_str());
	}

	if(respo.isDone()) {
		std::string& data = respo.generateNginxApacheResponse();
		RMap::const_iterator it;
		for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
			apr_table_set(r->headers_out, it->first.c_str(), it->second.c_str());
		}
		ap_set_content_length(r, data.length());

		char* ptr = apr_pstrdup(r->pool, respo.getHeader(HttpResponse::ContentType).c_str());
		ap_set_content_type(r, ptr);
		if(data.length()>0) {
			ap_set_content_length(r, data.length());
			unsigned int offset = 0;

			int remain_bytes = data.length();
			int bytes_send = 0;
			while (remain_bytes > 0)
			{
				bytes_send = ap_rwrite(&data[offset], remain_bytes, r);
				if (bytes_send <= 0)
				{
					return EIO;
				}
				remain_bytes -= bytes_send;
			}
		}
		r->status = respo.getCode();
		return OK;
		//ap_rprintf(r, data.c_str(), data.length());
	} else {
		apr_file_t *file;
		apr_finfo_t finfo;
		int rc, exists;
		rc = apr_stat(&finfo, req.getUrl().c_str(), APR_FINFO_MIN, r->pool);
		if (rc == APR_SUCCESS) {
			exists =
			(
				(finfo.filetype != APR_NOFILE) &&  !(finfo.filetype & APR_DIR)
			);
			if (!exists) {
				return HTTP_NOT_FOUND;
			}
		}
		else {
			return HTTP_FORBIDDEN;
		}

		std::string webPath = std::string(fconfig.path) + "/web";
		RegexUtil::replace(webPath,"[/]+","/");
		std::string acurl = req.getUrl();
		RegexUtil::replace(acurl,"[/]+","/");
		if(acurl.find(webPath)==0) {
			acurl = acurl.substr(webPath.length());
		}
		RegexUtil::replace(acurl,"[/]+","/");
		//logger << "static file will be processed by apache " << req->getUrl() << " " << acurl << std::endl;

		char* au = apr_pstrdup(r->pool, acurl.c_str());
		r->uri = au;
		r->finfo = finfo;
		char* fn = apr_pstrdup(r->pool, req.getUrl().c_str());
		r->filename = fn;
		apr_table_unset(r->headers_out, HttpResponse::Status.c_str());
		ap_set_content_type(r, CommonUtils::getMimeType(req.getExt()).c_str());

		return OK;
	}
}


//Every module must declare it's data structure as shown above. Since this module does not require any configuration most of the callback locations have been left blank, except for the last one - that one is invoked by the HTTPD core so that the module can declare other functions that should be invoked to handle various events (like an HTTP request).

/*
 * This function is a callback and it declares what
 * other functions should be called for request
 * processing and configuration requests. This
 * callback function declares the Handlers for
 * other events.
 */

void one_time_init()
{
	std::string serverRootDirectory;
	serverRootDirectory.append(fconfig.path);
	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::APACHE);
}

/*
 * This routine is called to perform any module-specific fixing of header
 * fields, et cetera.  It is invoked just before any content-handler.
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  If we return OK, the
 * server will still call any remaining modules with an handler for this
 * phase.
 */
static int mod_ffeadcp_post_config_hanlder(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
	/*void *data = NULL;
	const char *key = "dummy_post_config";

	// This code is used to prevent double initialization of the module during Apache startup
	apr_pool_userdata_get(&data, key, s->process->pool);
	if ( data == NULL ) {
	    apr_pool_userdata_set((const void *)1, key, apr_pool_cleanup_null, s->process->pool);
	    return OK;
	}*/

	// Get the module configuration
	//ffead_cpp_module_config *s_cfg = (ffead_cpp_module_config*)
	//		ap_get_module_config(s->module_config, &ffead_cpp_module);
	if(!doneOnce)
	{
		std::cout << "Configuring ffead-cpp....." << std::endl;
		one_time_init();
	}
	doneOnce = true;
    return OK;
}

static apr_status_t mod_ffeadcp_child_uninit(void* arg)
{
	ServerInitUtil::cleanUp();
	return OK;
}

static void mod_ffeadcp_child_init(apr_pool_t *p, server_rec *s)
{
	apr_pool_cleanup_register(p, NULL, mod_ffeadcp_child_uninit, apr_pool_cleanup_null);
	ServerInitUtil::init(logger);
}

static void mod_ffeadcpp_register_hooks (apr_pool_t *p)
{
	//ap_hook_insert_filter(myModuleInsertFilters, NULL, NULL, APR_HOOK_MIDDLE) ;
	//ap_register_input_filter(myInputFilterName, myInputFilter, NULL,AP_FTYPE_RESOURCE);
	/*ap_register_output_filter(myOutputFilterName, myOutputFilter, NULL, AP_FTYPE_RESOURCE) ;*/
	ap_hook_post_config(mod_ffeadcp_post_config_hanlder, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(mod_ffeadcp_child_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(mod_ffeadcpp_method_handler, NULL, NULL, APR_HOOK_MIDDLE);
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
 * name of this structure ('ffead_cpp_module') is important - it
 * must match the name of the module.  This structure is the
 * only "glue" between the httpd core and the module.
 */
	module AP_MODULE_DECLARE_DATA ffead_cpp_module =
	{
			// Only one callback function is provided.  Real
			// modules will need to declare callback functions for
			// server/directory configuration, configuration merging
			// and other tasks.
			STANDARD20_MODULE_STUFF,
			NULL,
			NULL,
			NULL,//create_modffeadcpp_config,
			NULL,
			mod_ffeadcpp_cmds,
			mod_ffeadcpp_register_hooks,      /* callback for registering hooks */
	};
};

