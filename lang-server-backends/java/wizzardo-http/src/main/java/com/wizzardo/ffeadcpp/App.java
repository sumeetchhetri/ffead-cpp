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
