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
* Element.cpp
*
*  Created on: Sep 5, 2009
*      Author: sumeet
*/
#include "Element.h"

Element Element::nullele;

Element::Element() {
	cdata = false;
	parent = NULL;
}

Element::~Element() {
	for (int var = 0; var < (int)elements.size(); ++var) {
		//delete elements.at(var);
	}
}

void Element::copy(Element* to) {
	*to = *this;
	this->elements.clear();
}

bool Element::operator == (const Element& ele) const
{
	if(this->tagName == ele.tagName
	   && this->attributes == ele.attributes)
		return true;
	else
		return false;
}

bool Element::operator == (Element *ele)
{
	if(this->tagName == ele->tagName
	   && this->attributes == ele->attributes)
		return true;
	else
		return false;
}

void Element::addElement(Element element)
{
	this->elements.push_back(element);
	Element* el = &(this->elements.back());
	if(mapOfEle.find(el->getTagName())==mapOfEle.end())
	{
		this->mapOfEle[el->getTagName()] = el;
	}
}
void Element::removeElement(Element* element)
{
	for(unsigned int i=0;i<this->elements.size();i++)
	{
		if(*element==elements.at(i))
		{
			this->elements.erase(elements.begin()+i);
			this->mapOfEle.erase(element->getTagName());
		}
	}
}

void Element::addAttribute(const std::string& k, const std::string& value, const bool& validate)
{
	if(StringUtil::trimCopy(value)=="")return;
	if(StringUtil::trimCopy(k).find("xmlns:")==0)
	{
		std::string key = k.substr(6);
		if(validate && this->namespaces.find(key)!=this->namespaces.end())
		{
			//throw ("Duplicate namespace found - xmlns:" + key);
		}
		else
		{
			this->namespaces[key] = value;
		}
		this->attributes["xmlns:"+key] = value;
	}
	else
	{
		this->attributes[k] = value;
	}
}
void Element::removeAttribute(const std::string& key)
{
	this->attributes.erase(key);
}

bool Element::isNull()
{
	if(this->getTagName()=="")
		return true;
	else
		return false;
}

const AttributeList& Element::getAttributes() const
{
	return this->attributes;
}
const std::string Element::getAttribute(const std::string& key) const
{
	if(this->attributes.find(key)!=this->attributes.end())
	{
		return this->attributes.find(key)->second;
	}
	return "";
}
const ElementList& Element::getChildElements() const
{
	return elements;
}
const std::string& Element::getTagName() const
{
	return this->tagName;
}
const std::string& Element::getNameSpc()
{
	return this->nameSpace;
}

std::string Element::getTagNameSpc() const
{
	if(this->nameSpace!="")
		return (this->nameSpace + ":" + this->tagName);
	else
		return this->tagName;
}
void Element::setTagName(const std::string& tagName)
{
	std::vector<std::string> vemp;
	StringUtil::split(vemp, tagName, (":"));
	if(vemp.size()==2)
	{
		this->tagName = vemp.at(1);
		this->nameSpace = vemp.at(0);
	}
	else
		this->tagName = tagName;
}

const std::string& Element::getText() const
{
	return this->text;
}

void Element::setText(const std::string& text)
{
	this->text = text;
}

bool Element::getCdata() const
{
	return this->cdata;
}

void Element::setCdata(const bool& cdata)
{
	this->cdata = cdata;
}

Element* Element::getElementByName(const std::string& name)
{
	if(mapOfEle.find(name)!=mapOfEle.end())
	{
		return this->mapOfEle.find(name)->second;
	}
	return NULL;
}

Element* Element::getElementByNameIgnoreCase(const std::string& name)
{
	ElementMap::iterator it;
	std::string lname = StringUtil::toLowerCopy(name);
	for(it=mapOfEle.begin();it!=mapOfEle.end();++it)
	{
		std::string itn = StringUtil::toLowerCopy(it->first);
		if(itn==lname)
		{
			return it->second;
		}
	}
	return NULL;
}

ElementList Element::getElementsByName(const std::string& name)
{
	ElementList list;
	for(int i=0;i<(int)this->elements.size();i++)
	{
		if(this->elements.at(i).getTagName()==name)
			list.push_back(this->elements.at(i));
	}
	return list;
}

std::string Element::render()
{
	std::string rend;
	rend.append(generateStartOpenTag(this->getTagName()));
	rend.append(generateAttributes(this->getAttributes()));
	//rend.append(generateAttributes(this->namespaces));
	rend.append(generateEndOpenTag());
	rend.append(this->getText());
	ElementList elements = this->getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		rend.append(elements.at(i).render());
	}
	rend.append(generateCloseTag(this->getTagName()));
	return rend;
}

std::string Element::renderSerialization()
{
	std::string rend;
	rend.append(generateStartOpenTag(this->getTagName()));
	rend.append(generateAttributes(this->getAttributes()));
	//rend.append(" namespace=\""+getNameSpcValue()+"\"");
	rend.append(generateEndOpenTag());
	rend.append(this->getText());
	ElementList elements = this->getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		rend.append(elements.at(i).renderSerialization());
	}
	rend.append(generateCloseTag(this->getTagName()));
	return rend;
}

std::string Element::renderChildren() const
{
	std::string rend;
	ElementList elements = this->getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		rend.append(elements.at(i).render());
	}
	return rend;
}

bool Element::isValidNamespace(Element* ele, const std::string& nameSpace)
{
	bool valid = false;
	if(ele==NULL)
	{
		return false;
	}

	if(ele->namespaces.find(nameSpace)!=ele->namespaces.end())
	{
		if(ele->namespaces.find(nameSpace)!=ele->namespaces.end())
		{
			valid = true;
		}
	}
	if(!valid) {
		valid = isValidNamespace(ele->parent, nameSpace);
	}
	return valid;
}

void Element::validateNs()
{
	if(nameSpace=="" || isValidNamespace(this, nameSpace))
	{
		ElementList elements = this->getChildElements();
		for(unsigned int i=0;i<elements.size();i++)
		{
			elements.at(i).parent = this;
			elements.at(i).validateNs();
		}
	}
	else
	{
		throw ("No namespace definition found - xmlns:" + nameSpace);
	}
}


std::string Element::getNmspc(Element* ele, const std::string& nameSpace)
{
	if(ele==NULL)
	{
		return "";
	}

	if(ele->namespaces.find(nameSpace)!=ele->namespaces.end())
	{
		if(ele->namespaces.find(nameSpace)!=ele->namespaces.end())
		{
			return ele->namespaces[nameSpace];
		}
	}
	return getNmspc(ele->parent, nameSpace);
}

std::string Element::getNameSpcValue()
{
	return getNmspc(this, nameSpace);
}
