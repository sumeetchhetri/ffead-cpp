#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([ffead-cpp], [2.0], [sumeet.chhetri@gmail.com])
AM_INIT_AUTOMAKE([no-define foreign])
AC_CONFIG_SRCDIR([src/framework/ConfigurationData.h])
AC_CONFIG_HEADERS([src/modules/common/AppDefines.h])
m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT([static shared dlopen])

support_debug=no
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debug, default: no]),
    [support_debug=$enableval]
)
if test "x$support_debug" = "xyes"; then
    AC_DEFINE(DEBUG, [], [set DEBUG mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -O2"
else
    AC_DEFINE(NDEBUG, [], [unset DEBUG mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -O2"
    support_debug=no
fi

generate_apache_module=no
AC_ARG_ENABLE(apachemod,
AS_HELP_STRING([--enable-apachemod],
               [enable apachemod, default: no]),
    [generate_apache_module=$enableval]
)

generate_nginx_module=no
AC_ARG_ENABLE(nginxmod,
AS_HELP_STRING([--enable-nginxmod],
               [enable nginxmod, default: no]),
    [generate_nginx_module=$enableval]
)

AC_CANONICAL_HOST
local_os_type="unknown"
is_librt_available="yes"
AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include"
LDFLAGS="$LDFLAGS -L/usr/local/lib"
case "$host" in
*-cygwin*)
    AC_DEFINE(OS_CYGWIN, [], [set OS to cygwin])
    local_os_type="cygwin"
    ;;
*mingw*)
    AC_DEFINE(OS_MINGW, [], [set OS to mingw])
    AC_DEFINE(USE_WIN_IOCP, [], [use io completion ports on windows])
    local_os_type="mingw"
    is_librt_available="no"
    CPPFLAGS="$CPPFLAGS -I/mingw/include -L/mingw/lib -D__USE_MINGW_ANSI_STDIO=1"
    AM_CXXFLAGS="$AM_CXXFLAGS -I/mingw/include -L/mingw/lib"
    LDFLAGS="$LDFLAGS -L/mingw/lib"
    ;;
*-linux*)
    AC_DEFINE(OS_LINUX, [], [set OS to linux])
	#hack for some 64-bit systems
	#AC_DEFINE(_GNU_SOURCE, [], [set GNU source flag for linux])
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
    AM_CXXFLAGS="$AM_CXXFLAGS -pthread"
    LDFLAGS="$LDFLAGS -L/usr/local/v6/lib"
    ;;
*-netbsd*)
    AC_DEFINE(OS_BSD, [], [set OS to bsd])
    AC_DEFINE(OS_NETBSD, [], [set OS to netbsd])
    local_os_type="netbsd"

    #our hack for netbsd system
    LDFLAGS="$LDFLAGS -L/usr/local/v6/lib"
    ;;
*-openbsd*)
    AC_DEFINE(OS_BSD, [], [set OS to bsd])
    AC_DEFINE(OS_OPENBSD, [], [set OS to openbsd])
    local_os_type="openbsd"

	#get openbsd release number, remove `.' from release number
	OBSD_REL=`/usr/bin/uname -r | sed 's/\.//g'`
	AC_DEFINE_UNQUOTED(OS_OPENBSD_REL, ${OBSD_REL}, [set openbsd release number])

    #our hack for openbsd system
    LIBS="$LIBS -L/usr/local/lib"
    ;;
*-osf*)
    AC_DEFINE(OS_OSF, [], [set OS to osf])
    local_os_type="osf"
    ;;
*-darwin*)
	is_librt_available="no"
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


########################################################################################################
########################## CACHE MODULES, Memory, Redis, Memcached #####################################

AC_ARG_ENABLE(mod_memorycache,
AS_HELP_STRING([--enable-mod_memorycache],
               [enable mod_memorycache, default: yes]),
[case "${enableval}" in
             yes) mod_memorycache=yes ;;
             no)  mod_memorycache=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_memorycache]) ;;
esac],
[mod_memorycache=yes])
AM_CONDITIONAL(MOD_MEMORYCACHE, test "x$mod_memorycache" = "xyes")
AM_CONDITIONAL(MOD_CACHE, test "x$mod_memorycache" = "xyes")
if test "x$mod_memorycache" = "xyes"; then
    AC_DEFINE(INC_MEMORYCACHE, [], [enable memory cache module])
fi

AC_ARG_ENABLE(mod_rediscache,
AS_HELP_STRING([--enable-mod_rediscache],
               [enable mod_rediscache, default: no]),
[case "${enableval}" in
             yes) mod_rediscache=yes ;;
             no)  mod_rediscache=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_rediscache]) ;;
