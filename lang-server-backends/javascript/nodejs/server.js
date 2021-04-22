var http = require('http');
var ffi = require('ffi-napi');
var ref = require('ref-napi');
var StructType = require('ref-struct-di')(ref);
var ArrayType = require('ref-array-di')(ref);
var debug = true;

var phr_header_fcp_js = StructType({
    name: ref.types.CString,
    name_len: ref.types.size_t,
    value: ref.types.CString,
    value_len: ref.types.size_t
});
var phr_header_fcp_js_ptr = ref.refType(phr_header_fcp_js);
var phr_header_fcp_js_array = ArrayType(phr_header_fcp_js);

var ffead_request_js = StructType({
    server_str: ref.types.CString,
    server_str_len: ref.types.size_t,
    method: ref.types.CString,
    method_len: ref.types.size_t,
    path: ref.types.CString,
    path_len: ref.types.size_t,
    headers: phr_header_fcp_js_array,
    headers_len: ref.types.size_t,
    body: ref.types.CString,
    body_len: ref.types.size_t,
    version: ref.types.int
});
var ffead_request_js_ptr = ref.refType(ffead_request_js);

var c_str_ptr = ref.refType(ref.refType(ref.types.void));
var c_int_ptr = ref.refType(ref.types.int);
var c_size_t_ptr = ref.refType(ref.types.size_t);
var void_ptr = ref.refType(ref.types.void);

var libffeadcpp = ffi.Library('libffead-framework', {
    'ffead_cpp_init': ['void', []],
    'ffead_cpp_cleanup': ['void', []],
    'ffead_cpp_bootstrap': ['void', ['CString', 'size_t', 'int']],
    /*
  	void* ffead_cpp_handle_c_1(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
		const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
		const char **out_body, size_t *out_body_len);
  	*/
    'ffead_cpp_handle_1s': ['pointer', [ffead_request_js_ptr, c_int_ptr, 'char **', c_size_t_ptr, c_str_ptr, c_size_t_ptr,
        phr_header_fcp_js_ptr, c_size_t_ptr, c_str_ptr, c_size_t_ptr
    ]],
    'ffead_cpp_resp_cleanup': ['void', [void_ptr]],
});


var server_dir = "/installs/ffead-cpp-6.0";
libffeadcpp.ffead_cpp_bootstrap(server_dir, server_dir.length, 27);
libffeadcpp.ffead_cpp_init();

function ffead_cpp_handle(request, response, body) {
	const headerNames = Object.keys(request.headers);
    var headers = phr_header_fcp_js_array(headerNames.length);
    for (let i = 0; i < headerNames.length; i++) {
        var hn = headerNames[i];
        var hv = request.headers[hn];
        headers[i] = new phr_header_fcp_js({
            name: hn,
            name_len: hn.length,
            value: hv,
            value_len: hv.length
        });
    }

    var blen = body ? body.length : 0;
    var freq = new ffead_request_js({
        server_str: 'nodejs',
        server_str_len: 6,
        method: request.method,
        method_len: request.method.length,
        path: request.url,
        path_len: request.url.length,
        headers: headers,
        headers_len: headerNames.length,
        body: body,
        body_len: blen,
        version: 10
    });
    
    var scode_r = ref.alloc(ref.types.int, 1);
    var smsg_r = Buffer.allocUnsafe(100);
    var smsg_len_r = ref.alloc(ref.types.size_t);
    var out_url_r = Buffer.alloc(2048);
    var out_url_len_r = ref.alloc(ref.types.size_t);
    var out_headers = phr_header_fcp_js_array(20);
    var out_headers_len_r = ref.alloc(ref.types.size_t);
    var out_body_r = Buffer.alloc(4096);
    var out_body_len_r = ref.alloc(ref.types.size_t);
    
    libffeadcpp.ffead_cpp_handle_1s(freq.ref(), scode_r, smsg_r.ref(), smsg_len_r, out_url_r.ref(), out_url_len_r, 
    	out_headers.ref(), out_headers_len_r, out_body_r.ref(), out_body_len_r);
    
    var scode = ref.deref(scode_r);
    
    if(debug) {
	    console.log(scode);
	    console.log(ref.deref(smsg_len_r));
	    console.log(smsg_r.toString('utf8',0,2));
	    //console.log(out_url_r);
	    //console.log(out_url_len_r);
	    console.log(ref.deref(out_url_len_r));
	    //console.log(out_headers);
	    //console.log(out_headers_len_r);
	    console.log(ref.deref(out_headers_len_r));
	    //console.log(out_body_r);
	    //console.log(out_body_len_r);
	    console.log(ref.deref(out_body_len_r));
	}
    
    if(scode>0) {
    	response.writeHead(200);
    	response.end();
    } else {
    	response.writeHead(404);
        response.end();
    }
}

var server = http.createServer(function(request, response) {
    if (request.method != "POST" && request.method != "PUT") {
        ffead_cpp_handle(request, response, null);
    } else {
        let body = "";
        req.on("data", (chunk) => {
            body += chunk;
        });
        req.on("end", () => {
            ffead_cpp_handle(request, response, body);
        });
    }
});

server.listen(8080);
console.log('Node.js http server at port 8080 is running..');
