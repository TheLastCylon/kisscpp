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
#include <vector>
#include <memory>
#include <boost/algorithm/string.hpp>    
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "boost_ptree.hpp"
#include "logstream.hpp"

namespace kisscpp
{
  //--------------------------------------------------------------------------------
  class ErrorState
  {
    public:
       ErrorState(const std::string &_id, const std::string &_description = "") : id(_id), description(_description), count(0) {}

      ~ErrorState() {};

      void         set           ()                               { count++;                }
      void         clear         (const unsigned int &amount = 1) { count = count - amount; }
      void         clear_all     ()                               { count = 0;              }
      unsigned int getSetCount   () const throw()                 { return count;           }
      bool         isSet         () const throw()                 { return (count > 0);     }
      std::string  getId         () const throw()                 { return id;              }
      std::string  getDescription() const throw()                 { return description;     }

    protected:
    private:
      std::string  id;
      std::string  description;
      unsigned int count;
  };

  typedef std::shared_ptr<ErrorState>           SharedErrorState;
  typedef std::map<std::string, SharedErrorState> ErrorStateMapType;
  typedef ErrorStateMapType::iterator             ErrorStateMapTypeIterator;

  typedef std::vector<SharedErrorState>           ErrorList;
  typedef std::shared_ptr<ErrorList>            SharedErrorList;
  typedef ErrorList::iterator                     ErrorListIterator;

  //--------------------------------------------------------------------------------
  class ErrorStateList
  {
    public:
      static ErrorStateList* instance();

      ~ErrorStateList() { kisscpp::LogStream log(__PRETTY_FUNCTION__); }

      void            set      (const std::string &id, const std::string  &description = "");
      void            clear    (const std::string &id, const unsigned int &amount = 1);
      void            clear_all(const std::string &id);
      SharedErrorList getStates();

    protected:
    private:
      ErrorStateList           ()                     { kisscpp::LogStream log(__PRETTY_FUNCTION__); }  // Private to prevent copying.
      ErrorStateList           (ErrorStateList const&){ kisscpp::LogStream log(__PRETTY_FUNCTION__); }; // Private to prevent copying.
      ErrorStateList& operator=(ErrorStateList const&); //{ kisscpp::LogStream log(__PRETTY_FUNCTION__); }; // Private to prevent assignment.

      static ErrorStateList *singleton_instance;

      ErrorStateMapType      errorStateMap;
      boost::mutex           errorMutex;
  };
}

#endif // _ERRORSTATE_HPP_

