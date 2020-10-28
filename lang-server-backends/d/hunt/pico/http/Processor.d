/// An example "HTTP server" with poor usability but sensible performance
///
module http.Processor;

import std.conv;
import std.array, std.exception, std.format, std.algorithm.mutation, std.socket;
import core.stdc.stdlib;
import core.thread, core.atomic;
import http.Parser;

import hunt.io.ByteBuffer;
import http.Common;
import hunt.logging;
import hunt.io;
import hunt.util.DateTime;
import std.array;
import std.string;
import core.stdc.string;
import core.stdc.stdlib;
import std.stdio;
import hunt.io.IoError;
import std.experimental.allocator;
private	alias Parser = HttpParser!HttpProcessor;


static this()
{
}

struct HttpRequest {
	private Parser* parser;

	HttpHeader[] headers(bool canCopy=false)() @property {
		return parser.headers!canCopy();
	}

	Parser* p() @property {
		return parser;
	}
}

abstract class HttpProcessor {

package:
	Parser parser;
	HttpRequest request;
	bool serving;


public:
	TcpStream client;

	this(TcpStream sock) {
		serving = true;
		client = sock;
		parser = httpParser(this);
		request.parser = &parser;
	}

	void run() {
		client.received((ByteBuffer buffer) {
			version(NO_HTTPPARSER) {
				client.write(cast(ubyte[])ResponseData);
			} else {
				try {
					if(!parser.done && parser._ireq.body_len>0) {
						//TODO
						/*if(parser._ireq.body_len<=buffer.getRemaining()) {
						} else {
						}*/
					}
					
					int pos = buffer.position();
					int len =  parser.execute(cast(ubyte[]) buffer.getRemaining());
					buffer.position(buffer.position() + len);
					
					if(parser._ireq.body_len==0) {
						parser.done = true;
					} else if(parser._ireq.body_len<=buffer.getRemaining().length) {
						parser.done = true;
						ubyte[] body = cast(ubyte[]) buffer.getRemaining();
						parser._ireq.body = cast(char*)body.ptr;
					} else {
						//TODO
					}
				} catch(Exception ex) {
					buffer.clear(); // drop all the  wrong data
				}
			}
		})
		.closed(() {
			// notifyClientClosed();
		})
		.error((IoError msg) {
			 warning("Error: ", msg.errorMsg());
		})
		.start();
	}

	protected void notifyClientClosed() {
		debug tracef("The connection[%s] is closed", client.remoteAddress());
	}
	
	void send404Response() {
		string pendHdrs = "HTTP/1.1 404 Not Found\r\nServer: hunt\r\nDate: " ~ DateTime.getDateAsGMT() ~ "\r\nConnection: close\r\n\r\n";
		client.write(cast(ubyte[])pendHdrs);
	}
	
	void sendResponse(const char* out_body, size_t out_body_len, const char* out_headers, size_t out_headers_len) {
		client.write(cast(ubyte[]) out_headers[0 .. out_headers_len]);
		string pendHdrs = "Server: hunt\r\nDate: " ~ DateTime.getDateAsGMT() ~ "\r\nConnection: ";
		if (parser.shouldKeepAlive) {
			pendHdrs ~= "Keep-Alive\r\n\r\n";
		} else {
			pendHdrs ~= "close\r\n\r\n";
		}
		client.write(cast(ubyte[])pendHdrs);
		client.write(cast(ubyte[]) out_body[0 .. out_body_len]);
	}

	void onChunk(ref HttpRequest req, const(ubyte)[] chunk) {
		// TODO: Tasks pending completion - 5/16/2019, 5:40:18 PM
		//
	}

	void onComplete(ref HttpRequest req);

	final int onBody(Parser* parser, const(ubyte)[] chunk) {
		onChunk(request, chunk);
		return 0;
	}

	final int onMessageComplete() {
		try {
			onComplete(request);
		} catch(Exception ex) {
			//respondWith(ex.msg, 500);
		}
		if (!parser.shouldKeepAlive)
			serving = false;
		return 0;
	}
}
