module http.DemoProcessor;


import std.json;
import std.conv;
import hunt.io;
import http.Common;
import http.Processor;
import hunt.logging.ConsoleLogger : trace, warning, tracef;
import ffeadcpp;
import std.algorithm;
import std.array;
import std.exception;
import std.random;
import std.string;
import std.file : read;
import http.Parser;
import std.file;
private	alias Parser = HttpParser!HttpProcessor;

class DemoProcessor : HttpProcessor {

    this(TcpStream client) {
        super(client);
    }

    override void onComplete(ref HttpRequest req) {
    	Parser* p = req.p;
    	p._ireq.headers = cast(phr_header_fcp*)p._headers;
    	
    	int scode = 1;
		const char* out_url;
		size_t out_url_len;
		const char* out_mime;
		size_t out_mime_len;
		const char* out_body;
		size_t out_body_len;
		const char* out_headers;
		size_t out_headers_len;
	
    	void* fres = ffead_cpp_handle_d_1(&p._ireq, &scode, &out_url, &out_url_len, &out_mime, &out_mime_len, &out_headers, &out_headers_len, &out_body, &out_body_len);
    
        if(scode > 0) {
        	sendResponse(out_body, out_body_len, out_headers, out_headers_len);
        } else {
        	string fn = cast(string)out_url[0..out_url_len];
        	if(fn.exists) {
	        	auto f = read(fn);
	        	string oh = "HTTP/1.1 200 OK\r\nContent-Type: " ~ cast(string)out_mime[0..out_mime_len] ~ "\r\nContent-Length: " ~ to!string(f.length) ~ "\r\n";
	        	sendResponse(cast(char*)f.ptr, f.length, cast(const char*)oh, oh.length);
	        } else {
	        	send404Response();
	        }
        }
    }
}
