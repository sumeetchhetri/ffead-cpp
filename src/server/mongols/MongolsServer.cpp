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
 * MongolsServer.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "MongolsServer.h"

Logger logger;

void MongolsServer::runServer(std::string ipaddr, int port, std::vector<std::string> servedAppNames, std::string serverRootDirectory, bool isSSLEnabled) {
	auto rqf = [](const mongols::request&) {
		return true;
	};
	auto rsf = [](const mongols::request& req2, mongols::response& res) {
		std::string_view cont = std::string_view(HttpRequest::BLANK);
		if(req2.form.find("__body__")!=req2.form.end()) {
			cont = std::string_view(req2.form.find("__body__")->second.data(), req2.form.find("__body__")->second.length());
		}
		phr_header_fcp headers[100];
		std::unordered_map<std::string, std::string>::const_iterator it;
		int hc = 0;
		for(it=req2.headers.begin();it!=req2.headers.end();++it,hc++) {
			headers[hc].name = it->first.c_str();
			headers[hc].name_len = it->first.length();
			headers[hc].value = it->second.c_str();
			headers[hc].value_len = it->second.length();
		}

		std::string_view meth = std::string_view(req2.method.data(), req2.method.length());
		std::string_view url = std::string_view(req2.uri.data(), req2.uri.length());
		std::string_view qv = std::string_view(req2.param.data(), req2.param.length());

		HttpRequest req(&headers, hc, url, qv, meth, 11, cont);
		HttpResponse respo;
		ServiceTask task;
		task.handle(&req, &respo);

		if(respo.isDone()) {
			for (int var = 0; var < (int)respo.getCookies().size(); var++)
			{
				res.headers.emplace("Set-Cookie", respo.getCookies().at(var));
			}
			std::map<std::string,std::string>::const_iterator it;
			for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
				res.headers.emplace(it->first, it->second);
			}

			res.content = respo.getContent();
			res.status = respo.getCode();
		} else {
			if(access(req.getUrl().c_str(), F_OK) != -1) {
				char * buffer = 0;
				long length;
				FILE * f = fopen (req.getUrl().c_str(), "rb");
				if(f) {
					fseek (f, 0, SEEK_END);
					length = ftell (f);
					fseek (f, 0, SEEK_SET);
					buffer = (char*)malloc (length);
					if (buffer)
					{
						fread (buffer, 1, length, f);
					}
					fclose (f);
					res.headers.emplace("content-length", std::to_string(length));
					res.headers.emplace("content-type", CommonUtils::getMimeType(req.getExt()));
					res.content = std::string(buffer, length);
					res.status = 200;
					free(buffer);
				} else {
					res.status = 404;
				}
			} else {
				res.status = 404;
			}
		}
	};
	mongols::http_server server(ipaddr, port, 5000, 8192, 0);
	server.set_enable_session(false);
	server.set_enable_cache(false);
	server.set_shutdown([&]() {
		std::cout << "process " << getpid() << " exit.\n";
	});
	server.run(rqf, rsf);
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

	bool isSSLEnabled = false;
	std::string sslEnabled = srprps["SSL_ENAB"];
	if(sslEnabled=="true" || sslEnabled=="TRUE")
		isSSLEnabled = true;

	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::EMBEDDED_NGHTTP2);
	ServerInitUtil::init(logger);

	MongolsServer::runServer(ipaddr, CastUtil::toInt(port), servedAppNames, serverRootDirectory, isSSLEnabled);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}
