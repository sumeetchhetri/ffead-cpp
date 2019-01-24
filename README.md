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

Features
==========
- Advanced ORM - SDORM (sql/monogo)
- Embedded Server, Apache/Nginx support (modules)
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

Quickstart
===========
- ./autogen.sh (Setup configure for your platform)
- ./configure (Setup makefiles)
- Stretch a bit :-)
- make all (Build ffead-cpp)
- Sip some coffee
- make build-apps (Build the sample applications in web + generate ffead-cpp binary)
- cd ffead-cpp-2.0-bin/ (Navigate to the ffead-cpp binary folder)
- ./server.sh (Startup ffead-cpp Njoy!!)

For further details checkout the [wiki](https://github.com/sumeetchhetri/ffead-cpp/wiki) page
