/*
	Copyright 2009-2020, Sumeet Chhetri 
  
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
	

}

Renderer::~Renderer() {
	
}

std::string Renderer::generateStartOpenTag(const std::string& tagName)
{
	std::string str = "<" + tagName;
	return str;
}
std::string Renderer::generateEndOpenTag()
{
	std::string str = ">";
	return str;
}
std::string Renderer::generateCloseTag(const std::string& tagName)
{
	std::string str = "</" + tagName + ">";
	return str;
}
std::string Renderer::generateAttributes(const AttributeList& attributes)
{
	std::string str;
	AttributeList::const_iterator itr;
	for(itr = attributes.begin();itr!=attributes.end();itr++)
	{
		str += (" " + itr->first + "=\"" + itr->second + "\" ");
	}
	return str;
}
