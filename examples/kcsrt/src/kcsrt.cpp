// File  : kcsrt.cpp
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

#include "kcsrt.hpp"

//--------------------------------------------------------------------------------
kcsrt::kcsrt(const std::string &in_port,
             const std::string &out_port) :
  Server ("localhost", in_port, 1)
{
  i_port  = in_port;
  o_port  = out_port;
  running = true;

  stats       = kisscpp::StatsKeeper::instance();
  errorStates = kisscpp::ErrorStateList::instance();

  stats->setStatValue("recieved",0);
  stats->setStatValue("sent"    ,0);
  stats->setStatValue("success" ,0);
  stats->setStatValue("fail"    ,0);

  std::string logfilePath = "/tmp/kcsrt_";
  logfilePath += in_port;
  logfilePath += ".log";

  kisscpp::LogStream log(__PRETTY_FUNCTION__, logfilePath, false, 10000);

  registerHandlers();
  threadGroup.create_thread(boost::bind(&kcsrt::sendingProcessor, this));
  log << "APP START" << kisscpp::manip::flush;
}

//--------------------------------------------------------------------------------
kcsrt::~kcsrt()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  stop();

  running = false;
  stop();
  threadGroup.join_all();
  log << "APP END" << kisscpp::manip::flush;
}

//--------------------------------------------------------------------------------
void kcsrt::registerHandlers()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

  echoHandler.reset(new EchoHandler()); /// Step 3. Instantiate your handler.
  register_handler(echoHandler);        /// Step 4. Register your handler.
}

//--------------------------------------------------------------------------------
void kcsrt::sendingProcessor()
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);
  unsigned int       messageCount = 0;
  unsigned int       failCount    = 0;

  log << kisscpp::manip::info_high
      << i_port
      << "->"
      << o_port
      << " : START"
      << kisscpp::manip::flush;

  while(running) {
    messageCount++;

    log << kisscpp::manip::debug_normal << i_port << "->" << o_port << " : Sending          : " << messageCount << kisscpp::manip::endl;

    try {

      BoostPtree request;
      BoostPtree response;

      request.put("kcm-cmd", "echo");
      request.put("kcm-hst", "localhost");
      request.put("kcm-prt", o_port);

      std::stringstream msg;

      msg << "Message Count from ["
          << i_port
          << "] is: "
          << messageCount;

      request.put("message", msg.str());

      kisscpp::client requestSender(request, response, 5); // Instantiation of the kisscpp::client class, sends the message.

      log << kisscpp::manip::debug_normal << i_port << "->" << o_port << " : Sent             : " << messageCount << kisscpp::manip::endl;

      stats->increment("sent");

    } catch(kisscpp::RetryableCommsFailure &e) {
      failCount++;
      log << kisscpp::manip::error_normal << i_port << "->" << o_port << " : Fail - Retryable : " << messageCount << kisscpp::manip::endl;
      stats->increment("fail");
    } catch(kisscpp::PerminantCommsFailure &e) {
      failCount++;
      stats->increment("fail");
      log << kisscpp::manip::error_normal << i_port << "->" << o_port << " : Fail - Perminant : " << messageCount << kisscpp::manip::endl;
    }

    sleep(0);
  }

  log << i_port
      << "->"
      << o_port
      << " : Sent: "
      << messageCount
      << " : Fail: "
      << failCount
      << kisscpp::manip::endl;

  log << kisscpp::manip::info_high
      << i_port
      << "->"
      << o_port
      << " : END"
      << kisscpp::manip::flush;
}
