// File  : kcsrt.hpp
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

#ifndef _KCSRT_HPP_
#define _KCSRT_HPP_

#include <sstream>
#include <boost/thread.hpp>
#include <kisscpp/server.hpp>
#include <kisscpp/client.hpp>
#include <kisscpp/logstream.hpp>
#include <kisscpp/logstream.hpp>
#include <kisscpp/statskeeper.hpp>
#include <kisscpp/configuration.hpp>
#include "handler_echo.hpp"

// ----------------------- TODO: -----------------------------
class kcsrt : public kisscpp::Server        /// Step 1: Derive your application's main class, from kisscpp::Server
{
  public:
    kcsrt(const std::string &instance,
          const std::string &out_port,
          const bool        &runAsDaemon);

    ~kcsrt();

  protected:
    void registerHandlers();                /// This isn't stricly needed, I just like to keep all my request-handler registreations in one place.
    void sendingProcessor();

  private:
    kisscpp::RequestHandlerPtr echoHandler; /// Step 2: you need atleast one request Handler.
    std::string                i_port;
    std::string                o_port;
    bool                       running;
    boost::thread_group        threadGroup;
    kisscpp::StatsKeeper      *stats;
    kisscpp::ErrorStateList   *errorStates;
};

#endif

