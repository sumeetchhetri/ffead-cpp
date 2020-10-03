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
package com.rapidoid.ffeadcpp;

import org.rapidoid.config.Conf;
import org.rapidoid.setup.App;

import com.ffead.cpp.FfeadCppInterface;

public class Main {

	public static void main(String[] args) {
		FfeadCppInterface.init(args[0].trim(), 21);
		
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
