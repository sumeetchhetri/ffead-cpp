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

Both **cmake** and **autoconf** builds are supported

Detailed OS specific instructions, instructions for installing the available server backends and docker/docker-compose scripts are located at [docker](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker)

Build validated for the following Operating systems/Toolchains
|   |   |   |
|---|---|---|
|[Ubuntu](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-UbuntuBionic-x64-ffead-cpp-5.0)|[CentOS](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Centos7-x64-ffead-cpp-5.0)|[OpenSUSE](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-OpenSuseTumbleweed-x64-ffead-cpp-5.0)|
|[Gentoo](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Gentoo-x64-ffead-cpp-5.0)|[ArchLinux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-ArchLinux-x64-ffead-cpp-5.0)|[Alpine Linux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Alpine-x64-ffead-cpp-5.0)|
|[MacOS](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-macos.yml)|[Windows Cygwin](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-win-cygwin.yml)|[Windows mingw-w64](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-win-mingw.yml)
|[ArchLinux (mingw-w64 cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-archlinux-mingw64)|[Ubuntu (musl cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-musl)|[Ubuntu (android cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-android)


[Latest Techempower benchmarks](https://www.techempower.com/benchmarks/#section=test&runid=223b29a9-851d-4f0d-9b3c-bb5b89d65d97&hw=ph&test=plaintext&a=2)

Features
==========
- Multiple server backends (HTTP 1.1/HTTP2/HTTP3)
    |   |   |   |   |
    |---|---|---|---|
    |embedded (HTTP1.1)|nghttp2 (HTTP2)|quiche (HTTP3)|nginx|
    |apache|cinatra (c++)|lithium (c++)|drogon (c++)|
    |libreactor \(c\)|h2o \(c\)|vweb (vlang)|picov (vlang)|
    |actix (rust)|hyper (rust)|thruster (rust)|rocket (rust)|
    |h2o\.cr (crystal)|crystal-http (crystal)|fasthttp (golang)|gnet (golang)|
    |firenio (java)|rapidoid (java)|wizzardo-http (java)|hunt (d)|
    |swift-nio (swift)|http.jl (julia)|mongols \(c\)|uv-cpp (c++)|
    |CppServer (c++)|openlitespeed (experimental)|||
- Web Socket Support
- Advanced ORM - SDORM (sql/monogo)
- Cache API (memcached/redis)
- Search Engine API (solr/elasticsearch) -- (experimental)
- Improved Thread/ThreadPool API(s)
- Marker based configuration (java style annotations)
- Reflection support
- Serialization support
- Date/Time Ultility functions
- Better Logging support
- HTTP2.0 Support (experimental)
- Dependency Injection
- Sample app for Webrtc Signalling (websocket + api) (horizontally scalable peerjs compatible signalling server)

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


Webrtc Example (Uses peerjs)
===========
- Follow Quickstart (Using cmake) above
- Build docker image from [docker-webrtc](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/webrtc-peerjs)
- Once the server starts, navigate to http://localhost:8080/peer-server/index.html in 2 tabs
- Enjoy p2p calling using the ffead-cpp webrtc (peerjs compatible) signalling server

For further details checkout the [wiki](https://github.com/sumeetchhetri/ffead-cpp/wiki) page 