esac],
[mod_rediscache=no])
AM_CONDITIONAL(MOD_REDISCACHE, test "x$mod_rediscache" = "xyes")
AM_CONDITIONAL(MOD_CACHE, test "x$mod_rediscache" = "xyes")
if test "x$mod_rediscache" = "xyes"; then
    AC_DEFINE(INC_REDISCACHE, [], [enable redis cache module])
fi

AC_ARG_ENABLE(mod_memcached,
AS_HELP_STRING([--enable-mod_memcached],
               [enable mod_memcached, default: no]),
[case "${enableval}" in
             yes) mod_memcached=yes ;;
             no)  mod_memcached=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_memcached]) ;;
esac],
[mod_memcached=no])
AM_CONDITIONAL(MOD_MEMCACHED, test "x$mod_memcached" = "xyes")
AM_CONDITIONAL(MOD_CACHE, test "x$mod_memcached" = "xyes")
if test "x$mod_memcached" = "xyes"; then
    AC_DEFINE(INC_MEMCACHED, [], [enable memcached cache module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
########################## Component Module ############################################################

AC_ARG_ENABLE(mod_comp,
AS_HELP_STRING([--enable-mod_comp],
               [enable mod_comp, default: no]),
[case "${enableval}" in
             yes) mod_comp=yes ;;
             no)  mod_comp=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_comp]) ;;
esac],
[mod_comp=no])
AM_CONDITIONAL(MOD_COMPONENT, test "x$mod_comp" = "xyes")
AM_CONDITIONAL(MOD_XMLSERIALIZE, test "x$mod_comp" = "xyes")
AM_CONDITIONAL(MOD_SDORM, test "x$mod_comp" = "xyes")
if test "x$mod_comp" = "xyes"; then
    AC_DEFINE(INC_CMP, [], [enable component module])
    AC_DEFINE(INC_XMLSER, [], [enable xml serialization module])
    AC_DEFINE(INC_SDORM, [], [enable data-source-orm module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
########################## MethInvoker Module ##########################################################

AC_ARG_ENABLE(mod_methinvoker,
AS_HELP_STRING([--enable-mod_methinvoker],
               [enable mod_methinvoker, default: no]),
[case "${enableval}" in
             yes) mod_methinvoker=yes ;;
             no)  mod_methinvoker=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_methinvoker]) ;;
esac],
[mod_methinvoker=no])
AM_CONDITIONAL(MOD_METHINVOKER, test "x$mod_methinvoker" = "xyes")
AM_CONDITIONAL(MOD_XMLSERIALIZE, test "x$mod_methinvoker" = "xyes")
if test "x$mod_methinvoker" = "xyes"; then
    AC_DEFINE(INC_MI, [], [enable method invoker module])
    AC_DEFINE(INC_XMLSER, [], [enable xml serialization module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
########################## MsgHandler Module ###########################################################

AC_ARG_ENABLE(mod_msghandler,
AS_HELP_STRING([--enable-mod_msghandler],
               [enable mod_msghandler, default: no]),
[case "${enableval}" in
             yes) mod_msghandler=yes ;;
             no)  mod_msghandler=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_msghandler]) ;;
esac],
[mod_msghandler=no])
AM_CONDITIONAL(MOD_MSGHANDLER, test "x$mod_msghandler" = "xyes")
AM_CONDITIONAL(MOD_BINSERIALIZE, test "x$mod_msghandler" = "xyes")
if test "x$mod_msghandler" = "xyes"; then
    AC_DEFINE(INC_MSGH, [], [enable message handler module])
    AC_DEFINE(INC_BINSER, [], [enable binary serialization module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
########################## SDORM MODULES, SQL, Mongo ###################################################

AC_ARG_ENABLE(mod_sdormsql,
AS_HELP_STRING([--enable-mod_sdormsql],
               [enable mod_sdormsql, default: no]),
[case "${enableval}" in
             yes) mod_sdormsql=yes ;;
             no)  mod_sdormsql=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_sdormsql]) ;;
esac],
[mod_sdormsql=no])
AM_CONDITIONAL(MOD_SDORM_SQL, test "x$mod_sdormsql" = "xyes")
AM_CONDITIONAL(MOD_SDORM, test "x$mod_sdormsql" = "xyes")
if test "x$mod_sdormsql" = "xyes"; then
    AC_DEFINE(INC_SDORM_SQL, [], [enable data-source-orm-sql module])
    AC_DEFINE(INC_SDORM, [], [enable data-source-orm module])
fi

