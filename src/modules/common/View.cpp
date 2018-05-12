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
 * View.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */
#include "View.h"

View::View()
{

}
View::~View()
{

}

std::string View::generateStartOpenTag(std::string tagName)
{
	std::string str = "<" + tagName;
	return str;
}

std::string View::generateEndOpenTag()
{
	std::string str = ">\n";
	return str;
}

std::string View::generateCloseTag(std::string tagName)
{
	std::string str = "</" + tagName + ">\n";
	return str;
}

std::string View::generateAttributes(AttributeList attributes)
{
	std::string str;
	AttributeList::iterator itr;
	for(itr = attributes.begin();itr!=attributes.end();itr++)
	{
		str += (" " + itr->first + "=\"" + itr->second + "\" ");
	}
	return str;
}

void View::traverseElement(std::string *ss, Element* element)
{
	ss->append(generateStartOpenTag(element->getTagName()));
	ss->append(generateAttributes(element->getAttributes()));
	ss->append(generateEndOpenTag());
	ss->append(element->getText());
	ElementList elements = element->getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		traverseElement(ss, &(elements.at(i)));
	}
	ss->append(generateCloseTag(element->getTagName()));
}

std::string View::generateDocument(Document& document)
{
	std::string ss;
	traverseElement(&ss, &(document.getRootElement()));
	return ss;
}


