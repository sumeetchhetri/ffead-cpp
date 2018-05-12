/*
 * GenericObject.h
 *
 *  Created on: 09-Oct-2014
 *      Author: sumeetc
 */

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#ifndef GENERICOBJECTTYPE_H_
#define GENERICOBJECTTYPE_H_
#include <cxxabi.h>
#include <typeinfo>
#include <wchar.h>
#include "XMLSerialize.h"
#include "Reflector.h"

class GenericObject {
	std::string typeName;
	std::string objSerState;
	void* objVal;
	std::vector<char> cstr;
	std::vector<unsigned char> ucstr;
	std::vector<wchar_t> wstr;
	void internalClear();
	void internalCopy(const GenericObject &obj);
public:
	GenericObject();
	GenericObject& operator = (const GenericObject &obj);
	GenericObject(const GenericObject &obj);
	~GenericObject();
	const std::string& getTypeName() const;
	bool isInstanceOf(const std::string&);
	bool isNumber();
	bool isBool();
	bool isFPN();
	bool isString();
	bool isObject();
	bool isNull();
	std::string getSerilaizedState();
	void* getPointer();

	static bool isNumber(const std::string& typeName);
	static bool isNumber32(const std::string& typeName);
	static bool isNumber64(const std::string& typeName);
	static bool isString(const std::string& typeName);
	static bool isFPN(const std::string& typeName);
	static bool isPrimitive(const std::string& typeName);
	template <typename T> static T getObjectFromSerilaizedState(const std::string& serilaizedState);

	template <typename T> void operator<<(T &t)
	{
		set(t);
	}

	template <typename T> void operator<<(T *t)
	{
		set(t);
	}

	template<class T> void set(T* t, const size_t& strlength= std::string::npos) {
		std::string typeName = CastUtil::getClassName(t);
		set((void*)t, typeName, strlength);
	}

	void set(void* t, std::string typeName, size_t strlength= std::string::npos) {
		if(typeName.at(typeName.length()-1)=='*')
		{
			typeName = typeName.substr(0, typeName.length()-1);
		}
		if(typeName.at(typeName.length()-1)=='*')
		{
			throw "Cannot handle double pointers and beyond...";
		}
		if(typeName.find(",")!=std::string::npos)
		{
			typeName = typeName.substr(0, typeName.find(",")+1);
		}
		this->typeName = typeName;
		internalClear();
		if(typeName=="short") objVal = new short(*(short*)t);
		else if(typeName=="int") objVal = new int(*(int*)t);
		else if(typeName=="long") objVal = new long(*(long*)t);
		else if(typeName=="long long") objVal = new long long(*(long long*)t);
		else if(typeName=="unsigned short") objVal = new unsigned short(*(unsigned short*)t);
		else if(typeName=="unsigned int") objVal = new unsigned int(*(unsigned int*)t);
		else if(typeName=="unsigned long") objVal = new unsigned long(*(unsigned long*)t);
		else if(typeName=="unsigned long long") objVal = new unsigned long long(*(unsigned long long*)t);
		else if(typeName=="bool") objVal = new bool(*(bool*)t);
		else if(typeName=="float") objVal = new float(*(float*)t);
		else if(typeName=="double") objVal = new double(*(double*)t);
		else if(typeName=="long double") objVal = new long double(*(long double*)t);
		else if(typeName=="std::string" || typeName=="string") objVal = new std::string(*(std::string*)t);
		else if(typeName=="char" || typeName=="char const") {
			char* src = (char*)t;
			if(src) {
				cstr.assign(src, src+strlen(src));
				char* cstrptr = new char[cstr.size()];
				std::copy(cstr.begin(), cstr.end(), cstrptr);
				objVal = cstrptr;
			}
		}
		else if(typeName=="unsigned char" || typeName=="unsigned char const") {
			unsigned char* src = (unsigned char*)t;
			if(src) {
				ucstr.assign(src, src+strlen((const char*)src));
				unsigned char* ucstrpr = new unsigned char[ucstr.size()];
				std::copy(ucstr.begin(), ucstr.end(), ucstrpr);
				objVal = ucstrpr;
			}
		}
		else if(typeName=="wchar_t" || typeName=="wchar_t const") {
			wchar_t* src = (wchar_t*)t;
			if(src) {
				wstr.assign(src, src+wcslen(src));
				wchar_t* wstrpr = new wchar_t[wstr.size()];
				std::copy(wstr.begin(), wstr.end(), wstrpr);
				objVal = wstrpr;
			}
		}
		//This mean this is some other object, try to serialize it...
		else
		{
			objSerState = XMLSerialize::serializeUnknown(t, typeName);
			objVal = XMLSerialize::unSerializeUnknown(objSerState, typeName);
		}
	}

