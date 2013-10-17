// File  : request_router.hpp
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

#ifndef _SERVER_REQUEST_ROUTER_HPP
#define _SERVER_REQUEST_ROUTER_HPP

#include <iostream>

#include <string>
#include <map>
#include <boost/noncopyable.hpp>
#include "boost_ptree.hpp"
#include "request_handler.hpp"
#include "logstream.hpp"
#include "request_status.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  // The router for all incoming requests.
  class RequestRouter : private boost::noncopyable
  {
    public:
      explicit RequestRouter() {};                                           

      //--------------------------------------------------------------------------------
      void register_handler(RequestHandlerPtr _handler)
      {
        LogStream log(-1, __PRETTY_FUNCTION__);
        requestHandlerMap[_handler->commandId()] = _handler;
      }

      // Handle a request and produce a reply.
      //--------------------------------------------------------------------------------
      //SharedPtree route_request(const SharedPtree request)
      void route_request(const BoostPtree &request, BoostPtree &response)
      {
        LogStream   log(-1, __PRETTY_FUNCTION__);
        try {
          std::string command = request.get<std::string>("kcm-cmd");

          if(requestHandlerMap.find(command) != requestHandlerMap.end()) {
            try {
              requestHandlerMap[command]->run(request, response);
            } catch (boost::property_tree::ptree_bad_path &e) {
              response.put("kcm-sts", RQST_MISSING_PARAMETER);
              response.put("kcm-erm", e.what());
            }
          } else {
            response.put("kcm-sts", RQST_COMMAND_NOT_SUPPORTED);
            response.put("kcm-erm","Unrecognized command: " + command);
          }
        } catch (boost::property_tree::ptree_bad_path &e) {
          response.put("kcm-sts", RQST_MISSING_PARAMETER);
          response.put("kcm-erm", e.what());
        }
      }

    private:
      std::map<std::string, RequestHandlerPtr> requestHandlerMap;
  };
}

#endif
