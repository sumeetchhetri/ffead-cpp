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

#define INC_MEMORYCACHE 1
/* #undef INC_REDISCACHE */
/* #undef INC_MEMCACHED */
#define INC_SDORM 1
#define INC_SDORM_SQL 1
#define INC_SDORM_MONGO 1
/* #undef INC_BINSER */
#define INC_JOBS 1
#define APPLE 1
/* #undef MINGW */
/* #undef CYGWIN */
/* #undef EMSCRIPTEN */
/* #undef DEBUG_MODE */

/* #undef USE_EPOLL */
#define USE_KQUEUE 1
/* #undef USE_EVPORT */
/* #undef USE_DEVPOLL */
/* #undef USE_POLL */
/* #undef USE_SELECT */
/* #undef USE_WIN_IOCP */
/* #undef USE_IO_URING */

#define HAVE_LIBPQ /usr/local/lib/libpq.dylib
#define HAVE_REGEX 1
#define HAVE_SSLINC 1
#define HAVE_SSLLIB /usr/local/opt/openssl/lib/libssl.dylib
/* #undef HAVE_REDISINC */
/* #undef HAVE_REDISLIB */
/* #undef HAVE_MEMCACHEDINC */
/* #undef HAVE_MEMCACHEDLIB */
#define HAVE_CURLLIB /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.0.sdk/usr/lib/libcurl.tbd
#define HAVE_EXECINFOINC 1
#define HAVE_UUIDINC 1
/* #undef HAVE_OSSPUUIDINC */
/* #undef HAVE_OSSPUUIDINC_2 */
/* #undef HAVE_BSDUUIDINC */
#define HAVE_SQLINC 1
#define HAVE_ODBCLIB /usr/local/lib/libodbc.dylib
#define HAVE_MONGOINC 1
#define HAVE_MONGOCLIB /usr/local/lib/libmongoc-1.0.dylib
#define HAVE_BSONINC 1
#define HAVE_BSONLIB /usr/local/lib/libbson-1.0.dylib
#define INC_JOBS 1
/* #undef OS_BSD */
/* #undef OS_SOLARIS */
/* #undef OS_LINUX */
/* #undef OS_MINGW */
#define OS_DARWIN 1
/* #undef OS_ANDROID */
#define SRV_EMB 1
/* #undef SRV_CINATRA */
/* #undef SRV_LITHIUM */
/* #undef SRV_DROGON */
/* #undef HAVE_ACCEPT4 */
/* #undef HAVE_TCP_QUICKACK */
/* #undef HAVE_TCP_DEFER_ACCEPT */
#define HAVE_TCP_FASTOPEN 1
/* #undef HAVE_SO_ATTACH_REUSEPORT_CBPF */
/* #undef HAVE_LIBPQ_BATCH */
/* #undef HAVE_LIBPQ_PIPELINE */
/* #undef IS_SENDFILE */
/* #undef HAVE_RAPID_JSON */
/* #undef HAVE_PUGI_XML */
/* #undef HAVE_SYSINFO */

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
