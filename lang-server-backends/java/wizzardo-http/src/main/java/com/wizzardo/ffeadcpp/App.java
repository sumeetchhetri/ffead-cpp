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

import com.ffead.cpp.FfeadCppInterface;
import com.wizzardo.http.HttpConnection;
import com.wizzardo.http.framework.WebApplication;
import com.wizzardo.http.request.ByteTree;
import com.wizzardo.http.request.Request;

public class App {
	
	public static void main(String[] args) {
		FfeadCppInterface.init(args[0].trim(), args[0].trim().length());
		
		String[] fargs = new String[args.length-2];
		for (int i=2;i<args.length;i++) {
			fargs[i-2] = args[i];
		}
		WebApplication webApplication = new WebApplication(fargs) {
			@Override
			protected void initHttpPartsCache() {
				ByteTree tree = httpStringsCache.getTree();
				for (Request.Method method : Request.Method.values()) {
					tree.append(method.name());
				}
				tree.append(HttpConnection.HTTP_1_1);
			}
		};

		webApplication.onSetup(app -> {
			app.getUrlMapping().append("/*", FfeadCppController.class, "handle");
		});

		webApplication.start();
		
		//FfeadCppInterface.cleanUp();
	}
}
