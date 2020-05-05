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
#include <ls.h>
#include <lsdef.h>
#include <lsr/ls_loopbuf.h>
#include <lsr/ls_xpool.h>
#include <lsr/ls_strtool.h>
#include <lsr/ls_confparser.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <stdint.h>
#include "stdlib.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#include "ServerInitUtil.h"


static Logger logger;

#define     MNAME       mod_ffeadcpp
extern "C" lsi_module_t MNAME;
#define     VERSION     "v1.3"

int write_log(const char *sHookName)
{
    g_api->log(NULL, LSI_LOG_DEBUG, "[Module:mod_ffeadcpp] launch point %s\n", sHookName);
    return 0;
}

/*
 * Parameter setup starts here
 */
//Setup the below array to let web server know these params
lsi_config_key_t myParam[] =
{
    {"param1",  0, 0},
    {"param2",  0, 0},
    {"param3",  0, 0},
    {NULL,  0, 0}   //The last position must have a NULL to indicate end of the array
};

//return 0 for correctly parsing
static int parseList(module_param_info_t *param, std::map<std::string, std::string> *pConfig)
{
    g_api->log(NULL,  LSI_LOG_INFO, "[Module:mod_ffeadcpp] parseList start\n");
    if (param->val_len <= 0)
        return 0;

    int *pParam;
    ls_confparser_t confparser;
    ls_confparser(&confparser);
    ls_objarray_t *pList = ls_confparser_line(&confparser, param->val, param->val + param->val_len);

    int count = ls_objarray_getsize(pList);
    assert(count > 0);

    //Comment: case param2, maxParamNum is 2, the line should be param2 [21 [22]],
    unsigned long maxParamNum = param->key_index + 1;
    if (maxParamNum > 3)
        maxParamNum = 1;

    long val;
    int i;
    ls_str_t* p1;

    for (i = 0; i < count && i < maxParamNum; ++i)
    {
        switch(param->key_index)
        {
        case 0:
        	p1 = (ls_str_t *)ls_objarray_getobj(pList, i);
        	(*pConfig)["param1"] = std::string(p1->ptr, p1->len);
            break;
        case 1:
        	p1 = (ls_str_t *)ls_objarray_getobj(pList, i);
        	(*pConfig)["param2"] = std::string(p1->ptr, p1->len);
            break;
        case 2:
        	p1 = (ls_str_t *)ls_objarray_getobj(pList, i);
        	(*pConfig)["param2"] = std::string(p1->ptr, p1->len);
            break;
        }
    }

    ls_confparser_d(&confparser);
    g_api->log(NULL,  LSI_LOG_INFO, "[Module:mod_ffeadcpp] parseList param1 %s end\n", pConfig->find("param1")->second.c_str());
    g_api->log(NULL,  LSI_LOG_INFO, "[Module:mod_ffeadcpp] parseList end\n");
    return 0;
}

static void *parseConfig(module_param_info_t *param, int param_count, void *_initial_config, int level, const char *name)
{
    g_api->log(NULL,  LSI_LOG_INFO, "[Module:mod_ffeadcpp] parseConfig start\n");
    int i;
    std::map<std::string, std::string> *pConfig = (std::map<std::string, std::string> *)_initial_config;
    if (!_initial_config) {
    	pConfig = new std::map<std::string, std::string>();
    }

    for (i=0 ;i<param_count; ++i)
    {
    	parseList(&param[i], pConfig);
    }
    g_api->log(NULL,  LSI_LOG_INFO, "[Module:mod_ffeadcpp] parseConfig end\n");
    return (void *)pConfig;
}

static void freeConfig(void *_config)
{
    free(_config);
}
/*
 * Parameter setup ends here
 */

/*
 * Server bootstrap and init functions start here
 */
int mainInit(lsi_param_t *rec) {
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] mainInit start\n");
	std::map<std::string, std::string>* paramMap = (std::map<std::string, std::string>*) g_api->get_config(NULL, &MNAME);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] mainInit param1 %s\n", paramMap->find("param1")->second.c_str());
	std::string serverRootDirectory = paramMap->find("param1")->second;
	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::OPENLITESPEED);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] mainInit finalizing end\n");
	return write_log("LSI_HKPT_MAIN_INITED");
}

int mainPrefork(lsi_param_t *rec) {
	return write_log("LSI_HKPT_MAIN_PREFORK");
}

int mainPostfork(lsi_param_t *rec) {
	return write_log("LSI_HKPT_MAIN_POSTFORK");
}

int workerInit(lsi_param_t *rec) {
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] workerInit start\n");
	ServerInitUtil::init(logger);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] workerInit end\n");
	return write_log("LSI_HKPT_WORKER_INIT");
}

