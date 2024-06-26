#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([ffead-cpp], [7.0], [sumeet.chhetri@gmail.com])
AM_INIT_AUTOMAKE([no-define foreign])
AC_CONFIG_SRCDIR([../AjaxInterface.cpp])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT([static shared dlopen])

mod_sdormsql=${MOD_SDORM_SQL}
mod_sdormmongo=${MOD_SDORM_MONGO}
mod_script=${MOD_SCRIPT}

support_debug=no
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debug, default: no]),
    [support_debug=$enableval]
)
if test "x$support_debug" = "xyes"; then
    AC_DEFINE(DEBUG, [], [set DEBUG mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Wno-uninitialized -O0"
else
    AC_DEFINE(NDEBUG, [], [unset DEBUG mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -O0"
    support_debug=no
fi

AC_CANONICAL_HOST
local_os_type="unknown"
is_librt_available="yes"
case "$host" in
*-cygwin*)
    AC_DEFINE(OS_CYGWIN, [], [set OS to cygwin])
    AC_DEFINE(CYGWIN, [1], [set OS to cygwin])
    local_os_type="cygwin"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include"
    ;;
*mingw*)
    AC_DEFINE(OS_MINGW, [], [set OS to mingw])
    local_os_type="mingw"
    is_librt_available="no"
    CPPFLAGS="$CPPFLAGS -I/mingw/include -L/mingw/lib -D__USE_MINGW_ANSI_STDIO=1"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include -I/mingw/include -L/mingw/lib"
    LDFLAGS="$LDFLAGS -L/mingw/lib"
    ;;
*-linux*)
    AC_DEFINE(OS_LINUX, [], [set OS to linux])
	#hack for some 64-bit systems
	AC_DEFINE(_GNU_SOURCE, [], [set GNU source flag for linux])
    local_os_type="linux"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include"
    ;;
*-irix*)
    AC_DEFINE(OS_IRIX, [], [set OS to irix])
    local_os_type="irix"
    ;;
*-sunos*)
    AC_DEFINE(OS_SUNOS, [], [set OS to sunos])
    local_os_type="sunos"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include -pthreads"
    LDFLAGS="$LDFLAGS -lsocket -lnsl -lresolv"
    ;;
*-solaris*)
    AC_DEFINE(OS_SOLARIS, [], [set OS to solaris])
    local_os_type="solaris"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include -pthreads"
    LDFLAGS="$LDFLAGS -lsocket -lnsl"
    ;;
*-aix*)
    AC_DEFINE(OS_AIX, [], [set OS to aix])
    local_os_type="aix"
    ;;
*-hpux*)
    AC_DEFINE(OS_HPUX, [], [set OS to hp-ux])
    local_os_type="hpux"
    ;;
*-freebsd*)
    AC_DEFINE(OS_BSD, [], [set OS to bsd])
    AC_DEFINE(OS_FREEBSD, [], [set OS to freebsd])
    local_os_type="freebsd"

    dnl our hack for freebsd system
    AM_CXXFLAGS="$AM_CXXFLAGS -pthread -I/usr/local/include"
    LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/local/v6/lib"
    ;;
*-netbsd*)
    AC_DEFINE(OS_BSD, [], [set OS to bsd])
    AC_DEFINE(OS_NETBSD, [], [set OS to netbsd])
    local_os_type="netbsd"

    #our hack for netbsd system
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include"
    LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/local/v6/lib"
    ;;
*-openbsd*)
    AC_DEFINE(OS_BSD, [], [set OS to bsd])
    AC_DEFINE(OS_OPENBSD, [], [set OS to openbsd])
    local_os_type="openbsd"

	#get openbsd release number, remove `.' from release number
	OBSD_REL=`/usr/bin/uname -r | sed 's/\.//g'`
	AC_DEFINE_UNQUOTED(OS_OPENBSD_REL, ${OBSD_REL}, [set openbsd release number])

    #our hack for openbsd system
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include"
    LIBS="$LIBS -L/usr/local/lib"
    ;;
*-osf*)
    AC_DEFINE(OS_OSF, [], [set OS to osf])
    local_os_type="osf"
    ;;
