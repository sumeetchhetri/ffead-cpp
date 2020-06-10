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
package com.wizzardo.ffeadcpp;

import static com.ffead.cpp.FfeadCppInterface.ffead_cpp_handle;

import java.io.File;
import java.io.IOException;
import java.util.Map;

import com.ffead.cpp.FfeadCppInterface.FfeadRequest;
import com.ffead.cpp.FfeadCppInterface.FfeadResponse;
import com.wizzardo.http.MultiValue;
import com.wizzardo.http.framework.Controller;
import com.wizzardo.http.request.Header;
import com.wizzardo.http.response.Status;

import io.netty.util.internal.StringUtil;

public class FfeadCppController extends Controller {
	
	static final String EH = "\r\n";
	static final String HS = ": ";
	
    @SuppressWarnings("unchecked")
	public void handle() {
    	FfeadRequest freq = new FfeadRequest();
    	freq.setMethod(request.method().name());
    	String fullPath = request.path().toString();
    	if(request.getQueryString()!=null) {
    		fullPath += "?" + request.getQueryString();
    	}
    	freq.setPath(fullPath);
    	freq.setServerName("wizzardo");
    	freq.setVersion(1);
    	if(request.getBody()!=null) {
    		freq.setBody(new String(request.getBody().bytes()));
    	}
    	
    	boolean isKeepAlive = false;
    	Map<String, MultiValue> headers = request.headers();
    	for (Map.Entry<String, MultiValue> entry: headers.entrySet()) {
			freq.getHeaders().put(entry.getKey(), entry.getValue().getValue().trim());
			if(entry.getKey().trim().equalsIgnoreCase(Header.KEY_CONNECTION.value) && entry.getValue().getValue().trim().toLowerCase().startsWith("keep-alive")) {
    			isKeepAlive = true;
    		}
		}
    	
    	//System.out.println(freq);
    	FfeadResponse fres = ffead_cpp_handle(freq);
    	//System.out.println(fres);
    	
    	response.appendHeader(Header.KEY_CONNECTION.value, isKeepAlive?"keep-alive":"close");
    	
    	if(fres.getStatusCode()>0) {
    		response.setStatus(Status.valueOf(fres.getStatusCode()));
    		if(!StringUtil.isNullOrEmpty(fres.getHeaderStr())) {
    			String[] ohdrs = fres.getHeaderStr().split(EH);
    			for (int i=1;i<ohdrs.length;i++) {
    				String ohdr = ohdrs[i];
					String[] kvp = ohdr.split(HS);
					response.appendHeader(kvp[0], kvp[1]);
				}
    		}
    		response.setBody(fres.getBody());
    	} else {
    		if(fres.getUrl()!=null) {
	    		File out = new File(fres.getUrl());
	    		if(out.exists()) {
	    			response.setStatus(Status._200);
	    			try {
	    				response.setHeader(Header.KEY_CONTENT_TYPE, fres.getMimeType());
						response.setBody(out);
					} catch (IOException e) {
					}
	    		} else {
	    			response.setStatus(Status._404);
	    		}
    		} else {
    			response.setStatus(Status._404);
    		}
    	}
    }
}
