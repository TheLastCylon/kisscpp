// File  : errorstates.cpp
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

#include "errorstate.hpp"

namespace kisscpp
{
  ErrorStateList* ErrorStateList::singleton_instance;

  //--------------------------------------------------------------------------------
  ErrorStateList* ErrorStateList::instance()
  {
    if(!singleton_instance) { // Only allow one instance of class to be generated.
      singleton_instance = new ErrorStateList();
    }

    return singleton_instance;
  }

  //--------------------------------------------------------------------------------
  void ErrorStateList::set(std::string id)
  {
    boost::lock_guard<boost::mutex> guard(errorMutex);
    if(errorStateMap.find(id) != errorStateMap.end()) {
      errorStateMap[id]->set();
    } else {
      SharedErrorState tempErrorState;
      tempErrorState.reset(new ErrorState(id));
      errorStateMap[id] = tempErrorState;
      errorStateMap[id]->set();
    }
  }

  //--------------------------------------------------------------------------------
  void ErrorStateList::clear(std::string id, unsigned int amount /*= 1*/)
  {
    boost::lock_guard<boost::mutex> guard(errorMutex);
    if(errorStateMap.find(id) != errorStateMap.end()) {
      errorStateMap[id]->clear(amount);
    } else {
      SharedErrorState tempErrorState;
      tempErrorState.reset(new ErrorState(id));
      errorStateMap[id] = tempErrorState;
      errorStateMap[id]->clear(amount);
    }
  }

  //--------------------------------------------------------------------------------
  void ErrorStateList::clear_all(std::string id)
  {
    boost::lock_guard<boost::mutex> guard(errorMutex);
    if(errorStateMap.find(id) != errorStateMap.end()) {
      errorStateMap[id]->clear_all();
    } else {
      SharedErrorState tempErrorState;
      tempErrorState.reset(new ErrorState(id));
      errorStateMap[id] = tempErrorState;
      errorStateMap[id]->clear_all();
    }
  }

  //--------------------------------------------------------------------------------
  SharedErrorListMap ErrorStateList::getStates()
  {
    boost::lock_guard<boost::mutex> guard(errorMutex);
    SharedErrorListMap              retval;

    retval.reset(new ErrorListMap());

    for(ErrorStateMapTypeIterator itr = errorStateMap.begin(); itr != errorStateMap.end(); itr++) {
      if((itr->second)->isSet()) {
        (*retval)[itr->first] = (itr->second)->getSetCount();
      }
    }

    return retval;
  }
}
