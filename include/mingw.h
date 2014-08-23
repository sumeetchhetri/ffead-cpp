
#ifndef MINGW_MISSING_H_
#define MINGW_MISSING_H_
#include "AppDefines.h"
#if defined(OS_MINGW)

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define USE_MINGW_SELECT 1
#define _WINDOWS_ 1
#define SQL_WCHART_CONVERT 1

#include <ws2tcpip.h>
#include <winsock2.h>
//#include <ws2tcpip.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
/*
 * things that are not available in header files
 */

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

typedef int pid_t;
#define hstrerror strerror

#define S_IFLNK    0120000 /* Symbolic link */
#define S_ISLNK(x) (((x) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(x) 0
#define S_IRGRP 0
#define S_IWGRP 0
#define S_IXGRP 0
#define S_ISGID 0
#define S_IROTH 0
#define S_IXOTH 0

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

#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec {
        time_t tv_sec;
        long tv_nsec;
};
#endif /* HAVE_STRUCT_TIMESPEC */

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1  
#endif

#ifndef HAVE_DECL_NANOSLEEP
#define HAVE_DECL_NANOSLEEP 0
#endif

// latest mingw64 has nanosleep. Earlier mingw and MSVC do not
#if !HAVE_DECL_NANOSLEEP
inline int nanosleep(const struct timespec *req, struct timespec *rem) {
  Sleep(req->tv_sec * 1000 + req->tv_nsec / 1000000);
  return 0;
}
#endif

/* total seconds since epoch until start of unix time (january 1, 1970 00:00 UTC) */
#define _DOVA_UNIX_SECONDS 11644473600

unsigned int sleep (unsigned int seconds);
struct tm *gmtime_r(const time_t *timep, struct tm *result);
struct tm *localtime_r(const time_t *timep, struct tm *result);
int clock_gettime (int clockid, struct timespec *tp);
//int gettimeofday(struct timeval *tv, void *tz);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);
static int inet_pton4(const char *src, unsigned char *dst);
static int inet_pton6(const char *src, unsigned char *dst);
typedef DWORD NUMEVENTS;
#else
typedef int SOCKET;
typedef int NUMEVENTS;
#define INVALID_SOCKET -1  // WinSock invalid socket
#define SOCKET_ERROR   -1  // basic WinSock error
#define closesocket(s) close(s);  // Unix uses file descriptors, WinSock doesn't...

#endif
#endif /* MINGW_MISSING_H_ */