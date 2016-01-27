#ifndef SMOOTHING_H
#define SMOOTHING_H

#include <skel/base.h>
#include <primitives/geometry.h>

namespace Skel{
namespace Update{
void Smooth( Skel::CurveSkeleton& cs  ){
    std::map<int, Primitives::Point3d> new_coords;
    for( Skel::Bone& b : cs.bones ){
        if( b.size() < 3 ) { return; }
        if( !cs.points[b.front()].isBranchingNode() && cs.points[b.back()].isBranchingNode() ){
            b.reverse();
        }

        for( int i = 1; i < b.size() - 1; ++i ){
            new_coords[ b[i] ] = ( cs.points[b[i]].coord + ( cs.points[b[i-1]].coord + cs.points[b[i+1]].coord )/2.0 )/2.0;
        }
    }
    for( const auto& item :new_coords ){
        cs.points[item.first].coord = item.second;
    }
}

}}


#endif // SMOOTHING_H
