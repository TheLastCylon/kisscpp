// File  : threadsafe_persisted_priority_queue.hpp
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

#ifndef _THREADSAFE_PERSISTED_PRIORITY_QUEUE_HPP_
#define _THREADSAFE_PERSISTED_PRIORITY_QUEUE_HPP_

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "persisted_queue.hpp"

//--------------------------------------------------------------------------------
namespace kisscpp
{

template <class _qoT, class _sT>
class ThreadsafePersistedPriorityQueue : public boost::noncopyable
{
  public:
    ThreadsafePersistedPriorityQueue(const std::string& queueName,
                                     const std::string& queueWorkingDir,
                                     const unsigned     priorityLevels,
                                     const unsigned     maxItemsPerPage) :
      name(queueName),
      workingDir(queueWorkingDir),
      levels(priorityLevels),
      maxPageSize(maxItemsPerPage),
      lastPopLevel(-1)
    {
      lastPopedObject.reset();
      createQueues();
    }

    ~ThreadsafePersistedPriorityQueue()
    {
      destroyQueues();
    }

    void push(boost::shared_ptr<_qoT> p, unsigned priority)
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      if(priority >= levels) {
        priority = levels-1;
      }
      persistedQList[priority]->push_back(p);
    }

    boost::shared_ptr<_qoT> pop()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      unsigned i = 0;
      while(i < levels && persistedQList[i]->empty()) {
        ++i;
      }
      lastPopLevel    = i;
      lastPopedObject = persistedQList[i]->pop_front();
      return lastPopedObject;
    }

    boost::shared_ptr<_qoT> last_pop_object()
    {
      return lastPopedObject;
    }

    void push_back_last_pop()
    {
      if(lastPopLevel >= 0) {
        boost::lock_guard<boost::mutex> guard(objectMutex);
        persistedQList[lastPopLevel]->push_back(lastPopedObject);
        lastPopLevel = -1;
        lastPopedObject.reset();
      }
    }

    bool empty()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      unsigned i = 0;
      while(i < levels && persistedQList[i]->empty()) {
        ++i;
      }
      return (i == levels);
    }

    void shutdown()
    {
      for(unsigned i = 0; i < levels; ++i) {
        persistedQList[i]->shutdown();
      }
    }

    void clear(unsigned level)
    {
      persistedQList[level]->clear();
    }

    void clear()
    {
      for(unsigned i = 0; i < levels; ++i) {
        persistedQList[i]->clear();
      }
    }

  protected:
  private:
    void createQueues()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      for(unsigned i = 0; i < levels; ++i) {
        std::stringstream ss;
        std::string       qn;

        ss << "priority_" << i;
        qn = name + ss.str();

        boost::shared_ptr<PersistedQueue<_qoT, _sT> > persistedQ;
        persistedQ.reset(new PersistedQueue<_qoT, _sT>(qn, workingDir, maxPageSize));
        persistedQList.push_back(persistedQ);
      }
    }

    void destroyQueues()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      for(unsigned i = 0; i < levels; ++i) {
        persistedQList[i].reset();
      }
    }

    std::string                                                 name;
    std::string                                                 workingDir;
    unsigned                                                    levels;
    unsigned                                                    maxPageSize;
    int                                                         lastPopLevel;
    boost::shared_ptr<_qoT>                                     lastPopedObject;
    std::vector<boost::shared_ptr<PersistedQueue<_qoT, _sT> > > persistedQList;
    boost::mutex                                                objectMutex;
};

}

#endif // _THREADSAFE_PERSISTED_PRIORITY_QUEUE_HPP_

