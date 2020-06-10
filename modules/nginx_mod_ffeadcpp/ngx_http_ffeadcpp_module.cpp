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
extern "C" {
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
}
#include "ServerInitUtil.h"

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

static ngx_int_t
ngx_http_static_init1(ngx_conf_t *cf)
{
	ngx_http_handler_pt        *h;
	ngx_http_core_main_conf_t  *cmcf;

	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

	h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
	if (h == NULL) {
		return NGX_ERROR;
	}

	*h = ngx_http_ffeadcpp_module_handler;

	return NGX_OK;
}


/*
 * The module context has hooks , here we have a hook for creating
 * location configuration
 */
static ngx_http_module_t ngx_http_ffeadcpp_module_module_ctx = {
		NULL,                          /* preconfiguration */
		ngx_http_static_init1,                          /* postconfiguration */

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
	ngx_str_t  *name = data; // i.e., first field of ngx_http_ffeadcpp_module_loc_conf_t

	if (ngx_strcmp(name->data, "") == 0) {
		return NGX_CONF_ERROR;
	}
	ffeadcpp_path.data = name->data;
	ffeadcpp_path.len = ngx_strlen(ffeadcpp_path.data);

	std::cout << "FFEAD in ngx_http_ffeadcpp " << name->data << std::endl;

	return NGX_CONF_OK;
}
}
/*
 * Main handler function of the module.
 */

