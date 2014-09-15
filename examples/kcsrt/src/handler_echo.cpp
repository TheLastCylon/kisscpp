// File  : handler_echo.cpp
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

#include "handler_echo.hpp"

void EchoHandler::run(const BoostPtree &request, BoostPtree &response)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  kisscpp::StatsKeeper::instance()->increment("recieved");

  try {

    // This is where you build your response
    response.put("kcm-sts" , kisscpp::RQST_SUCCESS);                // Make sure to set kcm-sts to RQST_SUCCESS in the event of success
    response.put("you-sent", request.get<std::string>("message"));  // populate any aditional response parameters.

  } catch (boost::property_tree::ptree_bad_path &e) {

    log << "Exception: " << e.what() << kisscpp::manip::endl;
    response.put("kcm-sts", kisscpp::RQST_MISSING_PARAMETER); // Make sure to set "kcm-sts" to the appropriate error status.
    response.put("kcm-erm", e.what());

  } catch (std::exception& e) {

    log << "Exception: " << e.what() << kisscpp::manip::endl;
    response.put("kcm-sts", kisscpp::RQST_UNKNOWN);                    
    response.put("kcm-erm", e.what());

  }
}