int workerExit(lsi_param_t *rec) {
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] workerExit start\n");
	ServerInitUtil::cleanUp();
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] workerExit end\n");
	return write_log("LSI_HKPT_WORKER_ATEXIT");
}

int mainExit(lsi_param_t *rec) {
	return write_log("LSI_HKPT_MAIN_ATEXIT");
}
/*
 * Server bootstrap and init functions end here
 */




/*
 * Request processing start here
 */
#define MAX_BLOCK_BUFSIZE   8192

static int httpinit(lsi_param_t *param)
{
    std::string *mydata = new std::string;
    g_api->set_module_data(param->session, &MNAME, LSI_DATA_HTTP, (void *)mydata);
    return 0;
}

static int httpreqread(lsi_param_t *param)
{
    char tmpBuf[MAX_BLOCK_BUFSIZE];
    int len;

    std::string* mydata = (std::string *)g_api->get_module_data(param->session, &MNAME, LSI_DATA_HTTP);
    if (mydata == NULL)
        return LS_FAIL;

    while ((len = g_api->stream_read_next(param, tmpBuf, MAX_BLOCK_BUFSIZE)) > 0)
    {
        g_api->log(NULL, LSI_LOG_DEBUG,
                   "#### waitfullreqbody httpreqread, inLn = %d\n", len);
        mydata->append(tmpBuf, len);
    }

    return mydata->length();
}

static void reqhdrcb(int key_id, const char * key, int key_len, const char * value, int val_len, void * arg)
{
	HttpRequest* req = (HttpRequest*)arg;
	req->addNginxApacheHeader(key, (size_t)key_len, value, (size_t)val_len);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] reqhdrcb key %s\n", std::string(key, key_len).c_str());
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] reqhdrcb value %s\n", std::string(value, val_len).c_str());
}

static void reqprmcb(int key_id, const char * key, int key_len, const char * value, int val_len, void * arg)
{
	std::string* req = (std::string*)arg;
	if(req->length()==0) {
		req->append(std::string(key, (size_t)key_len));
		req->append("=");
		req->append(std::string(value, (size_t)val_len));
	} else {
		req->append("&");
		req->append(std::string(key, (size_t)key_len));
		req->append("=");
		req->append(std::string(value, (size_t)val_len));
	}
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] reqprmcb key %s\n", std::string(key, key_len).c_str());
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] reqprmcb value %s\n", std::string(value, val_len).c_str());
}

static int on_read(const lsi_session_t *session)
{
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read start\n");
	std::string content;
	char buf[MAX_BLOCK_BUFSIZE];
	int ret;
	while ((ret = g_api->read_req_body(session, buf, MAX_BLOCK_BUFSIZE)) > 0)
	{
		content.append(buf, ret);
	}

	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read content_length %d\n", content.length());

	std::string methods;
    LSI_REQ_METHOD method = g_api->get_req_method(session);
    switch(method) {
    case LSI_METHOD_OPTIONS:
    	methods = "OPTION";break;
    case LSI_METHOD_GET:
    	methods = "GET";break;
    case LSI_METHOD_HEAD:
    	methods = "HEAD";break;
    case LSI_METHOD_POST:
    	methods = "POST";break;
    case LSI_METHOD_PUT:
    	methods = "PUT";break;
    case LSI_METHOD_DELETE:
    	methods = "DELETE";break;
    case LSI_METHOD_TRACE:
    	methods = "TRACE";break;
    }
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read http_method %s\n", methods.c_str());

    const char *uri;
    int ulen;
    uri = g_api->get_req_uri(session, &ulen);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read URL %s\n", std::string(uri, ulen).c_str());

    //std::string queryv;
	//const char * filt = NULL;
	//g_api->foreach(session, LSI_DATA_REQ_VAR, filt, reqprmcb, &queryv);

	const char *qstr;
	int qlen;
	qstr = g_api->get_req_query_string(session, &qlen);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read URL %s\n", std::string(qstr, qlen).c_str());

    HttpRequest* req = new HttpRequest(uri, (size_t)ulen, qstr, qlen, (const char*)methods.c_str(), methods.length(), std::move(content), 1100);

    const char *filt = NULL;
	g_api->foreach(session, LSI_DATA_REQ_HEADER, filt, reqhdrcb, &req);

	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read ffead-cpp handle start\n");
    HttpResponse respo;
	ServiceTask task;
	task.handle(req, &respo);
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read ffead-cpp handle end\n");

	std::string furl = req->getUrl();
	if(respo.isDone()) {
		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read ffead-cpp done received from framework\n");

		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read set cookies start\n");
		for (int var = 0; var < (int)respo.getCookies().size(); var++)
		{
			g_api->set_resp_header(session, LSI_RSPHDR_SET_COOKIE, NULL, 0, respo.getCookies().at(var).c_str(), respo.getCookies().at(var).length(), LSI_HEADEROP_SET);
		}
		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read set cookies end\n");

		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read set headers start\n");
		std::string& data = respo.generateNginxApacheResponse();
		std::map<std::string,std::string>::const_iterator it;
		for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
			g_api->set_resp_header(session, LSI_RSPHDR_UNKNOWN, it->first.c_str(), it->first.length(), it->first.c_str(), it->first.length(), LSI_HEADEROP_SET);
		}
		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read set headers end\n");

		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read set code/response start\n");
		/* set the status line */
		g_api->set_status_code(session, respo.getCode());

		if(data.length()>0)
		{
			g_api->append_resp_body(session, data.c_str(), data.length());
		}
		g_api->end_resp(session);
		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read set code/response end\n");
		delete req;
	} else {
		delete req;
		g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read static file request %s\n", furl.c_str());
		struct stat sb;
		off_t off = 0;
		off_t sz = -1; //-1 set to send all data
		if (stat(furl.c_str(), &sb) == 0)
		{
			g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read static file found\n");
			sz = sb.st_size;
		}
		else {
			g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read static file not found\n");
			g_api->set_status_code(session, 404);
			g_api->end_resp(session);
			return LS_FAIL;
		}

		if (g_api->send_file(session, furl.c_str(), off, sz) < 0)
		{
			g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read static file sendfile issues\n");
			g_api->set_status_code(session, 500);
			g_api->end_resp(session);
			return LS_FAIL;
		}

		// g_api->append_resp_body(session, txtlast, sizeof(txtlast) - 1);
		g_api->end_resp(session);
	}

	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] on_read end\n");

    return 0;
}

