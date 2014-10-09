#include "boost_ptree.hpp"

namespace kisscpp
{
  // nodes from pt2 will over write values of nodes in pt1
  // nodes from pt2 that do not exist in pt1 will be added to pt1.
  void ptreeMerge(BoostPtree &pt1, BoostPtree &pt2, std::string node /*= ""*/)
  {
    if(!node.empty()) {
      BOOST_FOREACH(bpt::ptree::value_type &v, pt2.get_child(node)) {
        std::string full_node_path = node + "." + v.first;

        if(v.second.size() == 0) {
          if(pt1.find(full_node_path) != pt1.not_found()) { // if the node exists
            pt1.put(full_node_path, v.second.data());
          } else {
            pt1.add(full_node_path, v.second.data());
          }
        } else {
          ptreeMerge(pt1, pt2, full_node_path);
        }
      }
    } else {
      BOOST_FOREACH(bpt::ptree::value_type &v, pt2) {
        if(v.second.size() == 0) {
          if(pt1.find(v.first) != pt1.not_found()) { // if the node exists
            pt1.put(v.first, v.second.data());
          } else {
            pt1.add(v.first, v.second.data());
          }
        } else {
          ptreeMerge(pt1, pt2, v.first);
        }
      }
    }
  }


  //--------------------------------------------------------------------------------
  void ptreeAddOrPut(BoostPtree &pt1, std::string name, std::string value)
  {
    if(pt1.find(name) != pt1.not_found()) { // if the node exists
      pt1.put(name, value);
    } else {
      pt1.add(name, value);
    }
  }
}

