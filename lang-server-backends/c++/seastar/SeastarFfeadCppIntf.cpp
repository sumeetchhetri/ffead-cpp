/*
 * This file is open source software, licensed to you under the terms
 * of the Apache License, Version 2.0 (the "License").  See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.  You may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright 2015 Cloudius Systems
 */

#include <stdio.h>
#include <seastar/http/httpd.hh>
#include <seastar/http/handlers.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/reactor.hh>
#include <seastar/net/inet_address.hh>
#include <seastar/core/thread.hh>
#include <seastar/core/app-template.hh>
#include "string_view"
#include "stop_signal.hh"
extern "C"
{
#include "ffead-cpp-direct.h"
}
#include "ServiceTask.h"
#include <seastar/core/sstring.hh>
#include <seastar/core/fstream.hh>

namespace bpo = boost::program_options;

using namespace seastar;
using namespace httpd;

thread_local void* fcpres = NULL;

class FfeadCppHandler : public httpd::handler_base {
public:
	sstring get_extension(const sstring& file) {
		size_t last_slash_pos = file.find_last_of('/');
		size_t last_dot_pos = file.find_last_of('.');
		sstring extension;
		if (last_dot_pos != sstring::npos && last_dot_pos > last_slash_pos) {
			extension = file.substr(last_dot_pos + 1);
		}
		return extension;
	}

	future<std::unique_ptr<reply> > handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) {
		if(fcpres!=NULL) {
			delete (HttpResponse*)fcpres;
		}

		phr_header_fcp f_headers[req->_headers.size()];
		int hc = 0;
		std::unordered_map<sstring, sstring>::iterator it = req->_headers.begin();
		for(;it!=req->_headers.end();++it,hc++) {
			f_headers[hc].name = (const char*)it->first.c_str();
			f_headers[hc].name_len = it->first.length();
			f_headers[hc].value = (const char*)it->second.c_str();
			f_headers[hc].value_len = it->second.length();
		}

		std::string_view meth = std::string_view((const char*)req->_method.c_str(), req->_method.length());
		std::string_view url = std::string_view((const char*)req->_url.c_str(), req->_url.length());
		std::string_view cont = std::string_view(HttpRequest::BLANK);
		if(req->content.length()>0) {
			cont = std::string_view((const char*)req->content.c_str(), req->content.length());
		}
		HttpRequest freq(&f_headers, hc, url, meth, 11, cont);
		HttpResponse* respo = new HttpResponse;
		fcpres = respo;
		ServiceTask task;
		task.handle(&freq, respo);

		if(respo->isDone()) {
			RMap::const_iterator it = respo->getCHeaders().cbegin();
			for(;it!=respo->getCHeaders().cend();++it) {
				rep->add_header(sstring(it->first.c_str(), it->first.length()), sstring(it->second.c_str(), it->second.length()));
			}
			rep->set_status(static_cast<reply::status_type>(respo->getCode()));
			const std::string& cnt = respo->generateNginxApacheResponse();
			if(cnt.length()>0) {
				rep->_content = reply_content(std::string_view(cnt.c_str(), cnt.length()));
			}
			rep->done();
			return make_ready_future<std::unique_ptr<reply>>(std::move(rep));
		} else {
			sstring url(respo->getUrl().c_str(), respo->getUrl().length());
			auto h = this;
			return engine().file_type(url).then([h, url, req = std::move(req), rep = std::move(rep)](auto val) mutable {
				if (val) {
					sstring extension = h->get_extension(url);
					rep->write_body(extension, [req = std::move(req), extension, url] (output_stream<char>&& s) mutable {
						return do_with(output_stream<char>(std::move(s)), [url] (output_stream<char>& os) {
							return open_file_dma(url, open_flags::ro).then([&os] (file f) {
								return do_with(input_stream<char>(make_file_input_stream(std::move(f))), [&os](input_stream<char>& is) {
									return copy(is, os).then([&os] {
										return os.close();
									}).then([&is] {
										return is.close();
									});
								});
							});
						});
					});
				}
				rep->set_status(reply::status_type::not_found).done();
				return make_ready_future<std::unique_ptr<reply>>(std::move(rep));

			});
			return make_ready_future<std::unique_ptr<reply>>(std::move(rep));
		}
	}
};

void set_routes(routes& r) {
	r.add_default_handler(new FfeadCppHandler());
}

int main(int ac, char** av) {
	app_template app;

	app.add_options()("address", bpo::value<sstring>()->default_value("0.0.0.0"), "HTTP Server address");
	app.add_options()("port", bpo::value<uint16_t>()->default_value(10000), "HTTP Server port");
	app.add_options()("fcpdir", bpo::value<sstring>()->default_value("/installs/ffead-cpp-6.0"), "ffead-cpp directory");

	std::atomic<bool> fcp_init = false;

	return app.run_deprecated(ac, av, [&] {
		return seastar::async([&] {
			seastar_apps_lib::stop_signal stop_signal;
			auto&& config = app.configuration();

			if(!fcp_init) {
				fcp_init = true;

				printf("Bootstrapping ffead-cpp start...\n");
				ffead_cpp_bootstrap(config["fcpdir"].as<sstring>().c_str(), config["fcpdir"].as<sstring>().length(), 7);
				printf("Bootstrapping ffead-cpp end...\n");

				printf("Initializing ffead-cpp start...\n");
				ffead_cpp_init();
				printf("Initializing ffead-cpp end...\n");
			}

			net::inet_address addr(config["address"].as<sstring>());
			uint16_t port = config["port"].as<uint16_t>();

			auto server = new http_server_control();
			server->start().get();
			server->set_routes(set_routes).get();
			server->listen(socket_address{addr, port}).get();

			std::cout << "Seastar HTTP server listening on port " << port << " ...\n";
			engine().at_exit([server] {
				std::cout << "Stoppping HTTP server" << std::endl;

				printf("Cleaning up ffead-cpp start...\n");
				ffead_cpp_cleanup();
				printf("Cleaning up ffead-cpp end...\n");

				return server->stop();
			});

			stop_signal.wait().get();
		});
	});
}
