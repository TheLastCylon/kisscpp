// File  : server.hpp
// Author: Dirk J. Botha <bothadj@gmail.com>
//
// This file is part of kisscpp library.
//
// The kisscpp library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The kisscpp library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the kisscpp library. If not, see <http://www.gnu.org/licenses/>.

#ifndef _SERVER_SERVER_HPP
#define _SERVER_SERVER_HPP

#include <sys/types.h>  // getpid()
#include <unistd.h>     // getpid()
#include <sys/stat.h>   // umask

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

#include "io_service_pool.hpp"
#include "connection.hpp"
#include "request_router.hpp"
#include "logstream.hpp"
#include "statskeeper.hpp"
#include "errorstate.hpp"
#include "standard_handlers.hpp"
#include "configuration.hpp"

namespace kisscpp
{
  class Server : private boost::noncopyable // The top-level class of the server.
  {
    public:
      // Construct the server to listen on the specified TCP address and port
      explicit Server(std::size_t        io_service_pool_size,
                      const std::string& application_id,
                      const std::string& application_instance,
                      bool               runAsDaemon          = true,
                      const std::string& config_root_path     = "",
                      const std::string& address              = "get_from_config",
                      const std::string& port                 = "get_from_config");

      ~Server()
      {
        removeLockFile();
      }

      void run();  // Run the server's io_service loop.
      void stop(); // stop the server.

      void register_handler(RequestHandlerPtr _handler);

      IoServicePool &getIoServicePool() { return io_service_pool_; };

    private:
      void start_accept();                                    // Initiate an asynchronous accept operation.
      void handle_accept(const boost::system::error_code& e); // Handle completion of an asynchronous accept operation.
      void handle_stop();                                     // Handle a request to stop the server.
      void handle_log_reopen();                               // Handle a request to reopen log.
      void initialize_standard_handlers();
      bool checkLockFile (const std::string &appid, const std::string& instance);
      bool createLockFile(const std::string &appid, const std::string& instance);
      void removeLockFile();
      void signalRegistrations();
      void initializeLogging(bool log2console);
      void becomeDaemonProcess();

      IoServicePool                  io_service_pool_;        // The pool of io_service objects used to perform asynchronous operations.
      boost::asio::signal_set        stop_signals_;           // The signal_set is used to register for process termination notifications.
      boost::asio::signal_set        log_reopen_signals_;     // The signal_set is used to register for process termination notifications.
      boost::asio::ip::tcp::acceptor acceptor_;               // Acceptor used to listen for incoming connections.
      ConnectionPtr                  new_connection_;         // The next connection to be accepted.
      RequestRouter                  request_router_;         // The handler for all incoming requests.
      bfs::path                      lockFilePath;

      // Standard Handlers
      RequestHandlerPtr              statsReporter;
      RequestHandlerPtr              errorReporter;
      RequestHandlerPtr              handlerReporter;
      RequestHandlerPtr              logLevelAdjuster;
  };
}

#endif
