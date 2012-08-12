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
 * RequestHandler.h
 *
 *  Created on: Aug 17, 2009
 *      Author: sumeet
 */

#ifndef REQUESTHANDLER_H_
#define REQUESTHANDLER_H_
#include <dlfcn.h>
#include "ClassInfo.h"
#include "CastUtil.h"
#include "View.h"
#include "XmlParser.h"
#include "TemplateHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "DynamicView.h"
#include "HttpRequest.h"
#include <algorithm>
#include "PropFileReader.h"
#include "AfcUtil.h"
#include "ServerThread.h"
#include "Thread.h"
#include "Mutex.h"

#define MAXBUFLEN 1024
//extern map<string,HttpSession> sessionMap;
class RequestHandler {
public:

	RequestHandler(int,string,string);

	~RequestHandler()
	{
		m_stoprequested = true;
		m_thread.join(); //Note 2
	}
	void service(int,string,string);
private:
    volatile bool m_stoprequested;
    Mutex m_mutex;
    Thread m_thread;
};

#endif /* REQUESTHANDLER_H_ */
