/*
	Copyright 2010, Sumeet Chhetri 
  
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
#include <boost/algorithm/string.hpp>
bool Element::operator == (Element ele)
{
	if(this->tagName == ele.tagName
	   && this->attributes == ele.attributes
	   && this->attributes == ele.attributes)
		return true;
	else
		return false;
}

bool Element::operator == (Element *ele)
{
	if(this->tagName == ele->tagName
	   && this->attributes == ele->attributes
	   && this->attributes == ele->attributes)
		return true;
	else
		return false;
}

void Element::addElement(Element element)
{
	this->elements.push_back(element);
	if(&(this->mapOfEle[element.getTagName()])!=NULL)
		this->mapOfEle[element.getTagName()] = element;
}
void Element::removeElement(Element element)
{
	for(unsigned int i=0;i<this->elements.size();i++)
	{
		Element ele = elements.at(i);
		if(element==ele)
		{
			this->elements.erase(elements.begin()+i);
			this->mapOfEle.erase(element.getTagName());
		}
	}
}

void Element::addAttribute(string key,string value)
{
	this->attributes[key] = value;
}
void Element::removeAttribute(string key)
{
	this->attributes.erase(key);
}

bool Element::isNull()
{
	if(this->getTagName()=="&#@^_NULL_&#@^")
		return true;
	else
		return false;
}
//void updateAttribute(string,string) = &addAttribute;
typedef map<string,string> AttributeList;
typedef vector<Element> ElementList;
AttributeList Element::getAttributes()
{
	return this->attributes;
}
string Element::getAttribute(string key)
{
	return this->attributes[key];
}
ElementList Element::getChildElements()
{
	return this->elements;
}
string Element::getTagName()
{
	return this->tagName;
}
string Element::getNameSpc()
{
	return this->nameSpace;
}
string Element::getNewTagNameSpc(string tag)
{
	if(this->nameSpace!="")
		return (this->nameSpace + ":" + tag);
	else
		return tag;
}

string Element::getTagNameSpc()
{
	if(this->nameSpace!="")
		return (this->nameSpace + ":" + this->tagName);
	else
		return this->tagName;
}
void Element::setTagName(string tagName)
{
	vector<string> vemp;
	boost::iter_split(vemp, tagName, boost::first_finder(":"));
	if(vemp.size()==2)
	{
		this->tagName = vemp.at(1);
		this->nameSpace = vemp.at(0);
	}
	else
		this->tagName = tagName;
}

string Element::getText() const
{
	return this->text;
}

void Element::setText(string text)
{
	this->text = text;
}

Element Element::getElementByName(string name)
{
	return this->mapOfEle[name];
}

string Element::render()
{
	string rend;
	//cout << this->getTagName() << flush;
	rend.append(generateStartOpenTag(this->getTagName()));
	rend.append(generateAttributes(this->getAttributes()));
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

string Element::renderChildren()
{
	string rend;
	ElementList elements = this->getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		rend.append(elements.at(i).render());
	}
	return rend;
}
