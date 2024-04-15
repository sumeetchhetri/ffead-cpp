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
 * Nghttp2Server.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "Nghttp2Server.h"

Logger logger;

void Nghttp2Server::handle(const request &req2, const response &res2, std::string_view meth, const uint8_t * data, std::size_t len) {
	std::string_view cont = std::string_view(HttpRequest::BLANK);
	if(data!=NULL && len>0) {
		cont = std::string_view(reinterpret_cast<const char *>(data), len);
	}
	phr_header_fcp headers[100];
	header_map::const_iterator it;
	int hc = 0;
	for(it=req2.header().begin();it!=req2.header().end();++it,hc++) {
		headers[hc].name = it->first.c_str();
		headers[hc].name_len = it->first.length();
		headers[hc].value = it->second.value.c_str();
		headers[hc].value_len = it->second.value.length();
	}

	std::string_view url = std::string_view(req2.uri().raw_path.data(), req2.uri().raw_path.length());
	std::string_view qv = std::string_view(req2.uri().raw_query.data(), req2.uri().raw_query.length());

	HttpRequest req(&headers, hc, url, qv, meth, 20, cont);
	HttpResponse respo;
	ServiceTask::handle(&req, &respo);

	auto hm = header_map();
	if(respo.isDone()) {
		for (int var = 0; var < (int)respo.getCookies().size(); var++)
		{
			hm.emplace(HttpResponse::SetCookie, header_value{value: respo.getCookies().at(var)});
		}
		RMap::const_iterator it;
		for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
			hm.emplace(it->first, header_value{value: std::string(it->second)});
		}

		res2.write_head(respo.getCode(), std::move(hm));
		std::string& data = respo.generateNginxApacheResponse();
		if(data.length()>0) {
			res2.end(data);
		}
	} else {
		if(access(req.getUrl().c_str(), F_OK) != -1) {
			auto fd = open(req.getUrl().c_str(), O_RDONLY);
			auto header = header_map();

			struct stat stbuf;
			if (stat(req.getUrl().c_str(), &stbuf) == 0) {
				header.emplace(HttpResponse::ContentLength, header_value{std::to_string(stbuf.st_size)});
				header.emplace(HttpResponse::LastModified, header_value{http_date(stbuf.st_mtime)});
			}
			res2.write_head(200, std::move(header));
			res2.end(file_generator_from_fd(fd));
		} else {
			res2.write_head(404);
			res2.end();
		}
	}
}

void Nghttp2Server::runServer(std::string ipaddr, std::string port, std::vector<std::string> servedAppNames, std::string serverRootDirectory, bool isSSLEnabled) {
	std::string certFile, keyFile;

	std::string respath = serverRootDirectory + "resources/";
	PropFileReader pread;
	propMap sslsec = pread.getProperties(respath+"/security.prop");
	if(sslsec.size()>0 && isSSLEnabled)
	{
		certFile = serverRootDirectory + sslsec["CERTFILE"];
		keyFile = serverRootDirectory + sslsec["KEYFILE"];
	}

	auto proc = []() -> int {
		std::ifstream cpuinfo("/proc/cpuinfo");
		return std::count(std::istream_iterator<std::string>(cpuinfo),
				std::istream_iterator<std::string>(),
				std::string("processor"));
	};

	auto num_threads = std::thread::hardware_concurrency();
	num_threads = num_threads ? num_threads : proc();
	boost::system::error_code ec;

	http2 server;
	server.num_threads(num_threads);

	server.handle("/", [](const request &req2, const response &res2) {
		std::string_view meth = std::string_view(req2.method().data(), req2.method().length());
		if(meth=="PUT" || meth=="POST") {
			req2.on_data([&req2, &res2, meth](const uint8_t *data, std::size_t len) {
				handle(req2, res2, meth, data, len);
			});
		} else {
			handle(req2, res2, meth, NULL, 0);
		}
	});

	if(isSSLEnabled && certFile!="" && keyFile!="") {
		boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);
		tls.use_private_key_file(keyFile.c_str(), boost::asio::ssl::context::pem);
		tls.use_certificate_chain_file(certFile.c_str());
		configure_tls_context_easy(ec, tls);

		if (server.listen_and_serve(ec, tls, ipaddr, port)) {
			std::cerr << "error: " << ec.message() << std::endl;
		}
	} else if (server.listen_and_serve(ec, ipaddr, port)) {
		std::cerr << "error: " << ec.message() << std::endl;
	}
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

	Nghttp2Server::runServer(ipaddr, port, servedAppNames, serverRootDirectory, isSSLEnabled);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}
