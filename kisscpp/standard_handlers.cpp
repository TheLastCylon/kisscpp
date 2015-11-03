#include "standard_handlers.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  void StatsReporter::run(const BoostPtree &request, BoostPtree &response)
  {
    LogStream log(__PRETTY_FUNCTION__);

    try {

      std::string stat_type = request.get<std::string>("type","current");

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

      unsigned int    error_type_count  = 0;
      unsigned int    total_error_count = 0;
      SharedErrorList selm;

      selm = ErrorStateList::instance()->getStates();

      response.put("kcm-sts" , RQST_SUCCESS);

      for(ErrorListIterator itr = selm->begin(); itr != selm->end(); ++itr) {
        BoostPtree error;

        error.put("type"       ,(*itr)->getId());
        error.put("description",(*itr)->getDescription());
        error.put("count"      ,(*itr)->getSetCount());

        response.add_child("error-state.errors.error", error);

        total_error_count += (*itr)->getSetCount();

        error_type_count++;
      }

      response.put("error-state.error-type-count"  , error_type_count );
      response.put("error-state.total-error-count" , total_error_count);

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
  void ErrorCleaner::run(const BoostPtree &request, BoostPtree &response)
  {
    LogStream log(__PRETTY_FUNCTION__);

    try {
      std::string  error_id            = request.get<std::string>("kcm-clear-error.id");
      std::string  number_to_clear_str = request.get<std::string>("kcm-clear-error.count","all");
      unsigned int number_to_clear_num = 0;

      boost::algorithm::to_lower(error_id);
      boost::algorithm::to_lower(number_to_clear_str);

      if(number_to_clear_str == "all") {
        kisscpp::ErrorStateList::instance()->clear_all(error_id);
      } else {
        number_to_clear_num = request.get<unsigned int>("kcm-clear-error.count");
        kisscpp::ErrorStateList::instance()->clear(error_id, number_to_clear_num);
      }

      response.put("kcm-sts" , RQST_SUCCESS);

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

      if(newLogType == "debug" ||
         newLogType == "info"  ||
         newLogType == "error") {


        if(newLogSeverity == "low"    ||
           newLogSeverity == "normal" ||
           newLogSeverity == "high") {

          log.setMessageType(newLogType    ,true);
          log.setSeverity   (newLogSeverity,true);

          response.put("kcm-sts"               , RQST_SUCCESS);
          response.put("new-log-level.type"    , newLogType);
          response.put("new-log-level.severity", newLogSeverity);

        } else {
          response.put("kcm-sts", RQST_INVALID_PARAMETER);
          response.put("kcm-erm", "invalid log severity");
        }


      } else {
        response.put("kcm-sts", RQST_INVALID_PARAMETER);
        response.put("kcm-erm", "invalid log type");
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

