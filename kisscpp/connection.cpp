// File  : connection.cpp
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

#include "connection.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  Connection::Connection(boost::asio::io_service& io_service, RequestRouter& handler) : socket_(io_service), request_router_(handler)
  {
    LogStream log(-1, __PRETTY_FUNCTION__);
    //parsed_request_.reset(new BoostPtree());
  }

  //--------------------------------------------------------------------------------
  boost::asio::ip::tcp::socket& Connection::socket()
  {
    LogStream log(-1, __PRETTY_FUNCTION__);
    return socket_;
  }

  //--------------------------------------------------------------------------------
  void Connection::start()
  {
    LogStream log(-1, __PRETTY_FUNCTION__);

    using namespace boost::property_tree::json_parser;

    std::istream              raw_request_     (&incomming_stream_buffer_);
    std::ostream              encoded_response_(&outgoing_stream_buffer_);
    boost::system::error_code error_code;

    try {
      log << "About to read from socket." << endl;

      boost::asio::read_until(socket_, incomming_stream_buffer_, '\n');

      std::string ts;

      std::getline(raw_request_, ts, '\n');

      log << "Raw socket read : " << ts << endl;
      
      std::stringstream ss;
      ss << ts;

      log << "Done reading from socket." << endl;

      read_json(ss, parsed_request_);
      //  read_json(raw_request_, (*parsed_request_.get()));

      log << "JSON string read. Routing." << endl;

      request_router_.route_request(parsed_request_, raw_response_);

      std::stringstream response;

      write_json(response, raw_response_, false);

      log << "About to send response: " << response.str() << endl;

      encoded_response_ << response.str();

      //write_json(encoded_response_, (*raw_response_.get()), false);

      boost::asio::write(socket_, outgoing_stream_buffer_, boost::asio::transfer_all());

    } catch(boost::property_tree::json_parser::json_parser_error &je) {
      log << "json parsing Error: " << je.message() << endl;
    } catch(boost::system::system_error &se) {
      log << "Boost System Error: " << se.what() << endl;
    }
  }
}

