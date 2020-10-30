/// Minimalistic low-overhead wrapper for nodejs/http-parser
/// Used for benchmarks with simple server
module http.Parser;



import http.Common;

import hunt.logging.ConsoleLogger;
import std.conv;
import std.range.primitives;
import core.stdc.string;

import std.experimental.allocator;
import ffeadcpp;

// =========== Public interface starts here =============

public:

class HttpException : Exception {
	HttpError error;

	pure @nogc nothrow this(HttpError error, string file = __FILE__,
			size_t line = __LINE__, Throwable nextInChain = null) {
		this.error = error;
		super("Http exception", file, line, nextInChain);
	}
}

struct HttpParser(Interceptor) {

private {
	Interceptor interceptor;
	Throwable failure;
}

	ffead_request _ireq;
	phr_header_fcp[50] _headers;
	size_t buflen = 0, prevbuflen = 0;
	bool done;

	alias interceptor this;

	this(Interceptor interceptor) {
		this.interceptor = interceptor;
	}

	@property bool status() pure @safe nothrow {
		return failure is null;
	}

	@property bool shouldKeepAlive() pure nothrow {
		return true;
	}

	@property ushort httpMajor() @safe pure nothrow {
		return 1;
	}

	@property ushort httpMinor() @safe pure nothrow {
		return cast(ushort)_ireq.version_;
	}

	int execute(const(ubyte)[] str) {
		return doexecute(str);
	}

	private int doexecute(const(ubyte)[] chunk) {
		debug trace(cast(string)chunk);

		failure = null;
		done = false;
		int contentLength = 0;
		_ireq.headers_len = cast(int)_headers.length;
		int pret = phr_parse_request_fcp(cast(const char*)chunk.ptr, cast(int)chunk.length,
					&_ireq.method, &_ireq.method_len, &_ireq.path, &_ireq.path_len,
					&_ireq.version_, _headers.ptr, &_ireq.headers_len, 0, &contentLength);
		debug {
			infof("buffer: %d bytes, request: %d bytes", chunk.length, pret);
		}

		if(pret > 0) {
			/* successfully parsed the request */
			onMessageComplete();

			if(pret < chunk.length) {
				debug infof("try to parse next request");
				pret += doexecute(chunk[pret .. $]); // try to parse next http request data
			}

			debug infof("pret=%d", pret);
			return pret;
		} else if(pret == -2) {
			debug warning("parsing incomplete");
			debug infof("pret=%d, chunk=%d", pret, chunk.length);
			return 0;
		}

		warning("wrong data format");
		failure = new HttpException(HttpError.UNKNOWN);
		throw failure;
	}

	void onMessageComplete() {
		// interceptor.onHeadersComplete();
		debug {
			tracef("method is %s", _ireq.method[0.._ireq.method_len]);
			tracef("path is %s", _ireq.path[0.._ireq.path_len]);
			tracef("HTTP version is 1.%d", _ireq.version_);
			foreach(ref phr_header_fcp h; _headers[0.._ireq.headers_len]) {
				tracef("Header: %s = %s", h.name[0..h.name_len], h.value[0..h.value_len]);
			}
		}
		interceptor.onMessageComplete();
	}
}

auto httpParser(Interceptor)(Interceptor interceptor) {
	return HttpParser!Interceptor(interceptor);
}
