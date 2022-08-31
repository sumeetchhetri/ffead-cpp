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
//#flag ffead-cpp-util.o
#include "ffead-cpp.h"
#flag -lffead-framework

//const char* srv, size_t srv_len, int type
fn C.ffead_cpp_bootstrap(byteptr, u64, int)
//no args
fn C.ffead_cpp_init()
type Cb_reg_ext_fd_pv = fn (int, voidptr)
fn C.ffead_cpp_init_for_pv(Cb_reg_ext_fd_pv)
//no args
fn C.ffead_cpp_cleanup()
fn C.fc_memcpy(voidptr, byteptr, u64) byteptr
fn C.get_date_2() byteptr
type Cb_into_pv = fn (hline byteptr, hline_len u64, body byteptr, body_len u64, fd int, pv voidptr) int

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
    fd int
    writer voidptr
}

struct C.ffead_request3_t {}

/*
	const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len, 
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
*/
fn C.ffead_cpp_handle_picov_1(&C.ffead_request3, &int, &string, &u64, &string, &u64, &string, &u64, &C.phr_header, &u64, &string, &u64) voidptr
fn C.ffead_cpp_handle_picov_2(&C.ffead_request3)
fn C.ffead_cpp_handle_picov_2_init_sock(int, voidptr, Cb_into_pv) voidptr
fn C.ffead_cpp_handle_picov_2_deinit_sock(int, voidptr)
fn C.ffead_cpp_handle_picov_ext_fd_cb(int, voidptr)

fn C.ffead_cpp_resp_cleanup(voidptr)

fn cpy_str_1(dst byteptr, src string) byteptr {
	return C.fc_memcpy(dst, src.str, src.len)
}

const (
	hdr_sep = ': '
	hdr_del = '\r\n'
	hdr_end = '\r\n\r\n'
	hdr_end_ = '\r\n\r\n'
)

