#ifndef _STATABLE_QUEUE_HPP_
#define _STATABLE_QUEUE_HPP_

#include <memory>

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

  typedef std::shared_ptr<StatAbleQueue> sharedStatAbleQ;
}

#endif

