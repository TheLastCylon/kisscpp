AUTOMAKE_OPTIONS    = subdir-objects
#ACLOCAL_AMFLAGS     = -I m4
bin_PROGRAMS        = kcsrt
AM_CPPFLAGS         = $(DEPS_CFLAGS) $(BOOST_CFLAGS)
kcsrt_LDADD         = $(DEPS_LIBS) $(BOOST_LIBS)
kcsrt_SOURCES       = src/handler_echo.cpp \
                      src/handler_echo.hpp \
                      src/kcsrt.cpp \
                      src/kcsrt.hpp \
                      src/appid.hpp \
                      src/main.cpp
dist_noinst_SCRIPTS = autogen.sh

