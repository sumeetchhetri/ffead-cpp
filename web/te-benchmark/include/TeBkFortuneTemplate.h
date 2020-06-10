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
 * TeBkFortuneTemplate.h
 *
 *  Created on: 11-Mar-2015
 *      Author: sumeetc
 */

#ifndef TEBKFORTUNETEMPLATE_H_
#define TEBKFORTUNETEMPLATE_H_
#include "TemplateHandler.h"
#include "TeBkFortune.h"
#include "vector"
#include "DataSourceManager.h"
#include <stdlib.h>
#include <algorithm>
#include "CryptoHandler.h"

#pragma @Template path="fortunes" file="fortunes.tpe"
class TeBkFortuneTemplate {
public:
	virtual ~TeBkFortuneTemplate();
	void getContext(HttpRequest* request, Context* context);
};

#endif /* TEBKFORTUNETEMPLATE_H_ */
