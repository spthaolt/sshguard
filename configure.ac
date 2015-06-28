# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.59])
AC_INIT([sshguard], [1.6.0], [sshguard-users@lists.sourceforge.net])
AC_CONFIG_SRCDIR([src/simclist.c])
AM_CONFIG_HEADER([src/config.h])
AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

##############################################################################
# Configuration Options

AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [Turn on debugging])])
AM_CONDITIONAL([DEBUG], [test x$enable_debug = xyes])

AC_ARG_WITH([firewall], [AS_HELP_STRING([--with-firewall=fw],
            [Firewall backend (one of pf, ipfw, iptables, ipfilter, hosts, aix, or null)])],
[
 # Substitute the correct commands into the firewall script.
 AC_SUBST_FILE([sshg_fw_subr])
 sshg_fw_subr=src/fwalls/sshg-fw.$withval

 case "$withval" in
     aix)
        useaix=true
        AC_CHECK_PROG([genfiltpath], [genfilt])
        if test x$genfiltpath = x
        then
            # genfilt not in PATH, use "/usr/sbin" as default path
            genfiltpath=/usr/sbin
            AC_MSG_WARN([genfilt program not in path! Using /usr/sbin as default unless --with-genfilt specified])
        fi
        ;;
     hosts)
        usehosts=true
        ;;
     ipfilter)
        useipfilter=true
        # is ipf in PATH?
        AC_CHECK_PROG([ipfpath], [ipf])
        if test x$ipfpath = x
        then
            # if ipf is not in PATH not, use "/sbin" as default path
            ipfpath=/sbin
            AC_MSG_WARN([ipf program not in path! Using /sbin as default unless --with-ipf specified])
        fi
        ;;
     ipfw)
        useipfw=true
        # is ipfw in PATH ?
        AC_CHECK_PROG([ipfwpath], [ipfw])
        if test x$ipfwpath = x
        then
            # if ipfw is not in PATH not, use "/sbin" as default path
            ipfwpath=/sbin
            AC_MSG_WARN([ipfw program not in path! Using /sbin as default unless --with-ipfw specified])
        fi
        ;;
     iptables)
        useiptables=true
        # is iptables in PATH ?
        AC_CHECK_PROG([iptablespath], [iptables])
        if test x$iptablespath = x
        then
            # if iptables is not in PATH not, use "/sbin" as default path
            iptablespath=/sbin
            AC_MSG_WARN([iptables program not in path! Using /sbin as default unless --with-iptables specified])
        fi
        ;;
     pf)
        usepf=true
        # is pfctl in PATH ?
        AC_CHECK_PROG([pfctlpath], [pfctl])
        if test x$pfctlpath = x
        then
            # if pfctl is not in PATH not, use "/sbin" as default path
            pfctlpath=/sbin
            AC_MSG_WARN([pfctl program not in path! Using /sbin as default unless --with-pfctl specified])
        fi
        ;;
     null)
        usenull=true
        ;;
     *)
        AC_MSG_ERROR([Invalid firewall backend (see help)])
        ;;
    esac
],
[
AC_MSG_ERROR([Please choose a firewall backend (see help)])
])

AC_ARG_WITH([genfilt], [AS_HELP_STRING([--with-genfilt=path],
            [Path to the genfilt command (default from PATH)])],
            [genfiltpath=`dirname $withval`])

AC_ARG_WITH([hosts], [AS_HELP_STRING([--with-hosts=file],
            [Path to allowed hosts file (default /etc/hosts.allow)])],
            [hostsfilepath=$withval],
            [hostsfilepath=/etc/hosts.allow])

AC_ARG_WITH([ipf], [AS_HELP_STRING([--with-ipf=path],
            [Path to the ipf command (default from PATH)])],
            [ipfpath=`dirname $withval`])

AC_ARG_WITH([ipfconf], [AS_HELP_STRING([--with-ipfconf=file],
            [Path to ipf configuration file (default /etc/ipf.rules)])],
            [ipfconf=$withval],
            [ipfconf=/etc/ipf.rules])

AC_ARG_WITH([ipfw], [AS_HELP_STRING([--with-ipfw=path],
            [Path to the ipfw command (default from PATH)])],
            [ipfwpath=`dirname $withval`])

AC_ARG_WITH([iptables], [AS_HELP_STRING([--with-iptables=path],
            [Path to the iptables command (default from PATH)])],
            [iptablespath=`dirname $withval`])

AC_ARG_WITH([pfctl], [AS_HELP_STRING([--with-pfctl=path],
            [Path to the pfctl command (default from PATH)])],
            [pfctlpath=`dirname $withval`])

##############################################################################
AS_BOX([Program Checks])
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_EGREP
AC_PROG_RANLIB
AC_PROG_YACC
AM_PROG_LEX

##############################################################################
AS_BOX([Library Checks])
AC_CHECK_LIB(pthread, pthread_create)

# Header Files
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h malloc.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h syslog.h unistd.h getopt.h])

# Types
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Compiler Characteristics
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT

# Sun Studio?
AC_CHECK_DECL([__SUNPRO_C], [SUNCC="yes"], [SUNCC="no"])

AS_IF([test "$SUNCC" = "yes"], [
    # sun compiler
    WARNING_CFLAGS="-v"
], [
    # other compiler (assume gcc-compatibile :( )
    WARNING_CFLAGS="-Wall"
])

AC_SUBST(WARNING_CFLAGS)

##############################################################################
AS_BOX([Library Functions])
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname inet_ntoa strerror strstr strtol])

# Solaris provides these functions in separate libraries
AC_SEARCH_LIBS([socket], [socket])
AC_SEARCH_LIBS([gethostbyname], [nsl])

# set what firewall backend has been used, for automake
# HOSTS chosen
AM_CONDITIONAL(FWALL_HOSTS, test x$usehosts = xtrue)

AC_DEFINE_UNQUOTED([PREFIX], "$prefix", [Prefix])

AC_DEFINE_UNQUOTED(FILT_PATH, "$genfiltpath", [Path to genfilt])
AC_DEFINE_UNQUOTED(HOSTSFILE_PATH, "$hostsfilepath", [Path to hosts.allow])
AC_DEFINE_UNQUOTED(IPFPATH, "$ipfpath", [Path to ipf])
AC_DEFINE_UNQUOTED(IPFW_PATH, "$ipfwpath", [Path to ipfw])
AC_DEFINE_UNQUOTED(IPTABLES_PATH, "$iptablespath", [Path to iptables])
AC_DEFINE_UNQUOTED(PFCTL_PATH, "$pfctlpath", [Path to pfctl])

# define $ipfconf as ipfilter configuration filename in config.h
AC_DEFINE_UNQUOTED(IPFILTER_CONFFILE, "$ipfconf", [filename of the ipfilter configuration file])

# define a suitable "grep -E" in config.h
AC_DEFINE_UNQUOTED(EGREP, $EGREP, [path and filename for a grep tool supporting -E])
# define a suitable awk
AC_DEFINE_UNQUOTED(AWK, $AWK, [path and filename for a suitable awk tool])

AC_OUTPUT([Makefile man/Makefile src/Makefile src/parser/Makefile src/fwalls/Makefile src/fwalls/sshg-fw])
