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
 * Nghttp2Server.h
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#ifndef SRC_SERVER_Nghttp2Server_H_
#define SRC_SERVER_Nghttp2Server_H_
#include "ServerInitUtil.h"
#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

class Nghttp2Server {
	static void handle(const request &req2, const response &res2, std::string_view meth, const uint8_t *, std::size_t);
public:	
	static void runServer(std::string ipaddr, std::string port, std::vector<std::string> servedAppNames, std::string serverRootDirectory, bool isSSLEnabled);
};

#endif /* SRC_SERVER_Nghttp2Server_H_ */
