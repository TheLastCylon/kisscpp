// File  : io_service_pool.hpp
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

#ifndef _SERVER_IO_SERVICE_POOL_HPP
#define _SERVER_IO_SERVICE_POOL_HPP

#include <stdexcept>
#include <vector>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>

namespace kisscpp
{
  class IoServicePool : private boost::noncopyable
  {
    public:
      explicit                 IoServicePool(std::size_t pool_size); /// Construct the io_service pool.
      void                     run();                                  /// Run all io_service objects in the pool.
      void                     stop();                                 /// Stop all io_service objects in the pool.
      boost::asio::io_service &get_io_service();                       /// Get an io_service to use.

    private:
      typedef std::shared_ptr<boost::asio::io_service>       io_service_ptr;
      typedef std::shared_ptr<boost::asio::io_service::work> work_ptr;

      std::vector<io_service_ptr> io_services_;     /// The pool of io_services.
      std::vector<work_ptr>       work_;            /// The work that keeps the io_services running.
      std::size_t                 next_io_service_; /// The next io_service to use for a connection.
  };

} // namespace server

#endif
