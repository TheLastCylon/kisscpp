// File  : errorstates.hpp
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

#ifndef _ERRORSTATE_HPP_
#define _ERRORSTATE_HPP_

#include <string>
#include <map>
#include <boost/algorithm/string.hpp>    
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>
#include "boost_ptree.hpp"
#include "logstream.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  class ErrorState
  {
    public:
      ErrorState(const std::string &_id) : id(_id), count(0) {}

      ~ErrorState() {};

      void         set        ()                               { count++; }
      void         clear      (const unsigned int &amount = 1) { count = count - amount; }
      void         clear_all  ()                               { count = 0; }
      unsigned int getSetCount()                               { return count; }
      bool         isSet      ()                               { return (count > 0); }

    protected:
    private:
      std::string  id;
      unsigned int count;
  };

  typedef boost::shared_ptr<ErrorState>           SharedErrorState;
  typedef std::map<std::string, SharedErrorState> ErrorStateMapType;
  typedef ErrorStateMapType::iterator             ErrorStateMapTypeIterator;

  typedef std::map<std::string, double>           ErrorListMap;
  typedef boost::shared_ptr<ErrorListMap>         SharedErrorListMap;
  typedef ErrorListMap::iterator                  ErrorListMapIterator;

  //--------------------------------------------------------------------------------
  class ErrorStateList
  {
    public:
      static ErrorStateList* instance();

      ~ErrorStateList() { kisscpp::LogStream log(__PRETTY_FUNCTION__); }

      void               set      (const std::string &id);
      void               clear    (const std::string &id, const unsigned int &amount = 1);
      void               clear_all(const std::string &id);
      SharedErrorListMap getStates();

    protected:
    private:
      ErrorStateList           ()                     { kisscpp::LogStream log(__PRETTY_FUNCTION__); }  // Private to prevent copying.
      ErrorStateList           (ErrorStateList const&){ kisscpp::LogStream log(__PRETTY_FUNCTION__); }; // Private to prevent copying.
      ErrorStateList& operator=(ErrorStateList const&){ kisscpp::LogStream log(__PRETTY_FUNCTION__); }; // Private to prevent assignment.

      static ErrorStateList *singleton_instance;

      ErrorStateMapType      errorStateMap;
      boost::mutex           errorMutex;
  };
}

#endif // _ERRORSTATE_HPP_

