// File  : client.hpp
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

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost_ptree.hpp"
#include "logstream.hpp"
#include "request_status.hpp"
#include "configuration.hpp"

using boost::asio::ip::tcp;

namespace kisscpp
{
  class client
  {
    public:
      client(BoostPtree &_request, BoostPtree &_response, int timeout = 10);

      ~client() { socket_.close(); };

    private:
      void handle_connect (const boost::system::error_code& error);
      void handle_write   (const boost::system::error_code& error);
      void handle_read    (const boost::system::error_code& error);
      void handle_timeout (const boost::system::error_code& error);

    private:
      boost::asio::io_service     io_service_;
      tcp::socket                 socket_;
      tcp::resolver::iterator     endpoint_iterator_;
      char                       *ackMessage_;
      BoostPtree                  request_;
      BoostPtree                  response_;
      boost::asio::deadline_timer timeout_timer_;
      boost::asio::streambuf      incomming_stream_buffer_;
  };
}

typedef boost::shared_ptr<kisscpp::client> SharedClient;

#endif //_CLIENT_CLIENT_HPP_

