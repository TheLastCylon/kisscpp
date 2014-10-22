// File  : statskeeper.hpp
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

#ifndef _STATSKEEPER_HPP_
#define _STATSKEEPER_HPP_

// THIS HAS TO BE A SINGLETON!!!!

#include <string>
#include <map>
#include <ctime>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>
#include "boost_ptree.hpp"
#include "logstream.hpp"
#include "statable_queue.hpp"

namespace kisscpp
{
  typedef std::map<std::string, std::deque<double> > GatheredStatsMapType;
  typedef GatheredStatsMapType::iterator             GatheredStatsMapTypeIterator;

  typedef std::map<std::string, double             > StatsMapType;
  typedef StatsMapType::iterator                     StatsMapTypeIterator;
  typedef boost::shared_ptr<StatsMapType>            SharedStatsMapType;

  typedef std::vector<double>                        StatsHistoryType;
  typedef std::map<std::string, StatsHistoryType>    StatsHistoryMapType;
  typedef StatsHistoryMapType::iterator              StatsHistoryMapTypeIterator;
  typedef boost::shared_ptr<StatsHistoryMapType>     SharedStatsHistoryMapType;

  typedef std::map<std::string, sharedStatAbleQ    > QueueStatsMapType;
  typedef QueueStatsMapType::iterator                QueueStatsMapTypeIterator;


  // The StasKeeper class is impelemented as a singleton.
  // For those of you with the opinion that singleton is an anti-patern, I have this to say:
  // Just because you don't know when a technique is valid, doesn't mean it's always invalid.
  // There is no such thing as an Anti-patern. There is however, such a thing as applying a
  // patern inapropriately. It is not the patern that is at fault. The developer and everyone
  // allowing that developer to do it, are the ones that need fixing. So, go back to your
  // text on the subject, and revise the material untill you get it. 
  class StatsKeeper
  {
    public:
      static StatsKeeper* instance(unsigned long int gp = 300,  // gp - Gather Period as a number of seconds (defaults to 5minutes)
                                   unsigned long int hl = 12);  // hl - history length, i.e. Number of historic gathered periods to keep. (default to 12 periods so that we have an hours worth of history.)
      void                start   ();
      void                stop    ();

      ~StatsKeeper()
      {
        kisscpp::LogStream log(__PRETTY_FUNCTION__);
        stop();
      };

      void                      setStatValue    (std::string id, double          value = 0);
      void                      increment       (std::string id, double          value = 1);
      void                      decrement       (std::string id, double          value = 1);
      void                      addStatableQueue(std::string id, sharedStatAbleQ ssq);

      SharedStatsMapType        getCurrentStats();
      SharedStatsMapType        getLastGatheredStats();
      SharedStatsHistoryMapType getFullStatsHistory();

    protected:
    private:
      StatsKeeper           ()                  { kisscpp::LogStream log(__PRETTY_FUNCTION__); }  // Private to prevent copying.
      StatsKeeper           (StatsKeeper const&){ kisscpp::LogStream log(__PRETTY_FUNCTION__); }; // Private to prevent copying.
      StatsKeeper& operator=(StatsKeeper const&){ kisscpp::LogStream log(__PRETTY_FUNCTION__); }; // Private to prevent assignment.

      StatsKeeper(unsigned long int gp,
                  unsigned long int hl) :
        gatherPeriod (gp),
        historyLength(hl),
        running(false)
      {
        kisscpp::LogStream log(__PRETTY_FUNCTION__);
        start();
      }

      void       gatherStats();

      static StatsKeeper   *singleton_instance;
      unsigned long int     gatherPeriod;
      unsigned long int     historyLength;
      bool                  running;
      boost::mutex          statMutex;
      boost::thread_group   threadGroup;
      GatheredStatsMapType  gatheredStatsMap;
      StatsMapType          statsMap;
      time_t                startTime;
      QueueStatsMapType     queueStatsMap;
  };
}

#endif // _STATSKEEPER_HPP_
