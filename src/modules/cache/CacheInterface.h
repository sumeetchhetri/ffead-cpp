/*
 * CacheInterface.h
 *
 *  Created on: 07-May-2014
 *      Author: sumeetc
 */

#ifndef CACHEINTERFACE_H_
#define CACHEINTERFACE_H_
#include "CastUtil.h"
#include "Connection.h"
#include "GenericObject.h"
#include "map"

class CacheInterface {
public:
	CacheInterface();
	virtual ~CacheInterface();
	virtual void init()=0;

	bool set(const std::string& key, const char* value, const int& expireSeconds);
	bool add(const std::string& key, const char* value, const int& expireSeconds);
	bool replace(const std::string& key, const char* value, const int& expireSeconds);

	bool set(const std::string& key, const std::string& value, const int& expireSeconds);
	bool add(const std::string& key, const std::string& value, const int& expireSeconds);
	bool replace(const std::string& key, const std::string& value, const int& expireSeconds);

	bool set(const std::string& key, const short& value, const int& expireSeconds);
	bool add(const std::string& key, const short& value, const int& expireSeconds);
	bool replace(const std::string& key, const short& value, const int& expireSeconds);
	bool set(const std::string& key, const unsigned short& value, const int& expireSeconds);
	bool add(const std::string& key, const unsigned short& value, const int& expireSeconds);
	bool replace(const std::string& key, const unsigned short& value, const int& expireSeconds);

	bool set(const std::string& key, const int& value, const int& expireSeconds);
	bool add(const std::string& key, const int& value, const int& expireSeconds);
	bool replace(const std::string& key, const int& value, const int& expireSeconds);
	bool set(const std::string& key, const unsigned int& value, const int& expireSeconds);
	bool add(const std::string& key, const unsigned int& value, const int& expireSeconds);
	bool replace(const std::string& key, const unsigned int& value, const int& expireSeconds);

	bool set(const std::string& key, const long& value, const int& expireSeconds);
	bool add(const std::string& key, const long& value, const int& expireSeconds);
	bool replace(const std::string& key, const long& value, const int& expireSeconds);
	bool set(const std::string& key, const unsigned long& value, const int& expireSeconds);
	bool add(const std::string& key, const unsigned long& value, const int& expireSeconds);
	bool replace(const std::string& key, const unsigned long& value, const int& expireSeconds);

	bool set(const std::string& key, const long long& value, const int& expireSeconds);
	bool add(const std::string& key, const long long& value, const int& expireSeconds);
	bool replace(const std::string& key, const long long& value, const int& expireSeconds);
	bool set(const std::string& key, const unsigned long long& value, const int& expireSeconds);
	bool add(const std::string& key, const unsigned long long& value, const int& expireSeconds);
	bool replace(const std::string& key, const unsigned long long& value, const int& expireSeconds);

	bool set(const std::string& key, const float& value, const int& expireSeconds);
	bool add(const std::string& key, const float& value, const int& expireSeconds);
	bool replace(const std::string& key, const float& value, const int& expireSeconds);

	bool set(const std::string& key, const double& value, const int& expireSeconds);
	bool add(const std::string& key, const double& value, const int& expireSeconds);
	bool replace(const std::string& key, const double& value, const int& expireSeconds);
	bool set(const std::string& key, const long double& value, const int& expireSeconds);
	bool add(const std::string& key, const long double& value, const int& expireSeconds);
	bool replace(const std::string& key, const long double& value, const int& expireSeconds);

	bool set(const std::string& key, const bool& value, const int& expireSeconds);
	bool add(const std::string& key, const bool& value, const int& expireSeconds);
	bool replace(const std::string& key, const bool& value, const int& expireSeconds);

	virtual bool set(const std::string& key, GenericObject& value, const int& expireSeconds)=0;
	virtual bool add(const std::string& key, GenericObject& value, const int& expireSeconds)=0;
	virtual bool replace(const std::string& key, GenericObject& value, const int& expireSeconds)=0;
	virtual bool remove(const std::string& key)=0;
	virtual long long increment(const std::string& key, const int& number= 1)=0;
	virtual long long decrement(const std::string& key, const int& number= 1)=0;
	virtual long double incrementFloat(const std::string& key, const double& number = 1.0)=0;
	virtual long double decrementFloat(const std::string& key, const double& number = 1.0)=0;
	virtual std::map<std::string, std::string> statistics()=0;
	virtual bool flushAll()=0;
	virtual std::string getValue(const std::string& key)=0;

	template<typename T> inline T get(const std::string& key) {
		std::string val = getValue(key);
		return GenericObject::getObjectFromSerilaizedState<T>(val);
	}

	virtual void* executeCommand(const std::string& command, ...)=0;
};

#endif /* CACHEINTERFACE_H_ */
