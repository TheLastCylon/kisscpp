#ifndef _STATABLE_QUEUE_HPP_
#define _STATABLE_QUEUE_HPP_

#include <boost/shared_ptr.hpp>

namespace kisscpp
{
  class StatAbleQueue
  {
    public:
      StatAbleQueue() {};
      virtual ~StatAbleQueue() {};

      virtual size_t size() = 0;

    protected:
    private:
  };

  typedef boost::shared_ptr<StatAbleQueue> sharedStatAbleQ;
}

#endif

