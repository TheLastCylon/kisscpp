// File  : server.cpp
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

#include "server.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  Server::Server(const std::string& address,
                 const std::string& port,
                 std::size_t        io_service_pool_size)
     : io_service_pool_   (io_service_pool_size),
       stop_signals_      (io_service_pool_.get_io_service()),
       log_reopen_signals_(io_service_pool_.get_io_service()),
       acceptor_          (io_service_pool_.get_io_service()),
       new_connection_    (),
       request_router_    ()
   {
     LogStream log(__PRETTY_FUNCTION__);
     log << manip::info_normal << "Address : " << address << " Port : " << port << endl;

     // Register to handle the signals that indicate when the server should exit.
     // It is safe to register for the same signal multiple times in a program,
     // provided all registration for the specified signal is made through Asio.
     stop_signals_.add(SIGINT);
     stop_signals_.add(SIGTERM);

#if defined(SIGQUIT)
     stop_signals_.add(SIGQUIT);
#endif

     log_reopen_signals_.add(SIGHUP);

     stop_signals_.async_wait(boost::bind(&Server::handle_stop, this));
     log_reopen_signals_.async_wait(boost::bind(&Server::handle_log_reopen, this));

     // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
     boost::asio::ip::tcp::resolver        resolver(acceptor_.get_io_service());
     boost::asio::ip::tcp::resolver::query query(address, port);
     boost::asio::ip::tcp::endpoint        endpoint = *resolver.resolve(query);

     acceptor_.open(endpoint.protocol());
     acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
     acceptor_.bind(endpoint);
     acceptor_.listen();

     start_accept();
   }

  //--------------------------------------------------------------------------------
  void Server::run()
  {
    LogStream log(__PRETTY_FUNCTION__);
    io_service_pool_.run();
  }

  //--------------------------------------------------------------------------------
  void Server::stop()
  {
    LogStream log(__PRETTY_FUNCTION__);
    handle_stop();
  }

  //--------------------------------------------------------------------------------
  void Server::register_handler(RequestHandlerPtr _handler)
  {
    LogStream log(__PRETTY_FUNCTION__);
    request_router_.register_handler(_handler);
  }

  //--------------------------------------------------------------------------------
  void Server::start_accept()
  {
    LogStream log(__PRETTY_FUNCTION__);
    new_connection_.reset(new Connection(io_service_pool_.get_io_service(), request_router_));
    acceptor_.async_accept(new_connection_->socket(),
                           boost::bind(&Server::handle_accept,
                           this,
                           boost::asio::placeholders::error));
  }

  //--------------------------------------------------------------------------------
  void Server::handle_accept(const boost::system::error_code& e)
  {
    LogStream log(__PRETTY_FUNCTION__);
    if(!e) {
      new_connection_->start();
    }

    start_accept();
  }

  //--------------------------------------------------------------------------------
  void Server::handle_stop()
  {
    LogStream log(__PRETTY_FUNCTION__);
    io_service_pool_.stop();
  }

  //--------------------------------------------------------------------------------
  void Server::handle_log_reopen()
  {
    LogStream log(__PRETTY_FUNCTION__);
    log << manip::debug_normal << "Setting reopen log" << endl;
    log.set2ReOpen();
  }
}

