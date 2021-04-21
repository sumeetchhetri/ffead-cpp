/*
 * Collie - An asynchronous event-driven network framework using Dlang development
 *
 * Copyright (C) 2015-2018  Shanghai Putao Technology Co., Ltd
 *
 * Developer: Putao's Dlang team
 *
 * Licensed under the Apache-2.0 License.
 *
 */
import std.getopt;
import std.stdio;
import hunt.io;
import hunt.system.Memory : totalCPUs;
import http.Processor;
import http.Server;
import http.DemoProcessor;
import std.experimental.allocator;
import ffeadcpp;

void main(string[] args) {
	ushort port = 8080;
	string ip = "0.0.0.0";
	string sdir = "/root/ffead-cpp-6.0";
	GetoptResult o = getopt(args, "server-dir|s", "Server Directory (default /root/ffead-cpp-6.0)", &sdir, "ip|h", "IP Address (default 0.0.0.0)", &ip, "port|p", "Port (default 8080)", &port);
	if (o.helpWanted) {
		defaultGetoptPrinter("A mini-http server powered by Hunt!", o.options);
		return;
	}

	writefln("Bootstrapping ffead-cpp start...");
	ffead_cpp_bootstrap(cast(char*)sdir, sdir.length, 7);
	writefln("Bootstrapping ffead-cpp end...");

	writefln("Initializing ffead-cpp start...");
	ffead_cpp_init();
	writefln("Initializing ffead-cpp end...");
	
	AbstractTcpServer httpServer = new HttpServer!(DemoProcessor)(ip, port, totalCPUs);
	writefln("listening on http://%s", httpServer.bindingAddress.toString());
	httpServer.start();
}
