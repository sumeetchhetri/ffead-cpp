/*
	Copyright 2009-2012, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
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
using namespace std;

class JSONElement {
public:
	static enum {JSON_OBJECT, JSON_ARRAY, JSON_STRING, JSON_NUMBER, JSON_BOOL, JSON_FLOAT} JSON_TYPE;
	JSONElement();
	virtual ~JSONElement();
	bool hasChildren();
	void addChild(JSONElement* child);
	vector<JSONElement*> getChildren();
	JSONElement* getNode(const string& name);
	int getType() const;
	void setType(int type);
	string getValue() const;
	void setValue(string value);
	string getName() const;
	void setName(string name);
	string toString();
private:
	string name;
	string value;
	int type;
	vector<JSONElement*> children;
	map<string, JSONElement*> allnodes;
};

#endif /* JSONELEMENT_H_ */
