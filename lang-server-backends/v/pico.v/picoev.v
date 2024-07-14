// Copyright (c) 2019-2020 Alexander Medvednikov. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.
module picoev

import picohttpparser

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>

#flag -I @VROOT/thirdparty/picoev
#flag -L @VROOT/thirdparty/picoev
#flag @VROOT/thirdparty/picoev/picoev.o

#include "src/picoev.h"

const (
	max_fds = 2048
	timeout_secs = 8
	max_timeout = 10
	max_read = 4096
	max_write = 8192
)

struct C.in_addr {
mut:
	s_addr int
}

struct C.sockaddr_in {
mut:
	sin_family int
	sin_port   int
	sin_addr   C.in_addr
}

struct C.sockaddr_storage {}

fn C.atoi() int
fn C.strncasecmp() int
fn C.socket() int
fn C.setsockopt() int
fn C.htonl() int
fn C.htons() int
fn C.bind() int
fn C.listen() int
fn C.accept() int
fn C.getaddrinfo() int
fn C.connect() int
fn C.send() int
fn C.recv() int
//fn C.read() int
fn C.shutdown() int
//fn C.close() int
fn C.ntohs() int
fn C.getsockname() int

fn C.fcntl() int
//fn C.write() int
struct C.picoev_loop {}

struct Picoev {
	loop &C.picoev_loop
	is_async bool
	is_cloop bool
	cb   fn(req picohttpparser.Request, mut res picohttpparser.Response)
	cb1  fn(req picohttpparser.Request, fd int, pv voidptr)
	open_cb fn(fd int) voidptr
	close_cb fn(fd int, fd_data voidptr)
	cb_ext_fd_cb fn(int, voidptr)
pub mut:
	cloop &C.picoev_loop
	date byteptr
	buf  byteptr
	idx  [2048]int
	out  byteptr
	oidx [2048]int
	data [2048]voidptr
}

fn C.picoev_del(&C.picoev_loop, int) int
fn C.picoev_set_timeout(&C.picoev_loop, int, int)
fn C.picoev_add(&C.picoev_loop, int, int, int, &C.picoev_handler, voidptr) int
fn C.picoev_init(int) int
fn C.picoev_create_loop(int) &C.picoev_loop
fn C.picoev_loop_once(&C.picoev_loop, int) int
fn C.picoev_destroy_loop(&C.picoev_loop) int
fn C.picoev_deinit() int
fn C.phr_parse_request() int
fn C.phr_parse_request_path_pipeline() int
fn C.phr_parse_request_path() int

[inline]
fn setup_sock(fd int) {
	on := 1
	if C.setsockopt(fd, C.IPPROTO_TCP, C.TCP_NODELAY, &on, sizeof(int)) < 0 {
		println('setup_sock.setup_sock failed')
	}
	if C.fcntl(fd, C.F_SETFL, C.O_NONBLOCK) != 0 {
		println('fcntl failed')
	}
}

[inline]
fn close_conn(loop &C.picoev_loop, fd int) {
	C.picoev_del(loop, fd)
	C.close(fd)
}

[inline]
fn myread(fd int, b byteptr, max_len, idx int) int {
	unsafe {
		return C.read(fd, b + idx, max_len - idx)
	}
}

[inline]
fn mysubstr(s byteptr, from, len int) string {
	unsafe {
		return tos(s + from, len)
	}
}

