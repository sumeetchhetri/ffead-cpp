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
 * ExceptionHandler.cpp
 *
 *  Created on: Jun 10, 2010
 *      Author: sumeet
 */

#include "ExceptionHandler.h"


ExceptionHandler::ExceptionHandler()
{
	static SingleTonHandler s_objHandler;
}

ExceptionTracer::ExceptionTracer()
{
	 /*void * array[25];
	 int nSize = backtrace(array, 25);
	 char ** symbols = backtrace_symbols(array, nSize);

	 for (int i = 0; i < nSize; i++)
	 {
		 cout << symbols[i] << endl;
	 }

	 free(symbols);*/
}