AC_ARG_ENABLE(mod_sdormmongo,
AS_HELP_STRING([--enable-mod_sdormmongo],
               [enable mod_sdormmongo, default: no]),
[case "${enableval}" in
             yes) mod_sdormmongo=yes ;;
             no)  mod_sdormmongo=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_sdormmongo]) ;;
esac],
[mod_sdormmongo=no])
AM_CONDITIONAL(MOD_SDORM_MONGO, test "x$mod_sdormmongo" = "xyes")
AM_CONDITIONAL(MOD_SDORM, test "x$mod_sdormmongo" = "xyes")
if test "x$mod_sdormmongo" = "xyes"; then
	AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/include/libmongoc-1.0/ -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0/ -I/usr/local/include/libbson-1.0"
    CPPFLAGS="$CPPFLAGS -I/usr/include/libmongoc-1.0/ -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0/ -I/usr/local/include/libbson-1.0"
    AC_DEFINE(INC_SDORM_MONGO, [], [enable data-source-orm-mongo module])
    AC_DEFINE(INC_SDORM, [], [enable data-source-orm module])
fi

AC_ARG_ENABLE(mod_gtm,
AS_HELP_STRING([--enable-mod_gtm],
               [enable mod_gtm, default: no]),
[case "${enableval}" in
             yes) mod_gtm=yes ;;
             no)  mod_gtm=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_gtm]) ;;
esac],
[mod_gtm=no])
AM_CONDITIONAL(MOD_GTM, test "x$mod_gtm" = "xyes")
if test "x$mod_gtm" = "xyes"; then
    AC_DEFINE(INC_GTM, [], [enable sdorm gtm module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
########################## Distocache module ###########################################################

AC_ARG_ENABLE(mod_distocache,
AS_HELP_STRING([--enable-mod_distocache],
               [enable mod_distocache, default: no]),
[case "${enableval}" in
             yes) mod_distocache=yes ;;
             no)  mod_distocache=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_distocache]) ;;
esac],
[mod_distocache=no])
AM_CONDITIONAL(MOD_DISTOCACHE, test "x$mod_distocache" = "xyes")
AM_CONDITIONAL(MOD_BINSERIALIZE, test "x$mod_distocache" = "xyes")
AM_CONDITIONAL(MOD_THREADS, test "x$mod_distocache" = "xyes")
if test "x$mod_distocache" = "xyes"; then
    AC_DEFINE(INC_DSTC, [], [enable distocache module])
    AC_DEFINE(INC_BINSER, [], [enable binary serialization module])
    AC_DEFINE(INC_THREADS, [], [enable threads module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Serializer Module XML,JSON,Binary #######################################

AC_ARG_ENABLE(mod_xmlserialize,
AS_HELP_STRING([--enable-mod_xmlserialize],
               [enable mod_xmlserialize, default: yes]),
[case "${enableval}" in
             yes) mod_xmlserialize=yes ;;
             no)  mod_xmlserialize=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_xmlserialize]) ;;
esac],
[mod_xmlserialize=yes])
AM_CONDITIONAL(MOD_XMLSERIALIZE, test "x$mod_xmlserialize" = "xyes")
if test "x$mod_xmlserialize" = "xyes"; then
    AC_DEFINE(INC_XMLSER, [], [enable xml serialization module])
fi

AC_ARG_ENABLE(mod_binserialize,
AS_HELP_STRING([--enable-mod_binserialize],
               [enable mod_binserialize, default: yes]),
[case "${enableval}" in
             yes) mod_binserialize=yes ;;
             no)  mod_binserialize=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_binserialize]) ;;
esac],
[mod_binserialize=yes])
AM_CONDITIONAL(MOD_BINSERIALIZE, test "x$mod_binserialize" = "xyes")
if test "x$mod_binserialize" = "xyes"; then
    AC_DEFINE(INC_BINSER, [], [enable binary serilaization module])
fi

AC_ARG_ENABLE(mod_jsonserialize,
AS_HELP_STRING([--enable-mod_jsonserialize],
               [enable mod_jsonserialize, default: yes]),
[case "${enableval}" in
             yes) mod_jsonserialize=yes ;;
             no)  mod_jsonserialize=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_jsonserialize]) ;;
esac],
[mod_jsonserialize=yes])
AM_CONDITIONAL(MOD_JSONSERIALIZE, test "x$mod_jsonserialize" = "xyes")
if test "x$mod_jsonserialize" = "xyes"; then
    AC_DEFINE(INC_JSONSER, [], [enable json serilaization module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## C++ Interpreter #########################################################

AC_ARG_ENABLE(mod_interpreter,
AS_HELP_STRING([--enable-mod_interpreter],
               [enable mod_interpreter, default: yes]),
[case "${enableval}" in
             yes) mod_interpreter=yes ;;
             no)  mod_interpreter=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_interpreter]) ;;
