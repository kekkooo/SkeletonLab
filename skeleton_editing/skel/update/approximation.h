#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include <skel/base.h>
#include <mesh/mesh.h>
#include <map>
#include <queue>
#include <vector>
#include <limits>
#define APPROXIMATION_EPSILON 0.0000001

namespace Skel{
namespace Update{


int getFarthestPointFromSegment( const std::map<int, RMesh::CGAL_Point >& cgal_points, const RMesh::CGAL_Segment& query ){
    return 0;
}

// the algorithm uses a modified versione of the Douglas-Peucker algorithm of curve approximation
// in order approximate each bone of the skeleton with a low number of nodes, depending on a given treshold
void approximate( Skel::CurveSkeleton& cs, double error_threshold ){
    assert( error_threshold > APPROXIMATION_EPSILON );

    std::map<int, RMesh::CGAL_Point > cgal_points;

    for( const Skel::SkelPoint& p : cs.points){
        cgal_points[p.id] = RMesh::build_CGAL_Point( sp.coord );
    }

    for( Skel::Bone& b : cs.bones ){
        std::queue<RMesh::CGAL_Segment>     q;
        std::vector<int, Skel::SkelPoint>   points_to_keep;

        RMesh::CGAL_Segment seg( cgal_points[b.front], cgal_points[b.back()] );
        q.push( seg );
        double error = std::numeric_limits<double>::max();
        while( !q.empty( ) && error > error_threshold ){
            // get current segment
            RMesh::CGAL_Segment curr = q.front();
            q.pop();
            // find farthest point
            int farthest = getFarthestPointFromSegment( cgal_points, curr  );
            // decide the real position and radius of the point
            Skel::SkelPoint new_p;
            // there should be more code that will set up new_p

            // create the two new segments
            RMesh::CGAL_Segment left( curr.start(), new_p ), right( new_p, curr.end( ));
            // put segments into queue
            q.push( left  );
            q.push( right );
        }


    }

}

}
}


#endif // APPROXIMATION_H