static void ngx_http_read_input_data(ngx_http_request_t *r, std::string& data)
{
	if (r->request_body == NULL) {
		return;
	}
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
		ngx_chain_t  *in;

		for (in = r->request_body->bufs; in; in = in->next)
		{
			buf = in->buf;
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

ngx_int_t set_custom_header_in_headers_out(ngx_http_request_t *r, const std::string& key, const std::string& value) {
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
	h->key.data = ngx_pcalloc(r->pool, key.size()+1);
	if (h->key.data == NULL) {
		return NGX_ERROR;
	}
	ngx_cpystrn(h->key.data, key.c_str(), key.size()+1);
	h->key.len = key.size();

	/*
    ... and the value.
	 */
	h->value.data = ngx_pcalloc(r->pool, value.size()+1);
	if (h->value.data == NULL) {
		return NGX_ERROR;
	}
	ngx_cpystrn(h->value.data, value.c_str(), value.size()+1);
	h->value.len = value.size();

	/*
    Mark the header as not deleted.
	 */
	h->hash = 1;

	return NGX_OK;
}

static bool ignoreHeader(const std::string& hdr)
{
	if(strcasecmp(hdr.c_str(), HttpResponse::Server.c_str())==0
		|| strcasecmp(hdr.c_str(), HttpResponse::DateHeader.c_str())==0
		|| strcasecmp(hdr.c_str(), HttpResponse::AcceptRanges.c_str())==0
		|| strcasecmp(hdr.c_str(), HttpResponse::ContentType.c_str())==0
		|| strcasecmp(hdr.c_str(), HttpResponse::Connection.c_str())==0)
	{
		return true;
	}
	return false;
}

static ngx_int_t ngx_http_ffeadcpp_module_handler_post_read(ngx_http_request_t *r)
{
	/*std::string cntpath = "";
	cntpath.append((const char*)ffeadcpp_path.data, ffeadcpp_path.len);
	cntpath += "/web/";
	HttpRequest req(cntpath);*/

	std::string content;
	ngx_http_read_input_data(r, content);

	HttpRequest req((const char*)r->uri.data, r->uri.len, (const char*)r->args.data, r->args.len,
			(const char*)r->main->method_name.data, r->main->method_name.len, std::move(content), r->http_version);

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
		req.addNginxApacheHeader((const char*)h[i].key.data, h[i].key.len, (const char*)h[i].value.data, h[i].value.len);
	}

	HttpResponse respo;
	ServiceTask task;
	task.handle(&req, &respo);

	if(respo.isDone()) {
		for (int var = 0; var < (int)respo.getCookies().size(); var++)
		{
			set_custom_header_in_headers_out(r, std::string("Set-Cookie"), respo.getCookies().at(var));
		}

		std::string& data = respo.generateNginxApacheResponse();
		r->headers_out.content_length_n = (int)data.length();
		std::map<std::string,std::string>::const_iterator it;
		for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
			set_custom_header_in_headers_out(r, it->first, it->second);
		}

		/* set the status line */
		r->headers_out.status = respo.getCode();

		if(data.length()>0)
		{
			/*std::string contType = respo.getHeader(HttpResponse::ContentType);
			r->headers_out.content_type.data = ngx_pcalloc(r->pool, contType.size()+1);
			if (r->headers_out.content_type.data == NULL) {
				ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
				return;
			}
			ngx_cpystrn(r->headers_out.content_type.data, contType.c_str(), contType.size()+1);
			r->headers_out.content_type.len = contType.size();*/

			b = ngx_create_temp_buf(r->pool, data.size());
			if (b == NULL) {
				ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
				return;
			}

			/* adjust the pointers of the buffer */
			ngx_memcpy(b->pos, data.c_str(), data.size());
			b->last = b->pos + data.size();
			b->memory = 1;    /* this buffer is in memory */
			b->last_buf = 1;  /* this is the last buffer in the buffer chain */

			/* attach this buffer to the buffer chain */
			out.buf = b;
			out.next = NULL;
		}
		else
		{
			ngx_http_send_header(r);
			ngx_http_finalize_request(r, NGX_HTTP_LAST);
			return ngx_http_send_special (r, NGX_HTTP_LAST);
		}

		/* send the headers of your response */
		rc = ngx_http_send_header(r);

		if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
			ngx_http_finalize_request(r, rc);
			return rc;
		}

		/* send the buffer chain of your response */
		ngx_http_finalize_request(r, ngx_http_output_filter(r, &out));
		return NGX_DONE;
	} else {
		u_char                    *last, *location;
		size_t                     len;
		ngx_str_t                  path;
		ngx_uint_t                 level;
		ngx_log_t                 *log;
		ngx_open_file_info_t       of;
		ngx_http_core_loc_conf_t  *clcf;

		clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

		log = r->connection->log;

		ngx_memzero(&of, sizeof(ngx_open_file_info_t));
		of.read_ahead = clcf->read_ahead;
		of.directio = clcf->directio;
		of.valid = clcf->open_file_cache_valid;
		of.min_uses = clcf->open_file_cache_min_uses;
		of.errors = clcf->open_file_cache_errors;
		of.events = clcf->open_file_cache_events;

		path.data = req.getUrl().c_str();
		path.len = ngx_strlen(path.data);

		rc = NGX_OK;

		if (ngx_http_set_disable_symlinks(r, clcf, &path, &of) != NGX_OK) {
			rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
			r->headers_out.status = rc;
			ngx_http_send_header(r);
			ngx_http_finalize_request(r, NGX_HTTP_LAST);
			return ngx_http_send_special (r, NGX_HTTP_LAST);
		}

		if (ngx_open_cached_file(clcf->open_file_cache, &path, &of, r->pool)
				!= NGX_OK)
		{
			switch (of.err) {

			case 0:
				rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
				break;
			case NGX_ENOENT:
			case NGX_ENOTDIR:
			case NGX_ENAMETOOLONG:

				level = NGX_LOG_ERR;
				rc = NGX_HTTP_NOT_FOUND;
				break;

			case NGX_EACCES:
#if (NGX_HAVE_OPENAT)
			case NGX_EMLINK:
			case NGX_ELOOP:
#endif

				level = NGX_LOG_ERR;
				rc = NGX_HTTP_FORBIDDEN;
				break;

			default:

				level = NGX_LOG_CRIT;
				rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
				break;
			}

			if (rc != NGX_HTTP_NOT_FOUND || clcf->log_not_found) {
				ngx_log_error(level, log, of.err,
						"%s \"%s\" faileddddddddddddddd", of.failed, path.data);
			}

			//return rc;
		}
		r->root_tested = !r->error_page;

		ngx_log_debug1(NGX_LOG_DEBUG_HTTP, log, 0, "http static fd: %d", of.fd);

		/*if (of.is_dir) {

			ngx_log_debug0(NGX_LOG_DEBUG_HTTP, log, 0, "http dir");

			ngx_http_clear_location(r);

			r->headers_out.location = ngx_palloc(r->pool, sizeof(ngx_table_elt_t));
			if (r->headers_out.location == NULL) {
				return NGX_HTTP_INTERNAL_SERVER_ERROR;
			}

			len = r->uri.len + 1;

			if (!clcf->alias && clcf->root_lengths == NULL && r->args.len == 0) {
				location = path.data + clcf->root.len;

		 *last = '/';

			} else {
				if (r->args.len) {
					len += r->args.len + 1;
				}

				location = ngx_pnalloc(r->pool, len);
				if (location == NULL) {
					return NGX_HTTP_INTERNAL_SERVER_ERROR;
				}

				last = ngx_copy(location, r->uri.data, r->uri.len);

		 *last = '/';

				if (r->args.len) {
		 *++last = '?';
					ngx_memcpy(++last, r->args.data, r->args.len);
				}
			}
		 */
		/*
		 * we do not need to set the r->headers_out.location->hash and
		 * r->headers_out.location->key fields
		 */

		/*r->headers_out.location->value.len = len;
			r->headers_out.location->value.data = location;

			return NGX_HTTP_MOVED_PERMANENTLY;
		}*/

#if !(NGX_WIN32) /* the not regular files are probably Unix specific */

		if (!of.is_file) {
			ngx_log_error(NGX_LOG_CRIT, log, 0,
					"\"%s\" is not a regular file", path.data);

			rc = NGX_HTTP_NOT_FOUND;
		}

#endif

		/*if (r->method == NGX_HTTP_POST) {
			return NGX_HTTP_NOT_ALLOWED;
		}*/

		if (rc != NGX_OK) {
			r->headers_out.status = rc;
			ngx_http_send_header(r);
			ngx_http_finalize_request(r, NGX_HTTP_LAST);
			return ngx_http_send_special (r, NGX_HTTP_LAST);
		}		

		/*rc = ngx_http_discard_request_body(r);

		if (rc != NGX_OK) {
			return rc;
		}*/

		log->action = "sending response to client";

		r->headers_out.status = NGX_HTTP_OK;
		r->headers_out.content_length_n = of.size;
		r->headers_out.last_modified_time = of.mtime;

		if (ngx_http_set_etag(r) != NGX_OK) {
			ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
			return;
		}

		if (ngx_http_set_content_type(r) != NGX_OK) {
			ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
			return;
		}

		if (r != r->main && of.size == 0) {
			r->headers_out.status = rc;
			ngx_http_send_header(r);
			ngx_http_finalize_request(r, NGX_HTTP_LAST);
			return ngx_http_send_special (r, NGX_HTTP_LAST);
		}

		r->allow_ranges = 1;

		/* we need to allocate all before the header would be sent */

		b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
		if (b == NULL) {
			ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
			return;
		}

		b->file = ngx_pcalloc(r->pool, sizeof(ngx_file_t));
		if (b->file == NULL) {
			ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
			return;
		}

		if (rc != NGX_OK || r->header_only) {
			r->headers_out.status = rc;
			ngx_http_send_header(r);
			ngx_http_finalize_request(r, NGX_HTTP_LAST);
			return ngx_http_send_special (r, NGX_HTTP_LAST);
		}

		rc = ngx_http_send_header(r);

		b->file_pos = 0;
		b->file_last = of.size;

		b->in_file = b->file_last ? 1: 0;
		b->last_buf = (r == r->main) ? 1: 0;
		b->last_in_chain = 1;

		b->file->fd = of.fd;
		b->file->name = path;
		b->file->log = log;
		b->file->directio = of.is_directio;

		out.buf = b;
		out.next = NULL;

		ngx_http_finalize_request(r, ngx_http_output_filter(r, &out));
		return NGX_DONE;
	}
}

/*
 * Function for the directive hello , it validates its value
 * and copies it to a static variable to be printed later
 */

static ngx_int_t init_module(ngx_cycle_t *cycle)
{
	std::string serverRootDirectory;
	serverRootDirectory.append((const char*)ffeadcpp_path.data, ffeadcpp_path.len);
	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::NGINX);
	return NGX_OK;
}

static ngx_int_t init_worker_process(ngx_cycle_t *cycle)
{
	ServerInitUtil::init(logger);
	return NGX_OK;
}

static ngx_int_t exit_process(ngx_cycle_t *cycle)
{
	ServerInitUtil::cleanUp();
	return NGX_OK;
}
