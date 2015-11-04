// File  : ptree_queue.hpp
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

#ifndef _PTREE_QUEUE_HPP_
#define _PTREE_QUEUE_HPP_

#include <sstream>
#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "threadsafe_persisted_queue.hpp"

namespace kisscpp
{

//--------------------------------------------------------------------------------
class PtreeBase64Bicoder : public Base64BiCoder<boost::property_tree::ptree>
{
  public:
    PtreeBase64Bicoder() {};
    ~PtreeBase64Bicoder() {};

    //--------------------------------------------------------------------------------
    virtual boost::shared_ptr<std::string> encode(const boost::shared_ptr<boost::property_tree::ptree> obj2encode)
    {
      using namespace boost::property_tree::json_parser;

      std::stringstream tosstrm;

      write_json(tosstrm, (*obj2encode.get()), false);

      return encodeToBase64String(tosstrm.str());
    }

    //--------------------------------------------------------------------------------
    virtual boost::shared_ptr<boost::property_tree::ptree> decode(const std::string& str2decode)
    {
      using namespace boost::property_tree::json_parser;

      boost::shared_ptr<boost::property_tree::ptree> tPtreePtr;
      boost::shared_ptr<std::string>                 jsonString = decodeFromBase64(str2decode);
      std::stringstream                              fromStream;

      tPtreePtr.reset(new boost::property_tree::ptree());

      fromStream << (*jsonString.get());

      read_json(fromStream, (*tPtreePtr.get()));

      return tPtreePtr;
    }
};

typedef PersistedQueue<boost::property_tree::ptree, PtreeBase64Bicoder>           PtreeQ;
typedef boost::shared_ptr<PtreeQ>                                                 SharedPtreeQ;
typedef boost::scoped_ptr<PtreeQ>                                                 ScopedPtreeQ;

typedef ThreadsafePersistedQueue<boost::property_tree::ptree, PtreeBase64Bicoder> SafePtreeQ;
typedef boost::shared_ptr<SafePtreeQ>                                             SharedSafePtreeQ;
typedef boost::scoped_ptr<SafePtreeQ>                                             ScopedSafePtreeQ;

}

#endif

