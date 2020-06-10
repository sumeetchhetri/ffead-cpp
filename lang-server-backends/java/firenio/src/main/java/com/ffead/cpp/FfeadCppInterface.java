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
package com.ffead.cpp;

import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Map;

import jnr.ffi.LibraryLoader;
import jnr.ffi.Memory;
import jnr.ffi.NativeType;
import jnr.ffi.Pointer;
import jnr.ffi.annotations.In;
import jnr.ffi.annotations.Out;
import jnr.ffi.byref.PointerByReference;
import jnr.ffi.types.size_t;

public class FfeadCppInterface {
	
	public static interface FfeadCppFrameworkLib {
		void ffead_cpp_bootstrap(@In String srv, @In @size_t long srv_len, @In int type);
		void ffead_cpp_init();
		void ffead_cpp_cleanup();
		Pointer ffead_cpp_handle_java(@Out Pointer scode, @Out PointerByReference out_url, @Out Pointer out_url_len, 
			@Out PointerByReference out_mime, @Out Pointer out_mime_len, 
			@Out PointerByReference out_body, @Out Pointer out_body_len, @Out PointerByReference out_headers, @Out Pointer out_headers_len,
			@In String server_str, @In @size_t long server_str_len,
			@In String method, @In @size_t long method_len, @In String path, @In @size_t long path_len,
			@In String body, @In @size_t long body_len, @In int version, @In int headers_len, @In Object... args);
		void ffead_cpp_resp_cleanup(Pointer ptr);
    }
	
	private static FfeadCppFrameworkLib libFFC = null;
	
	public static void init(String serverDir, int serverType) {
		//libFFC = LibraryLoader.create(FfeadCppFrameworkLib.class).load("/Users/sumeetc/Projects/GitHub/ffead-cpp/ffead-cpp-interface-test-lib/Debug/libffead-cpp-interface-test-lib.so");
		libFFC = LibraryLoader.create(FfeadCppFrameworkLib.class).load("ffead-framework");
		
		System.out.println("Bootstrapping ffead-cpp start...");
		libFFC.ffead_cpp_bootstrap(serverDir, serverDir.length(), serverType);
		System.out.println("Bootstrapping ffead-cpp end...");
		
		System.out.println("Initializing ffead-cpp start...");
		libFFC.ffead_cpp_init();
		System.out.println("Initializing ffead-cpp end...");
	}
	
	public static void cleanUp() {
		System.out.println("Cleaning up ffead-cpp start...");
		libFFC.ffead_cpp_cleanup();
		System.out.println("Cleaning up ffead-cpp end...");
	}
	
	public static FfeadCppFrameworkLib ffeadCpp() {
		return libFFC;
	}
	
	public static class FfeadRequest {
		private java.lang.String serverName;
		private java.lang.String method;
		private java.lang.String path;
		private java.lang.String body;
		private Map<String, String> headers = new HashMap<String, String>();
		private int version;
		public java.lang.String getServerName() {
			return serverName;
		}
		public void setServerName(java.lang.String serverName) {
			this.serverName = serverName;
		}
		public java.lang.String getMethod() {
			return method;
		}
		public void setMethod(java.lang.String method) {
			this.method = method;
		}
		public java.lang.String getPath() {
			return path;
		}
		public void setPath(java.lang.String path) {
			this.path = path;
		}
		public java.lang.String getBody() {
			return body;
		}
		public void setBody(java.lang.String body) {
			this.body = body;
		}
		public Map<String, String> getHeaders() {
			return headers;
		}
		public void setHeaders(Map<String, String> headers) {
			this.headers = headers;
		}
		public int getVersion() {
			return version;
		}
		public void setVersion(int version) {
			this.version = version;
		}
		@Override
		public String toString() {
			return "FfeadRequest [serverName=" + serverName + ", method=" + method + ", path=" + path + ", body=" + body
					+ ", headers=" + headers + ", version=" + version + "]";
		}
	}
	
