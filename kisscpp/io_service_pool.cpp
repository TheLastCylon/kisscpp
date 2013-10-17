// File  : io_service_pool.cpp
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

#include "io_service_pool.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  IoServicePool::IoServicePool(std::size_t pool_size) : next_io_service_(0)
  {
    if (pool_size == 0) {
      throw std::runtime_error("IoServicePool size is 0");
    }

    // Give all the io_services work to do so that their run() functions will not exit until they are explicitly stopped.
    for(std::size_t i = 0; i < pool_size; ++i) {
      io_service_ptr io_service(new boost::asio::io_service);
      work_ptr       work(new boost::asio::io_service::work(*io_service));
      io_services_.push_back(io_service);
      work_.push_back(work);
    }
  }

  //--------------------------------------------------------------------------------
  void IoServicePool::run()
  {
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread> > threads;
    for(std::size_t i = 0; i < io_services_.size(); ++i) {
      boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, io_services_[i])));
      threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i) {
      threads[i]->join();
    }
  }

  //--------------------------------------------------------------------------------
  void IoServicePool::stop()
  {
    // Explicitly stop all io_services.
    for (std::size_t i = 0; i < io_services_.size(); ++i) {
      io_services_[i]->stop();
    }
  }

  //--------------------------------------------------------------------------------
  boost::asio::io_service& IoServicePool::get_io_service()
  {
    // Use a round-robin scheme to choose the next io_service to use.
    boost::asio::io_service& io_service = *io_services_[next_io_service_];
    ++next_io_service_;
    if (next_io_service_ == io_services_.size()) {
      next_io_service_ = 0;
    }
    return io_service;
  }
} // namespace server

