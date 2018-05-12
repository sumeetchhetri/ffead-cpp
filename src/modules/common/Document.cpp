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
 * Document.cpp
 *
 *  Created on: Sep 24, 2009
 *      Author: sumeet
 */

#include "Document.h"

Element Document::nullele;

Document::Document() {
	// TODO Auto-generated constructor stub
}

Document::~Document() {
}

Element& Document::getRootElement()
{
	return this->root;
}

void Document::setRootElement(const Element& root)
{
	this->root = root;
}

const std::string& Document::getDocType() const
{
	return docType;
}

void Document::setDocType(const std::string& docType)
{
	this->docType = docType;
}

Element* Document::getElementByName(const std::string& name)
{
	return getElementByName(name, &(this->root));
}

Element* Document::getElementByName(const std::string& name, Element* ele)
{
	if(ele->getTagName()==name)
		return ele;
	else
	{
		ElementList chi = ele->getChildElements();
		for(unsigned int i=0;i<chi.size();i++)
		{
			Element* ele1 = getElementByName(name, &(chi.at(i)));
			if(ele1!=NULL)
			{
				return ele1;
			}
		}
	}
	return NULL;
}

std::string Document::render()
{
	std::string rend = this->docType;
	rend.append(generateStartOpenTag(this->root.getTagName()));
	rend.append(generateAttributes(this->root.getAttributes()));
	rend.append(generateEndOpenTag());
	rend.append(this->root.getText());
	ElementList elements = this->root.getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		rend.append(elements.at(i).render());
	}
	rend.append(generateCloseTag(this->root.getTagName()));
	return rend;
}
