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
#include "ConnectionPooler.h"

class CacheInterface {
protected:
	int defaultExpireSeconds;
	ConnectionPooler* pool;
public:
	CacheInterface();
	virtual ~CacheInterface();
	virtual void init()=0;

	bool set(const std::string& key, const char* value, int expireSeconds = -1);
	bool add(const std::string& key, const char* value, int expireSeconds = -1);
	bool replace(const std::string& key, const char* value, int expireSeconds = -1);

	bool set(const std::string& key, const std::string& value, int expireSeconds = -1);
	bool add(const std::string& key, const std::string& value, int expireSeconds = -1);
	bool replace(const std::string& key, const std::string& value, int expireSeconds = -1);

	bool set(const std::string& key, const short& value, int expireSeconds = -1);
	bool add(const std::string& key, const short& value, int expireSeconds = -1);
	bool replace(const std::string& key, const short& value, int expireSeconds = -1);
	bool set(const std::string& key, const unsigned short& value, int expireSeconds = -1);
	bool add(const std::string& key, const unsigned short& value, int expireSeconds = -1);
	bool replace(const std::string& key, const unsigned short& value, int expireSeconds = -1);

	bool set(const std::string& key, const int& value, int expireSeconds = -1);
	bool add(const std::string& key, const int& value, int expireSeconds = -1);
	bool replace(const std::string& key, const int& value, int expireSeconds = -1);
	bool set(const std::string& key, const unsigned int& value, int expireSeconds = -1);
	bool add(const std::string& key, const unsigned int& value, int expireSeconds = -1);
	bool replace(const std::string& key, const unsigned int& value, int expireSeconds = -1);

	bool set(const std::string& key, const long& value, int expireSeconds = -1);
	bool add(const std::string& key, const long& value, int expireSeconds = -1);
	bool replace(const std::string& key, const long& value, int expireSeconds = -1);
	bool set(const std::string& key, const unsigned long& value, int expireSeconds = -1);
	bool add(const std::string& key, const unsigned long& value, int expireSeconds = -1);
	bool replace(const std::string& key, const unsigned long& value, int expireSeconds = -1);

	bool set(const std::string& key, const long long& value, int expireSeconds = -1);
	bool add(const std::string& key, const long long& value, int expireSeconds = -1);
	bool replace(const std::string& key, const long long& value, int expireSeconds = -1);
	bool set(const std::string& key, const unsigned long long& value, int expireSeconds = -1);
	bool add(const std::string& key, const unsigned long long& value, int expireSeconds = -1);
	bool replace(const std::string& key, const unsigned long long& value, int expireSeconds = -1);

	bool set(const std::string& key, const float& value, int expireSeconds = -1);
	bool add(const std::string& key, const float& value, int expireSeconds = -1);
	bool replace(const std::string& key, const float& value, int expireSeconds = -1);

	bool set(const std::string& key, const double& value, int expireSeconds = -1);
	bool add(const std::string& key, const double& value, int expireSeconds = -1);
	bool replace(const std::string& key, const double& value, int expireSeconds = -1);
	bool set(const std::string& key, const long double& value, int expireSeconds = -1);
	bool add(const std::string& key, const long double& value, int expireSeconds = -1);
	bool replace(const std::string& key, const long double& value, int expireSeconds = -1);

	bool set(const std::string& key, const bool& value, int expireSeconds = -1);
	bool add(const std::string& key, const bool& value, int expireSeconds = -1);
	bool replace(const std::string& key, const bool& value, int expireSeconds = -1);

	virtual bool set(const std::string& key, GenericObject& value, int expireSeconds)=0;
	virtual bool add(const std::string& key, GenericObject& value, int expireSeconds)=0;
	virtual bool replace(const std::string& key, GenericObject& value, int expireSeconds)=0;
	virtual bool remove(const std::string& key)=0;
	virtual long long increment(const std::string& key, const int& number = 1)=0;
	virtual long long decrement(const std::string& key, const int& number = 1)=0;
	virtual long double incrementFloat(const std::string& key, const double& number = 1.0)=0;
	virtual long double decrementFloat(const std::string& key, const double& number = 1.0)=0;
	virtual std::map<std::string, std::string> statistics()=0;
	virtual bool flushAll()=0;
	virtual std::string getValue(const std::string& key)=0;
	virtual std::vector<std::string> getValues(const std::vector<std::string>& keys)=0;

	template<typename T> inline bool setO(const std::string& key, const T& t, int expireSeconds = -1) {
		GenericObject o;
		o << t;
		if(expireSeconds==-1) {
			expireSeconds = this->defaultExpireSeconds;
		}
		return set(key, o, expireSeconds);
	}
	template<typename T> inline bool msetO(const std::map<std::string, T*>& omap, int expireSeconds = -1) {
		typedef typename std::map<std::string, T*>::iterator iterator_type;
		iterator_type it;
		if(expireSeconds==-1) {
			expireSeconds = this->defaultExpireSeconds;
		}
		bool fl = true;
		for(it=omap.begin();it!=omap.end();++it) {
			GenericObject o;
			o << it->second;
			fl &= set(it->first, o, expireSeconds);
		}
		return fl;
	}
	template<typename T> inline T getO(const std::string& key) {
		std::string val = getValue(key);
		return GenericObject::getObjectFromSerilaizedState<T>(val);
	}
	template<typename T> inline std::vector<T> mgetO(const std::vector<std::string>& keys) {
		std::vector<T> rv;
		std::vector<std::string> vals = getValues(keys);
		for(int i=0;i<(int)vals.size();++i) {
			rv.push_back(GenericObject::getObjectFromSerilaizedState<T>(vals.at(i)));
		}
		return rv;
	}
	template<typename T> inline void mgetO(const std::vector<std::string>& keys, std::vector<T>& rv) {
		std::vector<std::string> vals = getValues(keys);
		for(int i=0;i<(int)vals.size();++i) {
			rv.push_back(GenericObject::getObjectFromSerilaizedState<T>(vals.at(i)));
		}
	}
	virtual void* executeCommand(const std::string& command, ...)=0;
};

#endif /* CACHEINTERFACE_H_ */
