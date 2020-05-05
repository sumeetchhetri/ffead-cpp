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

#flag -lffead-modules -lffead-framework -ldefault -loauthApp -lflexApp -lmarkers -lpeer_server -lte_benchmark -lte_benchmark_um

//const char* srv, size_t srv_len, int type
fn ffead_cpp_bootstrap(byteptr, u64, int)
//no args
fn ffead_cpp_init()
//no args
fn ffead_cpp_cleanup()

/*const char *server_str, size_t server_str_len, 
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, const char **out_headers, size_t *out_headers_len, 
    const char **out_body, size_t *out_body_len
*/
fn ffead_cpp_handle_return_all_cleanup(byteptr, u64, byteptr, u64, byteptr, int, &int, &string, &u64, &string, &u64, &string, &u64) voidptr

fn ffead_cpp_get_resp_cleanup(voidptr)

const (
	SERVER_NAME = 'VWeb'
	HEADER_SERVER = 'Server: $SERVER_NAME\r\n'
	HEADER_CONNECTION_CLOSE = 'Connection: close\r\n'
	HEADERS_CLOSE = '${HEADER_SERVER}${HEADER_CONNECTION_CLOSE}\r\n'
	HTTP_404 = 'HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n${HEADERS_CLOSE}404 Not Found'
	HTTP_500 = 'HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n${HEADERS_CLOSE}500 Internal Server Error'
	mime_types = {
		'.css': 'text/css; charset=utf-8',
		'.gif': 'image/gif',
		'.htm': 'text/html; charset=utf-8',
		'.html': 'text/html; charset=utf-8',
		'.jpg': 'image/jpeg',
		'.js': 'application/javascript',
		'.wasm': 'application/wasm',
		'.pdf': 'application/pdf',
		'.png': 'image/png',
		'.svg': 'image/svg+xml',
		'.xml': 'text/xml; charset=utf-8'
	}
	MAX_HTTP_POST_SIZE = 1024 * 1024
)

struct VRequestContext {
pub:
	conn net.Socket
mut:
	rq_headers string
	rq_body string

	rs_url string
	rs_headers string
	rs_body string
	done bool
}

fn run(port int) {
	println('Running a Vweb app on http://localhost:$port ...')
	l := net.listen(port) or { panic('Failed to listen') }
	for {
		conn := l.accept() or { panic('accept() failed') }
		handle_conn(conn)
	}
}

fn handle_conn(conn net.Socket) {
	$if debug {
		println('Got a new connection...')
	}

	first_line := conn.read_line()
	vals := first_line.split(' ')
	if vals.len < 3 {
		$if debug {
			println('no vals for http')
		}

		conn.send_string(HTTP_500) or {}
		conn.close() or {}
		return
	}

	mut vweb := VRequestContext{
		conn: conn
	}
	vweb.rq_headers += first_line + "\r\n"

	mut body_len := 0
	mut j := 0
	for {
		line := conn.read_line()
		sline := strip(line)
		if sline == '' {
			break
		} else {
			vweb.rq_headers += sline + "\r\n"
			if sline.starts_with('Content-Length') {
				body_len = sline.all_after(': ').int()
			}
		}

		if j >= 200 {
			break
		}
	}

	if body_len > 0 {
		buf := [1024]byte
		mut len := body_len
		if len > 1024 {
			len = 1024
		}
		for {
			m := conn.crecv(buf, len)
			vweb.rq_body += string(byteptr(buf), m)
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
		println('req.method = $vals[0]')
		println('req.path = $vals[1]')
		println('req.version = $vals[2]')
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
fn (vweb VRequestContext) init() {
}

fn (vweb VRequestContext) clean() {
}

fn (vweb VRequestContext) send_response_to_client(mimetype string, res string) bool {
	mut sb := strings.new_builder(1024)
	sb.write('HTTP/1.1 200 OK\r\nContent-Type: $mimetype\r\nContent-Length: ')
	sb.write(res.len.str())
	sb.write('\r\n$HEADERS_CLOSE$res')
	vweb.conn.send_string(sb.str()) or {
		sb.free()
		return false 
	}
	sb.free()
	return true
}

fn (vweb VRequestContext) not_found() {
	vweb.conn.send_string(HTTP_404) or { return }
}

fn (vweb VRequestContext) send_response() bool {
	$if debug {
		println('res.headers = $vweb.rs_headers')
		println('res.body = $vweb.rs_body')
	}

	mut sb := strings.new_builder(1024)
	sb.write('$vweb.rs_headers$vweb.rs_body')
	vweb.conn.send_string(sb.str()) or { return false }
	sb.free()
	return true
}

fn (vweb mut VRequestContext) handle() {
	server_name := 'VWeb'
	url_len := u64(0)
	hdrs_len := u64(0)
	body_len := u64(0)
	done := 0
	resp := ffead_cpp_handle_return_all_cleanup(server_name.str, u64(server_name.len), vweb.rq_headers.str, u64(vweb.rq_headers.len), 
		vweb.rq_body.str, vweb.rq_body.len, &done, &vweb.rs_url, &url_len, &vweb.rs_headers, &hdrs_len, &vweb.rs_body, &body_len)
	vweb.rs_url = tos(vweb.rs_url.str, int(url_len))
	vweb.rs_headers = tos(vweb.rs_headers.str, int(hdrs_len))

	$if debug {
		println('ffead-cpp.done = $done')
	}

	if done == 1 {
		vweb.rs_body = tos(vweb.rs_body.str, int(body_len))
		vweb.send_response()
		ffead_cpp_get_resp_cleanup(resp)
	} else {
		ffead_cpp_get_resp_cleanup(resp)
		$if debug {
			println('res.url = $vweb.rs_url')
		}

		// Serve a static file if it's one
		ext := file_ext(vweb.rs_url)
		mime_type := mime_types[ext]
		
		$if debug {
			println('res.mime_type = $mime_type')
		}

		if vweb.rs_url != '' && mime_type != '' {
			data := os.read_file(vweb.rs_url) or {
				$if debug {
					println('file not found')
				}

				vweb.not_found()
				return
			}
			vweb.send_response_to_client(mime_type, data)
			return
		}

		$if debug {
			println('file not found')
		}

		vweb.not_found()
		return
	}
}

fn main() {
	mut server_directory := './ffead-cpp-bin'
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
	ffead_cpp_bootstrap(server_directory.str, u64(server_directory.len), 7)
	println('Bootstrapping ffead-cpp end...')

	println('Initializing ffead-cpp start...')
	ffead_cpp_init()
	println('Initializing ffead-cpp end...')

	run(server_port)

	println('Cleaning up ffead-cpp start...')
	ffead_cpp_cleanup()
	println('Cleaning up ffead-cpp end...')
}