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
 * TeBkFortuneTemplate.cpp
 *
 *  Created on: 11-Mar-2015
 *      Author: sumeetc
 */

#include "TeBkFortuneTemplate.h"

TeBkFortuneTemplate::~TeBkFortuneTemplate() {
}

void TeBkFortuneTemplate::getContext(HttpRequest* request, Context* context)
{
	DataSourceInterface* sqli = DataSourceManager::getImpl();

	try {
		std::vector<TeBkFortune> flstT = sqli->getAll<TeBkFortune>();
		std::vector<TeBkFortune>* flst = new std::vector<TeBkFortune>;
		flst->swap(flstT);

		for(int i=0;i<(int)flst->size();i++)
		{
			std::string nm = flst->at(i).getMessage();
			CryptoHandler::sanitizeHtml(nm);
			flst->at(i).setMessage(nm);
		}

		TeBkFortune nf;
		nf.setId(0);
		nf.setMessage("Additional fortune added at request time.");
		flst->push_back(nf);
		std::sort (flst->begin(), flst->end());

		context->insert(std::pair<std::string, void*>("fortunes", flst));

		DataSourceManager::cleanImpl(sqli);
	} catch(...) {
		DataSourceManager::cleanImpl(sqli);
		throw;
	}
}
