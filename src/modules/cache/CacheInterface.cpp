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
 * CacheInterface.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "CacheInterface.h"

CacheInterface::CacheInterface() {
	this->defaultExpireSeconds = -1;
	pool = NULL;
}

CacheInterface::~CacheInterface() {
}

bool CacheInterface::set(const std::string& key, const char* value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const char* value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const char* value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const std::string& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const std::string& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const std::string& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const short& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const short& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const short& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const unsigned short& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const unsigned short& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const unsigned short& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const int& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const int& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const int& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const unsigned int& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const unsigned int& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const unsigned int& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const unsigned long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const unsigned long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const unsigned long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const long long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const long long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const long long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const unsigned long long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const unsigned long long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const unsigned long long& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const float& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const float& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const float& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const double& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const double& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const double& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const long double& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const long double& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const long double& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::set(const std::string& key, const bool& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = set(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::add(const std::string& key, const bool& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = add(key, ob, expireSeconds);
	return status;
}

bool CacheInterface::replace(const std::string& key, const bool& value, int expireSeconds) {
	GenericObject ob;
	ob.set(value);
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	bool status = replace(key, ob, expireSeconds);
	return status;
}