	template<class T> void set(T t, const size_t& strlength= -1) {
		std::string typeName = CastUtil::getClassName(t);
		if(typeName.at(typeName.length()-1)=='*')
		{
			throw "Cannot handle pointer types use 'set(T* t)' instead...";
		}
		this->typeName = typeName;
		internalClear();
		if(typeName=="short") objVal = new short(*(short*)&t);
		else if(typeName=="int") objVal = new int(*(int*)&t);
		else if(typeName=="long") objVal = new long(*(long*)&t);
		else if(typeName=="long long") objVal = new long long(*(long long*)&t);
		else if(typeName=="unsigned short") objVal = new unsigned short(*(unsigned short*)&t);
		else if(typeName=="unsigned int") objVal = new unsigned int(*(unsigned int*)&t);
		else if(typeName=="unsigned long") objVal = new unsigned long(*(unsigned long*)&t);
		else if(typeName=="unsigned long long") objVal = new unsigned long long(*(unsigned long long*)&t);
		else if(typeName=="bool") objVal = new bool(*(bool*)&t);
		else if(typeName=="float") objVal = new float(*(float*)&t);
		else if(typeName=="double") objVal = new double(*(double*)&t);
		else if(typeName=="long double") objVal = new long double(*(long double*)&t);
		else if(typeName=="std::string" || typeName=="string") objVal = new std::string(*(std::string*)&t);
		else if(typeName=="char" || typeName=="char const") {
			char* src = (char*)&t;
			if(src) {
				size_t stln = strlength;
				if(stln==std::string::npos) {
					stln = strlen(src);
				}
				cstr.assign(src, src+stln);
				char* cstrptr = new char[cstr.size()];
				std::copy(cstr.begin(), cstr.end(), cstrptr);
				objVal = cstrptr;
			}
		}
		else if(typeName=="unsigned char" || typeName=="unsigned char const") {
			unsigned char* src = (unsigned char*)&t;
			if(src) {
				size_t stln = strlength;
				if(stln==std::string::npos) {
					stln = strlen((char*)src);
				}
				ucstr.assign(src, src+strlength);
				unsigned char* ucstrpr = new unsigned char[ucstr.size()];
				std::copy(ucstr.begin(), ucstr.end(), ucstrpr);
				objVal = ucstrpr;
			}
		}
		else if(typeName=="wchar_t" || typeName=="wchar_t const") {
			wchar_t* src = (wchar_t*)&t;
			if(src) {
				size_t stln = strlength;
				if(stln==std::string::npos) {
					stln = wcslen(src);
				}
				wstr.assign(src, src+strlength);
				wchar_t* wstrpr = new wchar_t[wstr.size()];
				std::copy(wstr.begin(), wstr.end(), wstrpr);
				objVal = wstrpr;
			}
		}
		//This means this is some other object, try to serialize it...
		else
		{
			objSerState = XMLSerialize::serialize<T>(t);
			objVal = XMLSerialize::unserializeToPointer<T>(objSerState);
		}
	}

	template<class T> void get(T*& t) {
		std::string typeName = CastUtil::getClassName(t);
		if(typeName.at(typeName.length()-1)=='*')
		{
			typeName = typeName.substr(0, typeName.length()-1);
		}
		if(typeName.at(typeName.length()-1)=='*')
		{
			throw "Cannot handle double pointers and beyond...";
		}

		t = (T*)objVal;
	}

	template<class T> void get(T& t) {
		std::string typeName = CastUtil::getClassName(t);
		if(typeName.at(typeName.length()-1)=='*')
		{
			throw "Cannot handle pointer types use 'getP()' instead...";
		}
		t = *(T*)objVal;
	}
};

template<typename T>
inline T GenericObject::getObjectFromSerilaizedState(const std::string& serilaizedState) {
	T t;
	std::string typeName = CastUtil::getClassName(t);
	if(typeName.at(typeName.length()-1)=='*')
	{
		throw "Cannot handle pointer types use 'getP()' instead...";
	}
	if(GenericObject::isPrimitive(typeName)) t = CastUtil::lexical_cast<T>(serilaizedState);
	else t = XMLSerialize::unserialize<T>(serilaizedState);
	return t;
}

#endif /* GENERICOBJECTTYPE_H_ */
