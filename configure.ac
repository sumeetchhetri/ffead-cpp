#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([ffead-cpp], [1.9], [sumeet.chhetri@gmail.com])
AM_INIT_AUTOMAKE([1.9 no-define foreign])
AC_CONFIG_SRCDIR([include/ConfigurationData.h])
AC_CONFIG_HEADERS([include/AppDefines.h])

LT_INIT([dlopen])

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

generate_apache_module=no
AC_ARG_ENABLE(apachemod,
AS_HELP_STRING([--enable-apachemod],
               [enable apachemod, default: no]),
    [generate_apache_module=$enableval]
)

AC_CANONICAL_HOST
local_os_type="unknown"
case "$host" in
*-cygwin*)
    AC_DEFINE(OS_CYGWIN, [], [set OS to cygwin])
    local_os_type="cygwin"
    ;;
*-linux*)
    AC_DEFINE(OS_LINUX, [], [set OS to linux])
	#hack for some 64-bit systems
	AC_DEFINE(_GNU_SOURCE, [], [set GNU source flag for linux])
    local_os_type="linux"
    ;;
*-irix*)
    AC_DEFINE(OS_IRIX, [], [set OS to irix])
    local_os_type="irix"
    ;;
*-sunos*)
    AC_DEFINE(OS_SUNOS, [], [set OS to sunos])
    local_os_type="sunos"
    AM_CXXFLAGS="$AM_CXXFLAGS -pthreads"
    LDFLAGS="$LDFLAGS -lsocket -lnsl -lresolv"
    ;;
*-solaris*)
    AC_DEFINE(OS_SOLARIS, [], [set OS to solaris])
    local_os_type="solaris"
    AM_CXXFLAGS="$AM_CXXFLAGS -pthreads"
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
    ;;
*-sco*)
    AC_DEFINE(OS_SCO, [], [set OS to sco])
    AC_DEFINE(OS_SVR4, [], [set OS to svr4])
    local_os_type="sco"
    AC_MSG_ERROR([We do not support SCO's products!])
esac

AC_ARG_ENABLE(mod_comp,
AS_HELP_STRING([--mod-component],
               [enable mod_comp, default: no]),
[case "${enableval}" in
             yes) mod_comp=true ;;
             no)  mod_comp=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-component]) ;;
esac],
[mod_comp=false])
AM_CONDITIONAL(MOD_COMPONENT, test x"$mod_comp" = x"true")
if test "x$mod_comp" = "xtrue"; then
    AC_DEFINE(INC_CMP, [], [set DEBUG mode])
    AC_DEFINE(INC_XMLSER, [], [set DEBUG mode])
    AC_DEFINE(INC_CIB, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_methinvoker,
AS_HELP_STRING([--mod-methinvoker],
               [enable mod_methinvoker, default: no]),
[case "${enableval}" in
             yes) mod_methinvoker=true ;;
             no)  mod_methinvoker=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-methinvoker]) ;;
esac],
[mod_methinvoker=false])
AM_CONDITIONAL(MOD_METHINVOKER, test x"$mod_methinvoker" = x"true")
if test "x$mod_methinvoker" = "xtrue"; then
    AC_DEFINE(INC_MI, [], [set DEBUG mode])
    AC_DEFINE(INC_XMLSER, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_msghandler,
AS_HELP_STRING([--mod-msghandler],
               [enable mod_msghandler, default: no]),
[case "${enableval}" in
             yes) mod_msghandler=true ;;
             no)  mod_msghandler=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-msghandler]) ;;
esac],
[mod_msghandler=false])
AM_CONDITIONAL(MOD_MSGHANDLER, test x"$mod_msghandler" = x"true")
if test "x$mod_msghandler" = "xtrue"; then
    AC_DEFINE(INC_MSGH, [], [set DEBUG mode])
    AC_DEFINE(INC_BINSER, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_webservice,
AS_HELP_STRING([--mod-webservice],
               [enable mod_webservice, default: no]),
[case "${enableval}" in
             yes) mod_webservice=true ;;
             no)  mod_webservice=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-webservice]) ;;
esac],
[mod_webservice=false])
AM_CONDITIONAL(MOD_WEBSERVICE, test x"$mod_webservice" = x"true")
if test "x$mod_webservice" = "xtrue"; then
    AC_DEFINE(INC_WEBSVC, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_cibernate,
AS_HELP_STRING([--mod-cibernate],
               [enable mod_cibernate, default: no]),
[case "${enableval}" in
             yes) mod_cibernate=true ;;
             no)  mod_cibernate=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-cibernate]) ;;
