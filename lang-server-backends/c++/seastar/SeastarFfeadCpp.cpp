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
	#include "ffead-cpp.h"
}
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
			ffead_cpp_resp_cleanup(fcpres);
		}

		ffead_request freq;
		freq.server_str = "seastar";
		freq.server_str_len = 7;
		freq.method = (const char*)req->_method.c_str();
		freq.method_len = req->_method.length();
		freq.path = (const char*)req->_url.c_str();
		freq.path_len = req->_url.length();
		freq.body_len = 0;
		if(req->content.length()>0) {
			freq.body = (const char*)req->content.c_str();
			freq.body_len = req->content.length();
		}
		freq.version = 1;
		freq.headers_len = req->_headers.size();

		phr_header_fcp f_headers[req->_headers.size()];
		std::unordered_map<sstring, sstring>::iterator it = req->_headers.begin();
		for(int i=0;it!=req->_headers.end();++it,i++) {
			f_headers[i].name = (const char*)it->first.c_str();
			f_headers[i].name_len = it->first.length();
			f_headers[i].value = (const char*)it->second.c_str();
			f_headers[i].value_len = it->second.length();
		}
		freq.headers = f_headers;

		int scode = 0;
		const char* out_url;
		size_t out_url_len;
		const char* out_body;
		size_t out_body_len;
		phr_header_fcp out_headers[50];
		size_t out_headers_len;
		const char* smsg;
		size_t smsg_len;

		fcpres = ffead_cpp_handle_c_1(&freq, &scode, &smsg, &smsg_len, &out_url, &out_url_len, out_headers, &out_headers_len, &out_body, &out_body_len);
		if(scode>0) {
			for(int i=0;i<(int)out_headers_len;i++) {
				rep->add_header(sstring(out_headers[i].name, out_headers[i].name_len), sstring(out_headers[i].value, out_headers[i].value_len));
			}
			rep->set_status(static_cast<reply::status_type>(scode));
			if(out_body_len>0) {
				rep->_content = http::reply_content(std::string_view(out_body, out_body_len));
			}
			rep->done();
			return make_ready_future<std::unique_ptr<reply>>(std::move(rep));
		} else {
			sstring url(out_url, out_url_len);
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
	app.add_options()("fcpdir", bpo::value<sstring>()->default_value("/installs/ffead-cpp-7.0"), "ffead-cpp directory");

	std::atomic<bool> fcp_init = false;

	return app.run_deprecated(ac, av, [&] {
		return seastar::async([&] {
			seastar_apps_lib::stop_signal stop_signal;
			auto&& config = app.configuration();

			if(!fcp_init) {
				fcp_init = true;

				printf("Bootstrapping ffead-cpp start...\n");
				ffead_cpp_bootstrap(config["fcpdir"].as<sstring>().c_str(), config["fcpdir"].as<sstring>().length(), 30);
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
