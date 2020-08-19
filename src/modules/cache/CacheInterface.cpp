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
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return setRaw(key, value, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const char* value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return addRaw(key, value, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const char* value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return replaceRaw(key, value, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const std::string& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return setRaw(key, value.c_str(), expireSeconds);
}

bool CacheInterface::add(const std::string& key, const std::string& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return addRaw(key, value.c_str(), expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const std::string& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return replaceRaw(key, value.c_str(), expireSeconds);
}

bool CacheInterface::set(const std::string& key, const short& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[7];
	sprintf(v, "%d", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const short& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[7];
	sprintf(v, "%d", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const short& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[7];
	sprintf(v, "%d", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const unsigned short& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[7];
	sprintf(v, "%d", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const unsigned short& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[7];
	sprintf(v, "%d", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const unsigned short& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[7];
	sprintf(v, "%d", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const int& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%d", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const int& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%d", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const int& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%d", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const unsigned int& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[6];
	sprintf(v, "%u", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const unsigned int& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%u", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const unsigned int& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%u", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%ld", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%ld", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%ld", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const unsigned long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%lu", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const unsigned long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%lu", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const unsigned long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[11];
	sprintf(v, "%lu", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const long long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[21];
	sprintf(v, "%lld", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const long long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[21];
	sprintf(v, "%lld", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const long long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[21];
	sprintf(v, "%lld", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const unsigned long long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[6];
	sprintf(v, "%llu", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const unsigned long long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[22];
	sprintf(v, "%llu", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const unsigned long long& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[22];
	sprintf(v, "%llu", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const float& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[22];
	sprintf(v, "%f", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const float& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[317];
	sprintf(v, "%f", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const float& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[317];
	sprintf(v, "%f", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const double& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[317];
	sprintf(v, "%f", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const double& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[317];
	sprintf(v, "%f", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const double& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[317];
	sprintf(v, "%f", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const long double& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[500];
	sprintf(v, "%Lf", value);
	return setRaw(key, v, expireSeconds);
}

bool CacheInterface::add(const std::string& key, const long double& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[500];
	sprintf(v, "%Lf", value);
	return addRaw(key, v, expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const long double& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	char v[500];
	sprintf(v, "%Lf", value);
	return replaceRaw(key, v, expireSeconds);
}

bool CacheInterface::set(const std::string& key, const bool& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return setRaw(key, value?"1":"0", expireSeconds);
}

bool CacheInterface::add(const std::string& key, const bool& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return addRaw(key, value?"1":"0", expireSeconds);
}

bool CacheInterface::replace(const std::string& key, const bool& value, int expireSeconds) {
	if(expireSeconds==-1) {
		expireSeconds = this->defaultExpireSeconds;
	}
	return replaceRaw(key, value?"1":"0", expireSeconds);
}