esac],
[mod_interpreter=yes])
AM_CONDITIONAL(MOD_INTERPRETER, test "x$mod_interpreter" = "xyes")
AM_CONDITIONAL(MOD_REFLECTION, test "x$mod_interpreter" = "xyes")
if test "x$mod_interpreter" = "xyes"; then
    AC_DEFINE(INC_INTP, [], [enable c++ interpreter module])
    AC_DEFINE(INC_REFLECTION, [], [enable reflection module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Job Scheduler ###########################################################

AC_ARG_ENABLE(mod_jobs,
AS_HELP_STRING([--enable-mod_jobs],
               [enable mod_jobs, default: yes]),
[case "${enableval}" in
             yes) mod_jobs=yes ;;
             no)  mod_jobs=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_jobs]) ;;
esac],
[mod_jobs=yes])
AM_CONDITIONAL(MOD_JOBS, test "x$mod_jobs" = "xyes")
AM_CONDITIONAL(MOD_REFLECTION, test "x$mod_jobs" = "xyes")
AM_CONDITIONAL(MOD_THREADS, test "x$mod_jobs" = "xyes")
if test "x$mod_jobs" = "xyes"; then
    AC_DEFINE(INC_JOBS, [], [enable cron/batch jobs module])
    AC_DEFINE(INC_REFLECTION, [], [enable reflection module])
    AC_DEFINE(INC_THREADS, [], [enable threads module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Client Utility ##########################################################

AC_ARG_ENABLE(mod_clientutil,
AS_HELP_STRING([--enable-mod_clientutil],
               [enable mod_clientutil, default: yes]),
[case "${enableval}" in
             yes) mod_clientutil=yes ;;
             no)  mod_clientutil=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_clientutil]) ;;
esac],
[mod_clientutil=yes])
AM_CONDITIONAL(MOD_CLIENTUTIL, test "x$mod_clientutil" = "xyes")
if test "x$mod_clientutil" = "xyes"; then
    AC_DEFINE(INC_CLIENTUTIL, [], [enable client-utility module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## IOC Utility #############################################################

AC_ARG_ENABLE(mod_ioc,
AS_HELP_STRING([--enable-mod_ioc],
               [enable mod_ioc, default: yes]),
[case "${enableval}" in
             yes) mod_ioc=yes ;;
             no)  mod_ioc=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_ioc]) ;;
esac],
[mod_ioc=yes])
AM_CONDITIONAL(MOD_IOC, test "x$mod_ioc" = "xyes")
AM_CONDITIONAL(MOD_REFLECTION, test "x$mod_ioc" = "xyes")
if test "x$mod_ioc" = "xyes"; then
    AC_DEFINE(INC_IOC, [], [enable ioc module])
    AC_DEFINE(INC_REFLECTION, [], [enable reflection module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Reflection Utility ######################################################

AC_ARG_ENABLE(mod_reflection,
AS_HELP_STRING([--enable-mod_reflection],
               [enable mod_reflection, default: yes]),
[case "${enableval}" in
             yes) mod_reflection=yes ;;
             no)  mod_reflection=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_reflection]) ;;
esac],
[mod_reflection=yes])
AM_CONDITIONAL(MOD_REFLECTION, test "x$mod_reflection" = "xyes")
if test "x$mod_reflection" = "xyes"; then
    AC_DEFINE(INC_REFLECTION, [], [enable reflection module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## HTTP Utility ############################################################

AC_ARG_ENABLE(mod_http,
AS_HELP_STRING([--enable-mod_http],
               [enable mod_http, default: yes]),
[case "${enableval}" in
             yes) mod_http=yes ;;
             no)  mod_http=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_http]) ;;
esac],
[mod_http=yes])
AM_CONDITIONAL(MOD_HTTP, test "x$mod_http" = "xyes")
AM_CONDITIONAL(MOD_THREADS, test "x$mod_http" = "xyes")
if test "x$mod_http" = "xyes"; then
    AC_DEFINE(INC_HTTP, [], [enable http module])
    AC_DEFINE(INC_THREADS, [], [enable threads module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Server Utility ##########################################################

AC_ARG_ENABLE(mod_serverutil,
AS_HELP_STRING([--enable-mod_serverutil],
               [enable mod_serverutil, default: yes]),
[case "${enableval}" in
             yes) mod_serverutil=yes ;;
             no)  mod_serverutil=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_serverutil]) ;;
