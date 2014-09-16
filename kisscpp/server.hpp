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

#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "io_service_pool.hpp"
#include "connection.hpp"
#include "request_router.hpp"
#include "logstream.hpp"
#include "statskeeper.hpp"
#include "errorstate.hpp"
#include "standard_handlers.hpp"

namespace kisscpp
{
  class Server : private boost::noncopyable // The top-level class of the server.
  {
    public:
      // Construct the server to listen on the specified TCP address and port
      explicit Server(const std::string& address,
                      const std::string& port,
                      std::size_t        io_service_pool_size,
                      unsigned long int  gp = 300,  // statistics Gather Period as a number of seconds (defaults to 5minutes)
                      unsigned long int  hl = 12);  // number of historical gather periods to keep

      void run();  // Run the server's io_service loop.
      void stop(); // stop the server.

      void register_handler(RequestHandlerPtr _handler);

    private:
      void start_accept();                                    // Initiate an asynchronous accept operation.
      void handle_accept(const boost::system::error_code& e); // Handle completion of an asynchronous accept operation.
      void handle_stop();                                     // Handle a request to stop the server.
      void handle_log_reopen();                               // Handle a request to reopen log.
      void initialize_standard_handlers();

      IoServicePool                  io_service_pool_;        // The pool of io_service objects used to perform asynchronous operations.
      boost::asio::signal_set        stop_signals_;           // The signal_set is used to register for process termination notifications.
      boost::asio::signal_set        log_reopen_signals_;     // The signal_set is used to register for process termination notifications.
      boost::asio::ip::tcp::acceptor acceptor_;               // Acceptor used to listen for incoming connections.
      ConnectionPtr                  new_connection_;         // The next connection to be accepted.
      RequestRouter                  request_router_;         // The handler for all incoming requests.

      // Standard Handlers
      RequestHandlerPtr              statsReporter;
      RequestHandlerPtr              errorReporter;
      RequestHandlerPtr              handlerReporter;
  };
}

#endif