static int begin_process(const lsi_session_t *session)
{
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] begin_process start\n");
	g_api->set_req_wait_full_body(session);
	if (g_api->is_req_body_finished(session))
	{
		on_read(session);
	}
	else
	{
		g_api->flush(session);
	}
	g_api->log(NULL, LSI_LOG_INFO, "[Module:mod_ffeadcpp] begin_process end\n");
	return 0;
}

static int httprelease(void *data)
{
    g_api->log(NULL, LSI_LOG_DEBUG, "#### waitfullreqbody %s\n", "httprelease");
    return 0;
}

static int clean_up(const lsi_session_t *session)
{
    g_api->free_module_data(session, &MNAME, LSI_DATA_HTTP, httprelease);
    return 0;
}
/*
 * Request processing ends here
 */


/*
 * Openlitespeed module configuration starts here
 */

static lsi_serverhook_t serverHooks[] =
{
    { LSI_HKPT_MAIN_INITED,    	mainInit, 			LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
    { LSI_HKPT_MAIN_PREFORK,   	mainPrefork, 			LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
    { LSI_HKPT_MAIN_POSTFORK,  	mainPostfork, 			LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
    { LSI_HKPT_WORKER_INIT,    	workerInit, 			LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
    { LSI_HKPT_WORKER_ATEXIT,  	workerExit, 			LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
    { LSI_HKPT_MAIN_ATEXIT,    	mainExit, 			LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
	//{ LSI_HKPT_RCVD_REQ_HEADER, rcvd_req_header_cbf, 	LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
	//{ LSI_HKPT_HTTP_BEGIN, 		httpinit, 				LSI_HOOK_NORMAL, LSI_FLAG_ENABLED},
	//{ LSI_HKPT_RECV_REQ_BODY, 	httpreqread, 			LSI_HOOK_EARLY,  LSI_FLAG_TRANSFORM | LSI_FLAG_ENABLED},
    LSI_HOOK_END   //Must put this at the end position
};

static lsi_reqhdlr_t myhandler = { begin_process, on_read, NULL, clean_up, NULL, NULL, NULL };
static lsi_confparser_t myconfighanlder = { parseConfig, freeConfig, myParam };

static int init_module(lsi_module_t *pModule)
{
	pModule->about = VERSION;  //set version string
	g_api->init_module_data(pModule, write_log, LSI_DATA_HTTP);
	return 0;
}

extern "C" {
	LSMODULE_EXPORT lsi_module_t MNAME =
	{
		LSI_MODULE_SIGNATURE, init_module, &myhandler, &myconfighanlder, "mod_ffeadcpp v1.3", serverHooks, {0}
	};
}
/*
 * Openlitespeed module configuration ends here
 */
