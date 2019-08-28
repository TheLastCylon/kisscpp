// File  : request_handler.hpp
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

#ifndef _SERVER_REQUEST_HANDLER_HPP
#define _SERVER_REQUEST_HANDLER_HPP

#include <iostream>
#include <string>

#include <boost/noncopyable.hpp>

#include "boost_ptree.hpp"
#include "logstream.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  class RequestHandler : private boost::noncopyable
  {
    public:
      RequestHandler(const std::string &_id, const std::string &_description) : id(_id), description(_description)
      {
        LogStream log(__PRETTY_FUNCTION__);
      };

      virtual ~RequestHandler() {};

      virtual void run(const BoostPtree& request, BoostPtree& response) = 0;

      std::string commandId()      { return id; }
      std::string getDescription() { return description; }

    protected:
    private:
      std::string id;
      std::string description;
  };

  typedef std::shared_ptr<RequestHandler> RequestHandlerPtr;

}

#endif
