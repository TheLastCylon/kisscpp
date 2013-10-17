// File  : client.cpp
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

#include "client.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  client::client(BoostPtree &_request, BoostPtree &_response, int timeout /* = 10 */) :
    socket_         (io_service_),
    timeout_timer_  (io_service_, boost::posix_time::seconds(timeout))
  {
    timeout_timer_.async_wait(boost::bind(&client::handle_timeout, this, boost::asio::placeholders::error));

    response_ = _response;
    request_  = _request;
 
    LogStream            log     (-1, __PRETTY_FUNCTION__);
    tcp::resolver        resolver(io_service_);
    tcp::resolver::query query   (request_.get<std::string>("kcm-hst"), request_.get<std::string>("kcm-prt"));

    endpoint_iterator_ = resolver.resolve(query);

    boost::asio::async_connect(socket_,
                               endpoint_iterator_,
                               boost::bind(&client::handle_connect, this, boost::asio::placeholders::error));

    io_service_.run();
  }

  //--------------------------------------------------------------------------------
  void client::handle_connect(const boost::system::error_code& error)
  {
    LogStream log(-1, __PRETTY_FUNCTION__);
    if(!error) {

      boost::asio::streambuf request;
      std::ostream           request_stream(&request);

      std::stringstream ss;

      write_json(ss, request_, false);
      //write_json(request_stream, (*request_.get()), false);

      log << "Sending JSON request: " << ss.str() << endl;

      request_stream << ss.str();

      boost::asio::async_write(socket_,
                               request,
                               boost::bind(&client::handle_write, this, boost::asio::placeholders::error));
    } else {
      socket_.close();
      timeout_timer_.cancel();
      std::stringstream ss;
      ss << "boost::asio::placeholders::error [" << error << "]";
      throw PerminantCommsFailure(ss.str());
    }
  }

  //--------------------------------------------------------------------------------
  void client::handle_write(const boost::system::error_code& error)
  {
    LogStream log(-1, __PRETTY_FUNCTION__);
    if(!error) {
      boost::asio::async_read_until(socket_,
                                    incomming_stream_buffer_,
                                    '\n',
                                    boost::bind(&client::handle_read, this, boost::asio::placeholders::error));
    } else {
      socket_.close();
      timeout_timer_.cancel();
      std::stringstream ss;
      ss << "boost::asio::placeholders::error [" << error << "]";
      throw PerminantCommsFailure(ss.str());
    }
  }

  //--------------------------------------------------------------------------------
  void client::handle_read(const boost::system::error_code& error)
  {
    LogStream log(-1, __PRETTY_FUNCTION__);
    if(!error) {
      std::stringstream ss;
      std::string       ts;
      std::istream      raw_request_(&incomming_stream_buffer_);

      std::getline(raw_request_, ts, '\n');

      log << "Raw socket read : " << ts << endl;
      
      ss << ts;

      log << "Done reading from socket." << endl;

      read_json(ss, response_);

      unsigned int commsStatus = response_.get<unsigned int>("kcm-sts", RQST_UNKNOWN);

      if(commsStatus != RQST_SUCCESS) {
        socket_.close();
        timeout_timer_.cancel();
        std::string message = response_.get<std::string>("kcm-erm");
        switch(commsStatus) {
          case RQST_APPLICATION_BUSY        :
          case RQST_APPLICATION_SHUTING_DOWN: throw RetryableCommsFailure(message); break;
          case RQST_COMMAND_NOT_SUPPORTED   :
          case RQST_INVALID_PARAMETER       :
          case RQST_MISSING_PARAMETER       :
          case RQST_UNKNOWN                 :
          default                           : throw PerminantCommsFailure(message); break;
        }
      } else {
        timeout_timer_.cancel();
        log << "Excellent!" << endl;
      }

    } else {
      socket_.close();
      timeout_timer_.cancel();
      std::stringstream ss;
      ss << "boost::asio::placeholders::error [" << error << "]";
      throw PerminantCommsFailure(ss.str());
    }
  }

  //--------------------------------------------------------------------------------
  void client::handle_timeout(const boost::system::error_code& e)
  {
    kisscpp::LogStream log(-1, __PRETTY_FUNCTION__);

    if(e != boost::asio::error::operation_aborted) {
      socket_.cancel();
      socket_.close();
      throw RetryableCommsFailure("Message timed out.");
    }
  }
}

