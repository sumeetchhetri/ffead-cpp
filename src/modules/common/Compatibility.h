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

#ifndef MINGW_MISSING_H_
#define MINGW_MISSING_H_
#include "AppDefines.h"
#if defined(OS_MINGW)
#include <time.h>
#include <iomanip>
#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0600

#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#endif

#if defined(__CYGWIN__) || defined(CYGWIN)
#define _GNU_SOURCE
#undef __STRICT_ANSI__
#include <sys/select.h>
#include <strings.h>
#endif

#include "iostream"
#include <errno.h>
#include <signal.h>
#include "string"
#include "cstring"
#include <stdio.h>
#include <stdlib.h>
#include "vector"
#include "sstream"
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <iostream>
#if defined(OS_DARWIN) || defined(OS_BSD)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#elif defined(IS_SENDFILE)
#if !defined(OS_MINGW) && !defined(CYGWIN)
#include <sys/sendfile.h>
#endif
#endif
#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
#include <sched.h>

#if defined(OS_MINGW)
//#undef _WIN32_WINNT
//#define _WIN32_WINNT 0x0600
#define HAVE_STRUCT_TIMESPEC 1
#include <unistd.h>
#include <pthread.h>
//#define USE_MINGW_SELECT 1
#ifndef _WINDOWS_
#define _WINDOWS_ 1
#endif
#define SQL_WCHART_CONVERT 1
#include "windef.h"



#ifndef OVERLAPPED_ENTRY /* This is a very ugly hack, but mingw doesn't have
                            these defines. */

WINBASEAPI BOOL WINAPI
GetQueuedCompletionStatusEx(HANDLE CompletionPort,
                            LPOVERLAPPED_ENTRY lpCompletionPortEntries,
                            ULONG ulCount,
                            PULONG ulNumEntriesRemoved,
                            DWORD dwMilliseconds,
                            BOOL fAlertable);
#endif

#ifndef SOCKLEN_T_DEFINED
#define SOCKLEN_T_DEFINED 1
typedef int socklen_t;
#endif

#ifndef NS_INADDRSZ
#define NS_INADDRSZ	4
#endif
#ifndef NS_IN6ADDRSZ
#define NS_IN6ADDRSZ	16
#endif
#ifndef NS_INT16SZ
#define NS_INT16SZ	2
#endif

#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN       46
#endif /* INET6_ADDRSTRLEN */

#define hstrerror strerror

#define S_IFLNK    0120000 /* Symbolic link */
#define S_ISLNK(x) (((x) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(x) 0
//#define S_IRGRP 0
//#define S_IWGRP 0
//#define S_IXGRP 0
#define S_ISGID 0
//#define S_IROTH 0
//#define S_IXOTH 0

#define WIFEXITED(x) 1
#define WIFSIGNALED(x) 0
#define WEXITSTATUS(x) ((x) & 0xff)
#define WTERMSIG(x) SIGTERM

#define SIGHUP 1
#define SIGQUIT 3
#define SIGKILL 9
#define SIGPIPE 13
#define SIGALRM 14
#define SIGCHLD 17

#define F_GETFD 1
#define F_SETFD 2
#define FD_CLOEXEC 0x1

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1  
#endif

#ifndef HAVE_DECL_NANOSLEEP
#define HAVE_DECL_NANOSLEEP 0
#endif


/* total seconds since epoch until start of unix time (january 1, 1970 00:00 UTC) */
#define _DOVA_UNIX_SECONDS 11644473600

//typedef int pid_t;

#ifndef OS_MINGW_W64
struct tm *gmtime_r(const time_t *timep, struct tm *result);
struct tm *localtime_r(const time_t *timep, struct tm *result);
#endif

int clock_gettime (int clockid, struct timespec *tp);
//const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);
int inet_pton4(const char *src, unsigned char *dst);
int inet_pton6(const char *src, unsigned char *dst);
typedef DWORD NUMEVENTS;
extern "C" char* strptime(const char* s,
                          const char* f,
                          struct tm* tm);


#else

#if defined(OS_DARWIN)
#include <mach/clock.h>
#include <mach/mach.h>
#include <sys/sysctl.h>

#define SYSCTL_CORE_COUNT   "machdep.cpu.core_count"

typedef struct cpu_set {
  uint32_t    count;
} cpu_set_t;

static inline void
CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }
int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set);
int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
                           cpu_set_t *cpu_set);

static inline void
CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

static inline int
CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }


#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

static inline int clock_gettime (int clockid, struct timespec *ts)
{
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
	return 0;
}
#endif

#ifdef CYGWIN
extern char* strptime (const char *buf, const char *fmt, struct tm *tm);
#endif

typedef int SOCKET;
typedef int NUMEVENTS;
#define INVALID_SOCKET -1  // WinSock invalid socket
#define SOCKET_ERROR   -1  // basic WinSock error
#define closesocket(s) close(s);  // Unix uses file descriptors, WinSock doesn't...

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <pthread.h>
#endif
#endif /* MINGW_MISSING_H_ */
