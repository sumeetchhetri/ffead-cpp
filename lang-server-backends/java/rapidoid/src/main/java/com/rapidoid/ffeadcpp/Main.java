package com.rapidoid.ffeadcpp;

import org.rapidoid.config.Conf;
import org.rapidoid.setup.App;

import com.ffead.cpp.FfeadCppInterface;

public class Main {

	public static void main(String[] args) {
		FfeadCppInterface.init(args[0].trim(), args[0].trim().length());
		
		String[] fargs = new String[args.length-2];
		for (int i=2;i<args.length;i++) {
			fargs[i-2] = args[i];
		}
		App.run(new String[] {});

		Conf.HTTP.set("maxPipeline", 128);
		Conf.HTTP.set("timeout", 0);

		new FfeadCppController().listen(Integer.parseInt(args[1].trim()));
		
		//FfeadCppInterface.cleanUp();
	}

}
