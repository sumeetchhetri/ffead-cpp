/*
	Copyright 2009-2020, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 
  
        http://www.apache.org/licenses/LICENSE-2.0 
  
    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
*/

import os
import net
import strings
import flag

#flag -I./
#flag dtutil.o
#flag -lffead-framework

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>
#include "ffead-cpp.h"

fn C.get_date() byteptr

//const char* srv, size_t srv_len, int type
fn C.ffead_cpp_bootstrap(byteptr, u64, int)
//no args
fn C.ffead_cpp_init()
//no args
fn C.ffead_cpp_cleanup()

/*
	const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len, 
    const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
*/
fn C.ffead_cpp_handle_v(byteptr, u64, byteptr, u64, byteptr, u64, &int, &string, &u64, &string, &u64, &string, &u64, &string, &u64) voidptr

fn C.ffead_cpp_resp_cleanup(voidptr)

const (
	server_name = 'VWeb'
	header_server = 'Server: $server_name\r\n'
	header_connection_close = 'Connection: close\r\n'
	headers_close = '${header_server}${header_connection_close}\r\n'
	http_404 = 'HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n${headers_close}'
	http_500 = 'HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n${headers_close}'
)

struct FfeadCppContext {
pub:
	conn net.Socket
mut:
	rq_headers string
	rq_body string

	rs_url string
	rs_mime string
	rs_body string
	rs_headers string
	rs_date byteptr
}

struct DateStore {
mut:
	date byteptr
}

fn update_date(mut p DateStore) {
	for {
		p.date = C.get_date()
		C.usleep(1000000)
	}
}

fn run(port int) {
	mut dt := DateStore{
		date: C.get_date()
	}

	println('Running a Vweb app on http://localhost:$port ...')
	mut l := net.new_socket(C.AF_INET, C.SOCK_STREAM, 0) or {
		panic('Failed to create socket')
	}
	flag := 1
	assert C.setsockopt(l.sockfd, C.SOL_SOCKET, C.SO_REUSEADDR, &flag, sizeof(int)) == 0
	assert C.setsockopt(l.sockfd, C.SOL_SOCKET, C.SO_REUSEPORT, &flag, sizeof(int)) == 0
	$if linux {
		assert C.setsockopt(l.sockfd, C.IPPROTO_TCP, C.TCP_QUICKACK, &flag, sizeof(int)) == 0
		timeout := 10
		assert C.setsockopt(l.sockfd, C.IPPROTO_TCP, C.TCP_DEFER_ACCEPT, &timeout, sizeof(int)) == 0
		queue_len := 4096
		assert C.setsockopt(l.sockfd, C.IPPROTO_TCP, C.TCP_FASTOPEN, &queue_len, sizeof(int)) == 0
	}
	on := 1
	if C.setsockopt(l.sockfd, C.IPPROTO_TCP, C.TCP_NODELAY, &on, sizeof(int)) < 0 {
		println('setup_sock.setup_sock failed')
	}
	/*if C.fcntl(l.sockfd, C.F_SETFL, C.O_NONBLOCK) != 0 {
		println('fcntl failed')
	}*/
	l.bind(port) or {
		panic('Failed to bind socket')
	}
	l.listen_backlog(C.SOMAXCONN) or {
		panic('Failed to listen')
	}
	for {
		conn := l.accept() or { panic('accept() failed') }
		if C.setsockopt(conn.sockfd, C.IPPROTO_TCP, C.TCP_NODELAY, &on, sizeof(int)) < 0 {
			println('setup_sock.setup_sock failed')
		}
		/*if C.fcntl(conn.sockfd, C.F_SETFL, C.O_NONBLOCK) != 0 {
			println('fcntl failed')
		}*/
		handle_conn(conn, dt)
		go update_date(mut &dt)
	}
}

fn handle_conn(conn net.Socket, dt DateStore) {
	$if debug {
		println('Got a new connection...')
	}

	first_line := conn.read_line()
	vals := first_line.split(' ')
	if vals.len < 3 {
		$if debug {
			println('no vals for http')
		}

		conn.send_string(http_500) or {}
		conn.close() or {}
		return
	}

	mut vweb := FfeadCppContext{
		conn: conn
		rs_date: dt.date
	}
	vweb.rq_headers += first_line

	mut body_len := 0
	mut j := 0
	for {
		sline := conn.read_line()
		if sline == '\r\n' {
			break
		} else {
			vweb.rq_headers += sline
			if sline.starts_with('Content-Length') {
				body_len = sline.all_after(': ').int()
			} else if sline.starts_with('content-length') {
				body_len = sline.all_after(': ').int()
			}
		}

		if j >= 200 {
			break
		}
	}
	
	vweb.rq_headers += "\r\n"

	if body_len > 0 {
		buf := unsafe {malloc(1024)}
		mut len := body_len
		if len > 1024 {
			len = 1024
		}
		for {
			m := conn.crecv(buf, len)
			unsafe {
				vweb.rq_body += buf.vstring_with_len(m)
			}
			body_len -= m
			len = body_len
			if len > 1024 {
				len = 1024
			}
			if body_len == 0 { 
				break 
			}
		}
	}

	$if debug {
		println('req.headers = $vweb.rq_headers')
		println('req.body = $vweb.rq_body')
	}

	vweb.init()
	vweb.handle()

	conn.close() or {}
	vweb.clean()

	$if debug {
		println('Closing connection...')
	}
}

