#include <h2o.h>
#include <string.h>

#include "ffeadcpp.h"
#include "request_handler.h"
#include "utility.h"

static __thread void* tl_fres = NULL;

static int ffeadcpp(struct st_h2o_handler_t *self, h2o_req_t *request)
{
	if(tl_fres!=NULL) {
		ffead_cpp_resp_cleanup(tl_fres);
		tl_fres = NULL;
	}

	IGNORE_FUNCTION_PARAMETER(self);

	h2o_generator_t generator;
	memset(&generator, 0, sizeof(generator));

	ffead_request freq;
	freq.server_str = "libh2o";
	freq.server_str_len = 6;
	freq.method = (const char*)request->method.base;
	freq.method_len = request->method.len;
	freq.path = (const char*)request->path.base;
	freq.path_len = request->path.len;
	freq.body_len = 0;
	if(request->entity.len>0) {
		freq.body = (const char*)request->entity.base;
		freq.body_len = request->entity.len;
	}
	freq.version = request->version;
	freq.headers_len = request->headers.size;

	phr_header_fcp f_headers[request->headers.size];
	for(int i=0;i<(int)request->headers.size;i++) {
		f_headers[i].name = (const char*)request->headers.entries[i].name->base;
		f_headers[i].name_len = request->headers.entries[i].name->len;
		f_headers[i].value = (const char*)request->headers.entries[i].value.base;
		f_headers[i].value_len = request->headers.entries[i].value.len;
	}
	freq.headers = f_headers;

	int scode = 0;
	const char* out_url;
	size_t out_url_len;
	const char* out_body;
	size_t out_body_len;
	phr_header_fcp out_headers[100];
	size_t out_headers_len;
	const char* smsg;
	size_t smsg_len;

	tl_fres = ffead_cpp_handle_c_1(&freq, &scode, &smsg, &smsg_len, &out_url, &out_url_len, out_headers, &out_headers_len, &out_body, &out_body_len);
	if(scode>0) {
		for(int i=0;i<(int)out_headers_len;i++) {
			if(strncmp(out_headers[i].name, "Connection", 10)==0 || strncmp(out_headers[i].name, "Content-Length", 14)==0) {}
			else {
				h2o_set_header_by_str(&request->pool, &request->res.headers, out_headers[i].name, out_headers[i].name_len, 1, out_headers[i].value, out_headers[i].value_len, 1);
			}
		}
		request->res.status = scode;
		request->res.reason = smsg;
		h2o_iovec_t body;
		if(out_body_len>0) {
			body.base = (void*)out_body;
			body.len = out_body_len;
			request->res.content_length = out_body_len;
		}
		h2o_start_response(request, &generator);
		h2o_send(request, &body, 1, H2O_SEND_STATE_FINAL);
	} else {
		if(out_url_len>0 && access(out_url, F_OK) != -1 ) {
			h2o_file_send(request, 200, "OK", out_url, h2o_iovec_init(out_url, out_url_len), 0);
		} else {
			h2o_send_error_404(request, "Not Found", "not found", 0);
		}
	}

	return 0;
}

void initialize_ffeadcpp_handler(h2o_hostconf_t *hostconf, h2o_access_log_filehandle_t *log_handle)
{
	register_request_handler("/", ffeadcpp, hostconf, log_handle);
}
