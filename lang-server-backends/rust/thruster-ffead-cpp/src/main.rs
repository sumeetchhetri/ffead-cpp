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

extern crate libc;
extern crate thruster;

use hyper::StatusCode;
use libc::{c_char, c_int, size_t, c_void};
use std::ptr;
use std::mem;
use std::slice;
use std::env;
use std::path::Path;
use std::cell::RefCell;
use thruster::{Response, Context, async_middleware, middleware_fn};
use thruster::{App, Request, Server, ThrusterServer};
use thruster::{MiddlewareNext, MiddlewareResult};
use bytes::Bytes;
use std::fs::File;
use std::io::Read;

static SERVER_NAME: &str = "Thruster";
static SERVER_HDR: &str = "server";

thread_local! {
    pub static PREV_RESP: RefCell<*mut c_void> = RefCell::new(ptr::null_mut());
}

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub struct phr_header {
    pub name: *mut c_char,
    pub name_len: size_t,
    pub value: *mut c_char,
    pub value_len: size_t,
}
impl Default for phr_header {
    fn default() -> phr_header {
        phr_header {
            name: ptr::null_mut(),
            name_len: 0,
            value: ptr::null_mut(),
            value_len: 0,
        }
    }
}

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub struct ffead_request {
    pub server_str: *mut c_char,
    pub server_str_len: size_t,
    pub method: *mut c_char,
    pub method_len: size_t,
    pub path: *mut c_char,
    pub path_len: size_t,
    pub headers: *mut phr_header,
    pub headers_len: c_int,
    pub body: *mut c_char,
    pub body_len: size_t,
    pub version: c_int
}
impl Default for ffead_request {
    fn default() -> ffead_request {
        ffead_request {
            server_str: ptr::null_mut(),
            server_str_len: 0,
            method: ptr::null_mut(),
            method_len: 0,
            path: ptr::null_mut(),
            path_len: 0,
            headers: ptr::null_mut(),
            headers_len: 0,
            body: ptr::null_mut(),
            body_len: 0,
            version: 0
        }
    }
}

pub struct FfeadResponse {
    pub status_code: c_int,
    pub headers: [phr_header; 100],
    pub headers_len: c_int,
    pub body: *const c_char,
    pub body_len: size_t,
    pub url: *const c_char,
    pub url_len: size_t,
    pub url_mime: *const c_char,
    pub url_mime_len: size_t,
    pub fresp: *mut c_void,
}
impl Default for FfeadResponse {
    fn default() -> FfeadResponse {
        FfeadResponse {
            status_code: 0,
            headers: [phr_header::default(); 100],
            headers_len: 0,
            body: ptr::null(),
            body_len: 0,
            url: ptr::null(),
            url_len: 0,
            url_mime: ptr::null(),
            url_mime_len: 0,
            fresp: ptr::null_mut()
        }
    }
}

#[link(name = "ffead-framework")]
extern "C" {
    pub fn ffead_cpp_bootstrap(srv: *const c_char, srv_len: size_t, server_type: c_int);
    pub fn ffead_cpp_init();
    pub fn ffead_cpp_cleanup();
    pub fn ffead_cpp_handle_rust_swift_1(ffead_request: *const ffead_request, status_code: *mut c_int,
        out_url: *mut *const c_char, out_url_len: *mut size_t, out_url_mime: *mut *const c_char, out_url_mime_len: *mut size_t, 
        out_headers: *mut phr_header, out_headers_len: *mut c_int, 
        out_body: *mut *const c_char, out_body_len: *mut size_t
    ) -> *mut c_void;
    pub fn ffead_cpp_resp_cleanup(ptr: *mut c_void);
}

pub struct Ctx {
    response: Response,
    pub request: Request,
    pub scode: StatusCode
}

impl Context for Ctx {
    type Response = Response;

    fn get_response(mut self) -> Response {
        self.response.status_code(self.scode.as_u16() as u32, self.scode.as_str());
        self.response
    }

    fn set_body(&mut self, body: Vec<u8>) {
        self.response.response = body;
    }

    fn set_body_bytes(&mut self, _: Bytes) {
    }

    fn route(&self) -> &str {
        ""
    }

    fn set(&mut self, key: &str, value: &str) {
        self.response.header(key, value);
    }

    fn remove(&mut self, _: &str) {
    }
}

pub fn generate_context(request: Request, _: &(), _: &str) -> Ctx {
    let ctx = Ctx {
        request: request,
        response: Response::new(),
        scode: StatusCode::NOT_FOUND
    };

    ctx
}

fn slice_from_raw<'a>(pointer: *const c_char, len: size_t) -> &'a [u8] {
    unsafe { mem::transmute(slice::from_raw_parts(pointer, len)) }
}

