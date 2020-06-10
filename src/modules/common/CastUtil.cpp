/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
 * CastUtil.cpp
 *
 *  Created on: 19-Mar-2015
 *      Author: sumeetc
 */


#include "CastUtil.h"

libcuckoo::cuckoohash_map<std::string, std::string> CastUtil::_mangledClassNameMap;
const std::string CastUtil::STD_STRING = "std::string";
const std::string CastUtil::BOOL_TRUE = "true";
const std::string CastUtil::BOOL_FALSE = "false";

