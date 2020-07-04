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

package main

/*
#include <stddef.h>
#include <stdlib.h>
typedef struct phr_header_fcp {
    const char *name;
    size_t name_len;
    const char *value;
    size_t value_len;
}phr_header_fcp;
//No new line between the comments here end and the import "C" statement
#cgo LDFLAGS: -lffead-framework
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern void ffead_cpp_cleanup();
extern void* ffead_cpp_handle_go_1(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, const char *query, size_t query_len, int version,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);
extern void ffead_cpp_resp_cleanup(void* ptr);
*/
import "C"

import (
	"flag"
	"fmt"
	"github.com/valyala/fasthttp"
	"log"
	"os"
	"reflect"
	"runtime"
	"strings"
	"unsafe"
	fastprefork "github.com/valyala/fasthttp/prefork"
)

var (
	serverDirectory = flag.String("server_directory", "", "TCP address to listen to")
	addr            = flag.String("addr", ":8080", "TCP address to listen to")
	compress        = flag.Bool("compress", false, "Whether to enable transparent response compression")
	prefork			= flag.Bool("prefork", true, "use prefork")
)

var (
	//SERVER = name passed in teh server header
	SERVER = "fasthttp"
)

func asPtrAndLength(s string) (*C.char, C.size_t) {
	addr := &s
	hdr := (*reflect.StringHeader)(unsafe.Pointer(addr))
	p := (*C.char)(unsafe.Pointer(hdr.Data))
	n := C.size_t(hdr.Len)
	runtime.KeepAlive(addr)
	return p, n
}

func toCharP(bs []byte) *C.char {
	return (*C.char)(unsafe.Pointer(&bs[0]))
}

func requestHandler(ctx *fasthttp.RequestCtx) {
	var srvStr, srvStrLen = asPtrAndLength(SERVER)
	var qStr *C.char
	if len(ctx.URI().QueryString()) > 0 {
		qStr = toCharP(ctx.URI().QueryString())
	}
	var iBody *C.char
	if len(ctx.PostBody()) > 0 {
		iBody = toCharP(ctx.PostBody())
	}

	var outHeaders [100]C.phr_header_fcp
	var outURL *C.char
	var outURLlen C.size_t
	var outMime *C.char
	var outMimelen C.size_t
	var outHeadersLen C.size_t
	var outBody *C.char
	var outBodyLen C.size_t
	var scode C.int

	fresp := C.ffead_cpp_handle_go_1(srvStr, srvStrLen, toCharP(ctx.Method()), C.size_t(len(ctx.Method())),
		toCharP(ctx.Path()), C.size_t(len(ctx.Path())), qStr, C.size_t(len(ctx.URI().QueryString())), 1,
		toCharP(ctx.Request.Header.RawHeaders()), C.size_t(len(ctx.Request.Header.RawHeaders())),
		iBody, C.size_t(len(ctx.PostBody())), &scode, &outURL, &outURLlen, &outMime, &outMimelen,
		&outHeaders[0], &outHeadersLen, &outBody, &outBodyLen,
	)

	if scode > 0 {
		for i := 0; i < int(outHeadersLen); i = i + 1 {
			var key = C.GoStringN(outHeaders[i].name, C.int(outHeaders[i].name_len))
			if strings.EqualFold(key, "content-type") {
				ctx.SetContentType(C.GoStringN(outHeaders[i].value, C.int(outHeaders[i].value_len)))
			} else if !strings.EqualFold(key, "content-length") {
				ctx.Response.Header.Add(key, C.GoStringN(outHeaders[i].value, C.int(outHeaders[i].value_len)))
			}
		}
		ctx.SetStatusCode(int(scode))
		ctx.SetBody(C.GoBytes(unsafe.Pointer(outBody), C.int(outBodyLen)))
	} else {
		urlPath := C.GoStringN(outURL, C.int(outURLlen))
		if _, err := os.Stat(urlPath); err == nil {
			fasthttp.ServeFile(ctx, urlPath)
		} else {
			ctx.SetStatusCode(404)
		}
	}

	defer C.ffead_cpp_resp_cleanup(fresp)
}

func main() {
	flag.Parse()

	if *serverDirectory == "" {
		log.Fatalf("No ffead-cpp directory provided")
	}

	var srvDirC, srvDirCLen = asPtrAndLength(*serverDirectory)
	fmt.Println("Bootstrapping ffead-cpp start...")
	C.ffead_cpp_bootstrap(srvDirC, srvDirCLen, 12)
	fmt.Println("Bootstrapping ffead-cpp end...")

	fmt.Println("Initializing ffead-cpp start...")
	C.ffead_cpp_init()
	fmt.Println("Initializing ffead-cpp end...")

	h := requestHandler
	if *compress {
		h = fasthttp.CompressHandler(h)
	}

	// init fasthttp server
	server := &fasthttp.Server{
		Name:                          SERVER,
		Handler:                       h,
		DisableHeaderNamesNormalizing: true,
	}

	listenAndServe := server.ListenAndServe
	if *prefork {
		listenAndServe = fastprefork.New(server).ListenAndServe
	}

	*addr = "0.0.0.0:" + *addr
	if err := listenAndServe(*addr); err != nil {
		log.Fatalf("Error in ListenAndServe: %s", err)
	}

	fmt.Println("Cleaning up ffead-cpp start...")
	C.ffead_cpp_cleanup()
	fmt.Println("Cleaning up ffead-cpp end...")
}
