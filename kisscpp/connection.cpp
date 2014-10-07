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
    LogStream log(__PRETTY_FUNCTION__);
    //parsed_request_.reset(new BoostPtree());
  }

  //--------------------------------------------------------------------------------
  boost::asio::ip::tcp::socket& Connection::socket()
  {
    LogStream log(__PRETTY_FUNCTION__);
    return socket_;
  }

  //--------------------------------------------------------------------------------
  void Connection::start()
  {
    LogStream log(__PRETTY_FUNCTION__);

    using namespace boost::property_tree::json_parser;

    std::istream              raw_request_     (&incomming_stream_buffer_);
    std::ostream              encoded_response_(&outgoing_stream_buffer_);
    boost::system::error_code error_code;

    try {

      std::string       ts;
      std::stringstream ss;
      std::stringstream response;

      boost::asio::read_until(socket_, incomming_stream_buffer_, '\n');

      std::getline(raw_request_, ts, '\n');

      log << info_normal
          << "Recieved request from ["
          << socket_.remote_endpoint().address().to_string()
          << ":"
          << socket_.remote_endpoint().port()
          << "] > "
          << ts
          << endl;
      
      ss << ts;

      read_json(ss, parsed_request_);

      request_router_.route_request(parsed_request_, raw_response_);

      write_json(response, raw_response_, false);

      log << info_normal
          << "Sending response: "
          << response.str()
          << endl;

      encoded_response_ << response.str();

      boost::asio::write(socket_, outgoing_stream_buffer_, boost::asio::transfer_all());

    } catch(boost::property_tree::json_parser::json_parser_error &je) {
      log << manip::error_normal << "json parsing Error: " << je.message() << endl;
    } catch(boost::system::system_error &se) {
      log << manip::error_normal << "Boost System Error: " << se.what() << endl;
    }
  }
}

