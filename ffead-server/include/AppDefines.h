
#ifdef USE_EPOLL
# undef USE_EPOLL
#endif
#ifdef USE_KQUEUE
# undef USE_KQUEUE
#endif
#ifdef USE_EVPORT
# undef USE_EVPORT
#endif
#ifdef USE_DEVPOLL
# undef USE_DEVPOLL
#endif
#include <sys/select.h>
#define USE_SELECT 1
#define OS_CYGWIN 1

