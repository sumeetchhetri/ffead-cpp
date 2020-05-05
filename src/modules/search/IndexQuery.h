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
