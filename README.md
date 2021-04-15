# The ffead-cpp Framework

ffead-cpp is a web-framework, application framework, utilities all bundled into one. 
It also provides an embedded HTTP/Web-Socket compliant high-performance server core. 
It is a collection of modules all geared towards performing individual roles which together form the cohesive back-bone of ffead-cpp.

It provides a very simple to use and maintain web-framework library with advanced features like Reflection, Dependency Injection (IOC),
Inbuilt REST/SOAP support, Security/Authentication features. Moreover implementation for interfacing to caching tools like 
Memcached/Redis are provided in-built. 
Database integration/ORM framework (SDORM) solves all major issues with respect to interfacing with SQL/No-SQL database alike.

Multi-level serialization or template level serialization is a high point of ffead-cpp core serialization run-time. Any C++ POCO class
can be marked as serializable and the run-time will take care of marshalling/un-marshalling the object to its intended serializable form
and back(JSON/XML/BINARY).

ffead-cpp can be easily driven by XML configuration, Services/Controllers/Filters/Interfaces/API(s) are all simple POCO classes without
any need for them to extend any classes. 
The differentiating feature in ffead-cpp is the usage/implementation/support for markers (Annotations in java). Simple **#pragma**
directives can now be used to drive the entire configuration in ffead-cpp, so you can just forget XML configuration.

All in all ffead-cpp is the gap in the world of C++ web application or enterprise application development which I have tried to fill
with my humble/honest effort.

**cmake**, **autoconf**, **meson** and **xmake** builds are supported

Detailed OS specific instructions, instructions for installing the available server backends and docker/docker-compose scripts are located at [docker](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker)

Build validated for the following Operating systems/Toolchains
|   |   |   |
|---|---|---|
|[Ubuntu](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-UbuntuBionic-x64-ffead-cpp-5.0)|[CentOS](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Centos7-x64-ffead-cpp-5.0)|[OpenSUSE](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-OpenSuseTumbleweed-x64-ffead-cpp-5.0)|
|[Gentoo](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Gentoo-x64-ffead-cpp-5.0)|[ArchLinux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-ArchLinux-x64-ffead-cpp-5.0)|[Alpine Linux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Alpine-x64-ffead-cpp-5.0)|
|[MacOS](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-macos.yml)|[Windows Cygwin](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-win-cygwin.yml)|[Windows mingw-w64](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-win-mingw.yml)
|[ArchLinux (mingw-w64 cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-archlinux-mingw64)|[Ubuntu (musl cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-musl)|[Ubuntu (android cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-android)
|[Solaris](https://github.com/sumeetchhetri/ffead-cpp/tree/master/vagrant/solaris)|[FreeBSD](https://github.com/sumeetchhetri/ffead-cpp/tree/master/vagrant/freebsd)|[Emscripten](https://github.com/sumeetchhetri/ffead-cpp/tree/master/lang-server-backends/javascript)


[Latest Techempower benchmarks](https://www.techempower.com/benchmarks/#section=data-r20&hw=ph&test=composite)

Features
==========
- Multiple server backends (HTTP 1.1/HTTP 2/HTTP 3)
    |   |   |   |   |
    |---|---|---|---|
    |embedded (HTTP1.1)|nghttp2 (HTTP2)|quiche (HTTP3)|seastar|
    |apache|cinatra (c++)|lithium (c++)|drogon (c++)|
    |libreactor \(c\)|h2o \(c\)|vweb (vlang)|picov (vlang)|
    |actix (rust)|hyper (rust)|thruster (rust)|rocket (rust)|
    |h2o\.cr (crystal)|crystal-http (crystal)|fasthttp (golang)|gnet (golang)|
    |firenio (java)|rapidoid (java)|wizzardo-http (java)|hunt (d)|
    |swift-nio (swift)|http.jl (julia)|mongols \(c\)|uv-cpp (c++)|
    |CppServer (c++)|nginx|openlitespeed (experimental)||
- Support for multiple I/O event notification engines,
	- io_uring - Linux kernel >= 5.6
	- epoll - Linux
	- kqueue - BSD/MacOS
	- event_ports/devpoll - Solaris
	- wepoll - Windows (IOCP based epoll library)
	- poll/select - On all platforms that support these API's
- Embedded HTTP Server with single process and thread-pool backends
- SSL/TLS support
- Web Socket Support
- Advanced ORM - SDORM (sql/monogo)
- Raw SQL API for postgresql (libpq)
- Cache API (memcached/redis)
- Improved Thread/ThreadPool API(s)
- Marker based configuration (java style annotations)
- Reflection support
- Serialization support
- Date/Time Ultility functions
- Dependency Injection
- Serverside dynamic C++ Pages and template engine (HTML/C++ pages)
- Sample app for Webrtc Signalling (websocket + api) (horizontally scalable peerjs compatible signalling server)
- Embedded HTTP2.0 Server support (experimental)

Quickstart (Using cmake/make)
===========
- Install cmake >= 3.8.2 and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- mkdir build && cd build
- cmake -DSRV_EMB=on -DMOD_SDORM_MONGO=on ..
- make install -j4 (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ../ffead-cpp-5.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

Quickstart (Using cmake/ninja)
===========
- Install cmake >= 3.8.2, ninja and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- mkdir build && cd build
- cmake -GNinja -DSRV_EMB=on -DMOD_SDORM_MONGO=on ..
- ninja install (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ../ffead-cpp-5.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

Quickstart (Using xmake)
===========
- Install xmake >= 2.5 and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- xmake f --cxflags="-I/usr/local/include -w" --MOD_SDORM_MONGO=true -v -D -c
- xmake && xmake install (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ffead-cpp-5.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

Quickstart (Using meson)
===========
- Install meson and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- meson setup build_meson && cd build_meson
- ninja (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ffead-cpp-5.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

Quickstart (Using autoconf)
===========
- Install autoconf, automake and libtool [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- ./autogen.sh
- ./configure --enable-srv_emb=yes --enable-mod_sdormmongo=yes
- make install -j4 (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ffead-cpp-5.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

For further details checkout the [wiki](https://github.com/sumeetchhetri/ffead-cpp/wiki) page 
