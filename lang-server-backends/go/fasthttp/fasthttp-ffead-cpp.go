package main

/*
#include <stddef.h>
#include <stdlib.h>
typedef struct {
    const char* server_str;
    size_t server_str_len;
    const char* method;
    size_t method_len;
    const char* path;
    size_t path_len;
    const char* headers;
    size_t headers_len;
    const char* body;
    size_t body_len;
    int version;
}ffead_request1;
typedef struct {
    const char *name;
    size_t name_len;
    const char *value;
    size_t value_len;
}phr_header_fcp;
//No new line between the comments here end and the import "C" statement
#cgo LDFLAGS: -L/root/ffead-cpp-3.0/lib -lffead-framework
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern void ffead_cpp_cleanup();
extern void* ffead_cpp_handle_return_all_pico_raw(const ffead_request1 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len);
*/
import "C"

import (
	"flag"
	"fmt"
	"github.com/valyala/fasthttp"
	"log"
	"unsafe"
)

var (
	serverDirectory = flag.String("server_directory", "", "TCP address to listen to")
	addr            = flag.String("addr", ":8080", "TCP address to listen to")
	compress        = flag.Bool("compress", false, "Whether to enable transparent response compression")
)

var fsHandler = fs.NewRequestHandler()

func requestHandler(ctx *fasthttp.RequestCtx) {
	rawUrl = ctx.Path() + "?" + ctx.QueryArgs()
	request := C.ffead_request1{server_str: C.CString("Fasthttp"), server_str_len: C.size_t(8),
		method: C.CString(ctx.Method()), method_len: C.size_t(len(ctx.Method())),
		path: C.CString(rawUrl), path_len: C.size_t(len(rawUrl)),
		headers: C.CString(ctx.RawHeaders()), headers_len: C.size_t(len(ctx.RawHeaders())),
		body: C.CString(ctx.PostBody()), body_len: C.size_t(len(ctx.PostBody())),
		version: 1100,
	}
	defer C.free(request.server_str)
	defer C.free(request.method)
	defer C.free(request.path)
	defer C.free(request.headers)
	defer C.free(request.body)

	var outURL *C.char
	var outHeaders [100]C.phr_header_fcp
	var outBody *C.char
	scode := int(0)
	outURLlen := C.size_t(0)
	outHeadersLen := int(0)
	outBodyLen := C.size_t(0)
	fResp = C.ffead_cpp_handle_return_all_pico_raw(&request, &scode, &outURL, &outURLlen, &outHeaders, &outHeadersLen, &outBody, &outBodyLen)

	if scode > 0 {
		for i := 0; i < outHeadersLen; i = i + 1 {
			ctx.Response.Header.Add(C.GoStringN(outHeaders[i].name, outHeaders[i].name_len),
				C.GoStringN(outHeaders[i].value, outHeaders[i].value_len))
		}
		ctx.SetStatusCode(scode)
		ctx.SetBody(C.GoBytes(outBody, outBodyLen))
	} else {
		urlPath := C.GoStringN(outURL, outURLlen)
		if _, err := os.Stat(urlPath); err == nil {
			ctx.SetStatusCode(200)
			fsHandler(urlPath)
		} else {
			ctx.SetStatusCode(404)
		}
	}
}

func main() {
	flag.Parse()

	if *serverDirectory == "" {
		log.Fatalf("No ffead-cpp directory provided")
	}

	cServerDirectory := C.CString(*serverDirectory)
	defer C.free(unsafe.Pointer(cServerDirectory))
	fmt.Println("Bootstrapping ffead-cpp start...")
	C.ffead_cpp_bootstrap(cServerDirectory, serverDirectory.length(), 12)
	fmt.Println("Bootstrapping ffead-cpp end...")

	fmt.Println("Initializing ffead-cpp start...")
	C.ffead_cpp_init()
	fmt.Println("Initializing ffead-cpp end...")

	h := requestHandler
	if *compress {
		h = fasthttp.CompressHandler(h)
	}

	if err := fasthttp.ListenAndServe(*addr, h); err != nil {
		log.Fatalf("Error in ListenAndServe: %s", err)
	}

	fmt.Println("Cleaning up ffead-cpp start...")
	ffead_cpp_cleanup()
	fmt.Println("Cleaning up ffead-cpp end...")
}
