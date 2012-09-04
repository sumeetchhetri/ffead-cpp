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
#include "string"
using namespace std;

#define METHOD "method";
#define CLAZ "claz";
#define PARAMSIZE "paramsize";
#define PARAM "param_";

class Constants {
public:
	static string INTER_LIB_FILE;
	static bool SYS_FORK_AVAIL;
	static bool SCRIPT_EXEC_SHOW_ERRS;
};

#endif /* CONSTANTS_H_ */
