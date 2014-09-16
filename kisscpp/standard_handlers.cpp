#include "standard_handlers.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  void StatsReporter::run(const BoostPtree &request, BoostPtree &response)
  {
    LogStream log(__PRETTY_FUNCTION__);

    try {

      std::string stat_type = request.get<std::string>("type");

      response.put("kcm-sts" , RQST_SUCCESS);

      if      (stat_type == "full"    )   { full    (response); }
      else if (stat_type == "current" )   { current (response); }
      else  /*(stat_type == "gathered")*/ { gathered(response); }

    } catch (boost::property_tree::ptree_bad_path &e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_MISSING_PARAMETER);
      response.put("kcm-erm", e.what());

    } catch (std::exception& e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_UNKNOWN);                    
      response.put("kcm-erm", e.what());
    }
  }

  //--------------------------------------------------------------------------------
  void StatsReporter::gathered(BoostPtree &response)
  {
    SharedStatsMapType ssmt = StatsKeeper::instance()->getLastGatheredStats();

    for(StatsMapTypeIterator itr = ssmt->begin(); itr != ssmt->end(); ++itr) {
      response.put(itr->first, itr->second);
    }
  }

  //--------------------------------------------------------------------------------
  void StatsReporter::full    (BoostPtree &response)
  {
    SharedStatsHistoryMapType sshmt = StatsKeeper::instance()->getFullStatsHistory();

    for(StatsHistoryMapTypeIterator itr = sshmt->begin(); itr != sshmt->end(); ++itr) {
      for(unsigned int i = 0; i < (itr->second).size(); ++i) {
        response.put(itr->first, (itr->second)[i]);
      }
    }
  }

  //--------------------------------------------------------------------------------
  void StatsReporter::current (BoostPtree &response)
  {
    SharedStatsMapType ssmt = StatsKeeper::instance()->getCurrentStats();

    for(StatsMapTypeIterator itr = ssmt->begin(); itr != ssmt->end(); ++itr) {
      response.put(itr->first, itr->second);
    }
  }

  //--------------------------------------------------------------------------------
  void ErrorReporter::run(const BoostPtree &request, BoostPtree &response)
  {
    LogStream log(__PRETTY_FUNCTION__);

    try {

      SharedErrorListMap selm;

      selm = ErrorStateList::instance()->getStates();

      response.put("kcm-sts" , RQST_SUCCESS);

      for(ErrorListMapIterator itr = selm->begin(); itr != selm->end(); ++itr) {
        response.put(itr->first, itr->second);
      }

    } catch (boost::property_tree::ptree_bad_path &e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_MISSING_PARAMETER);
      response.put("kcm-erm", e.what());

    } catch (std::exception& e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_UNKNOWN);                    
      response.put("kcm-erm", e.what());
    }
  }

  //--------------------------------------------------------------------------------
  void HandlerReporter::run(const BoostPtree &request, BoostPtree &response)
  {
    LogStream log(__PRETTY_FUNCTION__);

    try {
      sharedRequestHandlerInfoList requestHandlerList = requestRouter.getHandlerDescriptions();

      response.put("kcm-sts" , RQST_SUCCESS);

      for(requestHandlerInfoListIter itr = requestHandlerList->begin(); itr != requestHandlerList->end(); ++itr) {
        BoostPtree handlerDetails;

        handlerDetails.put("id"         ,itr->first);
        handlerDetails.put("description",itr->second);
        response.add_child("handler"    ,handlerDetails); 
      }
    } catch (boost::property_tree::ptree_bad_path &e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_MISSING_PARAMETER);
      response.put("kcm-erm", e.what());

    } catch (std::exception& e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_UNKNOWN);                    
      response.put("kcm-erm", e.what());
    }
  }

  //--------------------------------------------------------------------------------
  void LogLevelAdjuster::run(const BoostPtree &request, BoostPtree &response)
  {
    LogStream log(__PRETTY_FUNCTION__);

    try {
      bool        validLogType     = false;
      bool        validLogSeverity = false;
      std::string newLogType       = request.get<std::string>("type");
      std::string newLogSeverity   = request.get<std::string>("severity");

      boost::algorithm::to_lower(newLogType);
      boost::algorithm::to_lower(newLogSeverity);

      if     (newLogType     == "debug" ) { log.setMessageType(LT_DEBUG ,true); validLogType = true; }
      else if(newLogType     == "info"  ) { log.setMessageType(LT_INFO  ,true); validLogType = true; }
      else if(newLogType     == "error" ) { log.setMessageType(LT_ERROR ,true); validLogType = true; }
      else {
        response.put("kcm-sts", RQST_INVALID_PARAMETER);
        response.put("kcm-erm", "invalid log type");
      }

      if(validLogType) {
        if     (newLogSeverity == "low"   ) { log.setSeverity(LS_LOW   ,true); validLogSeverity = true; }
        else if(newLogSeverity == "normal") { log.setSeverity(LS_NORMAL,true); validLogSeverity = true; }
        else if(newLogSeverity == "high"  ) { log.setSeverity(LS_HIGH  ,true); validLogSeverity = true; }
        else {
          response.put("kcm-sts", RQST_INVALID_PARAMETER);
          response.put("kcm-erm", "invalid log severity");
        }
      }

      if(validLogType && validLogSeverity) {
        response.put("kcm-sts" , RQST_SUCCESS);
      }

    } catch (boost::property_tree::ptree_bad_path &e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_MISSING_PARAMETER);
      response.put("kcm-erm", e.what());

    } catch (std::exception& e) {

      log << "Exception: " << e.what() << manip::endl;
      response.put("kcm-sts", RQST_UNKNOWN);                    
      response.put("kcm-erm", e.what());
    }
  }
}