esac],
[mod_serverutil=yes])
AM_CONDITIONAL(MOD_SERVERUTIL, test "x$mod_serverutil" = "xyes")
AM_CONDITIONAL(MOD_THREADS, test "x$mod_serverutil" = "xyes")
if test "x$mod_serverutil" = "xyes"; then
    AC_DEFINE(INC_SERVERUTIL, [], [enable server-utility module])
    AC_DEFINE(INC_THREADS, [], [enable threads module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## SSL Utility #############################################################

AC_ARG_ENABLE(mod_sslutil,
AS_HELP_STRING([--enable-mod_sslutil],
               [enable mod_sslutil, default: yes]),
[case "${enableval}" in
             yes) mod_sslutil=yes ;;
             no)  mod_sslutil=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_sslutil]) ;;
esac],
[mod_sslutil=yes])
AM_CONDITIONAL(MOD_SSLUTIL, test "x$mod_sslutil" = "xyes")
if test "x$mod_sslutil" = "xyes"; then
    AC_DEFINE(INC_SSLUTIL, [], [enable ssl-utility module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Threads Utility #########################################################

AC_ARG_ENABLE(mod_threads,
AS_HELP_STRING([--enable-mod_threads],
               [enable mod_threads, default: yes]),
[case "${enableval}" in
             yes) mod_threads=yes ;;
             no)  mod_threads=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_threads]) ;;
esac],
[mod_threads=yes])
AM_CONDITIONAL(MOD_THREADS, test "x$mod_threads" = "xyes")
if test "x$mod_threads" = "xyes"; then
    AC_DEFINE(INC_THREADS, [], [enable threads module])
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## HTTP Server & Framework #################################################

AM_CONDITIONAL(MOD_HTTPSERVER, test "x$yes" = "xyes")
AM_CONDITIONAL(MOD_SCRIPT, test "x$yes" = "xno")
AM_CONDITIONAL(MOD_DCP, test "x$yes" = "xno")
AM_CONDITIONAL(MOD_DVIEW, test "x$yes" = "xno")
AM_CONDITIONAL(MOD_DCP, test "x$yes" = "xno")
AM_CONDITIONAL(MOD_TPE, test "x$yes" = "xno")
AM_CONDITIONAL(MOD_APPFLOW, test "x$yes" = "xno")
AM_CONDITIONAL(MOD_WEBSERVICE, test "x$yes" = "xno")

AC_ARG_ENABLE(mod_httpframework,
AS_HELP_STRING([--enable-mod_httpframework],
               [enable mod_httpframework, default: yes]),
[case "${enableval}" in
             yes) mod_httpframework=yes ;;
             no)  mod_httpframework=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_httpframework]) ;;