esac],
[mod_cibernate=false])
AM_CONDITIONAL(MOD_CIBERNATE, test x"$mod_cibernate" = x"true")
if test "x$mod_cibernate" = "xtrue"; then
    AC_DEFINE(INC_CIB, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_distocache,
AS_HELP_STRING([--mod-distocache],
               [enable mod_distocache, default: no]),
[case "${enableval}" in
             yes) mod_distocache=true ;;
             no)  mod_distocache=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-distocache]) ;;
esac],
[mod_distocache=false])
AM_CONDITIONAL(MOD_DISTOCACHE, test x"$mod_distocache" = x"true")
if test "x$mod_distocache" = "xtrue"; then
    AC_DEFINE(INC_DSTC, [], [set DEBUG mode])
    AC_DEFINE(INC_BINSER, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_xmlserialize,
AS_HELP_STRING([--mod-xmlserialize],
               [enable mod_xmlserialize, default: no]),
[case "${enableval}" in
             yes) mod_xmlserialize=true ;;
             no)  mod_xmlserialize=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-xmlserialize]) ;;
esac],
[mod_xmlserialize=false])
AM_CONDITIONAL(MOD_XMLSERIALIZE, test x"$mod_xmlserialize" = x"true")
if test "x$mod_xmlserialize" = "xtrue"; then
    AC_DEFINE(INC_XMLSER, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_binserialize,
AS_HELP_STRING([--mod-binserialize],
               [enable mod_binserialize, default: no]),
[case "${enableval}" in
             yes) mod_binserialize=true ;;
             no)  mod_binserialize=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-binserialize]) ;;
esac],
[mod_binserialize=false])
AM_CONDITIONAL(MOD_BINSERIALIZE, test x"$mod_binserialize" = x"true")
if test "x$mod_binserialize" = "xtrue"; then
    AC_DEFINE(INC_BINSER, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_dcp,
AS_HELP_STRING([--mod-dcp],
               [enable mod_dcp, default: no]),
[case "${enableval}" in
             yes) mod_dcp=true ;;
             no)  mod_dcp=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-dcp]) ;;
esac],
[mod_dcp=false])
AM_CONDITIONAL(MOD_DCP, test x"$mod_dcp" = x"true")
if test "x$mod_dcp" = "xtrue"; then
    AC_DEFINE(INC_DCP, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_dview,
AS_HELP_STRING([--mod-dview],
               [enable mod_dview, default: no]),
[case "${enableval}" in
             yes) mod_dview=true ;;
             no)  mod_dview=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-dview]) ;;
esac],
[mod_dview=false])
AM_CONDITIONAL(MOD_DVIEW, test x"$mod_dview" = x"true")
if test "x$mod_dview" = "xtrue"; then
    AC_DEFINE(INC_DVIEW, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_tpe,
AS_HELP_STRING([--mod-tpe],
               [enable mod_tpe, default: no]),
[case "${enableval}" in
             yes) mod_tpe=true ;;
             no)  mod_tpe=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-tpe]) ;;
esac],
[mod_tpe=false])
AM_CONDITIONAL(MOD_TPE, test x"$mod_tpe" = x"true")
if test "x$mod_tpe" = "xtrue"; then
    AC_DEFINE(INC_TPE, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_interpreter,
AS_HELP_STRING([--mod-interpreter],
               [enable mod_interpreter, default: no]),
[case "${enableval}" in
             yes) mod_interpreter=true ;;
             no)  mod_interpreter=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-interpreter]) ;;
esac],
[mod_interpreter=false])
AM_CONDITIONAL(MOD_INTERPRETER, test x"$mod_interpreter" = x"true")
if test "x$mod_interpreter" = "xtrue"; then
    AC_DEFINE(INC_INTP, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_appflow,
AS_HELP_STRING([--mod-appflow],
               [enable mod_appflow, default: no]),
[case "${enableval}" in
             yes) mod_appflow=true ;;
             no)  mod_appflow=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-appflow]) ;;
