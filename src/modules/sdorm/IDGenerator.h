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
 * IDGenerator.h
 *
 *  Created on: 10-Oct-2014
 *      Author: sumeetc
 */

#ifndef IDGENERATOR_H_
#define IDGENERATOR_H_
#include "Compatibility.h"
#include "DataSourceMapping.h"
#include "GenericObject.h"
#include "HiloIdGenerator.h"
#ifdef HAVE_UUIDINC
#include <uuid/uuid.h>
#endif
#if defined(HAVE_BSDUUIDINC) || defined(HAVE_OSSPUUIDINC)
#include <uuid.h>
#endif

class IDGenerator {
public:
	void init(DataSourceEntityMapping& dsemp, const bool& forceReinit= false);
	virtual void executePreTable(DataSourceEntityMapping& dsemp, GenericObject&)=0;
	virtual void executePostTable(DataSourceEntityMapping& dsemp, GenericObject& id)=0;
	virtual void executeSequence(DataSourceEntityMapping& dsemp, GenericObject&)=0;
	virtual void executeIdentity(DataSourceEntityMapping& dsemp, GenericObject&)=0;
	virtual void executeCustom(DataSourceEntityMapping& dsemp, const std::string& customMethod, GenericObject&)=0;
	void next(DataSourceEntityMapping& dsemp, GenericObject&);
	IDGenerator();
	virtual ~IDGenerator();
};

#endif /* IDGENERATOR_H_ */
