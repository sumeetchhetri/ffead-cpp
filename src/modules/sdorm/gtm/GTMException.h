/*=========================================================================
 *
 *  Copyright OSEHRA
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//Copied from https://github.com/OSEHRA-Sandbox/gtm-bindings/blob/master/Source/GTMException.h

#ifndef __GTMException_h
#define __GTMException_h

#include <cstring>
#include <sstream>

class GTMException : public std::exception
{
public:

  GTMException( const char * errorMessage )
  {
    std::ostd::stringstream messageStream;
    messageStream << errorMessage;
    this->message = messageStream.str().c_str();
  }

private:

  const char * message;
};

#endif
