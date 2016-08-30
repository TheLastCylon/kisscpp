AUTOMAKE_OPTIONS    = subdir-objects
#ACLOCAL_AMFLAGS     = -I m4
AM_CPPFLAGS         = $(DEPS_CFLAGS) $(BOOST_CFLAGS) $(KISSCPP_CFLAGS)
kc_echo_LDADD       = $(DEPS_LIBS) $(BOOST_LIBS) $(PTHREAD_LIB) $(KISSCPP_LIB)
bin_PROGRAMS        = kc_echo
kc_echo_SOURCES     = src/handler_echo.cpp \
                      src/handler_echo.hpp \
                      src/kc_echo.cpp \
                      src/kc_echo.hpp \
                      src/main.cpp
dist_noinst_SCRIPTS = autogen.sh

