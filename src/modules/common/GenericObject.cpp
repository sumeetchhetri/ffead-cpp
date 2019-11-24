/*
 * GenericObject.cpp
 *
 *  Created on: 09-Oct-2014
 *      Author: sumeetc
 */

#include "GenericObject.h"

Reflector* GenericObject::ref = NULL;

void GenericObject::init(Reflector* r) {
	ref = r;
}

Reflector* GenericObject::getReflector() {
	return ref;
}

GenericObject::GenericObject() {
	objVal = NULL;
}

GenericObject::GenericObject(const GenericObject &obj) {
	internalCopy(obj);
}

const std::string& GenericObject::getTypeName() const {
	return typeName;
}

GenericObject::~GenericObject() {
	internalClear();
}

GenericObject& GenericObject::operator =(const GenericObject& obj) {
	internalCopy(obj);
	return *this;
}

void GenericObject::internalCopy(const GenericObject& obj) {
	objVal = NULL;
	if(obj.objSerState=="" && obj.objVal==NULL)return;
	typeName = obj.typeName;
	objSerState = obj.objSerState;
	cstr = obj.cstr;
	ucstr = obj.ucstr;
	wstr = obj.wstr;
	if(typeName=="short") objVal = new short(*(short*)obj.objVal);
	else if(typeName=="int") objVal = new int(*(int*)obj.objVal);
	else if(typeName=="long") objVal = new long(*(long*)obj.objVal);
	else if(typeName=="long long") objVal = new long long(*(long long*)obj.objVal);
	else if(typeName=="unsigned short") objVal = new unsigned short(*(unsigned short*)obj.objVal);
	else if(typeName=="unsigned int") objVal = new unsigned int(*(unsigned int*)obj.objVal);
	else if(typeName=="unsigned long") objVal = new unsigned long(*(unsigned long*)obj.objVal);
	else if(typeName=="unsigned long long") objVal = new unsigned long long(*(unsigned long long*)obj.objVal);
	else if(typeName=="bool") objVal = new bool(*(bool*)obj.objVal);
	else if(typeName=="float") objVal = new float(*(float*)obj.objVal);
	else if(typeName=="double") objVal = new double(*(double*)obj.objVal);
	else if(typeName=="long double") objVal = new long double(*(long double*)obj.objVal);
	else if(typeName=="std::string" || typeName=="string") objVal = new std::string(*(std::string*)obj.objVal);
	//Handle c strings
	else if(cstr.size()>0) {
		char* cstrpr = new char[cstr.size()];
		std::copy(cstr.begin(), cstr.end(), cstrpr);
		objVal = cstrpr;
	}
	else if(ucstr.size()>0) {
		unsigned char* ucstrpr = new unsigned char[ucstr.size()];
		std::copy(ucstr.begin(), ucstr.end(), ucstrpr);
		objVal = ucstrpr;
	}
	else if(wstr.size()>0) {
		wchar_t* wstrpr = new wchar_t[wstr.size()];
		std::copy(wstr.begin(), wstr.end(), wstrpr);
		objVal = wstrpr;
	}
	else {
		int serOpt = SerializeBase::identifySerOption(obj.typeName);
		objVal = XMLSerialize::unSerializeUnknown(obj.objSerState, serOpt, obj.typeName);
	}
}

void GenericObject::internalClear() {
	if(objVal!=NULL) {
		ref->destroy(objVal, typeName);
		objVal = NULL;
	}
	if(cstr.size()>0) {
		cstr.clear();
	}
	if(ucstr.size()>0) {
		ucstr.clear();
	}
	if(wstr.size()>0) {
		wstr.clear();
	}
}

bool GenericObject::isInstanceOf(const std::string& type) {
	return typeName==type;
}

bool GenericObject::isNumber() {
	return typeName=="short" || typeName=="short int" || typeName=="signed short" || typeName=="signed short int"
			|| typeName=="unsigned short" || typeName=="unsigned short int"
			|| typeName=="signed" || typeName=="int" || typeName=="signed int"
			|| typeName=="unsigned" || typeName=="unsigned int" || typeName=="long"
			|| typeName=="long int" || typeName=="signed long" || typeName=="signed long int"
			|| typeName=="unsigned long" || typeName=="unsigned long int"
			|| typeName=="long long" || typeName=="long long int" || typeName=="signed long long"
			|| typeName=="signed long long int" || typeName=="unsigned long long"
			|| typeName=="unsigned long long int";
}

