// File  : statskeeper.cpp
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

#include "statskeeper.hpp"

namespace kisscpp
{
  StatsKeeper                          * StatsKeeper::singleton_instance;

  //--------------------------------------------------------------------------------
  StatsKeeper* StatsKeeper::instance(unsigned long int gp /* = 300 */,
                                     unsigned long int hl /* = 10 */)
  {
    if (!singleton_instance) { // Only allow one instance of class to be generated.
      singleton_instance = new StatsKeeper(gp,hl);
    }

    return singleton_instance;
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::start()
  {
    if(!running) {
      startTime = time(NULL);
      running   = true;
      threadGroup.create_thread(boost::bind(&StatsKeeper::gatherStats, this));
    }
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::stop()
  {
    running = false;
    threadGroup.join_all();
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::setStatValue(std::string id, double value /* = 0*/)
  {
    boost::lock_guard<boost::mutex> guard(statMutex);
    statsMap[id] = value;
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::increment(std::string id, double value /* = 1 */)
  {
    boost::lock_guard<boost::mutex> guard(statMutex);
    if(statsMap.find(id) != statsMap.end()) {
      statsMap[id] += value;
    } else {
      statsMap[id]  = value;
    }
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::decrement(std::string id, double value /* = 1 */)
  {
    boost::lock_guard<boost::mutex> guard(statMutex);
    if(statsMap.find(id) != statsMap.end()) {
      statsMap[id] -= value;
    } else {
      statsMap[id]  = 0 - value;
    }
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::addStatableQueue(std::string id, sharedStatAbleQ ssq)
  {
    queueStatsMap[id] = ssq;
  }

  //--------------------------------------------------------------------------------
  SharedStatsMapType StatsKeeper::getCurrentStats()
  {
    boost::lock_guard<boost::mutex> guard(statMutex);
    SharedStatsMapType              retval;

    retval.reset(new StatsMapType());

    statsMap["kcs-uptime"] = time(NULL) - startTime;

    for(StatsMapTypeIterator         itr = statsMap.begin()        ; itr != statsMap.end()        ; ++itr) {
      std::string tpath = "stats.";
      (*retval)[tpath + itr->first] = itr->second;
    }

    for(QueueStatsMapTypeIterator    itr = queueStatsMap.begin()   ; itr != queueStatsMap.end()   ; ++itr) {
      std::string tpath = "stats.";
      (*retval)[tpath + itr->first] = (itr->second)->size();
    }

    return retval;
  }

  //--------------------------------------------------------------------------------
  SharedStatsMapType StatsKeeper::getLastGatheredStats()
  {
    boost::lock_guard<boost::mutex> guard(statMutex);
    SharedStatsMapType                      retval;

    retval.reset(new StatsMapType());

    for(GatheredStatsMapTypeIterator itr = gatheredStatsMap.begin(); itr != gatheredStatsMap.end(); ++itr) {
      std::string tpath = "stats.";
      (*retval)[tpath + itr->first] = (itr->second)[0];
    }

    return retval;
  }

  //--------------------------------------------------------------------------------
  SharedStatsHistoryMapType StatsKeeper::getFullStatsHistory()
  {
    boost::lock_guard<boost::mutex> guard(statMutex);
    SharedStatsHistoryMapType       retval;

    retval.reset(new StatsHistoryMapType());

    for(GatheredStatsMapTypeIterator itr = gatheredStatsMap.begin(); itr != gatheredStatsMap.end(); ++itr) {
      for(unsigned int i = 0; i < historyLength; ++i) {
        std::stringstream tmppath;
        tmppath << "stats." << itr->first << "." << std::setfill('0') << std::setw(2) << i;
        (*retval)[tmppath.str()].push_back((itr->second)[i]);
      }
    }
    return retval;
  }

  //--------------------------------------------------------------------------------
  void StatsKeeper::gatherStats()
  {
    while(running) {
      int sleepCount = 0;

      while(running && sleepCount < gatherPeriod) {
        usleep(1000000); // 1 second
        sleepCount++;
      }

      if(running) {

        statsMap["kcs-uptime"] = time(NULL) - startTime;

        for(StatsMapTypeIterator itr = statsMap.begin(); itr != statsMap.end(); ++itr) {
          gatheredStatsMap[itr->first].push_front(itr->second);
          itr->second = 0;
        }

        for(QueueStatsMapTypeIterator itr = queueStatsMap.begin(); itr != queueStatsMap.end(); ++itr) {
          gatheredStatsMap[itr->first].push_front((itr->second)->size());
        }

        for(GatheredStatsMapTypeIterator itr = gatheredStatsMap.begin(); itr != gatheredStatsMap.end(); ++itr) {
          if(gatheredStatsMap[itr->first].size() > historyLength) {
            gatheredStatsMap[itr->first].pop_back();
          }
        }
      }
    }
  }
}
