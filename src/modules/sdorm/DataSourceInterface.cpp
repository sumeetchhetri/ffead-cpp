/*
 * DataSourceInterface.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "DataSourceInterface.h"

std::string DataSourceInterface::BLANK = "";

DataSourceInterface::DataSourceInterface() {
	dlib = NULL;
	reflector = NULL;
	mapping = NULL;
	pool = NULL;
	context = NULL;
}

DataSourceInterface::~DataSourceInterface() {
	delete reflector;
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

bool DataSourceInterface::startSession(void* details) {
	if(context==NULL) {
		context = getContext(details);
		return true;
	}
	return false;
}

bool DataSourceInterface::startSession() {
	if(context==NULL) {
		context = getContext(NULL);
		return true;
	}
	return false;
}

bool DataSourceInterface::endSession() {
	if(context!=NULL) {
		destroyContext(context);
		context = NULL;
		return true;
	}
	return false;
}
