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
/*
 * Constants.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include "AppDefines.h"

#if defined(CYGWIN)
	#define INTER_LIB_FILE "cyginter.dll"
	#define DINTER_LIB_FILE "cygdinter.dll"
	#define SYS_FORK_AVAIL false
	#define SCRIPT_EXEC_SHOW_ERRS false
	#define IS_FILE_DESC_PASSING_AVAIL false
#elif defined(OS_MINGW)
	#define INTER_LIB_FILE "libinter.dll"
	#define DINTER_LIB_FILE "libdinter.dll"
	#define SYS_FORK_AVAIL false
	#define SCRIPT_EXEC_SHOW_ERRS false
	#define IS_FILE_DESC_PASSING_AVAIL false
#elif defined(OS_DARWIN)
	#define INTER_LIB_FILE "libinter.dylib"
	#define DINTER_LIB_FILE "libdinter.dylib"
	#define SYS_FORK_AVAIL true
	#define SCRIPT_EXEC_SHOW_ERRS true
	#define IS_FILE_DESC_PASSING_AVAIL false
#else
	#define INTER_LIB_FILE "libinter.so"
	#define DINTER_LIB_FILE "libdinter.so"
	#define SYS_FORK_AVAIL true
	#define SCRIPT_EXEC_SHOW_ERRS true
	#define IS_FILE_DESC_PASSING_AVAIL true
#endif

#endif /* CONSTANTS_H_ */
