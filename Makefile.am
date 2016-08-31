## libkisscpp:
## put descriptive splurge here.

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS  = -I m4

EXTRA_DIST       = bootstrap

## Define a libtool archive target "libexample-@EXAMPLE_API_VERSION@.la", with
## @EXAMPLE_API_VERSION@ substituted into the generated Makefile at configure
## time.
## The libtool archive file (.la) will be installed into the directory named
## by the predefined variable $(bindir), along with the actual shared library
## file (.so).
lib_LTLIBRARIES = libkisscpp-@KISSCPP_API_VERSION@.la

## Define the source file list for the "libexample-@EXAMPLE_API_VERSION@.la"
## target.  Note that @EXAMPLE_API_VERSION@ is not interpreted by Automake and
## will therefore be treated as if it were literally part of the target name,
## and the variable name derived from that.
## The file extension .cc is recognized by Automake, and makes it produce
## rules which invoke the C++ compiler to produce a libtool object file (.lo)
## from each source file.  Note that it is not necessary to list header files
## which are already listed elsewhere in a _HEADERS variable assignment.
libkisscpp_@KISSCPP_API_VERSION@_la_SOURCES = kisscpp/boost_ptree.cpp \
                                              kisscpp/client.cpp \
                                              kisscpp/connection.cpp \
                                              kisscpp/configuration.cpp \
                                              kisscpp/errorstate.cpp \
                                              kisscpp/io_service_pool.cpp \
                                              kisscpp/logstream.cpp \
                                              kisscpp/server.cpp \
                                              kisscpp/standard_handlers.cpp \
                                              kisscpp/statskeeper.cpp

## Instruct libtool to include ABI version information in the generated shared
## library file (.so).  The library ABI version is defined in configure.ac, so
## that all version information is kept in one place.
#libkisscpp_@KISSCPP_API_VERSION@_la_LDFLAGS = -version-info $(KISSCPP_API_VERSION)
libkisscpp_@KISSCPP_API_VERSION@_la_LDFLAGS = -version-info 1

## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
kisscpp_includedir = $(includedir)/kisscpp-$(KISSCPP_API_VERSION)
nobase_kisscpp_include_HEADERS = kisscpp/boost_ptree.hpp \
                                 kisscpp/client.hpp \
                                 kisscpp/connection.hpp \
                                 kisscpp/configuration.hpp \
                                 kisscpp/errorstate.hpp \
                                 kisscpp/io_service_pool.hpp \
                                 kisscpp/logstream.hpp \
                                 kisscpp/persisted_queue.hpp \
                                 kisscpp/persisted_queue.tpp \
                                 kisscpp/ptree_queue.hpp \
                                 kisscpp/request_status.hpp \
                                 kisscpp/request_router.hpp \
                                 kisscpp/request_handler.hpp \
                                 kisscpp/server.hpp \
                                 kisscpp/standard_handlers.hpp \
                                 kisscpp/statable_queue.hpp \
                                 kisscpp/statskeeper.hpp \
                                 kisscpp/threadsafe_persisted_delayed_queue.hpp \
                                 kisscpp/threadsafe_persisted_priority_queue.hpp \
                                 kisscpp/threadsafe_persisted_queue.hpp \
                                 kisscpp/threadsafe_queue.hpp

## The generated configuration header is installed in its own subdirectory of
## $(libdir).  The reason for this is that the configuration information put
## into this header file describes the target platform the installed library
## has been built for.  Thus the file must not be installed into a location
## intended for architecture-independent files, as defined by the Filesystem
## Hierarchy Standard (FHS).
## The nodist_ prefix instructs Automake to not generate rules for including
## the listed files in the distribution on 'make dist'.  Files that are listed
## in _HEADERS variables are normally included in the distribution, but the
## configuration header file is generated at configure time and should not be
## shipped with the source tarball.
kisscpp_libincludedir = $(libdir)/kisscpp-$(KISSCPP_API_VERSION)/include
#kisscpp_libincludedir = $(libdir)/kisscpp/include
nodist_kisscpp_libinclude_HEADERS = kisscppconfig.h

## Install the generated pkg-config file (.pc) into the expected location for
## architecture-dependent package configuration information.  Occasionally,
## pkg-config files are also used for architecture-independent data packages,
## in which case the correct install location would be $(datadir)/pkgconfig.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = kisscpp-$(KISSCPP_API_VERSION).pc

## Define an independent executable script for inclusion in the distribution
## archive.  However, it will not be installed on an end user's system due to
## the noinst_ prefix.
dist_noinst_SCRIPTS = autogen.sh