fn rw_callback(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	mut p := &Picoev(cb_arg)
	if (events & C.PICOEV_TIMEOUT) != 0 {
		close_conn(loop, fd)
		p.idx[fd] = 0
		return
	}
	else if (events & C.PICOEV_READ) != 0 {
		C.picoev_set_timeout(loop, fd, timeout_secs)
		mut buf := p.buf
		unsafe {
			buf += fd * max_read
		}
		idx := p.idx[fd]
		mut r := myread(fd, buf, max_read, idx)
		if r == 0 {
			close_conn(loop, fd)
			p.idx[fd] = 0
			return
		} else if r == -1 {
			if false { //errno == C.EAGAIN || errno == C.EWOULDBLOCK {
				// TODO
			} else {
				close_conn(loop, fd)
				p.idx[fd] = 0
				return
			}
		} else {
			r += idx
			mut s := tos(buf, r)
			mut out := p.out
			unsafe {
				out += fd * max_write
			}
			mut res := picohttpparser.Response{
				fd: fd
				date: p.date
				buf_start: out
				buf: out
			}
			unsafe {
				res.buf += p.oidx[fd]
			}
			mut req := picohttpparser.Request{}
			for {
				pret := req.parse_request(s, 100)
				if pret <= 0 && s.len > 0 {
					unsafe { C.memmove(buf, s.str, s.len) }
					p.idx[fd] = s.len
					p.oidx[fd] = int(res.buf) - int(res.buf_start)
					break
				}
				c0 := unsafe { req.method.str[0] }
				if c0 ==`p` || c0 == `P` || c0 == `d` || c0 == `D`  {
					mut j := 0
					for {
						if j == req.num_headers {
							break
						}
						if req.headers[j].name_len == 14 && C.strncasecmp(req.headers[j].name, "content-length", 14) == 0 {
							//cont_length := C.atoi(tos(req.headers[j].value, req.headers[j].value_len).str)
							//println('$cont_length')
							//TODO need to maintain state of incomplete request to collect body later
						}
						j = j+1
					}
				}
				p.cb(req, mut &res)
				if pret >= s.len {
					p.idx[fd] = 0
					p.oidx[fd] = 0
					if res.end() < 0 {
						close_conn(loop, fd)
						return
					}
					break
				}
				s = mysubstr(buf, pret, s.len - pret)
			}
		}
	}
}

fn accept_callback(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	newfd := C.accept(fd, 0, 0)
	if newfd != -1 {
		setup_sock(newfd)
		C.picoev_add(loop, newfd, C.PICOEV_READ, timeout_secs, rw_callback, cb_arg)
	}
}

fn accept_callback_cloop(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	newfd := C.accept(fd, 0, 0)
	if newfd != -1 {
		setup_sock(newfd)
		mut p := &Picoev(cb_arg)
		C.picoev_add(p.cloop, newfd, C.PICOEV_READ, timeout_secs, rw_callback, cb_arg)
	}
}

pub fn external_fd_rw_callback(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	if (events & C.PICOEV_READ) != 0 {
		pv.cb_ext_fd_cb(fd, cb_arg)
	}
}

pub fn register_external_fd(fd int, cb_arg voidptr) {
	C.picoev_add(pv.loop, fd, C.PICOEV_READ, 0, external_fd_rw_callback, cb_arg)
}

fn rw_callback_async(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	mut p := &Picoev(cb_arg)
	if (events & C.PICOEV_TIMEOUT) != 0 {
		close_conn(loop, fd)
		p.close_cb(fd, p.data[fd])
		p.idx[fd] = 0
		return
	}
	else if (events & C.PICOEV_READ) != 0 {
		C.picoev_set_timeout(loop, fd, timeout_secs)
		mut buf := p.buf
		unsafe {
			buf += fd * max_read
		}
		idx := p.idx[fd]
		mut r := myread(fd, buf, max_read, idx)
		if r == 0 {
			close_conn(loop, fd)
			p.close_cb(fd, p.data[fd])
			p.idx[fd] = 0
			p.data[fd] = 0
			return
		} else if r == -1 {
			if false { //errno == C.EAGAIN || errno == C.EWOULDBLOCK {
				// TODO
			} else {
				close_conn(loop, fd)
				p.close_cb(fd, p.data[fd])
				p.idx[fd] = 0
				p.data[fd] = 0
				return
			}
		} else {
			r += idx
			mut s := tos(buf, r)
			mut out := p.out
			unsafe {
				out += fd * max_write
			}
			mut res := picohttpparser.Response{
				fd: fd
				date: p.date
				buf_start: out
				buf: out
			}
			unsafe {
				res.buf += p.oidx[fd]
			}
			mut req := picohttpparser.Request{}
			for {
				pret := req.parse_request(s, 100)
				if pret <= 0 && s.len > 0 {
					//unsafe { C.memmove(buf, s.str, s.len) }
					p.idx[fd] = s.len
					p.oidx[fd] = int(res.buf) - int(res.buf_start)
					break
				}
				p.cb1(req, fd, p.data[fd])
				if pret >= s.len {
					p.idx[fd] = 0
					p.oidx[fd] = 0
					break
				}
				s = mysubstr(buf, pret, s.len - pret)
			}
		}
	}
}

