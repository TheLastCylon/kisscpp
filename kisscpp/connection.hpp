// File  : connection.hpp
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

#ifndef _SERVER_CONNECTION_HPP
#define _SERVER_CONNECTION_HPP

#define BOOST_SPIRIT_THREADSAFE

#include <iostream>
#include <sstream>
#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/basic_streambuf.hpp>

#include "boost_ptree.hpp"
#include "request_router.hpp"
#include "logstream.hpp"
#include "configuration.hpp"

namespace kisscpp
{
  // Represents a single connection from a client.
  class Connection : public  boost::enable_shared_from_this<Connection>,
                     private boost::noncopyable
  {
    public:
      explicit Connection(boost::asio::io_service& io_service, RequestRouter& handler); // Construct a connection with the given io_service.

      ~Connection() { socket_.close(); };

      boost::asio::ip::tcp::socket& socket(); // Get the socket associated with the connection.

      void start();

    private:
      bool allowedIpAddress(const std::string &ip_address);
      bool allowedClient   ();

      boost::asio::ip::tcp::socket socket_;
      RequestRouter               &request_router_;
      boost::asio::streambuf       incomming_stream_buffer_;
      boost::asio::streambuf       outgoing_stream_buffer_;
      BoostPtree                   parsed_request_;
      BoostPtree                   raw_response_;
  };

  typedef boost::shared_ptr<Connection> ConnectionPtr;
}

#endif