esac],
[mod_appflow=false])
AM_CONDITIONAL(MOD_APPFLOW, test x"$mod_appflow" = x"true")
if test "x$mod_appflow" = "xtrue"; then
    AC_DEFINE(INC_APPFLOW, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_jobs,
AS_HELP_STRING([--mod-jobs],
               [enable mod_jobs, default: no]),
[case "${enableval}" in
             yes) mod_jobs=true ;;
             no)  mod_jobs=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-jobs]) ;;
esac],
[mod_jobs=false])
AM_CONDITIONAL(MOD_JOBS, test x"$mod_jobs" = x"true")
if test "x$mod_jobs" = "xtrue"; then
    AC_DEFINE(INC_JOBS, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_script,
AS_HELP_STRING([--mod-script],
               [enable mod_script, default: no]),
[case "${enableval}" in
             yes) mod_script=true ;;
             no)  mod_script=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-script]) ;;
esac],
[mod_script=false])
AM_CONDITIONAL(MOD_SCRH, test x"$mod_script" = x"true")
if test "x$mod_script" = "xtrue"; then
    AC_DEFINE(INC_SCRH, [], [set DEBUG mode])
fi

AC_ARG_ENABLE(mod_all,
AS_HELP_STRING([--mod-all],
               [enable mod_all, default: yes]),
[case "${enableval}" in
             yes) mod_all=true ;;
             no)  mod_all=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --mod-all]) ;;
esac],
[mod_all=true])
AM_CONDITIONAL(MOD_ALL, test x"$mod_all" = x"true")
if test "x$mod_all" = "xtrue"; then
    AC_DEFINE(INC_COMP, [], [set DEBUG mode])
    AC_DEFINE(INC_DCP, [], [set DEBUG mode])
    AC_DEFINE(INC_DVIEW, [], [set DEBUG mode])
    AC_DEFINE(INC_TPE, [], [set DEBUG mode])
    AC_DEFINE(INC_SCRH, [], [set DEBUG mode])
    AC_DEFINE(INC_INTP, [], [set DEBUG mode])
    AC_DEFINE(INC_APPFLOW, [], [set DEBUG mode])
    AC_DEFINE(INC_JOBS, [], [set DEBUG mode])
    AC_DEFINE(INC_CIB, [], [set DEBUG mode])
    AC_DEFINE(INC_MSGH, [], [set DEBUG mode])
    AC_DEFINE(INC_MI, [], [set DEBUG mode])
    AC_DEFINE(INC_WEBSVC, [], [set DEBUG mode])
    AC_DEFINE(INC_DSTC, [], [set DEBUG mode])
    AC_DEFINE(INC_XMLSER, [], [set DEBUG mode])
    AC_DEFINE(INC_BINSER, [], [set DEBUG mode])
fi

# store current user given compiler flags to avoid default setup via AC_PROG_CXX
OLD_CXXFLAGS=$CXXFLAGS

dnl# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

if test "x$generate_apache_module" = "xyes"; then
	AC_SUBST(GENERATE_APACHEMOD,[apache_mod_ffeadcpp/autotools])
	if test "x$local_os_type" = "xcygwin"; then
		AC_CHECK_LIB(httpd2core, AP_INIT_TAKE1,[AC_SUBST(APACHE_HTTPD_LIB,[-lhttpd2core])],[])
		if test -f /usr/sbin/apxs2; then
			echo 'apxs2,apachectl2 found..'
			AC_CHECK_PROG(APXS, apxs2, [/usr/sbin/apxs2], [apxs2 not found],[])
		elif test -f /usr/local/bin/apxs2; then
			echo 'apxs2,apachectl2 found..'
			AC_CHECK_PROG(APXS, apxs2, [/usr/local/bin/apxs2], [apxs2 not found],[])
		else
			AC_CHECK_PROG(APXS, apxs2, [apxs2], [apxs2 not found],[])
		fi
	else
		AC_SUBST(APACHE_HTTPD_LIB,[ ])
		AC_CHECK_PROG(APXS, apxs2, [apxs2], [apxs2 not found],[])
	fi
else
	AC_SUBST(GENERATE_APACHEMOD,[ ])
	AC_SUBST(APACHE_HTTPD_LIB,[ ])
fi


# reset compiler flags to initial flags
CXXFLAGS=$OLD_CXXFLAGS

AC_PROG_MAKE_SET

dnl# Checks for libraries.
AC_DEFINE(BUILT_WITH_CONFGURE, [1], [set configure mode])
AM_CONDITIONAL(BUILT_WITH_CONFGURE, test x"1" = x"1")