bool GenericObject::isNumber(const std::string& typeName) {
	return typeName=="short" || typeName=="short int" || typeName=="signed short" || typeName=="signed short int"
			|| typeName=="unsigned short" || typeName=="unsigned short int"
			|| typeName=="signed" || typeName=="int" || typeName=="signed int"
			|| typeName=="unsigned" || typeName=="unsigned int" || typeName=="long"
			|| typeName=="long int" || typeName=="signed long" || typeName=="signed long int"
			|| typeName=="unsigned long" || typeName=="unsigned long int"
			|| typeName=="long long" || typeName=="long long int" || typeName=="signed long long"
			|| typeName=="signed long long int" || typeName=="unsigned long long"
			|| typeName=="unsigned long long int";
}

bool GenericObject::isNumber32(const std::string& typeName) {
	return typeName=="short" || typeName=="short int" || typeName=="signed short" || typeName=="signed short int"
			|| typeName=="unsigned short" || typeName=="unsigned short int"
			|| typeName=="signed" || typeName=="int" || typeName=="signed int"
			|| typeName=="unsigned" || typeName=="unsigned int" || typeName=="long"
			|| typeName=="long int" || typeName=="signed long" || typeName=="signed long int";
}

bool GenericObject::isNumber64(const std::string& typeName) {
	return typeName=="unsigned long" || typeName=="unsigned long int"
			|| typeName=="long long" || typeName=="long long int" || typeName=="signed long long"
			|| typeName=="signed long long int" || typeName=="unsigned long long"
			|| typeName=="unsigned long long int";
}

bool GenericObject::isString(const std::string& typeName) {
	return typeName=="string" || typeName=="std::string"
			|| typeName=="char" || typeName=="signed char" || typeName=="unsigned char"
			|| typeName=="wchar_t";
}

bool GenericObject::isPrimitive(const std::string& typeName) {
	return isNumber(typeName) || isString(typeName) || isFPN(typeName) || typeName=="bool";
}

std::string GenericObject::getSerilaizedState() {
	std::string valueStr = objSerState;
	if(objVal==NULL)return std::string();
	if(typeName=="short") valueStr = CastUtil::fromNumber(*(short*)objVal);
	else if(typeName=="int") valueStr = CastUtil::fromNumber(*(int*)objVal);
	else if(typeName=="long") valueStr = CastUtil::fromNumber(*(long*)objVal);
	else if(typeName=="long long") valueStr = CastUtil::fromNumber(*(long long*)objVal);
	else if(typeName=="unsigned short") valueStr = CastUtil::fromNumber(*(unsigned short*)objVal);
	else if(typeName=="unsigned int") valueStr = CastUtil::fromNumber(*(unsigned int*)objVal);
	else if(typeName=="unsigned long") valueStr = CastUtil::fromNumber(*(unsigned long*)objVal);
	else if(typeName=="unsigned long long") valueStr = CastUtil::fromNumber(*(unsigned long long*)objVal);
	else if(typeName=="bool") valueStr = CastUtil::fromBool(*(bool*)objVal);
	else if(typeName=="float") valueStr = CastUtil::fromFloat(*(float*)objVal);
	else if(typeName=="double") valueStr = CastUtil::fromDouble(*(double*)objVal);
	else if(typeName=="long double") valueStr = CastUtil::fromLongdouble(*(long double*)objVal);
	else if(typeName=="std::string" || typeName=="string")  valueStr = *(std::string*)objVal;
	else if(typeName=="char" || typeName=="char const") {
		char* src = (char*)objVal;
		valueStr = std::string(src, cstr.size());
	}
	else if(typeName=="unsigned char" || typeName=="unsigned char const") {
		unsigned char* src = (unsigned char*)objVal;
		valueStr = std::string((char*)src, ucstr.size());
	}
	else if(typeName=="wchar_t" || typeName=="char wchar_t") {
		wchar_t* src = (wchar_t*)objVal;
		valueStr = std::string((char*)src, wstr.size());
	}
	return valueStr;
}

bool GenericObject::isFPN(const std::string& typeName) {
	return typeName=="float" || typeName=="double" || typeName=="long double";
}

bool GenericObject::isBool() {
	return typeName=="bool";
}

bool GenericObject::isFPN() {
	return typeName=="float" || typeName=="double" || typeName=="long double";
}

bool GenericObject::isString() {
	return typeName=="string" || typeName=="std::string"
			|| typeName=="char" || typeName=="signed char" || typeName=="unsigned char"
			|| typeName=="wchar_t";
}

bool GenericObject::isNull() {
	return typeName=="";
}

bool GenericObject::isObject() {
	return !isNumber() && !isBool() && !isString();
}

void* GenericObject::getPointer() {
	return objVal;
}