fn file_ext(path string) string {
	pos := path.last_index('.') or {
		return ''
	}
	return path[pos..]
}

fn strip(s string) string {
	return s.trim('\r\n')
}

//----------------------------------------------------------------\\
fn (vweb FfeadCppContext) init() {
}

fn (vweb FfeadCppContext) clean() {
}

fn (vweb FfeadCppContext) send_response_to_client(res string) bool {
	mut sb := strings.new_builder(1024)
	sb.write('HTTP/1.1 200 OK\r\nContent-Type: ${vweb.rs_mime}\r\nContent-Length: ')
	sb.write(res.len.str())
	sb.write('\r\nDate: ')
	sb.write_bytes(vweb.rs_date, 29)
	sb.write('\r\n$headers_close$res')
	vweb.conn.send_string(sb.str()) or {
		sb.free()
		return false 
	}
	sb.free()
	return true
}

fn (vweb FfeadCppContext) not_found() {
	vweb.conn.send_string(http_404) or { return }
}

fn (vweb FfeadCppContext) send_response() bool {
	$if debug {
		println('headers = $vweb.rs_headers')
		println('response = $vweb.rs_body')
	}

	mut sb := strings.new_builder(1024)
	sb.write('${vweb.rs_headers}Date: ')
	sb.write_bytes(vweb.rs_date, 29)
	sb.write('\r\n\r\n$vweb.rs_body')
	vweb.conn.send_string(sb.str()) or { return false }
	sb.free()
	return true
}

fn (mut vweb FfeadCppContext) handle() {
	server_name := 'vweb'
	url_len := u64(0)
	mime_len := u64(0)
	headers_len := u64(0)
	body_len := u64(0)
	scode := 0
	
	resp := C.ffead_cpp_handle_v(server_name.str, u64(server_name.len), vweb.rq_headers.str, u64(vweb.rq_headers.len), 
		vweb.rq_body.str, u64(vweb.rq_body.len), &scode, &vweb.rs_url, &url_len, &vweb.rs_mime, &mime_len, 
		&vweb.rs_headers, &headers_len, &vweb.rs_body, &body_len)

	$if debug {
		println('ffead-cpp.scode = $scode')
	}

	if scode > 0 {
		vweb.rs_headers = tos(vweb.rs_headers.str, int(headers_len))
		vweb.rs_body = tos(vweb.rs_body.str, int(body_len))
		vweb.send_response()
		C.ffead_cpp_resp_cleanup(resp)
	} else {
		vweb.rs_url = tos(vweb.rs_url.str, int(url_len))
		vweb.rs_mime = tos(vweb.rs_mime.str, int(mime_len))
		
		$if debug {
			println('res.url = $vweb.rs_url')
		}
		
		$if debug {
			println('res.mime_type = $vweb.rs_mime')
		}

		if vweb.rs_url != '' && vweb.rs_mime != '' {
			data := os.read_file(vweb.rs_url) or {
				$if debug {
					println('file not found')
				}

				vweb.not_found()
				C.ffead_cpp_resp_cleanup(resp)
				return
			}
			vweb.send_response_to_client(data)
			C.ffead_cpp_resp_cleanup(resp)
			return
		}

		$if debug {
			println('file not found')
		}

		vweb.not_found()
		C.ffead_cpp_resp_cleanup(resp)
		return
	}
}

fn main() {
	mut server_directory := '/installs/ffead-cpp-5.0'
	mut server_port := 8080

	mut fp := flag.new_flag_parser(os.args)
	fp.application('vweb + ffead-cpp')
	fp.version('v1.0')
	fp.description('ffead-cpp over vweb server')
	fp.skip_executable()

	server_directory = fp.string('server_dir', 0, '', 'ffead-cpp server Directory is required')
	server_port = fp.int('server_port', 0, 8080, 'Server port is required')

	println('Received $server_directory for server dirfrom args')
	println('Received $server_port for server port from args')

	//assert '' != server_directory
	//assert 0 < server_port

	println('Bootstrapping ffead-cpp start...')
	C.ffead_cpp_bootstrap(server_directory.str, u64(server_directory.len), 9)
	println('Bootstrapping ffead-cpp end...')

	println('Initializing ffead-cpp start...')
	C.ffead_cpp_init()
	println('Initializing ffead-cpp end...')

	run(server_port)

	println('Cleaning up ffead-cpp start...')
	C.ffead_cpp_cleanup()
	println('Cleaning up ffead-cpp end...')
}