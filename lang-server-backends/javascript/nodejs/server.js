var http = require('http');
var concat = require('concat-stream');
var ffi = require('ffi');
var ref = require('ref');
var StructType = require('ref-struct');
var ArrayType = require('ref-array');

var phr_header_fcp_js = StructType({
	name: CString,
	name_len: size_t,
	value: CString,
	value_len: size_t
});
var phr_header_fcp_js_ptr = ref.refType(phr_header_fcp_js);
var phr_header_fcp_js_array = ArrayType(phr_header_fcp_js);

var ffead_request_js = StructType({
	server_str: CString, 
	server_str_len: size_t,
	method: CString, 
	method_len: size_t,
	path: CString, 
	path_len: size_t, 
	headers: phr_header_fcp_js_ptr
	headers_len: size_t,
	body: CString, 
	body_len: size_t,
	version: int
});
var ffead_request_js_ptr = ref.refType(ffead_request_js);

var c_str_ptr = ref.refType(ref.types.CString);
var c_int_ptr = ref.refType(ref.types.int);
var c_size_t_ptr = ref.refType(ref.types.size_t);
var void_ptr = ref.refType(ref.types.void);

var libffeadcpp = ffi.Library('libffead-framework', {
  'ffead_cpp_init': [ 'void', [ ] ],
  'ffead_cpp_cleanup': [ 'void', [ ] ],
  'ffead_cpp_bootstrap': [ 'void', [ 'CString', 'size_t', 'int' ] ],
  /*
  	void* ffead_cpp_handle_c_1(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
		const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
		const char **out_body, size_t *out_body_len);
  */
  'ffead_cpp_handle_c_1': [ 'pointer', [ 'ffead_request_js_ptr', 'c_int_ptr', 'c_str_ptr', 'c_size_t_ptr', 'c_str_ptr', 'c_size_t_ptr',
  										 'phr_header_fcp_js_ptr', 'c_size_t_ptr', 'c_str_ptr', 'c_size_t_ptr' ] ],
  'ffead_cpp_resp_cleanup': [ 'void', [ void_ptr ] ],
});


var server_dir = "/installs/ffead-cpp-5.0";
libffeadcpp.bootstrap(server_dir, server_dir.length, 27);
libffeadcpp.ffead_cpp_init();

function get_ffead_cpp_request(req, body, headers, hlen) {
	var blen = body?body.length:0;
	return new ffead_request_js({
		server_str: 'nodejs', server_str_len: 6,
		method: req.method, method_len: req.method.length,
		path: req.path, path_len: req.path.length,
		headers: headers, headers_len: hlen,
		body: body, method_len: blen,
		version: 10
	});
}

var server = http.createServer(function (request, response) {
	const headerNames = request.getRawHeaderNames();
	var headers = phr_header_fcp_js_array(headerNames.length);
	for(let i=0;i<headerNames.length;i++) {
		var hn = headerNames[i];
		var hv = request.getHeader(hn);
		headers[i] = new phr_header_fcp_js({name: hn, name_len: hn.length, value: hv, value_len: hv.length});
	}
	if(request.method!="POST" && request.method!="PUT") {
		var freq = get_ffead_cpp_request(res, null, headers, headerNames.length);
	    libffeadcpp.ffead_cpp_handle_c_1(freq, );
	} else {
		concat(req, body => {
	    	var freq = get_ffead_cpp_request(res, body, headers, headerNames.length);
	    	libffeadcpp.ffead_cpp_handle_c_1(freq, );
			console.log(req);
	  	});
	  }
});

server.listen(8080);
console.log('Node.js http server at port 8080 is running..')