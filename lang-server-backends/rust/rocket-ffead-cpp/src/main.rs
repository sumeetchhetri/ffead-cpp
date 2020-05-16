#![feature(proc_macro_hygiene, decl_macro)]
//rustup override set nightly
extern crate rocket;
extern crate libc;

use libc::{c_char, c_int, size_t, c_void};
use std::ptr;
use std::path::PathBuf;
use std::mem;
use std::slice;
use rocket::config::{Config, Environment};
use rocket::http::Method::{Get, Put, Post, Delete, Options};
use std::env;
use rocket::{Request, Handler, Route, Data};
use rocket::http::Status;
use rocket::response::{self, Responder, Response};
use rocket::handler::Outcome;
use std::io::Cursor;
use std::path::Path;
use rocket::response::NamedFile;
use std::cell::RefCell;

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
            fresp: ptr::null_mut()
        }
    }
}

#[link(name = "ffead-framework")]
extern "C" {
    pub fn ffead_cpp_bootstrap(srv: *const c_char, srv_len: size_t, server_type: c_int);
    pub fn ffead_cpp_init();
    pub fn ffead_cpp_cleanup();
    pub fn ffead_cpp_handle_1(ffead_request: *const ffead_request, status_code: *mut c_int,
        out_url: *mut *const c_char, out_url_len: *mut size_t, out_headers: *mut phr_header, out_headers_len: *mut c_int, 
        out_body: *mut *const c_char, out_body_len: *mut size_t
    ) -> *mut c_void;
    pub fn ffead_cpp_resp_cleanup(ptr: *mut c_void);
}

fn slice_from_raw<'a>(pointer: *const c_char, len: size_t) -> &'a [u8] {
    unsafe { mem::transmute(slice::from_raw_parts(pointer, len)) }
}

#[derive(Clone)]
struct CustomHandler {
}

impl CustomHandler {
    fn new() -> Vec<Route> {
        vec![Route::new(Get, "/<path..>", Self { }), 
            Route::new(Put, "/<path..>", Self { }),
            Route::new(Post, "/<path..>", Self { }),
            Route::new(Delete, "/<path..>", Self { }),
            Route::new(Options, "/<path..>", Self { })]
    }
}

impl<'a> Responder<'a> for FfeadResponse {
    fn respond_to(self, _: &Request) -> response::Result<'a> {
        let mut rb = Response::build();
        let scode = self.status_code as u16;
        rb.status(Status::raw(scode));
        let resp_headers_len = self.headers_len;
        for i in 0..resp_headers_len as usize {
            let k = slice_from_raw(self.headers[i].name, self.headers[i].name_len);
            let v = slice_from_raw(self.headers[i].value, self.headers[i].value_len);
            rb.raw_header(std::str::from_utf8(k).unwrap(), std::str::from_utf8(v).unwrap());
        }
        let body = slice_from_raw(self.body, self.body_len);
        rb.sized_body(Cursor::new(body));
        Ok(rb.finalize())
    }
}

impl Handler for CustomHandler {
    fn handle<'r>(&self, _req: &'r Request, data: Data) -> Outcome<'r> {
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
        request.method = std::ffi::CString::new(_req.method().as_str()).unwrap().into_raw();
        request.method_len = _req.method().as_str().len();
        request.path = std::ffi::CString::new(_req.uri().path()).unwrap().into_raw();
        request.path_len = _req.uri().path().len();
        request.version = 1100;//if _req.version() == Version::HTTP_10 { 0 } else { 1};
        let mut bytes = Vec::with_capacity(4096);
        data.stream_to(&mut bytes).unwrap();
        request.body_len = bytes.len();
        unsafe {
            request.body = std::ffi::CString::from_vec_unchecked(bytes).into_raw();
        }
        
        let headers = _req.headers();
        let mut num_headers = 0;
        for header in headers.iter() {
            phrheaders[num_headers].name = std::ffi::CString::new(header.name()).unwrap().into_raw();
	        phrheaders[num_headers].name_len = header.name().len();
	        phrheaders[num_headers].value = std::ffi::CString::new(header.value()).unwrap().into_raw();
	        phrheaders[num_headers].value_len = header.value().len();
	        num_headers = num_headers + 1;
        }
        request.headers = phrheaders.as_mut_ptr();
        request.headers_len = num_headers as c_int;

        let mut response = FfeadResponse::default();
        unsafe {
            response.fresp = ffead_cpp_handle_1(&request, &mut response.status_code, &mut response.url, &mut response.url_len, 
                response.headers.as_mut_ptr(), &mut response.headers_len, &mut response.body, &mut response.body_len);
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
                let url_path: PathBuf = url_path.parse().unwrap();
                Outcome::from(_req, NamedFile::open(url_path))
            } else {
                Outcome::from(_req, response)
            }
        } else {
            Outcome::from(_req, response)
        }
    }
}

/*fn not_found_handler<'r>(req: &'r Request) -> response::Result<'r> {
    let res = Custom(Status::Ok, "Hello!");
    res.respond_to(req)
}*/

thread_local! {
    pub static PREV_RESP: RefCell<*mut c_void> = RefCell::new(ptr::null_mut());
}

static SERVER_NAME: &str = "Rocket";

fn main() {
    let args: Vec<String> = env::args().collect();

    let arg0 = &args[1];
    let arg1 = &args[2];

    let c_server_directory = std::ffi::CString::new(arg0.as_str()).unwrap();

    println!("Bootstrapping ffead-cpp start...");
    unsafe {
        ffead_cpp_bootstrap(c_server_directory.as_ptr(), arg0.len(), 11);
    }
	println!("Bootstrapping ffead-cpp end...");

	println!("Initializing ffead-cpp start...");
    unsafe {
        ffead_cpp_init();
    }
	println!("Initializing ffead-cpp end...");

    let port: u16 = arg1.parse().unwrap();

    //let not_found_catcher = Catcher::new(404, not_found_handler);
    let config = Config::build(Environment::Staging)
        .address("127.0.0.1")
        .port(port)
        .finalize().unwrap();
    rocket::custom(config).mount("/", CustomHandler::new()).launch();
    //rocket::custom(config).register(vec![not_found_catcher]).launch();

    println!("Cleaning up ffead-cpp start...");
    unsafe {
        ffead_cpp_cleanup();
    }
    println!("Cleaning up ffead-cpp end...");
}