dnl# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])
AC_CHECK_HEADER([sys/epoll.h],
                     [AC_DEFINE(USE_EPOLL, [1], [Define to 1 if you have <sys/epoll.h>.])],
                     [AC_DEFINE(USE_EPOLL, [0], [epoll support not found])])
AC_CHECK_HEADER([sys/event.h],
                     [AC_DEFINE(USE_KQUEUE, [1], [Define to 1 if you have <sys/event.h>.])],
                     [AC_DEFINE(USE_KQUEUE, [0], [kqueue support not found])])
AC_CHECK_HEADER([port.h],
                     [AC_DEFINE(USE_EVPORT, [1], [Define to 1 if you have <port.h>.])],
                     [AC_DEFINE(USE_EVPORT, [0], [event port support not found])])
AC_CHECK_HEADER([sys/devpoll.h],
                     [AC_DEFINE(USE_DEVPOLL, [1], [Define to 1 if you have <sys/devpoll.h>.])],
                     [AC_DEFINE(USE_DEVPOLL, [0], [devpoll support not found])])
AC_CHECK_HEADER([sys/poll.h],
                     [AC_DEFINE(USE_POLL, [1], [Define to 1 if you have <sys/poll.h>.])],
                     [AC_DEFINE(USE_POLL, [0], [poll support not found])])
AC_CHECK_HEADER([sys/select.h],
                     [AC_DEFINE(USE_SELECT, [1], [Define to 1 if you have <sys/select.h>.])],
                     [AC_DEFINE(USE_SELECT, [0], [select support not found])])
                     
if test -d /usr/local/include; then
     if test -f /usr/local/include/openssl/ssl.h; then
     	echo 'ssl headers found..'
     else
     	AC_CHECK_HEADER([openssl/ssl.h],,[AC_MSG_ERROR([ssl header not found])])	
     fi
     if test -f /usr/local/include/sql.h; then
     	echo 'unixODBC sql.h header found..'
     else
     	AC_CHECK_HEADER([sql.h],,[AC_MSG_ERROR([unixODBC sql.h header not found])])	
     fi
fi

#AC_CHECK_HEADER([sql.h],,[AC_MSG_ERROR([unixODBC sql.h header not found])])
#AC_CHECK_HEADER([sqlext.h],,[AC_MSG_ERROR([unixODBC sqlext.h header not found])])
#AC_CHECK_HEADER([sqltypes.h],,[AC_MSG_ERROR([unixODBC sqltypes.h header not found])])

#if test "x$local_os_type" = "xcygwin"; then
#	[AC_DEFINE(USE_POLL, [0], [Define to 0 if you have <sys/poll.h> on cygwin])],
#fi

dnl# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

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
AC_CHECK_LIB(odbc, main, , [AC_MSG_ERROR([Not found libodbc library])])
AC_SEARCH_LIBS(SQLGetPrivateProfileString, odbcinst, [],[AC_MSG_ERROR([unixODBC library "odbcinst" not found])])
AC_CHECK_LIB(pthreads, pthread_create,[], [AC_CHECK_LIB(pthread, pthread_create)])
AC_CHECK_LIB(ssl, SSL_read, [], [AC_MSG_ERROR([ssl library not found])])
AC_CHECK_LIB([crypto], [SHA1_Init], [], [AC_MSG_ERROR([libcrypto not found])])
AC_CHECK_LIB(z, compress,,AC_MSG_ERROR([no zlib; please install zlib or equivalent]))
AC_CHECK_LIB(rt, clock_gettime,,[AC_MSG_ERROR([librt required but not found])])
    
# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
AC_SUBST([AM_CXXFLAGS])    
    
AC_CONFIG_FILES([Makefile
				src/autotools/Makefile
				tests/autotools/Makefile
				apache_mod_ffeadcpp/autotools/Makefile
				web/default/src/autotools/Makefile
				web/flexApp/src/autotools/Makefile
				web/oauthApp/src/autotools/Makefile])
AC_OUTPUT

echo "AM_CXXFLAGS: $AM_CXXFLAGS"
echo "LDFLAGS: $LDFLAGS"
echo "Debug mode: $support_debug"
echo "Make Executable: $MAKE"
echo "Generate Apache Module: $generate_apache_module"
