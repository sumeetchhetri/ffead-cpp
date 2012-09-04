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
 * CibernateConnectionPool.h
 *
 *  Created on: Jan 17, 2010
 *      Author: sumeet
 */

#ifndef CIBERNATECONNECTIONPOOL_H_
#define CIBERNATECONNECTIONPOOL_H_
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include "vector"
#include "string"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"
#include "Logger.h"
using namespace std;

class Connection
{
	Logger logger;
	Connection();
	friend class CibernateConnectionPool;
public:
	bool busy;
	bool type;
	SQLHDBC conn;
	~Connection();
};
typedef vector<Connection*> connVec;
class CibernateConnectionPool {
	Logger logger;
	int readNumber;
	string dbName,uname,pass;
	SQLHENV	V_OD_Env;// Handle ODBC environment
	connVec readConnections;
	connVec writeConnections;
	void newConnection(bool);
	void createPool(int,string,string,string);
public:
	CibernateConnectionPool(int,string,string,string);
	virtual ~CibernateConnectionPool();
	//static CibernateConnectionPool* getInstance();
	Connection* getReadConnection();
	Connection* getWriteConnection();
	void closeConnection(Connection*);
	bool initialized;
};

#endif /* CIBERNATECONNECTIONPOOL_H_ */
