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
#ifdef HAVE_LIBODBC
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#endif
#include "vector"
#include "string"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"
#include "LoggerFactory.h"
#include "Mutex.h"
using namespace std;

class Connection
{
	Logger logger;
	Connection();
	friend class CibernateConnectionPool;
public:
	bool busy;
	bool type;
#ifdef HAVE_LIBODBC
	SQLHDBC conn;
#endif
	~Connection();
};
typedef vector<Connection*> connVec;
class CibernateConnectionPool {
	Logger logger;
	int readNumber;
	string dbName,uname,pass,dialect;
#ifdef HAVE_LIBODBC
	SQLHENV	V_OD_Env;// Handle ODBC environment
#endif
	connVec readConnections;
	connVec writeConnections;
	Mutex mutex;
	Connection* newConnection(bool);
	void createPool(int,string,string,string,string);
public:
	string getDialect() const;
	CibernateConnectionPool(int,string,string,string,string);
	virtual ~CibernateConnectionPool();
	Connection* getReadConnection();
	Connection* getWriteConnection();
	void closeConnection(Connection*);
	bool initialized;
};

#endif /* CIBERNATECONNECTIONPOOL_H_ */
