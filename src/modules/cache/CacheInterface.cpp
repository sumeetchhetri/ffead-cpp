/*
 * CacheInterface.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "CacheInterface.h"

CacheInterface::CacheInterface() {
}

CacheInterface::~CacheInterface() {
}

bool CacheInterface::set(const string& key, const char* value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const char* value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const char* value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const string& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const string& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const string& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const short& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const short& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const short& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const unsigned short& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const unsigned short& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const unsigned short& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const int& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const int& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const int& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const unsigned int& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const unsigned int& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const unsigned int& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const unsigned long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const unsigned long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const unsigned long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const long long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const long long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const long long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const unsigned long long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const unsigned long long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const unsigned long long& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const float& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const float& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const float& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const double& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const double& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const double& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const long double& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const long double& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const long double& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const string& key, const bool& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const string& key, const bool& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const string& key, const bool& value, const int& expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = replace(key, ob, expireSeconds);
	return status;
}

