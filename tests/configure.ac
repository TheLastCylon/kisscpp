AC_INIT([testkisscpp], [0.1], [bothadjt@gmail.com], [testkisscpp], [http://djb.co.za/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_PROG_LIBTOOL

BOOST_REQUIRE([1.54.0])
BOOST_SYSTEM([mt-pn])
BOOST_THREAD([mt-pn])
BOOST_FILESYSTEM([mt-pn])
BOOST_REGEX([pn])
BOOST_DATE_TIME([mt-pn])
BOOST_PROGRAM_OPTIONS([mt-pn])

AC_ARG_WITH([pthread],
            [AS_HELP_STRING([--without-pthread], [ignore pthread support @<:@default: automatic@:>@])],
            [PTHREAD_LIB=""],
            [PTHREAD_LIB="-lpthread"])
AC_SUBST([PTHREAD_LIB])

KISSCPP_CPPFLAGS="-I../../build/.libs"
AC_ARG_WITH([kisscpp-include-path],
            [AS_HELP_STRING([--with-kisscpp-include-path], [location of the kisscpp headers, defaults to /usr/include/kisscpp-0.5])],
            [KISSCPP_CPPFLAGS="-I$withval"],
            [KISSCPP_CPPFLAGS="-I../../kisscpp"])
AC_SUBST([KISSCPP_CPPFLAGS]) 

AC_ARG_WITH([kisscpp-lib-path],
            [AS_HELP_STRING([--with-kisscpp-lib-path], [location of the kisscpp library])],
            [KISSCPP_LIB="-L$withval -lkisscpp"],
            [KISSCPP_LIB="-lkisscpp"])
AC_SUBST([KISSCPP_LIB])

PKG_CHECK_MODULES([DEPS], [kisscpp-0.5 >= 0.5])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

