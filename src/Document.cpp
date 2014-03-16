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

Document::Document() {
	// TODO Auto-generated constructor stub

}

Document::~Document() {
}

Element Document::getRootElement()
{
	return this->root;
}

void Document::setRootElement(Element root)
{
	this->root = root;
}


string Document::getDocType() const
{
	return docType;
}

void Document::setDocType(string docType)
{
	this->docType = docType;
}
Element Document::getElementByName(string name)
{
	Element ele = getElementByName(name,this->root);
	return ele;
}
Element Document::getElementByName(string name,Element ele)
{
	if(ele.getTagName()==name)
		return ele;
	else
	{
		typedef vector<Element> ElementList;
		ElementList chi = ele.getChildElements();
		for(unsigned int i=0;i<chi.size();i++)
		{
			Element ele1 = getElementByName(name,chi.at(i));
			if(!ele1.isNull())
				return ele1;
		}
	}
	Element nullele;
	nullele.setTagName("&#@^_NULL_&#@^");
	return nullele;
}
string Document::render()
{
	string rend = this->docType;
	Element element = this->getRootElement();
	rend.append(generateStartOpenTag(element.getTagName()));
	rend.append(generateAttributes(element.getAttributes()));
	rend.append(generateEndOpenTag());
	rend.append(element.getText());
	ElementList elements = element.getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		rend.append(elements.at(i).render());
	}
	rend.append(generateCloseTag(element.getTagName()));
	return rend;
}
