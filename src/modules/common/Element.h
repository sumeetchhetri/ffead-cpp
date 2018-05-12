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

#ifndef ELEMENT_H_
#define ELEMENT_H_


#include "map"
#include "vector"
#include "StringUtil.h"
#include "string"
#include "Renderer.h"
#include <iostream>


class Element;
typedef std::vector<Element> ElementList;
typedef std::map<std::string, std::string> AttributeList;
typedef std::map<std::string, Element*> ElementMap;

class Element : public Renderer
{
public:
	Element();
	~Element();

	//typedef std::vector<Element*> ElementList;
	void addElement(Element);
	void removeElement(Element*);
	//void updateElement(Element*);
	void addAttribute(const std::string& key, const std::string& value, const bool& validate= false);
	void removeAttribute(const std::string& key);
	const AttributeList& getAttributes() const;
	const std::string getAttribute(const std::string&) const;
	const ElementList& getChildElements() const;
	bool isNull();
	const std::string& getTagName() const;
	std::string getTagNameSpc() const;
	const std::string& getNameSpc();
	void setTagName(const std::string& tagName);
	bool operator == (const Element&) const;
	bool operator == (Element *);
	bool getCdata() const;
	void setCdata(const bool&);
	const std::string& getText() const;
	void setText(const std::string&);
	Element* getElementByName(const std::string&);
	Element* getElementByNameIgnoreCase(const std::string&);
	ElementList getElementsByName(const std::string& name);
	std::string render();
	std::string renderSerialization();
	std::string renderChildren() const;
	void validateNs();
	std::string getNameSpcValue();
	void copy(Element* to);
private:
	static Element nullele;
	bool isValidNamespace(Element* ele, const std::string& nameSpace);
	std::string getNmspc(Element* ele, const std::string& nameSpace);
	Element* parent;
	AttributeList attributes;
	AttributeList namespaces;
	AttributeList allnmspcs;
	ElementList elements;
	std::string tagName;
	std::string nameSpace;
	std::string text;
	bool cdata;
	ElementMap mapOfEle;
	friend class Document;
};
#endif


