AUTOMAKE_OPTIONS    = subdir-objects
#ACLOCAL_AMFLAGS     = -I m4
AM_CPPFLAGS         = $(DEPS_CFLAGS) $(BOOST_CFLAGS) -I/usr/local/include/kisscpp-0.5
kc_echo_LDADD       = $(DEPS_LIBS) $(BOOST_LIBS)
bin_PROGRAMS        = kc_echo
kc_echo_SOURCES     = src/handler_echo.cpp \
                      src/handler_echo.hpp \
                      src/kc_echo.cpp \
                      src/kc_echo.hpp \
                      src/main.cpp
dist_noinst_SCRIPTS = autogen.sh

