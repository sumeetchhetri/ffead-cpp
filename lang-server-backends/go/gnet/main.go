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
extern void* ffead_cpp_handle_go_2(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, int version,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);
extern void ffead_cpp_resp_cleanup(void* ptr);
*/
import "C"
import (
	"flag"
	"fmt"
	"github.com/panjf2000/gnet"
	"io/ioutil"
	"log"
	"os"
	"reflect"
	"runtime"
	"strconv"
	"strings"
	"time"
	"unsafe"
)

var res string

type request struct {
	proto, method string
	path, query   string
	head, body    string
	remoteAddr    string
}

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

type httpServer struct {
	*gnet.EventServer
}

var errMsg = "Internal Server Error"
var errMsgBytes = []byte(errMsg)

type httpCodec struct {
	req request
}

var (
	//SERVER = name passed in teh server header
	SERVER = "gnet"
)

func (hc *httpCodec) Encode(c gnet.Conn, buf []byte) (out []byte, err error) {
	if c.Context() == nil {
		return buf, nil
	}
	return appendErrResp(out, "500 Internal Server Error", errMsg+"\n"), nil
}

func (hc *httpCodec) Decode(c gnet.Conn) (out []byte, err error) {
	buf := c.Read()
	c.ResetBuffer()

	// process the pipeline
	var leftover []byte
pipeline:
	leftover, err = parseReq(buf, &hc.req)
	// bad thing happened
	if err != nil {
		c.SetContext(err)
		return nil, err
	} else if len(leftover) == len(buf) {
		// request not ready, yet
		return
	}

	var srvStr, srvStrLen = asPtrAndLength(SERVER)
	var methStr, methStrLen = asPtrAndLength(hc.req.method)
	var pathStr, pathStrLen = asPtrAndLength(hc.req.path)
	var hdrsStr, hdrsStrLen = asPtrAndLength(hc.req.head)
	var bodyStr, bodyStrLen = asPtrAndLength(hc.req.body)

	//fmt.Printf("%s %s %s %s %s\n", hc.req.method, hc.req.path, hc.req.query, hc.req.head, hc.req.body)

	var outURL *C.char
	var outURLlen C.size_t
	var outMime *C.char
	var outMimelen C.size_t
	var outHeaders *C.char
	var outHeadersLen C.size_t
	var outBody *C.char
	var outBodyLen C.size_t
	var scode C.int

	fresp := C.ffead_cpp_handle_go_2(srvStr, srvStrLen, methStr, methStrLen, pathStr, pathStrLen, 1,
		hdrsStr, hdrsStrLen, bodyStr, bodyStrLen, &scode, &outURL, &outURLlen, &outMime, &outMimelen, 
		&outHeaders, &outHeadersLen, &outBody, &outBodyLen
	)

	if scode > 0 {
		out = append(out, C.GoBytes(unsafe.Pointer(outHeaders), C.int(outHeadersLen)))
		out = time.Now().AppendFormat(out, "Mon, 02 Jan 2006 15:04:05 GMT\r\n\r\n")
		out = append(out, C.GoBytes(unsafe.Pointer(outBody), C.int(outBodyLen)))
	} else {
		urlPath := C.GoStringN(outURL, C.int(outURLlen))
		if _, err := os.Stat(urlPath); err == nil {
			content, err := ioutil.ReadFile(urlPath)
			if err != nil {
				out = append(out, "HTTP/1.1 404 Not Found\r\nServer: gnet\r\nDate: "...)
				out = time.Now().AppendFormat(out, "Mon, 02 Jan 2006 15:04:05 GMT\r\n\r\n")
				log.Println(err)
			} else {
				out = appendResp(out, content)
			}
		} else {
			out = append(out, "HTTP/1.1 404 Not Found\r\nServer: gnet\r\nDate: "...)
			out = time.Now().AppendFormat(out, "Mon, 02 Jan 2006 15:04:05 GMT\r\n\r\n")
		}
	}

	defer C.ffead_cpp_resp_cleanup(fresp)
	buf = leftover
	goto pipeline
}

func (hs *httpServer) OnInitComplete(srv gnet.Server) (action gnet.Action) {
	log.Printf("HTTP server is listening on %s (multi-cores: %t, loops: %d)\n",
		srv.Addr.String(), srv.Multicore, srv.NumEventLoop)
	return
}

func (hs *httpServer) React(frame []byte, c gnet.Conn) (out []byte, action gnet.Action) {
	if c.Context() != nil {
		// bad thing happened
		out = errMsgBytes
		action = gnet.Close
		return
	}
	// handle the request
	out = frame
	return
}

