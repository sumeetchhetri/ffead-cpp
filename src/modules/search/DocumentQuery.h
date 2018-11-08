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
	std::map<std::string, std::string> properties;
	std::map<std::string, std::string> fields;
	std::string data;
public:
	DocumentQuery();
	virtual ~DocumentQuery();

	const std::string& getData() const;
	const std::map<std::string, std::string>& getFields() const;
	void setFields(const std::map<std::string, std::string>& fields);
	const std::string& getId() const;
	void setId(const std::string& id);
	const std::string& getIndexName() const;
	void setIndexName(const std::string& indexName);
	const std::map<std::string, std::string>& getProperties() const;
	void setProperties(const std::map<std::string, std::string>& properties);

	template<class T> bool add(T& t) {
		if(data=="") {
			data = JSONSerialize::serialize(t);
		}
	}
	template<class T> bool update(T& t) {
		if(data=="") {
			data = JSONSerialize::serialize(t);
		}
	}
	template<class T> std::map<int, std::string> addMulti(std::vector<T>& vecT) {
		if(data=="") {
			data = JSONSerialize::serialize(vecT);
		}
	}
};

#endif /* DOCUMENTQUERY_H_ */
