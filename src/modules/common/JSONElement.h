/*
	Copyright 2009-2012, Sumeet Chhetri

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
 * JSONElement.h
 *
 *  Created on: 06-Aug-2012
 *      Author: sumeetc
 */

#ifndef JSONELEMENT_H_
#define JSONELEMENT_H_
#include "map"
#include "vector"
#include "string"


class JSONElement {
public:
	static enum {JSON_OBJECT, JSON_ARRAY, JSON_STRING, JSON_NUMBER, JSON_BOOL, JSON_FLOAT} JSON_TYPE;
	JSONElement();
	virtual ~JSONElement();
	bool hasChildren() const;
	//void addChild(const JSONElement& child);
	void addChild(JSONElement);
	const std::vector<JSONElement>& getChildren() const;
	//const JSONElement& getNode(const std::string& name);
	JSONElement* getNodeP(const std::string& name);
	int getType() const;
	void setType(const int& type);
	const std::string& getValue() const;
	void setValue(const std::string& value);
	const std::string& getName() const;
	void setName(const std::string& name);
	std::string toString() const;
private:
	static JSONElement nullele;
	std::string name;
	std::string value;
	int type;
	std::vector<JSONElement> children;
	std::map<std::string, JSONElement*> allnodes;
};

#endif /* JSONELEMENT_H_ */
