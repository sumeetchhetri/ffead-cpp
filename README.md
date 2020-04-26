# The ffead-cpp Framework

ffead-cpp is a web-framework, application framework, utilities all bundled into one. 
It also provides an embedded HTTP/HTT2/Web-Socket compliant high-performance server core. 
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

Starting version 3.0 ffead-cpp supports both **autoconf** and **cmake** for builds

Detailed OS specific instrauctions and docker/docker-compose scripts are located at [docker](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker)

Official ffead-cpp pre-built images can be found at [dockerhub](https://cloud.docker.com/repository/docker/sumeetchhetri/ffead-cpp-3.0/tags)


Features
==========
- Webrtc Signalling (websocket + api) (horizontally scalable peerjs compatible signalling server)
- Advanced ORM - SDORM (sql/monogo)
- Embedded Server, Apache/Nginx support (stable modules), Openlitespeed (experimental)
- Web Socket Support
- HTTP2.0 Support
- Cache API (memcached/redis)
- Improved Thread/ThreadPool API(s)
- Marker based configuration (Java style annotations)
- Improved autotools support
- Reflection support
- Serialization support
- Date/Time Ultility functions
- Better Logging support
- Dependency Injection

Quickstart (Using cmake)
===========
- Install cmake >= 3.8.2 and [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- cmake -DMOD_SDORM_MONGO=on .
- make install -j4 (Build ffead-cpp along-with the sample applications in web + generate ffead-cpp binary)
- Sip some coffee
- cd ffead-cpp-3.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

Quickstart (Using autoconf)
===========
- Install [prerequisites](https://github.com/sumeetchhetri/ffead-cpp/wiki/Prerequisites)
- ./autogen.sh (Setup configure for your platform)
- ./configure --enable-mod_sdormmongo (Setup makefiles)
- Stretch a bit :-)
- make install (Build ffead-cpp)
- Sip some coffee
- make build-apps (Build the sample applications in web + generate ffead-cpp binary)
- cd ffead-cpp-3.0-bin/ (Navigate to the ffead-cpp binary folder)
- chmod +x *.sh
- ./server.sh (Startup ffead-cpp Njoy!!)

Webrtc Example (Uses peerjs)
===========
- Follow Quickstart (Using cmake) above
- Build docker image from [docker-webrtc](https://github.com/sumeetchhetri/ffead-cpp/tree/master/docker/webrtc-peerjs)
- Once the server starts, navigate to http://localhost:8080/peer-server/index.html in 2 tabs
- Enjoy p2p calling using the ffead-cpp webrtc (peerjs compatible) signalling server

For further details checkout the [wiki](https://github.com/sumeetchhetri/ffead-cpp/wiki) page