fn accept_callback_async(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	newfd := C.accept(fd, 0, 0)
	if newfd != -1 {
		setup_sock(newfd)
		mut p := &Picoev(cb_arg)
		p.data[newfd] = p.open_cb(newfd)
		C.picoev_add(loop, newfd, C.PICOEV_READ, timeout_secs, rw_callback_async, cb_arg)
	}
}

fn accept_callback_async_cloop(loop &C.picoev_loop, fd, events int, cb_arg voidptr) {
	newfd := C.accept(fd, 0, 0)
	if newfd != -1 {
		setup_sock(newfd)
		mut p := &Picoev(cb_arg)
		p.data[newfd] = p.open_cb(newfd)
		C.picoev_add(p.cloop, newfd, C.PICOEV_READ, timeout_secs, rw_callback_async, cb_arg)
	}
}

__global pv Picoev
pub fn (pv Picoev) listen(port int) {
	fd := C.socket(C.AF_INET, C.SOCK_STREAM, 0)
	assert fd != -1

	flag := 1
	assert C.setsockopt(fd, C.SOL_SOCKET, C.SO_REUSEADDR, &flag, sizeof(int)) == 0
	assert C.setsockopt(fd, C.SOL_SOCKET, C.SO_REUSEPORT, &flag, sizeof(int)) == 0
	$if linux {
		assert C.setsockopt(fd, C.IPPROTO_TCP, C.TCP_QUICKACK, &flag, sizeof(int)) == 0
		timeout := 10
		assert C.setsockopt(fd, C.IPPROTO_TCP, C.TCP_DEFER_ACCEPT, &timeout, sizeof(int)) == 0
		queue_len := 4096
		assert C.setsockopt(fd, C.IPPROTO_TCP, C.TCP_FASTOPEN, &queue_len, sizeof(int)) == 0
	}

	mut addr := C.sockaddr_in{}
	addr.sin_family = C.AF_INET
	addr.sin_port = C.htons(port)
	addr.sin_addr.s_addr = C.htonl(C.INADDR_ANY)
	size := 16 // sizeof(C.sockaddr_in)
	bind_res := C.bind(fd, &addr, size)
	assert bind_res == 0

	listen_res := C.listen(fd, C.SOMAXCONN)
	assert listen_res == 0

	setup_sock(fd)

	if !pv.is_async {
		if !pv.is_cloop {
			C.picoev_add(pv.loop, fd, C.PICOEV_READ, 0, accept_callback, &pv)
		} else {
			C.picoev_add(pv.loop, fd, C.PICOEV_READ, 0, accept_callback_cloop, &pv)
		}
	} else {
		if !pv.is_cloop {
			C.picoev_add(pv.loop, fd, C.PICOEV_READ, 0, accept_callback_async, &pv)
		} else {
			C.picoev_add(pv.loop, fd, C.PICOEV_READ, 0, accept_callback_async_cloop, &pv)
		}
	}
}
pub fn new(port int, cb voidptr, open_cb voidptr, close_cb voidptr, cb_ext_fd_cb voidptr, is_async bool, is_cloop bool) &Picoev {
	C.picoev_init(max_fds)
	loop := C.picoev_create_loop(max_timeout)
	if !is_async {
		mut pv := &Picoev{
			loop: loop
			cloop: 0
			is_async: is_async
			is_cloop: is_cloop
			cb: cb
			date: C.get_date()
			buf: malloc(max_fds * max_read + 1)
			out: malloc(max_fds * max_write + 1)
		}
		if is_cloop {
			pv.cloop = C.picoev_create_loop(max_timeout)
		}
		go update_date(mut pv)
		return pv
	} else {
		mut pv := &Picoev{
			loop: loop
			cloop: 0
			cb1: cb
			is_async: is_async
			is_cloop: is_cloop
			open_cb: open_cb
			close_cb: close_cb
			cb_ext_fd_cb: cb_ext_fd_cb
			buf: malloc(max_fds * max_read + 1)
			out: malloc(max_fds * max_write + 1)
		}
		if is_cloop {
			pv.cloop = C.picoev_create_loop(max_timeout)
		}
		return pv
	}
}

pub fn (p Picoev) serve_c() {
	for {
		C.picoev_loop_once(p.cloop, 1)
	}
}

pub fn (p Picoev) serve() {
	if p.is_cloop {
		go p.serve_c()
	}
	for {
		C.picoev_loop_once(p.loop, 1)
	}
}


fn update_date(mut p Picoev) {
	for {
		p.date = C.get_date()
		C.usleep(1000000)
	}
}