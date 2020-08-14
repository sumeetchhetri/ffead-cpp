/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
 * LibpqDataSourceImpl.h
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */
#ifndef LibpqDataSourceIMPL_H_
#define LibpqDataSourceIMPL_H_
#include "Compatibility.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_LIBPQ
#include "libpq-fe.h"
#endif
#include "DataSourceInterface.h"
#include "DialectHelper.h"

struct LibpqRes {
	const char* d;
	int l;
};

typedef void (*LipqResFunc) (void* ctx, int, std::vector<LibpqRes>&);

class LibpqDataSourceImpl {
	Logger logger;
	std::string url;
	bool trx;
#ifdef HAVE_LIBPQ
	PGconn* conn; //statement
#endif
public:
	LibpqDataSourceImpl(const std::string&);
	~LibpqDataSourceImpl();

	bool init();

	bool begin();
	bool commit();
	bool rollback();

	void executeQuery(const std::string &query, const std::vector<const char*>& pvals, void* ctx, LipqResFunc cb);
	bool executeUpdateQuery(const std::string &query, const std::vector<const char*>& pvals);
};

#endif /* LibpqDataSourceIMPL_H_ */
