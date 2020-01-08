// https://cirosantilli.com/linux-kernel-module-cheat#boost
//
// Example adapted from: https://stackoverflow.com/questions/12174997/boostbimap-equivalent-of-bidirectional-multimap/12175238#12175238

#include <iostream>

#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

namespace bimaps = boost::bimaps;

int main()
{
   typedef boost::bimap<bimaps::multiset_of<int>, bimaps::set_of<int>> bimap_t;
   typedef bimap_t::value_type value_type;
   bimap_t bimap;
   bimap.insert(value_type(1, 1));
   bimap.insert(value_type(1, 2));
   bimap.insert(value_type(2, 2));
   auto& left = bimap.left;
   auto it = left.find(1);
   std::cout << "LEFT" << std::endl;
   for (; it != left.end(); ++it)
   {
      std::cout << it->first <<  " " << it->second << std::endl;
   }
   auto& right = bimap.right;
   auto r_it = right.find(2);
   std::cout << "RIGHT" << std::endl;
   for (; r_it != right.end(); ++r_it)
   {
      std::cout << r_it->first << " " << r_it->second << std::endl;
   }
}
