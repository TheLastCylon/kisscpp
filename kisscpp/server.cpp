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
                 std::size_t        io_service_pool_size,
                 const std::string& application_id       /*= "kisscpp_application"*/,
                 const std::string& application_instance /*= "0"*/,
                 const std::string& config_root_path     /*= ""*/,
                 unsigned long int  gp                   /*= 300*/,
                 unsigned long int  hl                   /*= 12*/)
    : io_service_pool_   (io_service_pool_size),
      stop_signals_      (io_service_pool_.get_io_service()),
      log_reopen_signals_(io_service_pool_.get_io_service()),
      acceptor_          (io_service_pool_.get_io_service()),
      new_connection_    (),
      request_router_    ()
  {
    if (checkLockFile(application_id, application_instance) {
      createLockFile();

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

      Config::instance(application_id, application_instance, config_root_path);

      StatsKeeper::instance(gp,hl); // create the stats keeper instance here.
                                    // so that it's available as soon as the server is
                                    // constructed.

      ErrorStateList::instance();   // same goes for the error state list.

      initialize_standard_handlers();

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

  //--------------------------------------------------------------------------------
  void Server::initialize_standard_handlers()
  {
    statsReporter.reset(new StatsReporter());
    errorReporter.reset(new ErrorReporter());
    handlerReporter.reset(new HandlerReporter(request_router_));
    logLevelAdjuster.reset(new LogLevelAdjuster());

    register_handler(statsReporter);
    register_handler(errorReporter);
    register_handler(handlerReporter);
    register_handler(logLevelAdjuster);
  }

  //--------------------------------------------------------------------------------
  bool Server::checkLockFile(const std::string &appid, const std::string& instance)
  {
    bool         retval        = false;
    std::string  lockFileName  = appid + "." + instance + ".lock";
    char        *kcpp_lock_dir = std::getenv("KCPP_LOCK_DIR");

    if(kcpp_lock_dir) {
      lockFilePath = std::string(kcpp_lock_dir);
    } else {
      lockFilePath = "/var/run";
    }

    lockFilePath += "/" + lockFileName;

    if(!bfs::exists(lockFilePath)) {
      retval = true;
    } else {
      std::cerr << "Lockfile for this appid and instance already exists ["
                << lockFilePath.native()
                << "] stop the executing process before starting it again or remove the lock file if there is no process executing with this applicaiton and instnace id combination."
                << std::endl();
    }

    return retval;
  }

  //--------------------------------------------------------------------------------
  void Server::createLockFile()
  {
    std::ofstream lockFile;
    lockFile.open(lockFilePath.c_str(), std::ios::out);
    lockFile << std::time(NULL);
    lockFile.close();
  }

  //--------------------------------------------------------------------------------
  void Server::removeLockFile()
  {
    if(bfs::exists(file)) {
      bfs::filesystem::remove(file);
    }
  }
}

