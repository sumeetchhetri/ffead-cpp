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
 * LithiumServer.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "LithiumServer.h"

Logger logger;

void LithiumServer::runServer(int port, std::vector<std::string> servedAppNames) {

	std::function<void(li::http_request&, li::http_response&)> ffead_cpp_handler = [&](li::http_request& request, li::http_response& response) {
		std::string_view meth = request.http_ctx.method();
		request.http_ctx.index_headers();
		request.http_ctx.read_whole_body();
		HttpRequest req(request.http_ctx.header_map, request.http_ctx.url_.substr(0, request.http_ctx.url_.find_last_not_of(" ")+1), 
			request.http_ctx.get_parameters_string_, meth, request.http_ctx.http_version_, request.http_ctx.body_);

		HttpResponse respo;
		ServiceTask task;
		task.handle(&req, &respo);

		if(respo.isDone()) {
			for (int var = 0; var < (int)respo.getCookies().size(); var++)
			{
				response.set_header("Set-Cookie", respo.getCookies().at(var));
			}
			RMap::const_iterator it;
			for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
				response.set_header(it->first, it->second);
			}

			response.set_status(respo.getCode());
			std::string& data = respo.generateNginxApacheResponse();
			if(data.length()>0) {
				response.write(data);
			}
		} else {
			if(access(req.getUrl().c_str(), F_OK) != -1) {
				response.http_ctx.send_static_file(req.getUrl().c_str());
			} else {
				response.set_status(404);
			}
		}
	};

	li::http_api api;
	api.global_handler() = ffead_cpp_handler;

	//Start a http server.
	http_serve(api, port);
}

int main(int argc, char *argv[]) {
	if(argc == 1)
	{
		//cout << "No Server root directory specified, quitting..." << std::endl;
		return 0;
	}

	std::string serverRootDirectory = argv[1];
	serverRootDirectory += "/";
	if(serverRootDirectory.find("//")==0)
	{
		RegexUtil::replace(serverRootDirectory,"[/]+","/");
	}

	PropFileReader pread;
	std::string respath = serverRootDirectory + "resources/";
	propMap srprps = pread.getProperties(respath+"server.prop");

	std::ofstream local("/dev/null");
	if(StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])!="true") {
		//std::streambuf* cout_buff = std::cout.rdbuf();
		std::cout.rdbuf(local.rdbuf());
	}

	std::string port = srprps["PORT_NO"];
	std::string ipaddr = srprps["IP_ADDR"];
	std::string servingAppNames = srprps["SRV_APPS"];
	std::vector<std::string> servedAppNames = StringUtil::splitAndReturn<std::vector<std::string> >(servingAppNames, ",");

	propMultiMap mpmap = pread.getPropertiesMultiMap(respath+"server.prop");
	if(mpmap.find("VHOST_ENTRY")!=mpmap.end() && mpmap["VHOST_ENTRY"].size()>0) {
		//TODO
	}

	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::LITHIUM);
	ServerInitUtil::init(logger);
	LithiumServer::runServer(CastUtil::toInt(port), servedAppNames);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}
