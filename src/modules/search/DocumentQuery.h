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
 * DocumentQuery.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef DOCUMENTQUERY_H_
#define DOCUMENTQUERY_H_

#include "string"
#include "map"
#include "vector"
#include "JSONSerialize.h"

class DocumentQuery {
private:
	std::string id;
	std::string indexName;
	std::string opType;
	std::map<std::string, std::string> properties;
	std::string data;
	bool commit;
	friend class SolrSearch;
public:
	DocumentQuery(bool commit = false);
	virtual ~DocumentQuery();

	void setData(const std::string &data);
	std::string& getData();
	std::map<std::string, std::string>& getFields();
	void setFields(const std::map<std::string, std::string>& fields);
	const std::string& getId() const;
	void setId(const std::string& id);
	const std::string& getIndexName() const;
	void setIndexName(const std::string& indexName);
	std::map<std::string, std::string>& getProperties();
	void setProperties(const std::map<std::string, std::string>& properties);
	const std::string& getOpType() const;
	void setOpType(const std::string &opType);

	template<class T> bool add(T& t) {
		if(data=="") {
			std::string cn = CastUtil::getClassName(t);
			int serOpt = SerializeBase::identifySerOption(cn);
			data = JSONSerialize::serialize(t, serOpt);
		}
		return false;
	}
	template<class T> bool update(T& t) {
		if(data=="") {
			std::string cn = CastUtil::getClassName(t);
			int serOpt = SerializeBase::identifySerOption(cn);
			data = JSONSerialize::serialize(t, serOpt);
		}
		return false;
	}
	template<class T> bool addMulti(std::vector<T>& vecT) {
		if(data=="") {
			std::string cn = CastUtil::getClassName(vecT);
			int serOpt = SerializeBase::identifySerOption(cn);
			data = JSONSerialize::serialize(vecT, serOpt);
		}
		return false;
	}
};

#endif /* DOCUMENTQUERY_H_ */
