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
#include "ScyllaDBRawDataSourceImpl.h"

DSType ScyllaDBRawDataSourceImpl::getType() {
	return SD_RAW_CASS;
}

ScyllaDBRawDataSourceImpl::ScyllaDBRawDataSourceImpl(ConnectionPooler* pool) {
	props = &(pool->getProps());
	cluster = NULL;
	session = NULL;
}

ScyllaDBRawDataSourceImpl::~ScyllaDBRawDataSourceImpl() {
	cass_cluster_free(cluster);
	cass_session_free(session);
}

bool ScyllaDBRawDataSourceImpl::init() {
	cluster = cass_cluster_new();
	session = cass_session_new();
	//CassAuthenticatorCallbacks auth_callbacks = { on_auth_initial, on_auth_challenge, on_auth_success, on_auth_cleanup };
	//cass_cluster_set_contact_points(cluster, props->getNodes().at(0).getHost().c_str());
	//cass_cluster_set_authenticator_callbacks(cluster, &auth_callbacks, NULL, &(props->getNodes().at(0)));
	CassFuture* connect_future = cass_session_connect(session, cluster);
	connect_future = cass_session_connect(session, cluster);

	bool flag = true;
	if (cass_future_error_code(connect_future) == CASS_OK) {
		printf("Successfully connected!\n");
	} else {
		/* Handle error */
		const char* message;
		size_t message_length;
		cass_future_error_message(connect_future, &message, &message_length);
		fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length, message);
		flag = false;
	}

	cass_future_free(connect_future);
	return flag;
}

void ScyllaDBRawDataSourceImpl::setArgs(CassStatement* statement, std::list<LibpqParam>& pvals) {
	int var = 0;
	for(std::list<LibpqParam>::iterator it=pvals.begin(); it !=pvals.end(); ++it, var++) {
		switch(it->t) {
			case 1: {
				cass_statement_bind_int16(statement, var, it->s);
				break;
			}
			case 2: {
				cass_statement_bind_int32(statement, var, it->i);
				break;
			}
			case 3: {
				cass_statement_bind_int64(statement, var, it->l);
				break;
			}
			case 4: {
				cass_statement_bind_string(statement, var, it->sv.p);
				break;
			}
			case 5: {
				cass_statement_bind_string_n(statement, var, it->sv.p, it->sv.l);
				break;
			}
			/*case 6: {
				cass_statement_bind_string_n(statement, var, it->st.c_str(), it->st.length());
				break;
			}*/
		}
	}
}

void ScyllaQuery::withParamInt2(unsigned short i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.s = htons(i);
	par.t = 1;
}

void ScyllaQuery::withParamInt4(unsigned int i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.i = htonl(i);
	par.t = 2;
}

void ScyllaQuery::withParamInt8(long long i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.l = i;
	par.t = 3;
}

void ScyllaQuery::withParamStr(const char *i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.sv.p = i;
	par.sv.l = strlen(i);
	par.t = 4;
}

void ScyllaQuery::withParamBin(const char *i, size_t len) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.sv.p = i;
	par.sv.l = len;
	par.t = 5;
}

/*void ScyllaQuery::withParamStr(std::string& str) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.sv.st = str;
	par.t = 6;
}*/
