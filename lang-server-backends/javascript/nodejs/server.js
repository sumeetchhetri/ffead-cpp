var http = require('http');
var ffi = require('ffi-napi');
var ref = require('ref-napi');
var fileSystem = require('fs')
var StructType = require('ref-struct-di')(ref);
var ArrayType = require('ref-array-di')(ref);

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
	'ffead_cpp_is_inited': ['bool', []],
    'ffead_cpp_cleanup': ['void', []],
    'ffead_cpp_bootstrap': ['void', ['CString', 'size_t', 'int']],
    /*
	  	void* ffead_cpp_handle_js_1(const ffead_request *request, int* scode, size_t *out_url_len, size_t *out_headers_len, size_t *out_body_len);
		const char* ffead_cpp_handle_js_out_url(void* res);
		const char* ffead_cpp_handle_js_out_body(void* res);
		const char* ffead_cpp_handle_js_out_hdr_name(void* res, int pos, size_t* name_len);
		const char* ffead_cpp_handle_js_out_hdr_value(void* res, int pos, size_t* name_len);
  	*/
    'ffead_cpp_handle_js_1': ['pointer', [ffead_request_js_ptr, c_int_ptr, c_size_t_ptr, c_size_t_ptr, c_size_t_ptr]],
    'ffead_cpp_handle_js_out_url': ['char *', [void_ptr]],
    'ffead_cpp_handle_js_out_body': ['char *', [void_ptr]],
    'ffead_cpp_handle_js_out_hdr_name': ['char *', [void_ptr, 'int', c_size_t_ptr]],
    'ffead_cpp_handle_js_out_hdr_value': ['char *', [void_ptr, 'int', c_size_t_ptr]],
    'ffead_cpp_resp_cleanup': ['void', [void_ptr]]
});

function getStr(buf, len) {
	return ref.reinterpretUntilZeros(buf, len).toString('utf8').substring(0, len);
}

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
    
    var scode_r = ref.alloc(ref.types.int, 0);
    var out_url_len_r = ref.alloc(ref.types.size_t);
    var out_mime_len_r = ref.alloc(ref.types.size_t);
    var out_headers_len_r = ref.alloc(ref.types.size_t);
    var out_body_len_r = ref.alloc(ref.types.size_t);
    
    var fres = libffeadcpp.ffead_cpp_handle_js_1(freq.ref(), scode_r, out_url_len_r, out_headers_len_r, out_body_len_r);
    
    var scode = ref.deref(scode_r);
    var out_url_len = ref.deref(out_url_len_r);
    var out_mime_len = ref.deref(out_mime_len_r);
    var out_headers_len = ref.deref(out_headers_len_r);
    var out_body_len = ref.deref(out_body_len_r);
    
    if(debug) {
	    console.log("Status=" + scode + ", UrlLength=" + out_url_len + ", HeadersLength=" + out_headers_len + ", BodyLength=" + out_body_len);
	    if(out_url_len>0) {
	    	console.log(getStr(libffeadcpp.ffead_cpp_handle_js_out_url(fres), out_url_len));
	    }
	    for(var i=0;i<out_headers_len;i++) {
	    	var len = ref.alloc(ref.types.size_t);
	    	var hn = libffeadcpp.ffead_cpp_handle_js_out_hdr_name(fres, i, len); 
	    	console.log(getStr(hn, len.deref()));
	    	var vl = libffeadcpp.ffead_cpp_handle_js_out_hdr_value(fres, i, len); 
	    	console.log(getStr(vl, len.deref()));
	    }
	    if(out_body_len>0) {
	    	console.log(getStr(libffeadcpp.ffead_cpp_handle_js_out_body(fres), out_body_len));
	    }
	}
    
    response.setHeader("Server", "nodejs");
    if(scode>0) {
    	response.statusCode = scode;
    	for(var i=0;i<out_headers_len;i++) {
	    	var hl = ref.alloc(ref.types.size_t);
	    	var hn = libffeadcpp.ffead_cpp_handle_js_out_hdr_name(fres, i, hl); 
	    	var vll = ref.alloc(ref.types.size_t);
	    	var vl = libffeadcpp.ffead_cpp_handle_js_out_hdr_value(fres, i, vll); 
	    	response.setHeader(getStr(hn, hl.deref()), getStr(vl, vll.deref()));
	    }
	    if(out_body_len>0) {
	    	response.end(getStr(libffeadcpp.ffead_cpp_handle_js_out_body(fres), out_body_len));
	    } else {
	    	response.end();
	    }
    } else {
    	if(out_url_len>0) {
	    	var url = getStr(libffeadcpp.ffead_cpp_handle_js_out_url(fres), out_url_len);
	    	var stat = fileSystem.statSync(url);
	    	for(var i=0;i<out_headers_len;i++) {
		    	var hl = ref.alloc(ref.types.size_t);
		    	var hn = libffeadcpp.ffead_cpp_handle_js_out_hdr_name(fres, i, hl); 
		    	var vll = ref.alloc(ref.types.size_t);
		    	var vl = libffeadcpp.ffead_cpp_handle_js_out_hdr_value(fres, i, vll); 
		    	response.setHeader(getStr(hn, hl.deref()), getStr(vl, vll.deref()));
		    }
		    response.setHeader('Content-Length', stat.size);
		    var readStream = fileSystem.createReadStream(url);
	    	readStream.pipe(response);
	    } else {
	    	response.statusCode = 404;
	    	response.end();
	    }
    }
    libffeadcpp.ffead_cpp_resp_cleanup(fres);
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

var args = process.argv.slice(2);
var server_dir = (args.length>1 && args[1].trim()!="")?args[1].trim():"/installs/ffead-cpp-7.0";
var debug = (args.length>2 && args[2].toLowerCase().trim()=="true");

libffeadcpp.ffead_cpp_bootstrap(server_dir, server_dir.length, 27);

libffeadcpp.ffead_cpp_init();

server.listen(args[0].trim()*1);
console.log('nodejs+ffead-cpp http server at port '+args[0].trim()+' is running..');
