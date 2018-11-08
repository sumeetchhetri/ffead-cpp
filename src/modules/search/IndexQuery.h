/*
 * IndexQuery.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef INDEXQUERY_H_
#define INDEXQUERY_H_

#include "string"
#include "map"

class IndexQuery {
private:
	int status;
	std::string error;
	std::string name;
	std::map<std::string, std::string> properties;
	std::map<std::string, std::string> mappings;
public:
	IndexQuery();
	virtual ~IndexQuery();
	const std::string& getError() const;
	IndexQuery& setError(const std::string& error);
	const std::map<std::string, std::string>& getMappings() const;
	IndexQuery& setMappings(const std::map<std::string, std::string>& mappings);
	const std::string& getName() const;
	IndexQuery& setName(const std::string& name);
	const std::map<std::string, std::string>& getProperties() const;
	IndexQuery& setProperties(const std::map<std::string, std::string>& properties);
	int getStatus() const;
	IndexQuery& setStatus(int status);
	IndexQuery& addProperty(std::string& prop, std::string& value);
	IndexQuery& addMapping(std::string& prop, std::string& value);
};

#endif /* INDEXQUERY_H_ */
