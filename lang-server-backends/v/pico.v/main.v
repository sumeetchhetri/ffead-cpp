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

import os
import picoev
import picohttpparser
import flag

#flag -I ./
#include "ffead-cpp.h"
#flag -lffead-framework

//const char* srv, size_t srv_len, int type
fn C.ffead_cpp_bootstrap(byteptr, u64, int)
//no args
fn C.ffead_cpp_init()
//no args
fn C.ffead_cpp_cleanup()

struct C.ffead_request3 {
pub mut:
	server_str byteptr
    server_str_len u64
    method byteptr
    method_len u64
    path byteptr
    path_len u64
    qstr byteptr
    qstr_len u64
    headers &C.phr_header
    headers_len u64
    body byteptr
    body_len u64
    version int
}
struct C.ffead_request3_t {}

/*
	const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len, 
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
*/
fn C.ffead_cpp_handle_crystal_picov_1(&C.ffead_request3, &int, &string, &u64, &string, &u64, &string, &u64, &C.phr_header, &u64, &string, &u64) voidptr

fn C.ffead_cpp_resp_cleanup(voidptr)


fn cpy_str_1(dst byteptr, src string) int {
	unsafe {C.memcpy(dst, src.str, src.len)}
	return src.len
}

fn callback(req picohttpparser.Request, mut res picohttpparser.Response) {
	$if debug {
		println('${req.method} ${req.path} ${req.num_headers}')
		mut j := 0
		for {
			if j == req.num_headers {
				break
			}
			k := tos(req.headers[j].name, req.headers[j].name_len)
			v := tos(req.headers[j].value, req.headers[j].value_len)
			$if debug {
				println('${k} ${v}')
			}
			j = j+1
		}
	}
	
 	freq := C.ffead_request3{
	    server_str: 'picov'.str
	    server_str_len: u64(5)
	    method: req.method.str
	    method_len: u64(req.method.len)
	    path: req.path.str
	    path_len: u64(req.path.len)
	    qstr: byteptr(0)
	    qstr_len: 0
	    headers: &req.headers[0]
	    headers_len: req.num_headers
	    body: byteptr(0)
	    body_len: 0
	    version: 1
	}
	
	scode := 0
	mut smsg := ''
	smsg_len := u64(0)
	mut out_mime := ''
	out_mime_len := u64(0)
	mut out_url := ''
	out_url_len := u64(0)
	mut out_body := ''
	out_body_len := u64(0)
	headers_len := u64(0)
	
	resp := C.ffead_cpp_handle_crystal_picov_1(&freq, &scode, &smsg, &smsg_len, &out_mime, &out_mime_len, &out_url, &out_url_len, &req.headers[0], &headers_len, &out_body, &out_body_len)

	$if debug {
		println('ffead-cpp.scode = $scode')
	}

	if scode > 0 {
		smsg = tos(smsg.str, int(smsg_len))
		unsafe {
			res.buf += cpy_str_1(res.buf, "HTTP/1.1 ${scode} ${smsg}\r\n")
		}
		res.header_server()
		res.header_date()
		j = 0
		for {
			if j == int(headers_len) {
				break
			}
			k := tos(req.headers[j].name, int(req.headers[j].name_len))
			v := tos(req.headers[j].value, int(req.headers[j].value_len))
			unsafe {
				res.buf += cpy_str_1(res.buf, "${k}: ${v}\r\n")
			}
			j = j + 1
		}
		out_body = tos(out_body.str, int(out_body_len))
		res.body(out_body)
		C.ffead_cpp_resp_cleanup(resp)
	} else {
		out_mime = tos(out_mime.str, int(out_mime_len))
		out_url = tos(out_url.str, int(out_url_len))
		
		$if debug {
			println('res.url = $out_url')
			println('res.mime_type = $out_mime')
		}

		if out_url != '' && out_mime != '' {
			data := os.read_file(out_url) or {
				$if debug {
					println('file not found')
				}

				res.http_404()
				C.ffead_cpp_resp_cleanup(resp)
				return
			}
			res.http_ok()
			res.header_server()
			res.header_date()
			res.content_type(out_mime)
			res.body(data)
			C.ffead_cpp_resp_cleanup(resp)
			return
		}

		$if debug {
			println('file not found')
		}

		res.http_404()
		C.ffead_cpp_resp_cleanup(resp)
		return
	}
}

fn main() {
	mut server_directory := '/installs/ffead-cpp-5.0'
	mut server_port := 8080

	mut fp := flag.new_flag_parser(os.args)
	fp.application('picov + ffead-cpp')
	fp.version('v1.0')
	fp.description('ffead-cpp over picov server')
	fp.skip_executable()

	server_directory = fp.string('server_dir', 0, '', 'ffead-cpp server Directory is required')
	server_port = fp.int('server_port', 0, 8080, 'Server port is required')

	println('Received $server_directory for server dirfrom args')
	println('Received $server_port for server port from args')

	//assert '' != server_directory
	//assert 0 < server_port

	println('Bootstrapping ffead-cpp start...')
	C.ffead_cpp_bootstrap(server_directory.str, u64(server_directory.len), 10)
	println('Bootstrapping ffead-cpp end...')

	println('Initializing ffead-cpp start...')
	C.ffead_cpp_init()
	println('Initializing ffead-cpp end...')

	picoev.new(server_port, &callback).serve()

	println('Cleaning up ffead-cpp start...')
	C.ffead_cpp_cleanup()
	println('Cleaning up ffead-cpp end...')
}