func main() {
	var port int
	var multicore bool
	var serverDirectory string

	// Example command: go run http.go --port 8080 --multicore=true
	flag.IntVar(&port, "port", 8080, "server port")
	flag.BoolVar(&multicore, "multicore", true, "multicore")
	flag.StringVar(&serverDirectory, "server_directory", "", "TCP address to listen to")
	flag.Parse()
	if serverDirectory == "" {
		log.Fatalf("No ffead-cpp directory provided")
	}

	res = "Hello World!\r\n"

	var srvDirC, srvDirCLen = asPtrAndLength(serverDirectory)
	fmt.Println("Bootstrapping ffead-cpp start...")
	C.ffead_cpp_bootstrap(srvDirC, srvDirCLen, 12)
	fmt.Println("Bootstrapping ffead-cpp end...")

	fmt.Println("Initializing ffead-cpp start...")
	C.ffead_cpp_init()
	fmt.Println("Initializing ffead-cpp end...")

	http := new(httpServer)
	hc := new(httpCodec)

	// Start serving!
	log.Fatal(gnet.Serve(http, fmt.Sprintf("tcp://:%d", port), gnet.WithMulticore(multicore), gnet.WithCodec(hc)))

	fmt.Println("Cleaning up ffead-cpp start...")
	C.ffead_cpp_cleanup()
	fmt.Println("Cleaning up ffead-cpp end...")
}

func appendResp(b []byte, body []byte) []byte {
	b = append(b, "HTTP/1.1 200 OK\r\nServer: gnet\r\nDate: "...)
	b = time.Now().AppendFormat(b, "Mon, 02 Jan 2006 15:04:05 GMT\r\n")
	if len(body) > 0 {
		b = append(b, "Content-Length: "...)
		b = strconv.AppendInt(b, int64(len(body)), 10)
		b = append(b, '\r', '\n')
	}
	b = append(b, '\r', '\n')
	if len(body) > 0 {
		b = append(b, body...)
	}
	return b
}

func appendErrResp(b []byte, status, body string) []byte {
	b = append(b, "HTTP/1.1"...)
	b = append(b, ' ')
	b = append(b, status...)
	b = append(b, '\r', '\n')
	b = append(b, "Server: gnet\r\n"...)
	b = append(b, "Date: "...)
	b = time.Now().AppendFormat(b, "Mon, 02 Jan 2006 15:04:05 GMT")
	b = append(b, '\r', '\n')
	if len(body) > 0 {
		b = append(b, "Content-Length: "...)
		b = strconv.AppendInt(b, int64(len(body)), 10)
		b = append(b, '\r', '\n')
	}
	b = append(b, '\r', '\n')
	if len(body) > 0 {
		b = append(b, body...)
	}
	return b
}

func b2s(b []byte) string {
	return *(*string)(unsafe.Pointer(&b))
}

// parseReq is a very simple http request parser. This operation
// waits for the entire payload to be buffered before returning a
// valid request.
func parseReq(data []byte, req *request) (leftover []byte, err error) {
	req.body = ""
	sdata := b2s(data)
	var i, s int
	var clen int
	// method, path, proto line
	for ; i < len(sdata); i++ {
		if sdata[i] == ' ' {
			req.method = sdata[s:i]
			for i, s = i+1, i+1; i < len(sdata); i++ {
				if sdata[i] == ' ' {
					req.path = sdata[s:i]
					for i, s = i+1, i+1; i < len(sdata); i++ {
						if sdata[i] == '\n' && sdata[i-1] == '\r' {
							req.proto = sdata[s:i]
							i, s = i+1, i+1
							break
						}
					}
					break
				}
			}
			break
		}
	}
	if req.proto == "" {
		return data, fmt.Errorf("malformed request")
	}
	var ps = s
	for ; i < len(sdata); i++ {
		if i > 1 && sdata[i] == '\n' && sdata[i-1] == '\r' {
			line := sdata[s : i-1]
			s = i + 1
			if line == "" {
				req.head = sdata[ps : i+1]
				i++
				if clen > 0 {
					if len(sdata[i:]) < clen {
						break
					}
					req.body = sdata[i : i+clen]
					i += clen
				}
				return data[i:], nil
			}
			if strings.HasPrefix(line, "Content-Length:") {
				n, err := strconv.ParseInt(strings.TrimSpace(line[len("Content-Length:"):]), 10, 64)
				if err == nil {
					clen = int(n)
				}
			} else if strings.HasPrefix(line, "content-length:") {
				n, err := strconv.ParseInt(strings.TrimSpace(line[len("content-length:"):]), 10, 64)
				if err == nil {
					clen = int(n)
				}
			}
		}
	}
	// not enough data
	return data, nil
}
