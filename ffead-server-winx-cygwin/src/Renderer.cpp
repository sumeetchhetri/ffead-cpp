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
 * Renderer.cpp
 *
 *  Created on: Jan 2, 2010
 *      Author: sumeet
 */

#include "Renderer.h"

Renderer::Renderer() {
	// TODO Auto-generated constructor stub

}

Renderer::~Renderer() {
	// TODO Auto-generated destructor stub
}

string Renderer::generateStartOpenTag(string tagName)
{
	string str = "<" + tagName;
	return str;
}
string Renderer::generateEndOpenTag()
{
	string str = ">";
	return str;
}
string Renderer::generateCloseTag(string tagName)
{
	string str = "</" + tagName + ">";
	return str;
}
string Renderer::generateAttributes(AttributeList attributes)
{
	string str;
	AttributeList::iterator itr;
	for(itr = attributes.begin();itr!=attributes.end();itr++)
	{
		str += (" " + itr->first + "=\"" + itr->second + "\" ");
	}
	return str;
}
