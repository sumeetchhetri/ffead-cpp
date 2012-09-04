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
typedef map<string,string> AttributeList;
typedef vector<Element> ElementList;
View::View()
{

}
View::~View()
{

}

string generateStartOpenTag(string tagName)
{
	string str = "<" + tagName;
	return str;
}
string generateEndOpenTag()
{
	string str = ">\n";
	return str;
}
string generateCloseTag(string tagName)
{
	string str = "</" + tagName + ">\n";
	return str;
}
string generateAttributes(AttributeList attributes)
{
	string str;
	AttributeList::iterator itr;
	for(itr = attributes.begin();itr!=attributes.end();itr++)
	{
		str += (" " + itr->first + "=\"" + itr->second + "\" ");
	}
	return str;
}

void traverseElement(string *ss,Element element)
{
	ss->append(generateStartOpenTag(element.getTagName()));
	ss->append(generateAttributes(element.getAttributes()));
	ss->append(generateEndOpenTag());
	ss->append(element.getText());
	ElementList elements = element.getChildElements();
	for(unsigned int i=0;i<elements.size();i++)
	{
		traverseElement(ss,elements.at(i));
	}
	ss->append(generateCloseTag(element.getTagName()));
}

string View::generateDocument(Document document)
{
	Element root = document.getRootElement();
	string ss;
	traverseElement(&ss,root);
	return ss;
}


