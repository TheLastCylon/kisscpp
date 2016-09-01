// File  : threadsafe_persisted_queue.hpp
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

#ifndef _THREADSAFE_PERSISTED_DELAYED_QUEUE_HPP_
#define _THREADSAFE_PERSISTED_DELAYED_QUEUE_HPP_

#include <string>
#include <map>
#include <ctime>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ptree_queue.hpp"
#include "persisted_queue.hpp"
#include "statable_queue.hpp"

//--------------------------------------------------------------------------------
namespace kisscpp
{

template <class _qoT, class _sT>
class ThreadsafePersistedDelayedQueue : public StatAbleQueue, public boost::noncopyable
{
  typedef boost::shared_ptr<_qoT>                       sqot;
  typedef std::list        <sqot>                       sqotList;
  typedef std::map         <time_t, sqotList>           sqotListMap;
  typedef typename std::map<time_t, sqotList>::iterator sqotListMapIter;

  public:
    ThreadsafePersistedDelayedQueue(const std::string& queueName,
                                    const std::string& queueWorkingDir,
                                    const unsigned     maxItemsPerPage)
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      std::string MapName = queueName + "_map";
      persistedQ  .reset(new PersistedQueue<_qoT, _sT>(queueName, queueWorkingDir, maxItemsPerPage));
    }

    ~ThreadsafePersistedDelayedQueue()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      moveExpired(true);
      persistedQ.reset();
    }

    void push(boost::shared_ptr<_qoT> p, unsigned int seconds2delay = 0)
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      time_t                          time2release = time(NULL) + seconds2delay;

      if(seconds2delay == 0) {
        persistedQ->push_back(p);
      } else {
        delayedMap[time2release].push_back(p);
      }

      moveExpired();
    }

    void shutdown()
    {
      moveExpired(true);
      persistedQ->shutdown();
    }

    boost::shared_ptr<_qoT> pop()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      moveExpired();
      return persistedQ->pop_front();
    }

    bool empty()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      moveExpired();
      return (persistedQ->empty() && mapSize() == 0);
    }

    size_t size()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      return (persistedQ->size() + mapSize());
    }

    size_t mapSize()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      size_t                          count = 0;

      for(sqotListMapIter delayedMapIter = delayedMap.begin(); delayedMapIter != delayedMap.end(); ++delayedMapIter) {
        count += (delayedMapIter->second).size();
      }

      return count;
    }

  protected:
  private:
    void moveExpired(bool force_move = false)
    {
      if(force_move || persistedQ->empty()) {
        std::vector<time_t> map_keys;
        time_t              now = time(NULL);

        boost::copy(delayedMap | boost::adaptors::map_keys, std::back_inserter(map_keys));

        for(unsigned int i = 0; i < map_keys.size(); ++i) {
          if(force_move || map_keys[i] < now) {
            while(delayedMap[map_keys[i]].size() > 0) {
              persistedQ->push_back(delayedMap[map_keys[i]].front());
              delayedMap[map_keys[i]].pop_front();
            }
            delayedMap.erase(map_keys[i]);
          }
        }
      }
    }

    boost::scoped_ptr<PersistedQueue<_qoT, _sT> > persistedQ;
    sqotListMap                                   delayedMap;
    boost::mutex                                  objectMutex;
};

typedef ThreadsafePersistedDelayedQueue<boost::property_tree::ptree, kisscpp::PtreeBase64Bicoder> SafeDelayedQueue;
typedef boost::shared_ptr<SafeDelayedQueue>                                                       SharedSafeDelayedQueue;


}
#endif