esac],
[mod_httpframework=yes])
AM_CONDITIONAL(MOD_HTTPFRAMEWORK, test "x$mod_httpframework" = "xyes")
if test "x$mod_httpframework" = "xyes"; then
    AC_DEFINE(INC_HTTPFRAMEWORK, [], [enable http framework module])
    
    AC_ARG_ENABLE(mod_dcp,
	AS_HELP_STRING([--enable-mod_dcp],
	               [enable mod_dcp, default: yes]),
	[case "${enableval}" in
	             yes) mod_dcp=yes ;;
	             no)  mod_dcp=no ;;
	             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_dcp]) ;;
	esac],
	[mod_dcp=yes])
	AM_CONDITIONAL(MOD_DCP, test "x$mod_dcp" = "xyes")
	if test "x$mod_dcp" = "xyes"; then
	    AC_DEFINE(INC_DCP, [], [enable http framework dynamic c++ pages module])
	fi
	
	AC_ARG_ENABLE(mod_dview,
	AS_HELP_STRING([--enable-mod_dview],
	               [enable mod_dview, default: yes]),
	[case "${enableval}" in
	             yes) mod_dview=yes ;;
	             no)  mod_dview=no ;;
	             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_dview]) ;;
	esac],
	[mod_dview=yes])
	AM_CONDITIONAL(MOD_DVIEW, test "x$mod_dview" = "xyes")
	if test "x$mod_dview" = "xyes"; then
	    AC_DEFINE(INC_DVIEW, [], [enable http framework dynamic views module])
	fi
	
	AC_ARG_ENABLE(mod_tpe,
	AS_HELP_STRING([--enable-mod_tpe],
	               [enable mod_tpe, default: yes]),
	[case "${enableval}" in
	             yes) mod_tpe=yes ;;
	             no)  mod_tpe=no ;;
	             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_tpe]) ;;
	esac],
	[mod_tpe=yes])
	AM_CONDITIONAL(MOD_TPE, test "x$mod_tpe" = "xyes")
	if test "x$mod_tpe" = "xyes"; then
	    AC_DEFINE(INC_TPE, [], [enable http framework templates module])
	fi
	
	AC_ARG_ENABLE(mod_appflow,
	AS_HELP_STRING([--enable-mod_appflow],
	               [enable mod_appflow, default: no]),
	[case "${enableval}" in
	             yes) mod_appflow=yes ;;
	             no)  mod_appflow=no ;;
	             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_appflow]) ;;
	esac],
	[mod_appflow=no])
	AM_CONDITIONAL(MOD_APPFLOW, test "x$mod_appflow" = "xyes")
	if test "x$mod_appflow" = "xyes"; then
	    AC_DEFINE(INC_APPFLOW, [], [enable http framework application flow module])
	fi
	
	AC_ARG_ENABLE(mod_webservice,
	AS_HELP_STRING([--enable-mod_webservice],
	               [enable mod_webservice, default: yes]),
	[case "${enableval}" in
	             yes) mod_webservice=yes ;;
	             no)  mod_webservice=no ;;
	             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_webservice]) ;;
	esac],
	[mod_webservice=yes])
	AM_CONDITIONAL(MOD_WEBSERVICE, test "x$mod_webservice" = "xyes")
	if test "x$mod_webservice" = "xyes"; then
	    AC_DEFINE(INC_WEBSVC, [], [enable http framework web-service module])
	fi
	
	AC_ARG_ENABLE(mod_script,
	AS_HELP_STRING([--enable-mod_script],
	               [enable mod_script, default: no]),
	[case "${enableval}" in
	             yes) mod_script=yes ;;
	             no)  mod_script=no ;;
	             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_script]) ;;
	esac],
	[mod_script=no])
	AM_CONDITIONAL(MOD_SCRH, test "x$mod_script" = "xyes")
	if test "x$mod_script" = "xyes"; then
	    AC_DEFINE(INC_SCRH, [], [enable http framework scripting support module])
	    AM_CXXFLAGS="$AM_CXXFLAGS -U__STRICT_ANSI__"
	fi
fi

########################################################################################################
########################################################################################################


########################################################################################################
############################## Default modules #########################################################

AC_ARG_ENABLE(mod_default,
AS_HELP_STRING([--enable-mod_default],
               [enable mod_default, default: yes]),
[case "${enableval}" in
             yes) mod_default=yes ;;
             no)  mod_default=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_default]) ;;
esac],
[mod_default=yes])
AM_CONDITIONAL(MOD_DEFAULT, test "x$mod_default" = "xyes")

########################################################################################################
########################################################################################################


########################################################################################################
############################## All the modules #########################################################

AC_ARG_ENABLE(mod_all,
AS_HELP_STRING([--enable-mod_all],
               [enable mod_all, default: no]),
[case "${enableval}" in
             yes) mod_all=yes ;;
             no)  mod_all=no ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-mod_all]) ;;
esac],
[mod_all=no])
AM_CONDITIONAL(MOD_ALL, test "x$mod_all" = "xyes")

########################################################################################################
########################################################################################################


# store current user given compiler flags to avoid default setup via AC_PROG_CXX
OLD_CXXFLAGS=$CXXFLAGS

STDCXX_11_SWITCH=

dnl# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11(,mandatory)

#if test "x$GCC" = "xno"; then
#	AC_MSG_ERROR([Please install GNU GCC/G++ to continue.])
#fi

if test "x$mod_rediscache" = "xyes"; then
	AC_CHECK_HEADER([hiredis/hiredis.h],,[AC_MSG_ERROR([hiredis header files not found, please install hiredis])])
	AC_SEARCH_LIBS(redisCommand, hiredis, [],[AC_MSG_ERROR([redis library "hiredis" not found])])
fi

if test "x$mod_memcached" = "xyes"; then
	AC_CHECK_HEADER([libmemcached/memcached.h],,[AC_MSG_ERROR([libmemcached header files not found, please install libmemcached])])
	AC_SEARCH_LIBS(memcached_pool, memcachedutil, [],[AC_MSG_ERROR([memcached library "libmemcachedutil" not found])])
fi