	public static class FfeadResponse {
		private java.lang.String url;
		private java.lang.String mimeType;
		private java.lang.String body;
		private java.lang.String headerStr;
		private int statusCode;
		public java.lang.String getHeaderStr() {
			return headerStr;
		}
		public void setHeaderStr(java.lang.String headerStr) {
			this.headerStr = headerStr;
		}
		public java.lang.String getUrl() {
			return url;
		}
		public void setUrl(java.lang.String url) {
			this.url = url;
		}
		public java.lang.String getBody() {
			return body;
		}
		public void setBody(java.lang.String body) {
			this.body = body;
		}
		public java.lang.String getMimeType() {
			return mimeType;
		}
		public void setMimeType(java.lang.String mimeType) {
			this.mimeType = mimeType;
		}
		public int getStatusCode() {
			return statusCode;
		}
		public void setStatusCode(int statusCode) {
			this.statusCode = statusCode;
		}
		@Override
		public String toString() {
			return "FfeadResponse [url=" + url + ", mimeType=" + mimeType + ", body=" + body + ", headerStr="
					+ headerStr + ", statusCode=" + statusCode + "]";
		}
	}
	
	public static FfeadResponse ffead_cpp_handle(FfeadRequest req) {
		if(ffeadCppResponse.get()!=null) {
    		ffeadCpp().ffead_cpp_resp_cleanup(ffeadCppResponse.get());
    		ffeadCppResponse.set(null);
    	}
		
    	Object[] args = new Object[req.getHeaders().size()*4];
    	int index = 0;
    	Map<String, String> headers = req.getHeaders();
    	for (Map.Entry<String, String> entry: headers.entrySet()) {
    		args[index++] = entry.getKey();
    		args[index++] = (long)entry.getKey().length();
    		args[index++] = entry.getValue();
    		args[index++] = (long)entry.getValue().length();
		}
    	
    	Pointer scode = Memory.allocateTemporary(jnr.ffi.Runtime.getRuntime(libFFC), NativeType.SINT);
    	PointerByReference out_url = new PointerByReference();
    	Pointer out_url_len = Memory.allocateTemporary(jnr.ffi.Runtime.getRuntime(libFFC), NativeType.UINT);
    	PointerByReference out_mime = new PointerByReference();
    	Pointer out_mime_len = Memory.allocateTemporary(jnr.ffi.Runtime.getRuntime(libFFC), NativeType.UINT);
    	PointerByReference out_headers = new PointerByReference();
    	Pointer out_headers_len = Memory.allocateTemporary(jnr.ffi.Runtime.getRuntime(libFFC), NativeType.UINT);
    	PointerByReference out_body = new PointerByReference();
    	Pointer out_body_len = Memory.allocateTemporary(jnr.ffi.Runtime.getRuntime(libFFC), NativeType.UINT);
    	
    	int reqBodyLen = req.getBody()!=null?req.getBody().length():0;
    	Pointer fres = ffeadCpp().ffead_cpp_handle_java(scode, out_url, out_url_len, out_mime, out_mime_len, out_body, out_body_len, out_headers, out_headers_len,
    			req.getServerName(), (long)req.getServerName().length(), req.getMethod(), (long)req.getMethod().length(), req.getPath(), (long)req.getPath().length(),
    			req.getBody(), (long)reqBodyLen, req.getVersion(), args.length, args);
    	ffeadCppResponse.set(fres);
    	
    	FfeadResponse res = new FfeadResponse();
    	res.setStatusCode(scode.getInt(0));
    	if(res.getStatusCode()>0) {
    		Pointer ptr = out_headers.getValue();
    		res.setHeaderStr(ptr.getString(0, out_headers_len.getInt(0), Charset.defaultCharset()));
    		ptr = out_body.getValue();
    		res.setBody(ptr.getString(0, out_body_len.getInt(0), Charset.defaultCharset()));
    	} else {
    		Pointer ptr = out_url.getValue();
    		res.setUrl(ptr.getString(0, out_url_len.getInt(0), Charset.defaultCharset()));
    		ptr = out_mime.getValue();
    		res.setMimeType(ptr.getString(0, out_mime_len.getInt(0), Charset.defaultCharset()));
    	}
    	return res;
	}
	
	static ThreadLocal<Pointer> ffeadCppResponse = new ThreadLocal<Pointer>();
}
