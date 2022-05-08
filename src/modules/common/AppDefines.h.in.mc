/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#define BUILD_CMAKE 1

#cmakedefine INC_MEMORYCACHE @INC_MEMORYCACHE@
#cmakedefine INC_REDISCACHE @INC_REDISCACHE@
#cmakedefine INC_MEMCACHED @INC_MEMCACHED@
#cmakedefine INC_SDORM @INC_SDORM@
#cmakedefine INC_SDORM_SQL @INC_SDORM_SQL@
#cmakedefine INC_SDORM_MONGO @INC_SDORM_MONGO@
#cmakedefine INC_BINSER @INC_BINSER@
#cmakedefine INC_JOBS @INC_JOBS@
#cmakedefine APPLE @APPLE@
#cmakedefine MINGW @MINGW@
#cmakedefine CYGWIN @CYGWIN@
#cmakedefine EMSCRIPTEN @EMSCRIPTEN@
#cmakedefine DEBUG_MODE @CMAKE_BUILD_TYPE@

#cmakedefine USE_EPOLL @USE_EPOLL@
#cmakedefine USE_KQUEUE @USE_KQUEUE@
#cmakedefine USE_EVPORT @USE_EVPORT@
#cmakedefine USE_DEVPOLL @USE_DEVPOLL@
#cmakedefine USE_POLL @USE_POLL@
#cmakedefine USE_SELECT @USE_SELECT@
#cmakedefine USE_WIN_IOCP @USE_WIN_IOCP@
#cmakedefine USE_IO_URING @USE_IO_URING@
#cmakedefine USE_PICOEV @USE_PICOEV@

#cmakedefine HAVE_LIBPQ @HAVE_LIBPQ@
#cmakedefine HAVE_REGEX @HAVE_REGEX@
#cmakedefine HAVE_SSLINC @HAVE_SSLINC@
#cmakedefine HAVE_SSLLIB @HAVE_SSLLIB@
#cmakedefine HAVE_REDISINC @HAVE_REDISINC@
#cmakedefine HAVE_REDISLIB @HAVE_REDISLIB@
#cmakedefine HAVE_MEMCACHEDINC @HAVE_MEMCACHEDINC@
#cmakedefine HAVE_MEMCACHEDLIB @HAVE_MEMCACHEDLIB@
#cmakedefine HAVE_CURLLIB @HAVE_CURLLIB@
#cmakedefine HAVE_EXECINFOINC @HAVE_EXECINFOINC@
#cmakedefine HAVE_UUIDINC @HAVE_UUIDINC@
#cmakedefine HAVE_OSSPUUIDINC @HAVE_OSSPUUIDINC@
#cmakedefine HAVE_OSSPUUIDINC_2 @HAVE_OSSPUUIDINC_2@
#cmakedefine HAVE_BSDUUIDINC @HAVE_BSDUUIDINC@
#cmakedefine HAVE_SQLINC @HAVE_SQLINC@
#cmakedefine HAVE_ODBCLIB @HAVE_ODBCLIB@
#cmakedefine HAVE_MONGOINC @HAVE_MONGOINC@
#cmakedefine HAVE_MONGOCLIB @HAVE_MONGOCLIB@
#cmakedefine HAVE_BSONINC @HAVE_BSONINC@
#cmakedefine HAVE_BSONLIB @HAVE_BSONLIB@
#cmakedefine INC_JOBS @INC_JOBS@
#cmakedefine OS_BSD @OS_BSD@
#cmakedefine OS_SOLARIS @OS_SOLARIS@
#cmakedefine OS_LINUX @OS_LINUX@
#cmakedefine OS_MINGW @OS_MINGW@
#cmakedefine OS_DARWIN @OS_DARWIN@
#cmakedefine OS_ANDROID @OS_ANDROID@
#cmakedefine SRV_EMB @SRV_EMB@
#cmakedefine SRV_CINATRA @SRV_CINATRA@
#cmakedefine SRV_LITHIUM @SRV_LITHIUM@
#cmakedefine SRV_DROGON @SRV_DROGON@
#cmakedefine HAVE_ACCEPT4 @HAVE_ACCEPT4@
#cmakedefine HAVE_TCP_QUICKACK @HAVE_TCP_QUICKACK@
#cmakedefine HAVE_TCP_DEFER_ACCEPT @HAVE_TCP_DEFER_ACCEPT@
#cmakedefine HAVE_TCP_FASTOPEN @HAVE_TCP_FASTOPEN@
#cmakedefine HAVE_SO_ATTACH_REUSEPORT_CBPF @HAVE_SO_ATTACH_REUSEPORT_CBPF@
#cmakedefine HAVE_LIBPQ_BATCH @HAVE_LIBPQ_BATCH@
#cmakedefine HAVE_LIBPQ_PIPELINE @HAVE_LIBPQ_PIPELINE@
#cmakedefine IS_SENDFILE @IS_SENDFILE@
#cmakedefine HAVE_RAPID_JSON @HAVE_RAPID_JSON@
#cmakedefine HAVE_PUGI_XML @HAVE_PUGI_XML@
#cmakedefine HAVE_SYSINFO @HAVE_SYSINFO@
#cmakedefine HAVE_SOLR @HAVE_SOLR@
#cmakedefine HAVE_ELASTIC @HAVE_ELASTIC@
#cmakedefine HAVE_REDIS_CLUSTERINC @HAVE_REDIS_CLUSTERINC@
#cmakedefine HAVE_REDIS_CLUSTERLIB @HAVE_REDIS_CLUSTERLIB@

#ifdef HAVE_ODBCLIB
#define HAVE_LIBODBC 1
#endif

#define INC_WEBSVC 1
#define INC_TPE 1
#define INC_DVIEW 1
#define INC_DCP 1
#define INC_XMLSER 1
#define BUILD_CMAKE 1

#ifdef APPLE
#define OS_DARWIN 1
#endif