AM_CONDITIONAL(MOD_APACHEMOD, test "x$generate_apache_module" = "xyes")
if test "x$generate_apache_module" = "xyes"; then
	AC_SUBST(GENERATE_APACHEMOD,[modules/apache_mod_ffeadcpp/autotools])
	if test "x$local_os_type" = "xcygwin"; then
		AC_SUBST(LIB_TYPE,[-l])
		AC_SUBST(LIBRARY_EXTENSION,[dll])
		AC_CHECK_LIB(httpd2core, main, [AC_SUBST(APACHE_HTTPD_LIB,[-lhttpd2core])],[])
		if test -f /usr/sbin/apxs2; then
			echo 'apxs2 found in /usr/sbin'
			AC_CHECK_PROG(APXS, [apxs2], [/usr/sbin/apxs2], [no],[$PATH$PATH_SEPARATOR/usr/sbin/])
		elif test -f /usr/local/bin/apxs2; then
			echo 'apxs2 found in /usr/local/bin/'
			AC_CHECK_PROG(APXS, [apxs2], [/usr/local/bin/apxs2], [no],[$PATH$PATH_SEPARATOR/usr/local/bin/])
		else
			AC_CHECK_PROG(APXS, apxs2, [apxs2], [no],[])
		fi
		if test "x$APXS" = "xno" ; then
		    AC_MSG_ERROR([Please install apache development tools (apxs/apxs2) to continue.])
		fi
	elif test "x$local_os_type" = "xmingw"; then
		AC_SUBST(LIB_TYPE,[-llib])
		AC_SUBST(LIBRARY_EXTENSION,[dll])
		AC_SUBST(APACHE_HTTPD_LIB,[ ])
		AC_CHECK_PROG(APXS, apxs.pl, [apxs.pl], [no],[])
		if test "x$APXS" = "xno" ; then
		    AC_MSG_ERROR([Please install apache development tools (apxs/apxs2) to continue, apxs.pl not found, make sure you have your Apache directory in the path and apxs is installed correctly])
		fi
	else
		AC_SUBST(LIB_TYPE,[-l])
		AC_SUBST(LIBRARY_EXTENSION,[so])
		AC_SUBST(APACHE_HTTPD_LIB,[ ])
		AC_CHECK_PROG(APXS, apxs2, [apxs2], [apxs2 not found],[])
		if test "x$APXS" = "xno" ; then
		    AC_MSG_ERROR([Please install apache development tools (apxs/apxs2) to continue.])
		fi
	fi
else
	AC_SUBST(LIB_TYPE,[-l])
	AC_SUBST(GENERATE_APACHEMOD,[ ])
	AC_SUBST(APACHE_HTTPD_LIB,[ ])
fi


# reset compiler flags to initial flags
CXXFLAGS=$OLD_CXXFLAGS

AC_PROG_MAKE_SET

dnl# Checks for libraries.
AC_DEFINE(BUILT_WITH_CONFGURE, [1], [set configure mode])
AM_CONDITIONAL(BUILT_WITH_CONFGURE, test "x1" = "x1")