*-darwin*)
    AC_DEFINE(OS_BSD, [], [set OS to bsd])
    AC_DEFINE(OS_DARWIN, [], [set OS to darwin])
    local_os_type="darwin"
    is_librt_available="no"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include -I/opt/homebrew/include"
    LDFLAGS="$LDFLAGS -L/usr/local/lib -L/opt/homebrew/lib"
    ;;
*-sco*)
    AC_DEFINE(OS_SCO, [], [set OS to sco])
    AC_DEFINE(OS_SVR4, [], [set OS to svr4])
    local_os_type="sco"
    AC_MSG_ERROR([We do not support SCO's products!])
esac

if test "x$mod_script" = "xtrue"; then
	AM_CXXFLAGS="$AM_CXXFLAGS -U__STRICT_ANSI__"
fi

if test "x$mod_sdormmongo" = "xtrue"; then
	AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/include/libmongoc-1.0/ -I/usr/include/libbson-1.0 -I/opt/homebrew/include/libmongoc-1.0 -I/opt/homebrew/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0/ -I/usr/local/include/libbson-1.0"
fi

# store current user given compiler flags to avoid default setup via AC_PROG_CXX
OLD_CXXFLAGS=$CXXFLAGS

dnl# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# reset compiler flags to initial flags
CXXFLAGS=$OLD_CXXFLAGS

AC_PROG_MAKE_SET

dnl# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl# Checks for libraries.
AC_DEFINE(BUILT_WITH_CONFGURE, [1], [set configure mode])
AM_CONDITIONAL(BUILT_WITH_CONFGURE, test x"1" = x"1")

dnl# unixODBC wants the following to get sane behavior for ODBCINT64
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(void *)
AC_CHECK_TYPES(long long)
AC_CHECK_TYPES(signed char)

dnl# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([clock_gettime floor gethostbyname memset pow regcomp select socket strcasecmp strtol strtoul])
AC_CHECK_LIB(pthreads, pthread_create,[], [AC_CHECK_LIB(pthread, pthread_create)])
AC_CHECK_LIB(ssl, SSL_read, [], [AC_MSG_ERROR([ssl library not found])])
AC_CHECK_LIB([crypto], [SHA1_Init], [], [AC_MSG_ERROR([libcrypto not found])])
AC_CHECK_LIB(z, compress,,AC_MSG_ERROR([no zlib; please install zlib or equivalent]))
AC_CHECK_LIB(c, dlopen, LIBDL="", [AC_CHECK_LIB(dl, dlopen, LIBDL="-ldl")])

if test "x$mod_dsorm" = "xtrue"; then
	AC_CHECK_LIB(uuid, uuid_generate, [], [AC_MSG_ERROR([uuid library not found])])
	AC_CHECK_LIB(odbc, main, , [AC_MSG_ERROR([Not found libodbc library])])
	AC_SEARCH_LIBS(SQLGetPrivateProfileString, odbcinst, [],[AC_MSG_ERROR([unixODBC library "odbcinst" not found])])
fi

if test "x$mod_sdormmongo" = "xtrue"; then
	AC_CHECK_LIB(uuid, uuid_generate, [], [AC_MSG_ERROR([uuid library not found])])
	AC_CHECK_LIB(mongoc-1.0, mongoc_init, , [AC_MSG_ERROR([Not found libmongo library])])
	AC_CHECK_LIB(bson-1.0, bson_new, , [AC_MSG_ERROR([Not found libbson library])])
fi

if test "x$is_librt_available" = "xyes"; then
	AC_CHECK_LIB(rt, clock_gettime,,[AC_MSG_ERROR([librt required but not found])])
fi

if test "x$local_os_type" = "xmingw"; then
	AC_CHECK_LIB(wsock32, main, , [AC_MSG_ERROR([Not found libwsock32 library])])
	AC_CHECK_LIB(ws2_32, main, , [AC_MSG_ERROR([Not found libws2_32 library])])
	AC_CHECK_LIB(regex, regcomp, , [AC_MSG_ERROR([Not found libregex library])])
fi
    
# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
AC_SUBST([AM_CXXFLAGS])    
    
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
