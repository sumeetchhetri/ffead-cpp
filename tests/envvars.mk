
IS_OS_CYGWIN := $(shell uname|tr '[A-Z]' '[a-z]'|awk 'index($$0,"cygwin") != 0 {print "cygwin"}')
IS_OS_BSD := $(shell uname|tr '[A-Z]' '[a-z]'|awk 'index($$0,"bsd") != 0 {print "bsd"}')
IS_OS_SOLARIS := $(shell uname|tr '[A-Z]' '[a-z]'|awk 'index($$0,"sunos") != 0 {print "sunos"}')
IS_OS_LINUX := $(shell uname|tr '[A-Z]' '[a-z]'|awk 'index($$0,"linux") != 0 {print "linux"}')


ifeq ($(IS_OS_LINUX), linux)
MAKE_EXE := make
PTHREADS :=
EX_LIBS := 
RES_LIB_EXT := so
endif

ifeq ($(IS_OS_BSD), bsd)
MAKE_EXE := gmake
PTHREADS :=
EX_LIBS := 
RES_LIB_EXT := so
endif

ifeq ($(IS_OS_SOLARIS), sunos)
MAKE_EXE := gmake
PTHREADS := -pthreads
EX_LIBS := -lsocket -lnsl
RES_LIB_EXT := so
endif

ifeq ($(IS_OS_CYGWIN), cygwin)
MAKE_EXE := make
PTHREADS :=
EX_LIBS := 
RES_LIB_EXT := a
endif
