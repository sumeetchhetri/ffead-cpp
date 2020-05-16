extern crate libc;

use actix_http::http::Version;
use actix_http::http::StatusCode;
use libc::{c_char, c_int, size_t, c_void};
use std::ptr;
use std::path::PathBuf;
use std::mem;
use std::slice;
use futures::StreamExt;
use actix_web::dev::HttpResponseBuilder;
use actix_web::{web, App, HttpRequest, HttpResponse, HttpServer, Either, Error};
use actix_files::NamedFile;
use std::env;
use std::cell::RefCell;

type RegisterResult = Either<HttpResponse, Result<NamedFile, Error>>;

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
            url_len: 0
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

async fn handle(_req: HttpRequest, mut body: web::Payload) -> RegisterResult {
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
    request.path = std::ffi::CString::new(_req.path()).unwrap().into_raw();
    request.path_len = _req.path().len();
    request.version = if _req.version() == Version::HTTP_10 { 0 } else { 1};
    let mut bytes = Vec::with_capacity(4096);
    while let Some(item) = body.next().await {
        let item = item.unwrap();
        bytes.extend_from_slice(&item);
    }
    request.body_len = bytes.len();
    unsafe {
        request.body = std::ffi::CString::from_vec_unchecked(bytes).into_raw();
    }
    
    let headers = _req.headers();
    let mut num_headers = 0;
    for (k, v) in headers {
    	phrheaders[num_headers].name = std::ffi::CString::new(k.as_str()).unwrap().into_raw();
        phrheaders[num_headers].name_len = k.as_str().len();
        phrheaders[num_headers].value = std::ffi::CString::new(v.to_str().unwrap()).unwrap().into_raw();
        phrheaders[num_headers].value_len = v.to_str().unwrap().len();
        num_headers = num_headers + 1;
    }
    request.headers = phrheaders.as_mut_ptr();
    request.headers_len = num_headers as c_int;

    let mut response = FfeadResponse::default();
    let fresp: *mut c_void;
    unsafe {
        fresp = ffead_cpp_handle_1(&request, &mut response.status_code, &mut response.url, &mut response.url_len, 
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
        *resp.borrow_mut() = fresp
    });

    if response.status_code > 0 {
        let scode = StatusCode::from_u16(response.status_code as u16).unwrap();
        let mut response_builder = HttpResponseBuilder::new(scode);
        let resp_headers_len = response.headers_len;
        for i in 0..resp_headers_len as usize {
            let k = slice_from_raw(response.headers[i].name, response.headers[i].name_len);
            let v = slice_from_raw(response.headers[i].value, response.headers[i].value_len);
            response_builder.set_header(k, v);
        }
        let body = slice_from_raw(response.body, response.body_len);
        Either::A(response_builder.body(body))
    } else {
        let url_path = slice_from_raw(response.url, response.url_len);
        let path: PathBuf = std::str::from_utf8(url_path).unwrap().parse().unwrap();
        Either::B(Ok(NamedFile::open(path).unwrap()))
    }
}

thread_local! {
    pub static PREV_RESP: RefCell<*mut c_void> = RefCell::new(ptr::null_mut());
}

static SERVER_NAME: &str = "Actix";

#[actix_rt::main]
async fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();

    let arg0 = &args[1];
    let arg1 = &args[2];

    let c_server_directory = std::ffi::CString::new(arg0.as_str()).unwrap();

    println!("Bootstrapping ffead-cpp start...");
    unsafe {
        ffead_cpp_bootstrap(c_server_directory.as_ptr(), arg0.len(), 9);
    }
	println!("Bootstrapping ffead-cpp end...");

	println!("Initializing ffead-cpp start...");
    unsafe {
        ffead_cpp_init();
    }
	println!("Initializing ffead-cpp end...");

    let addr_str = format!("{}:{}", "127.0.0.1", arg1);
    HttpServer::new(|| App::new().route("/*", web::to(handle)))
        .bind(addr_str)?
        .run()
        .await?;

    println!("Cleaning up ffead-cpp start...");
    unsafe {
        ffead_cpp_cleanup();
    }
    println!("Cleaning up ffead-cpp end...");

    Ok(())
}