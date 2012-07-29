/*
	Copyright 2010, Sumeet Chhetri 
  
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
/*
 * Trace.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "Trace.h"
#include <iostream>
Trace::Trace() {
	logger = Logger::getLogger("Trace");
}

Trace::~Trace() {
	// TODO Auto-generated destructor stub
}

void Trace::trace()
{
	using namespace abi;
	enum{MAX_DEPTH = 10};
	void *trace[MAX_DEPTH];
	Dl_info dlinfo;
	int trace_size = backtrace(trace, MAX_DEPTH);
	int status;
	const char *symname;
	char *demangled;
	logger << "Call stack: " << endl;

	for (int i=0; i<trace_size; ++i)
	{
		if(!dladdr(trace[i], &dlinfo))
			continue;

		symname = dlinfo.dli_sname;
		demangled = __cxa_demangle(symname, NULL, 0, &status);
		if(status == 0 && demangled)
			symname = demangled;
		logger << "object: " << dlinfo.dli_fname << ", function: " << symname << endl;
		if (demangled)
			free(demangled);
	}
}

string Trace::demangle(const char *mangled)
{
	int status;
	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	logger << "\n---------Demanged --" << demangled << endl;
	stringstream ss;
	ss << demangled;
	string s;
	ss >> s;
	return s;

}

string Trace::getClassName(void* instance)
{
	const char *mangled = typeid(instance).name();
	return demangle(mangled);
}