#[middleware_fn]
async fn index(mut ctx: Ctx, _next: MiddlewareNext<Ctx>) -> MiddlewareResult<Ctx> {
    let fresp = PREV_RESP.with(|resp| resp.borrow().clone());
    if fresp != ptr::null_mut() {
        unsafe {
            ffead_cpp_resp_cleanup(fresp);
        }
    }

    let mut request = ffead_request::default();
    let mut phrheaders = [phr_header::default(); 100];

    request.server_str = std::ffi::CString::new(SERVER_NAME).unwrap().into_raw();
    request.server_str_len = SERVER_NAME.len();
    request.method = std::ffi::CString::new(ctx.request.method()).unwrap().into_raw();
    request.method_len = ctx.request.method().len();
    request.path = std::ffi::CString::new(ctx.request.path()).unwrap().into_raw();
    request.path_len = ctx.request.path().len();
    request.version = 1;
    
    let headers = ctx.request.headers();
    let mut num_headers = 0;
    for (k, v) in headers {
        let klen = k.len();
        phrheaders[num_headers].name = std::ffi::CString::new(k).unwrap().into_raw();
        phrheaders[num_headers].name_len = klen;
        let vlen = v.len();
        phrheaders[num_headers].value = std::ffi::CString::new(v).unwrap().into_raw();
        phrheaders[num_headers].value_len = vlen;
        num_headers = num_headers + 1;
    }
    request.headers = phrheaders.as_mut_ptr();
    request.headers_len = num_headers as c_int;
    request.body = std::ffi::CString::new(ctx.request.raw_body()).unwrap().into_raw();
    request.body_len = ctx.request.raw_body().len();

    let mut response = FfeadResponse::default();
    unsafe {
        response.fresp = ffead_cpp_handle_rust_swift_1(&request, &mut response.status_code, &mut response.url, &mut response.url_len, 
            &mut response.url_mime, &mut response.url_mime_len, response.headers.as_mut_ptr(), &mut response.headers_len, 
            &mut response.body, &mut response.body_len);
    }

    //Cleanup all raw pointers passed to C
    unsafe {
        let _ = std::ffi::CString::from_raw(request.server_str);
        let _ = std::ffi::CString::from_raw(request.method);
        let _ = std::ffi::CString::from_raw(request.path);
        let _ = std::ffi::CString::from_raw(request.body);
        for i in 0..num_headers {
            let _ = std::ffi::CString::from_raw(phrheaders[i].name);
            let _ = std::ffi::CString::from_raw(phrheaders[i].value);
        }
    }

    PREV_RESP.with(|resp| {
        *resp.borrow_mut() = response.fresp
    });

    if response.status_code == 0 {
        let url_path = slice_from_raw(response.url, response.url_len);
        let url_path = std::str::from_utf8(url_path).unwrap();
        if Path::new(url_path).exists() {
            let mut file = File::open(url_path).unwrap();
            let mut data = Vec::new();
            file.read_to_end(&mut data).unwrap();
            ctx.set_body(data);
            ctx.scode = StatusCode::OK;
        } else {
            ctx.scode = StatusCode::NOT_FOUND;
        }
    } else {
        ctx.scode = StatusCode::from_u16(response.status_code as u16).unwrap();
        let resp_headers_len = response.headers_len;
        for i in 0..resp_headers_len as usize {
            let k = slice_from_raw(response.headers[i].name, response.headers[i].name_len);
            let v = slice_from_raw(response.headers[i].value, response.headers[i].value_len);
            ctx.set(std::str::from_utf8(k).unwrap(), std::str::from_utf8(v).unwrap());
        }
        ctx.set(SERVER_HDR, SERVER_NAME);
        let body = slice_from_raw(response.body, response.body_len);
        ctx.set_body(body.to_vec());
    }

    Ok(ctx)
}

fn main() {
    let args: Vec<String> = env::args().collect();

    let arg0 = &args[1];
    let arg1 = &args[2];

    let c_server_directory = std::ffi::CString::new(arg0.as_str()).unwrap();

    println!("Bootstrapping ffead-cpp start...");
    unsafe {
        ffead_cpp_bootstrap(c_server_directory.as_ptr(), arg0.len(), 14);
    }
	println!("Bootstrapping ffead-cpp end...");

	println!("Initializing ffead-cpp start...");
    unsafe {
        ffead_cpp_init();
    }
	println!("Initializing ffead-cpp end...");

    let mut app = App::<Request, Ctx, ()>::create(generate_context, ());

    app.set404(async_middleware!(Ctx, [index]));

    let addr_str = format!("{}:{}", "0.0.0.0", arg1);
    let addr: String = addr_str.parse().unwrap();

    println!("Listening on http://{}", addr);

    let server = Server::new(app);
    server.start("0.0.0.0", arg1.parse::<u16>().unwrap());

    println!("Cleaning up ffead-cpp start...");
    unsafe {
        ffead_cpp_cleanup();
    }
    println!("Cleaning up ffead-cpp end...");
}
