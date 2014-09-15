// File  : handler_echo.hpp
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

#ifndef _HANDLER_ECHO_HPP_
#define _HANDLER_ECHO_HPP_

#include <iostream>
#include <string>
#include <unistd.h>

#include <kisscpp/logstream.hpp>
#include <kisscpp/request_handler.hpp>
#include <kisscpp/boost_ptree.hpp>
#include <kisscpp/request_status.hpp>
#include <kisscpp/statskeeper.hpp>

class EchoHandler : public kisscpp::RequestHandler  /// Step 5. Derive your handler from kisscpp::RequestHandler
{
  public:
    EchoHandler() :
      kisscpp::RequestHandler("echo", "Will echo back what you send in.") /// "echo" is the unique string that is used to identify your handler.
                                                                          /// i.e. requests to your application, that have "kcm-cmd" set to "echo"
                                                                          /// will cause this handler's run() method to be executed.
    {
      kisscpp::LogStream log(__PRETTY_FUNCTION__);
    }

    ~EchoHandler() {};

    void run(const BoostPtree &request, BoostPtree &response); /// Step 6. You must override the run method from kisscpp::RequestHandler
  protected:
  private:
};

#endif

