# The ffead-cpp Framework

[![CodeQL](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/codeql-analysis.yml)

[![Almalinux](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-almalinux.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-almalinux.yml)
[![Alpine](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-alpine.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-alpine.yml)
[![Archlinux](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-archlinux.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-archlinux.yml)
[![Gentoo](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-gentoo.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-gentoo.yml)
[![MacOS](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-macos.yml)
[![Opensuse](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-opensuse.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-opensuse.yml)
[![Ubuntu](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-ubuntu.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-ubuntu.yml)
[![Windows Cygwin](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-win-cygwin.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-win-cygwin.yml)
[![Windows Mingw-w64](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-win-mingw.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-win-mingw.yml)

[![Musl Cross Compilation](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-cross-musl.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-cross-musl.yml)
[![Dockcross uclibc-wrt Compilation](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-dockcross-uclibc-wrt.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-dockcross-uclibc-wrt.yml)
[![Android Cross Compilation](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-cross-android.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-cross-android.yml)
[![Mingw-w64 Cross Compilation](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-cross-mingw-w64.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-cross-mingw-w64.yml)

[![Bazel build](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-bazel.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-bazel.yml)
[![Buck2 build](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-buck2.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-buck2.yml)
[![Meson build](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-meson.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-meson.yml)
[![Scons build](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-scons.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-scons.yml)
[![Shellb build](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-shellb.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-shellb.yml)
[![Xmake build](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-xmake.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-xmake.yml)

[![Nodejs and Emscripten](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-emscripten-nodejs.yml/badge.svg)](https://github.com/sumeetchhetri/ffead-cpp/actions/workflows/ci-emscripten-nodejs.yml)

**According to the latest tfb results, ffead-cpp [embedded+v-picov] is currently the 12th fastest web/http/sql framework in the world** [Latest Techempower benchmarks](https://tfb-status.techempower.com)

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

**cmake**, **autoconf**, **meson**, **xmake**, **scons**, **[shellb](https://github.com/sumeetchhetri/shellb)**, **bazel** and **buck2** builds are supported

Detailed OS specific instructions, instructions for installing the available server backends and docker/docker-compose scripts are located at [docker](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker)

Build validated for the following Operating systems/Toolchains
|   |   |   |
|---|---|---|
|[Ubuntu](https://github.com/sumeetchhetri/ffead-cpp/blob/master/docker/os-based/DockerFile-Ubuntu-x64-ffead-cpp)|[Almalinux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Almalinux-x64-ffead-cpp)|[OpenSUSE](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-OpenSuse-x64-ffead-cpp)|
|[Gentoo](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Gentoo-x64-ffead-cpp)|[ArchLinux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-ArchLinux-x64-ffead-cpp)|[Alpine Linux](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based/DockerFile-Alpine-x64-ffead-cpp)|
|[MacOS](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-macos.yml)|[Windows Cygwin](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-win-cygwin.yml)|[Windows mingw-w64](https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows/ci-win-mingw.yml)
|[ArchLinux (mingw-w64 cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-archlinux-mingw64)|[Ubuntu (musl cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-musl)|[Ubuntu (android cross compiler)](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/DockerFile-cross-android)
|[Solaris](https://github.com/sumeetchhetri/ffead-cpp/tree/master/vagrant/solaris)|[FreeBSD](https://github.com/sumeetchhetri/ffead-cpp/tree/master/vagrant/freebsd)|[Emscripten](https://github.com/sumeetchhetri/ffead-cpp/tree/master/lang-server-backends/javascript/emscripten)
|[Dockcross armv5-uclibc](https://github.com/sumeetchhetri/ffead-cpp/blob/master/docker/DockerFile-dockcross-build)


Features
==========
- Multiple server backends (HTTP 1.1/HTTP 2/HTTP 3)
    |   |   |   |   |
    |---|---|---|---|
    |embedded (HTTP1.1)|nghttp2 (HTTP2)|quiche (HTTP3)|seastar|
    |apache|cinatra (c++)|lithium (c++)|drogon (c++)|
    |libreactor \(c\)|h2o \(c\)|vweb (vlang)|picov (vlang)|
    |actix (rust)|hyper (rust)|thruster (rust)||
    |h2o\.cr (crystal)|crystal-http (crystal)|fasthttp (golang)|gnet (golang)|
    |firenio (java)|rapidoid (java)|wizzardo-http (java)|hunt (d)|
    |swift-nio (swift)|http.jl (julia)|mongols (c++)|uv-cpp (c++)|
    |CppServer (c++)|nginx|nodejs (javascript)|openlitespeed (WIP)|
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
- Raw database access (postgresql/mongodb/scylladb)
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

Examples (as per feedback from HN)
- REST API example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/te-benchmark/include/TeBkRestController.h
- Template example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/markers/tpe/mark.tpe & https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/src/DefTemp.cpp
- Websocket example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/src/DefaultWebSocketHandler.cpp
- Wefilter example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/src/DefaultIOFilter.cpp
- Webcontroller example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/src/DefaultController.cpp
- Dynamic C++ page example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/dcp/test.dcp
- Action based view example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/fviews/test.html & https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/src/TestFormController.cpp
- XML driven configuration example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/config/application.xml
- Annotation (#pragma) driven example - https://github.com/sumeetchhetri/ffead-cpp/tree/master/web/markers/include
- ORM example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/config/sdorm-mysql.xml
- Caching example - https://github.com/sumeetchhetri/ffead-cpp/blob/master/web/default/config/cache.xml
- Script execution example - https://github.com/sumeetchhetri/ffead-cpp/tree/master/web/default/scripts
- Programming language integration examples - https://github.com/sumeetchhetri/ffead-cpp/tree/master/lang-server-backends
- OS Build scripts - https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/os-based
- Cross compiling (android, mingw54, dockcross) - https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker
- I/O Multiplexing - https://github.com/sumeetchhetri/ffead-cpp/blob/master/src/modules/server-util/SelEpolKqEvPrt.h
- ThreadPool - https://github.com/sumeetchhetri/ffead-cpp/tree/master/src/modules/threads
- Backend Server integration - https://github.com/sumeetchhetri/ffead-cpp/tree/master/src/server
- Server modules (apache/nginx/openlitespeed\*) - https://github.com/sumeetchhetri/ffead-cpp/tree/master/modules
- Cron Jobs module - https://github.com/sumeetchhetri/ffead-cpp/tree/master/src/modules/jobs
- SOLR-Zookeeper/Elasticsearch (Search module) - https://github.com/sumeetchhetri/ffead-cpp/tree/master/src/modules/search
- Toy/Useless interpreter - https://github.com/sumeetchhetri/ffead-cpp/tree/master/src/modules/interpreter
- Solaris/BSD vagrant builds - https://github.com/sumeetchhetri/ffead-cpp/tree/master/vagrant
- A Dumb code autogenerator - https://github.com/sumeetchhetri/ffead-cpp/blob/master/ffead_gen.sh
- All github workflows for build validations - https://github.com/sumeetchhetri/ffead-cpp/tree/master/.github/workflows

Quickstart (Using cmake/make)
===========
- Install cmake >= 3.8.2 and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- mkdir build && cd build
- cmake -DSRV_EMB=on -DMOD_SDORM_MONGO=on ..
- make install -j4 (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ../ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using cmake/ninja)
===========
- Install cmake >= 3.8.2, ninja and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- mkdir build && cd build
- cmake -GNinja -DSRV_EMB=on -DMOD_SDORM_MONGO=on ..
- ninja install (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ../ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using xmake)
===========
- Install xmake >= 2.5 and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- xmake f --cxflags="-I/usr/local/include -w" --MOD_SDORM_MONGO=true -v -D -c
- xmake && xmake install (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using meson)
===========
- Install meson and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- meson setup build_meson && cd build_meson
- ninja install (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ../ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using scons)
===========
- Install scons `pip3 install scons` and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- scons -Q (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ../ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using shellb)
===========
- Install shellb with `wget -q https://github.com/sumeetchhetri/shellb/releases/download/2.0.0/shellb -P . && chmod +x ./shellb ` and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites), Always check for the latest version or use the one available already in the package
- chmod +x ./shellb && ./shellb ffead-cpp-shellb (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd shellb_out/ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using bazel)
===========
- Install [bazel](https://github.com/sumeetchhetri/ffead-cpp/blob/master/docker/os-based/DockerFile-Ubuntu-x64-ffead-cpp-bazel)
- chmod +x ./shellb && ./shellb ffead-cpp-bazel (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- The above command will auto-generate BUILD/WORKSPACE/OTHER bazel files for building with bazel
- Sip some coffee
- cd shellb_out/ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using buck2)
===========
- Install [buck2](https://github.com/sumeetchhetri/ffead-cpp/blob/master/docker/os-based/DockerFile-Ubuntu-x64-ffead-cpp-buck2)
- chmod +x ./shellb && ./shellb ffead-cpp-buck2 (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- The above command will auto-generate .buckconfig/prelude/BUCK buck files for building with buck2
- Sip some coffee
- cd shellb_out/ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

Quickstart (Using autoconf)
===========
- Install autoconf, automake and libtool [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- ./autogen.sh
- ./configure --enable-srv_emb=yes --enable-mod_sdormmongo=yes
- make install -j4 (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ffead-cpp-6.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp !!)

For further details checkout the [wiki](https://github.com/sumeetchhetri/ffead-cpp/wiki) page 
