#include "handler_echo.hpp"

void EchoHandler::run(const BoostPtree &request, BoostPtree &response)
{
  kisscpp::LogStream log(__PRETTY_FUNCTION__);

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

