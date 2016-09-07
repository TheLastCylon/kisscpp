// File  : boost_ptree.hpp
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

#ifndef _BOOST_PTREE_HPP_
#define _BOOST_PTREE_HPP_

// #define BOOST_SPIRIT_THREADSAFE
// Moved this define to global at compile time.
// NOTE: If an application uses read_json, it should have the same define in the compilation options.
// This absolutely has to be a GLOBAL define in order to prevent read_json crashing.

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

typedef boost::property_tree::ptree   BoostPtree;
typedef boost::shared_ptr<BoostPtree> SharedPtree;
typedef boost::scoped_ptr<BoostPtree> ScopedPtree;

namespace bpt = boost::property_tree;

namespace kisscpp
{
  void ptreeMerge   (BoostPtree &pt1, BoostPtree &pt2, std::string node = "");
  void ptreeAddOrPut(BoostPtree &pt1, std::string name, std::string value);
}

#endif
