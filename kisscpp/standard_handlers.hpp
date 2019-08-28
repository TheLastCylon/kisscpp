#ifndef _STANDARD_HANDLERS_HPP_
#define _STANDARD_HANDLERS_HPP_

#include <iostream>
#include <string>
#include <unistd.h>
#include <memory>
#include <boost/algorithm/string.hpp>    

#include "logstream.hpp"
#include "request_handler.hpp"
#include "request_router.hpp"
#include "boost_ptree.hpp"
#include "request_status.hpp"
#include "statskeeper.hpp"
#include "errorstate.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  class StatsReporter : public RequestHandler
  {
    public:
      StatsReporter() :
        RequestHandler("kch-stats", "retrieves the application statistics")
      {
        LogStream log(__PRETTY_FUNCTION__);
      }

      ~StatsReporter() {};

      void run(const BoostPtree &request, BoostPtree &response);
    protected:
    private:
      void gathered(BoostPtree &response);
      void full    (BoostPtree &response);
      void current (BoostPtree &response);
  };

  //--------------------------------------------------------------------------------
  class ErrorReporter : public RequestHandler
  {
    public:
      ErrorReporter() :
        RequestHandler("kch-errstat", "retrieves the application error states")
      {
        LogStream log(__PRETTY_FUNCTION__);
      }

      ~ErrorReporter() {};

      void run(const BoostPtree &request, BoostPtree &response);
    protected:
    private:
  };

  //--------------------------------------------------------------------------------
  class ErrorCleaner : public RequestHandler
  {
    public:
      ErrorCleaner() :
        RequestHandler("kch-errclear", "Used to marks an application error state as cleared.")
      {
        LogStream log(__PRETTY_FUNCTION__);
      }

      ~ErrorCleaner() {};

      void run(const BoostPtree &request, BoostPtree &response);
    protected:
    private:
  };

  //--------------------------------------------------------------------------------
  class HandlerReporter : public RequestHandler
  {
    public:
      explicit HandlerReporter(RequestRouter &rr) :
        RequestHandler("kch-handlers", "shows a list of the request handlers for this application"),
        requestRouter(rr)
      {
        LogStream log(__PRETTY_FUNCTION__);
      }

      ~HandlerReporter() {};

      void run(const BoostPtree &request, BoostPtree &response);
    protected:
    private:
      RequestRouter &requestRouter;
  };

  //--------------------------------------------------------------------------------
  class LogLevelAdjuster : public RequestHandler
  {
    public:
      LogLevelAdjuster() :
        RequestHandler("kch-loglevel", "Adjust the log level to specified levels")
      {
        LogStream log(__PRETTY_FUNCTION__);
      }

      ~LogLevelAdjuster() {};

      void run(const BoostPtree &request, BoostPtree &response);
    protected:
    private:
  };
}

#endif // _STANDARD_HANDLERS_HPP_