fn callback(req picohttpparser.Request, mut res picohttpparser.Response) {
	/*$if debug {
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
	}*/
	
 	freq := C.ffead_request3{
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
	    fd: -1
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
	
	resp := C.ffead_cpp_handle_picov_1(&freq, &scode, &smsg, &smsg_len, &out_mime, &out_mime_len, &out_url, &out_url_len, &req.headers[0], &headers_len, &out_body, &out_body_len)

	/*$if debug {
		println('ffead-cpp.scode = $scode')
	}*/

	if scode > 0 {
		if scode == 200 {
			res.http_ok()
		} else {
			smsg = tos(smsg.str, int(smsg_len))
			res.buf = cpy_str_1(res.buf, "HTTP/1.1 ${scode} ${smsg}\r\n")
		}
		
		res.header_server()
		res.header_date()
		
		mut j := 0
		for {
			if j == int(headers_len) {
				break
			}
			
			res.buf = C.fc_memcpy(res.buf, req.headers[j].name, req.headers[j].name_len)
			res.buf = C.fc_memcpy(res.buf, hdr_sep.str, hdr_sep.len)
			res.buf = C.fc_memcpy(res.buf, req.headers[j].value, req.headers[j].value_len)
			res.buf = C.fc_memcpy(res.buf, hdr_del.str, hdr_del.len)
			j = j + 1
		}
		
		res.buf = C.fc_memcpy(res.buf, hdr_del.str, hdr_del.len)
		res.buf = C.fc_memcpy(res.buf, out_body.str, out_body_len)
		
		C.ffead_cpp_resp_cleanup(resp)
	} else {
		out_mime = tos(out_mime.str, int(out_mime_len))
		out_url = tos(out_url.str, int(out_url_len))
		
		/*$if debug {
			println('res.url = $out_url')
			println('res.mime_type = $out_mime')
		}*/

		if out_url != '' && out_mime != '' {
			data := os.read_file(out_url) or {
				$if debug {
					println('file not found')
				}

				res.buf = cpy_str_1(res.buf, "HTTP/1.1 404 Not Found\r\n")
				res.header_server()
				res.header_date()
				res.buf = cpy_str_1(res.buf, "Content-Length: 0\r\n\r\n")
				
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

		/*$if debug {
			println('file not found')
		}*/

		res.buf = cpy_str_1(res.buf, "HTTP/1.1 404 Not Found\r\n")
		res.header_server()
		res.header_date()
		res.buf = cpy_str_1(res.buf, "Content-Length: 0\r\n\r\n")
		
		C.ffead_cpp_resp_cleanup(resp)
		return
	}
}

fn fcp_callback_write(hline byteptr, hline_len u64, body byteptr, body_len u64, fd int, pv voidptr) int {
	mut p := &picoev.Picoev(pv)
	mut out := p.out
	unsafe {
		out += fd * picoev.max_write
		out += p.oidx[fd]
	}
	buf_start := out
	out = C.fc_memcpy(out, hline, hline_len)
	out = C.fc_memcpy(out, p.date, 51)
	if body_len > 0 {
		out = C.fc_memcpy(out, "Content-Length: ", 16)
		unsafe {
			out += C.u64toa(out, body_len)
		}
		out = C.fc_memcpy(out, hdr_end.str, 4)
		out = C.fc_memcpy(out, body, body_len)
	} else {
		out = C.fc_memcpy(out, hdr_end.str, 4)
	}
	n := int(out) - int(buf_start)
	if C.write(fd, buf_start, n) != n {
		return -1
	}
	return n
}

fn open_cb_async(fd int) voidptr {
	return C.ffead_cpp_handle_picov_2_init_sock(fd, &pv, &fcp_callback_write)
}

fn close_cb_async(fd int, data voidptr) {
	C.ffead_cpp_handle_picov_2_deinit_sock(fd, data)
}

fn callback_async(req picohttpparser.Request, fd int, writer voidptr) {
 	freq := C.ffead_request3{
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
	    fd: fd
	    writer: writer
	}
	
	C.ffead_cpp_handle_picov_2(&freq)
}

__global pv picoev.Picoev

fn main() {
	mut server_directory := '/installs/ffead-cpp-6.0'
	mut server_port := 8080
	mut is_async := false

	mut fp := flag.new_flag_parser(os.args)
	fp.application('picov + ffead-cpp')
	fp.version('v1.0')
	fp.description('ffead-cpp over picov server')
	fp.skip_executable()

	server_directory = fp.string('server_dir', 0, '', 'ffead-cpp server Directory is required')
	server_port = fp.int('server_port', 0, 8080, 'Server port is required')
	is_async = fp.bool('is_async', 0, false, 'Async mode flag is required')

	println('Received $server_directory for server dirfrom args')
	println('Received $server_port for server port from args')

	//assert '' != server_directory
	//assert 0 < server_port

	println('Bootstrapping ffead-cpp start...')
	C.ffead_cpp_bootstrap(server_directory.str, u64(server_directory.len), 10)
	println('Bootstrapping ffead-cpp end...')

	if is_async {
		mut pv = picoev.new(server_port, &callback_async, &open_cb_async, &close_cb_async, &C.ffead_cpp_handle_picov_ext_fd_cb, true)
	} else {
		mut pv = picoev.new(server_port, &callback, &open_cb_async, &close_cb_async, &C.ffead_cpp_handle_picov_ext_fd_cb, false)
	}

	println('Initializing ffead-cpp start...')
	if !is_async {
		C.ffead_cpp_init()
	} else {
		C.ffead_cpp_init_for_pv(&picoev.register_external_fd)
	}
	println('Initializing ffead-cpp end...')
	
	go update_date()
	pv.serve()

	println('Cleaning up ffead-cpp start...')
	C.ffead_cpp_cleanup()
	println('Cleaning up ffead-cpp end...')
}

fn update_date() {
	for {
		pv.date = C.get_date_2()
		C.usleep(1000000)
	}
}