AC_CHECK_HEADER([_mingw_mac.h], 
					 [AC_DEFINE(OS_MINGW_W64, [1], [Define to 1 if you have _mingw_mac.h])], 
					 [AC_DEFINE(OS_MINGW_W64, [0], [Define to 0 if you don't have _mingw_mac.h])])
AC_CHECK_HEADER([sys/sendfile.h], 
					 [AC_DEFINE(IS_SENDFILE, [1], [Define to 1 if you have sys/sendfile.h])], 
					 [AC_DEFINE(IS_SENDFILE, [0], [Define to 0 if you don't have sys/sendfile.h])])
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
                     
                     
OLD_CPP=$CPP
CPP="$CXXCPP $STDCXX_11_SWITCH"
AC_CHECK_HEADER([atomic],
                     [AC_DEFINE(USE_ATOMIC_H, [1], [Define to 1 if you have <atomic.h>.])],
                     [AC_DEFINE(USE_ATOMIC_H, [0], [atomic support not found])], [-])
AC_CHECK_HEADER([cstdatomic],
                     [AC_DEFINE(USE_CSTDATOMIC_H, [1], [Define to 1 if you have <cstdatomic.h>.])],
                     [AC_DEFINE(USE_CSTDATOMIC_H, [0], [cstdatomic support not found])], [-])
CPP=$OLD_CPP

AC_CHECK_HEADER([openssl/ssl.h],,[AC_MSG_ERROR([ssl header not found])])

if test "x$mod_sdormsql" = "xyes"; then
	AC_CHECK_HEADER([uuid/uuid.h],,[AC_MSG_ERROR([uuid.h header not found])])
	AC_CHECK_HEADER([sql.h],,[AC_MSG_ERROR([ODBC sql.h header not found])])
fi

if test "x$mod_sdormmongo" = "xyes"; then
	AC_CHECK_HEADER([uuid/uuid.h],,[AC_MSG_ERROR([uuid.h header not found])])
	AC_CHECK_HEADER([mongoc.h],,[AC_MSG_ERROR([mongodb mongoc.h header not found])])
	AC_CHECK_HEADER([bson.h],,[AC_MSG_ERROR([mongodb bson.h header not found])])
fi

if test "x$mod_gtm" = "xyes"; then
     if test -f /usr/local/include/gtmxc_types.h; then
		echo 'gtm gtmxc_types.h header found..'
     else
		AC_CHECK_HEADER([gtmxc_types.h],,[AC_MSG_ERROR([gtm gtmxc_types.h header not found])])	
     fi
fi

dnl# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl# unixODBC wants the following to get sane behavior for ODBCINT64
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(void *)
AC_CHECK_TYPES(long long)
AC_CHECK_TYPES(signed char)

ADD_LIBDL=

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
#AC_CHECK_LIB(c, dlopen, LIBDL="", [AC_CHECK_LIB(dl, dlopen, LIBDL="-ldl")])
AC_CHECK_LIB(c, dlopen, ADD_LIBDL="", [AC_CHECK_LIB(dl, dlopen, ADD_LIBDL="-ldl")])
AC_CHECK_LIB(rt, clock_nanosleep, [AC_DEFINE(HAVE_CLOCK_NANOSLEEP, 1, [Define if you have clock_nanosleep()])], [])

if test "x$mod_sdormsql" = "xyes"; then
	AC_CHECK_LIB(uuid, uuid_generate, [], [AC_MSG_ERROR([uuid library not found])])
	AC_CHECK_LIB(odbc, main, , [AC_MSG_ERROR([Not found libodbc library])])
	AC_SEARCH_LIBS(SQLGetPrivateProfileString, odbcinst, [],[AC_MSG_ERROR([unixODBC library "odbcinst" not found])])
fi

if test "x$mod_sdormmongo" = "xyes"; then
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
	AC_CHECK_LIB(kernel32, main, , [AC_MSG_ERROR([Not found libkernel32 library])])
	AC_CHECK_LIB(regex, regcomp, , [AC_MSG_ERROR([Not found libregex library])])
fi

if test "x$mod_gtm" = "xyes"; then
    AC_CHECK_LIB(gtmshr, main, , [AC_MSG_ERROR([Not found libgtmshr library])])
fi
 
AM_CXXFLAGS="$AM_CXXFLAGS $STDCXX_11_SWITCH"
# distribute additional compiler and linker flags among Makefiles
# --> set and change these variables instead of CXXFLAGS or LDFLAGS (for user only)
AC_SUBST([AM_CXXFLAGS])    
AC_SUBST([ADD_LIBDL])

AM_CONDITIONAL(MOD_NGINXMOD, test "x$generate_nginx_module" = "xyes")
if test "x$generate_nginx_module" = "xyes"; then
        INC_PATHSNGX="$CPPFLAGS $AM_CXXFLAGS"
        AC_SUBST(GENERATE_NGINXMOD,[modules/nginx_mod_ffeadcpp])
        AC_SUBST(INCPATHS_NGINXMOD,[$INC_PATHSNGX])

        AC_SUBST(LIBPATHS_NGINXMOD,[$LDFLAGS])
else
        AC_SUBST(GENERATE_NGINXMOD,[ ])
fi

AC_SUBST(BUILD_CPPFLAGS,[$CPPFLAGS])
AC_SUBST(BUILD_AM_CXXFLAGS,[$AM_CXXFLAGS])
AC_SUBST(BUILD_LDFLAGS,[$LDFLAGS])
AC_SUBST(BUILD_DEBUG,[$support_debug])

AC_CONFIG_FILES([Makefile
				src/autotools/Makefile
				tests/autotools/Makefile
				modules/apache_mod_ffeadcpp/autotools/Makefile
				web/default/src/autotools/Makefile
				web/flexApp/src/autotools/Makefile
				web/oauthApp/src/autotools/Makefile
				web/markers/src/autotools/Makefile
				web/te-benchmark/src/autotools/Makefile])
AC_OUTPUT

echo "Host: $host"
echo "CPPFLAGS: $CPPFLAGS"
echo "AM_CXXFLAGS: $AM_CXXFLAGS"
echo "LDFLAGS: $LDFLAGS"
echo "Debug mode: $support_debug"
echo "Make Executable: $MAKE"
echo "Generate Apache Module: $generate_apache_module"

