#ifndef BVH_BALANCING_H
#define BVH_BALANCING_H
#ifdef use_gurobi
#include <sstream>
#include <map>
#include <set>

namespace BVH_balancing{

    void Solve( const std::map<int, std::set<int> >& compatibility,
                size_t no_bones, std::map<int, int>& assignments );

}
#endif
#endif // BVH_BALANCING_H
