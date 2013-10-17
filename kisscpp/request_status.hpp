// File  : request_status.hpp
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

#ifndef __REQUEST_STATUS_HPP__
#define __REQUEST_STATUS_HPP__

#include <stdexcept>
#include <string>

namespace kisscpp
{
  //! \enum
  //! The RequestStatus enumeration is used by servers in responses to clients,
  //! to indicate various states/statuses with regard to processing the reqest.
  //! The numerical value of "kcm-sts" is obtained from this enumeration.

  enum RequestStatus {
    RQST_SUCCESS,                       //!< The request was successfully processed.
    RQST_COMMAND_NOT_SUPPORTED,         //!< No matching identifier for the supplied value of kcm-cmd was found
    RQST_INVALID_PARAMETER,             //!< used to indicate invalid values for supplied parameters
    RQST_MISSING_PARAMETER,             //!< used to indicate the absence of an expected parameter
    RQST_APPLICATION_BUSY,              //!< used by server applications to inform requesters that a request won't be processed due to server overload.
    RQST_APPLICATION_SHUTING_DOWN,      //!< used by server applications to inform requesters that a request won't be processed due to the server shutting down.
    RQST_UNKNOWN                        //!< Sorry, I don't know what went wrong.
  };

  //--------------------------------------------------------------------------------
  class RetryableCommsFailure : public std::runtime_error
  {
    public:
      RetryableCommsFailure(std::string s) : std::runtime_error(s) {};
  };

  //--------------------------------------------------------------------------------
  class PerminantCommsFailure : public std::runtime_error
  {
    public:
      PerminantCommsFailure(std::string s) : std::runtime_error(s) {};
  };
}

#endif // __COMMUNICATION_ERRORS_HPP__

