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
  Server::Server(std::size_t        io_service_pool_size,
                 const std::string& application_id,
                 const std::string& application_instance,
                 bool               runAsDaemon          /*= true*/,
                 const std::string& config_root_path     /*= ""*/,
                 const std::string& address              /*= "get_from_config"*/,
                 const std::string& port                 /*= "get_from_config"*/)
    : io_service_pool_   (io_service_pool_size),
      stop_signals_      (io_service_pool_.get_io_service()),
      log_reopen_signals_(io_service_pool_.get_io_service()),
      acceptor_          (io_service_pool_.get_io_service()),
      new_connection_    (),
      request_router_    ()
  {
    if (createLockFile(application_id, application_instance)) {

      if(runAsDaemon) {
        becomeDaemonProcess();
      }

      signalRegistrations();

      Config::instance(application_id, application_instance, config_root_path);

      initializeLogging((!runAsDaemon));

      // create the stats keeper instance here. So that it's available as soon as the server is constructed.
      StatsKeeper::instance(Config::instance()->get<unsigned long int>("kcc-stats.gather-period" ,300),
                            Config::instance()->get<unsigned long int>("kcc-stats.history-length",12));

      ErrorStateList::instance();   // same goes for the error state list.

      initialize_standard_handlers();

      // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
      boost::asio::ip::tcp::resolver        resolver(acceptor_.get_io_service());
      //boost::asio::ip::tcp::resolver::query query(address, port);

      boost::asio::ip::tcp::resolver::query query(Config::instance()->get<std::string>("kcc-server.address"),
                                                  Config::instance()->get<std::string>("kcc-server.port"));

      boost::asio::ip::tcp::endpoint        endpoint = *resolver.resolve(query);

      acceptor_.open(endpoint.protocol());
      acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
      acceptor_.bind(endpoint);
      acceptor_.listen();

      start_accept();
    } else {
      std::cerr << "Could not create Lockfile for this appid and instance: ["
                << lockFilePath.native()
                << "]"
                << std::endl
                << "If this message is not preceded by a message indicating that a lock file already exists, "
                << "you most likely have a permissions problem, or the directory in which you wish to create lock files, "
                << "does not exist."
                << std::endl;

      throw std::runtime_error("Could not create lock file: Terminating.");
    }
  }

  //--------------------------------------------------------------------------------
  void Server::run()
  {
    LogStream log(__PRETTY_FUNCTION__);

    log << manip::info_normal
        << "--------------------------------------------------------------------------------"  << manip::endl
        << "Starting Process : " << Config::instance()->getAppId()                             << manip::endl
        << "Instance         : " << Config::instance()->getAppInstance()                       << manip::endl
        << "Server address   : " << Config::instance()->get<std::string>("kcc-server.address") << manip::endl
        << "Server port      : " << Config::instance()->get<std::string>("kcc-server.port")    << manip::endl
        << "--------------------------------------------------------------------------------"  << manip::flush;

    io_service_pool_.run();
  }

  //--------------------------------------------------------------------------------
  void Server::stop()
  {
    LogStream log(__PRETTY_FUNCTION__);
    handle_stop();

    log << manip::info_normal
        << "--------------------------------------------------------------------------------"  << manip::endl
        << "Stopping Process : " << Config::instance()->getAppId()                             << manip::endl
        << "Instance         : " << Config::instance()->getAppInstance()                       << manip::endl
        << "Server address   : " << Config::instance()->get<std::string>("kcc-server.address") << manip::endl
        << "Server port      : " << Config::instance()->get<std::string>("kcc-server.port")    << manip::endl
        << "--------------------------------------------------------------------------------"  << manip::flush;
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
    log << manip::debug_normal << "Setting reopen log" << manip::endl;
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
                << "]"
                << std::endl
                << "If there is no process executing with this applicaiton id and instnace combination, you will have to remove the lock file manually."
                << std::endl;
    }

    return retval;
  }

  //--------------------------------------------------------------------------------
  bool Server::createLockFile(const std::string &appid, const std::string& instance)
  {
    bool retval = false;

    if (checkLockFile(appid, instance)) {
      std::ofstream lockFile;
      lockFile.open(lockFilePath.c_str(), std::ios::out);

      if(!lockFile.fail()) {
        lockFile << std::time(NULL) << std::endl;
        lockFile.close();
        retval = true;
      }
    }

    return retval;
  }

  //--------------------------------------------------------------------------------
  void Server::removeLockFile()
  {
    if(bfs::exists(lockFilePath)) {
      bfs::remove(lockFilePath);
    }
  }

  //--------------------------------------------------------------------------------
  void Server::signalRegistrations()
  {
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

  }

  //--------------------------------------------------------------------------------
  void Server::initializeLogging(bool log2console)
  {
    std::string  logFileRoot   = "/tmp";
    std::string  logFileName   = Config::instance()->getAppId()       + "." +
                                 Config::instance()->getAppInstance() + ".log";

    std::string  logType       = Config::instance()->get<std::string> ("kcc-log-level.type"     ,"info");
    std::string  logSeverity   = Config::instance()->get<std::string> ("kcc-log-level.severity" ,"low");
    unsigned int maxLinesBuff  = Config::instance()->get<unsigned int>("kcc-log-level.buff-size",5000);

    char        *kcpp_log_root = std::getenv("KCPP_LOG_ROOT");
    char        *kcpp_exec_env = std::getenv("KCPP_EXEC_ENV");
    bfs::path    logFileRootPath;
    bfs::path    logFilePath;

    if(kcpp_log_root) {
      logFileRoot = std::string(kcpp_log_root);
    }

    if(kcpp_exec_env) {
      logFileRoot += "/" + std::string(kcpp_exec_env);
    }
    
    logFileRoot    += "/" + Config::instance()->getAppId();

    logFileRootPath = logFileRoot;

    bfs::create_directories(logFileRootPath);

    logFilePath  = logFileRoot + "/" + logFileName;

    kisscpp::LogStream log(__PRETTY_FUNCTION__,
                           logFilePath.native(),
                           log2console,
                           maxLinesBuff);

    log.setMessageType(logType    , true);
    log.setSeverity   (logSeverity, true);
  }

  //--------------------------------------------------------------------------------
  void Server::becomeDaemonProcess()
  {
    if(getpid() == 1) return; // If the process is already running as a Daemon, do nothing.

    pid_t pid, sid;
        
    pid = fork();

    if(pid < 0) {
      throw std::runtime_error("Could not fork child process to become a daemon.");
    } else if(pid > 0) {
      _exit(EXIT_SUCCESS);
    }

    umask(0);
                
    sid = setsid();
    if (sid < 0) {
      throw std::runtime_error("Could not obtain valid session id for this child process.");
    }
        
    if ((chdir("/")) < 0) {
      throw std::runtime_error("Could not change working directory to save, guarenteed root path.");
    }
        
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
  }
}

