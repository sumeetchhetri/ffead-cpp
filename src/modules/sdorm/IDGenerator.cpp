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
 * IDGenerator.cpp
 *
 *  Created on: 10-Oct-2014
 *      Author: sumeetc
 */

#include "IDGenerator.h"

IDGenerator::IDGenerator() {
	
}

void IDGenerator::init(DataSourceEntityMapping& dsemp, const bool& forceReinit) {
	if(dsemp.getIdgentype()=="hilo" || dsemp.getIdgentype()=="multihilo")
	{
		if(!forceReinit && dsemp.getIdgentype()=="hilo" && HiloIdGenerator::isInitialized()) {
			return;
		}
		else if(!forceReinit && dsemp.getIdgentype()=="multihilo" && HiloIdGenerator::isInitialized(dsemp.getClassName())) {
			return;
		}

		GenericObject hiValue;
		if(dsemp.getIdgendbEntityType()=="table")
		{
			executePreTable(dsemp, hiValue);
			if(hiValue.getTypeName()!="") {
				executePostTable(dsemp, hiValue);
			}
		}
		else if(dsemp.getIdgendbEntityType()=="sequence")
		{
			executeSequence(dsemp, hiValue);
		}
		else if(dsemp.getIdgendbEntityType().find("custom:")==0)
		{
			std::string custMethod = dsemp.getIdgendbEntityType().substr(7);
			executeCustom(dsemp, custMethod, hiValue);
		}

		if(hiValue.isNumber())
		{
			long long hival;
			hiValue.get(hival);
			if(dsemp.getIdgentype()=="multihilo")
				HiloIdGenerator::init(dsemp.getClassName(), hival, dsemp.getIdgenlowValue(), forceReinit);
			else
				HiloIdGenerator::init(hival, dsemp.getIdgenlowValue(), forceReinit);
		}
	}
}

void IDGenerator::next(DataSourceEntityMapping& dsemp, GenericObject& id) {
	if(!dsemp.isIdGenerate())return;
	if(dsemp.getIdgendbEntityType()=="identity")
	{
		executeIdentity(dsemp, id);
	}
	else if(dsemp.getIdgentype()=="hilo" || dsemp.getIdgentype()=="multihilo")
	{
		long long idv = -1;
		if(dsemp.getIdgentype()=="multihilo")
		{
			idv = HiloIdGenerator::next(dsemp.getIdgendbEntityName());
		}
		else
		{
			idv = HiloIdGenerator::next();
		}
		if(idv==-2) {
			init(dsemp, true);
			next(dsemp, id);
			id.set(idv);
		} else if(idv!=-1) {
			id.set(idv);
		}
	}
	else if(dsemp.getIdgendbEntityType()=="table")
	{
		executePreTable(dsemp, id);
		if(id.getTypeName()!="") {
			executePostTable(dsemp, id);
		}
	}
	else if(dsemp.getIdgendbEntityType()=="sequence")
	{
		executeSequence(dsemp, id);
	}
#ifdef HAVE_UUIDINC
	else if(dsemp.getIdgendbEntityType()=="uuid")
	{
		uuid_t idt;
		uuid_generate(idt);
		std::string ids;
		for(int i=0;i<16;i++){
			ids.push_back(idt[i]);
		}
		id.set(ids);
	}
#elif defined(HAVE_OSSPUUIDINC) || defined(HAVE_OSSPUUIDINC_2)
	else if(dsemp.getIdgendbEntityType()=="uuid")
	{
		uuid_t* idt;
		uuid_rc_t status = uuid_create(&idt);
		std::string ids((const char *) &idt, sizeof(idt));
		uuid_destroy(idt);
		id.set(ids);
	}
#elif HAVE_BSDUUIDINC
	else if(dsemp.getIdgendbEntityType()=="uuid")
	{
		uuid_t idt;
		uint32_t status;
		uuid_create(&idt, &status);
		std::string ids((const char *) &idt, sizeof(idt));
		id.set(ids);
	}
#endif
	else if(dsemp.getIdgendbEntityType().find("custom:")==0)
	{
		std::string custMethod = dsemp.getIdgendbEntityType().substr(7);
		executeCustom(dsemp, custMethod, id);
	}
}

IDGenerator::~IDGenerator() {
	
}

