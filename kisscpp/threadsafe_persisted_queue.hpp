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

#ifndef _THREADSAFE_PERSISTED_QUEUE_HPP_
#define _THREADSAFE_PERSISTED_QUEUE_HPP_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "persisted_queue.hpp"
#include "statable_queue.hpp"

//--------------------------------------------------------------------------------
namespace kisscpp
{

template <class _qoT, class _sT>
class ThreadsafePersistedQueue : public StatAbleQueue, public boost::noncopyable
{
  public:
    ThreadsafePersistedQueue(const std::string& queueName,
                             const std::string& queueWorkingDir,
                             const unsigned     maxItemsPerPage)
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      persistedQ.reset(new PersistedQueue<_qoT, _sT>(queueName, queueWorkingDir, maxItemsPerPage));
    }

    ~ThreadsafePersistedQueue()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      persistedQ.reset();
    }

    void push(boost::shared_ptr<_qoT> p)
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      persistedQ->push_back(p);
    }

    void shutdown()
    {
      persistedQ->shutdown();
    }

    boost::shared_ptr<_qoT> pop()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      return persistedQ->pop_front();
    }

    boost::shared_ptr<_qoT> front()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      return persistedQ->front();
    }

    bool empty()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      return persistedQ->empty();
    }

    size_t size()
    {
      boost::lock_guard<boost::mutex> guard(objectMutex);
      return persistedQ->size();
    }

  protected:
  private:

    boost::scoped_ptr<PersistedQueue<_qoT, _sT> > persistedQ;
    boost::mutex                                  objectMutex;
};

}
#endif

