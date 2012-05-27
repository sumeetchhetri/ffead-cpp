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
#ifndef COMPONENT_TEST_BEAN_REMOTE_H
#define COMPONENT_TEST_BEAN_REMOTE_H
#include "Service1.h"
#include "Service2.h"
#include "RemoteComponent.h"
class Component_TEST_BEAN_Remote : public RemoteComponent 
{
public:
~Component_TEST_BEAN_Remote();
Component_TEST_BEAN_Remote();
string myFirstService();
string mySecondService(string &_1);
};
#endif
