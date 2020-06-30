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
 * DataSourceInterface.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "DataSourceInterface.h"

std::string DataSourceInterface::BLANK = "";

DataSourceInterface::DataSourceInterface() {
	reflector = NULL;
	mapping = NULL;
	pool = NULL;
	isSingleEVH = false;
}

DataSourceInterface::~DataSourceInterface() {
}

bool DataSourceInterface::executeInsertInternal(Query& query, void* entity) {
	DataSourceEntityMapping& dsemp = mapping->getDataSourceEntityMapping(query.getClassName());
	ClassInfo* clas = reflector->getClassInfo(query.getClassName(), appName);

	if(dsemp.isIdGenerate() && dsemp.getIdgendbEntityType()!="identity") {
		assignId(dsemp, clas, entity);
	}

	bool flag = executeInsert(query, entity);

	if(flag && dsemp.isIdGenerate() && dsemp.getIdgendbEntityType()=="identity") {
		assignId(dsemp, clas, entity);
	}

	return flag;
}


void DataSourceInterface::assignId(DataSourceEntityMapping& dsemp, ClassInfo* clas, void* entity) {
	GenericObject idv;
	next(dsemp, idv);
	if(!idv.isNull())
	{
		Field fld = clas->getField(dsemp.getIdPropertyName());
		std::vector<void *> valus;
		if(GenericObject::isNumber32(idv.getTypeName()) && GenericObject::isNumber(fld.getType()))
		{
			long* id = new long;
			idv.get(id);
			valus.push_back(id);
		}
		else if(GenericObject::isNumber64(idv.getTypeName()) && GenericObject::isNumber(fld.getType()))
		{
			long long* id = new long long;
			idv.get(id);
			valus.push_back(id);
		}
		else if(GenericObject::isFPN(idv.getTypeName()) && GenericObject::isFPN(fld.getType()))
		{
			long double* id = new long double;
			idv.get(id);
			valus.push_back(id);
		}
		else if(GenericObject::isString(idv.getTypeName()) && GenericObject::isString(fld.getType()))
		{
			std::string* id = new std::string;
			idv.set(id);
			valus.push_back(id);
		}
		else
		{
			throw std::runtime_error("Data-Source-Object/Entity types do not match for id property" + dsemp.getClassName() + ":" + dsemp.getIdPropertyName());
		}

		args argus;
		argus.push_back(fld.getType());
		std::string methname = "set"+StringUtil::capitalizedCopy(fld.getFieldName());
		Method meth = clas->getMethod(methname,argus);
		reflector->invokeMethodGVP(entity,meth,valus,true);
	}
}
