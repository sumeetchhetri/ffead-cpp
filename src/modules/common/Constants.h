/*
	Copyright 2009-2012, Sumeet Chhetri

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

#define METHOD "method"
#define CLAZ "claz"
#define PARAMSIZE "paramsize"
#define PARAM "param_"

#if defined(OS_CYGWIN)
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

class Constants {
public:
	static const std::string BLANK;
	static const std::string INT_TYPE;
	static const std::string UINT_TYPE;
	static const std::string SHORTINT_TYPE;
	static const std::string SSHORTINT_TYPE;
	static const std::string SSHORT_TYPE;
	static const std::string SHORT_TYPE;
	static const std::string USHORT_TYPE;
	static const std::string LONG_TYPE;
	static const std::string ULONG_TYPE;
	static const std::string LONGLONG_TYPE;
	static const std::string ULONGLONG_TYPE;
	static const std::string FLOAT_TYPE;
	static const std::string DOUBLE_TYPE;
	static const std::string LDOUBLE_TYPE;
	static const std::string BOOL_TYPE;
	static const std::string CHAR_TYPE;
	static const std::string UCHAR_TYPE;
	static const std::string STD_STRING_TYPE;
	static const std::string STRING_TYPE;
};

#endif /* CONSTANTS_H_ */
