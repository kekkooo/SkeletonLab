#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include <skel/base.h>
#include <skel/update/topology.h>
#include <mesh/mesh.h>
#include <map>
#include <queue>
#include <vector>
#include <limits>
#define APPROXIMATION_EPSILON 0.0000001

namespace Skel{
namespace Update{

typedef std::pair<int, int>                         id_range;
typedef std::pair<id_range, RMesh::CGAL_Segment >   range_and_segment;


void getFarthestPointFromSegment( const std::map<int, RMesh::CGAL_Point >& cgal_points, const Skel::Bone& b, const range_and_segment& query, std::pair< int, double >& result ){

    assert( std::abs( query.first.first - query.first.second ) > 1 );
    result.first  = -1;
    result.second = std::numeric_limits<double>::min( );


    bool start = false, stop = false;
    for( int i = query.first.first; i <= query.first.second; ++i ){
        int index = b.at( i );
        double squared_dist = CGAL::squared_distance( cgal_points.at(index), query.second );
        if( squared_dist > result.second ) { result.first = i; result.second = squared_dist;}
    }
    result.second = sqrt( result.second );
}

// the algorithm uses a modified versione of the Douglas-Peucker algorithm of curve approximation
// in order approximate each bone of the skeleton with a low number of nodes, depending on a given treshold
void approximate( Skel::CurveSkeleton& cs, double error_threshold, size_t target_reduction = 0 ){
    assert( error_threshold > APPROXIMATION_EPSILON );

    double reduction_ratio = target_reduction >= 100 ? 1.0 : static_cast<double>(target_reduction)/100.0;

    std::map<int, RMesh::CGAL_Point > cgal_points;

    for( const Skel::SkelPoint& p : cs.points){
        cgal_points[p.id] = RMesh::build_CGAL_Point( p.coord );
    }

    std::cout << "# CGAL points created " << std::endl;
    int count = 0;

    for( Skel::Bone& b : cs.bones ){        

        std::cout << "# working on bone " <<  count++ << std::endl;

        if( b.size() < 3 ) { continue; } // there would be nothing to do;
        size_t target_nodes_number = std::max( static_cast<size_t>( static_cast<double>( b.size( )) * ( 1.0 - reduction_ratio )),
                                               static_cast<size_t>(2) );
        std::queue<range_and_segment>    q;
        std::set<int>                    points_to_keep;

        RMesh::CGAL_Segment seg( cgal_points[b.front()], cgal_points[b.back()] );

        q.push( make_pair( make_pair( 0, b.size( ) - 1 ), seg ));
        points_to_keep.insert( b.front() );
        points_to_keep.insert( b.back() );

        double local_error_threshold = std::numeric_limits<double>::max();
        double error = std::numeric_limits<double>::min();

        for( int id : b ){
         if( local_error_threshold > cs.points[id].radius ){ local_error_threshold = cs.points[id].radius; }
        }
        local_error_threshold *= error_threshold;

        std::cout << "nodes_number_target : " << target_nodes_number << " # error_threshold : " << local_error_threshold << std::endl;

        // q.empty
        // error > threshold
        // n_nodes > target

//        while( !(q.empty( ) || error > error_threshold || points_to_keep.size() >= target_nodes_number )){
        while( !(q.empty( ) || points_to_keep.size() >= target_nodes_number )){
            // get current segment
            range_and_segment curr = q.front();
            q.pop();

            std::cout << "current range " << curr.first.first << ",  " << curr.first.second << std::endl;

            // find farthest point
            std::pair< int, double > farthest;
            getFarthestPointFromSegment( cgal_points, b, curr, farthest );
            // decide the real position and radius of the point            
//            RMesh::CGAL_Point midpoint = RMesh::build_CGAL_Point( cs.points[farthest.first].coord );
            RMesh::CGAL_Point midpoint = RMesh::build_CGAL_Point( cs.points[ b[farthest.first] ].coord );

            error = farthest.second;
            std::cout << " # current error : " << error << std::endl;
            // new point should be added only if it is too far from the approximating segment
//            if( error > error_threshold ){
            if( error > local_error_threshold ){
                points_to_keep.insert( b[farthest.first] );

                std::cout << "midpoint is : " << farthest.first << std::endl;
                // create the two new segments only if their size is bigger than 1
                if( abs(curr.first.first - farthest.first) > 1 ){
                    std::cout << "enqueuing left range " << curr.first.first << ",  " << farthest.first << std::endl;
                    RMesh::CGAL_Segment left_seg( curr.second.start(), midpoint );
                    range_and_segment left = make_pair( make_pair( curr.first.first, farthest.first ), left_seg );
                    q.push( left  );
                }
                if( abs( curr.first.second - farthest.first ) > 1 ){
                    std::cout << "enqueuing right range " << farthest.first << ", " << curr.first.second << std::endl;
                    RMesh::CGAL_Segment right_seg( midpoint, curr.second.end() );
                    range_and_segment right = make_pair( make_pair( farthest.first, curr.first.second ), right_seg );
                    q.push( right  );
                }
            }
            std::cout << "queue size : " << q.size() << " # current nodes saved : " << points_to_keep.size() << " # current error : " << error << std::endl;
        }
        std::cout << "approximation calculated" << std::endl;
        assert( points_to_keep.size() >= 2 );

        // TODO : clean up B, keeping only the selected points.
        // debug
        std::cout << "before cleaning bone was : " << std::endl;
        for( int p : b ){
            std::cout << p << ", ";
        }
        std::cout << std::endl << std::endl;

        std::cout << "nodes to keep: " << std::endl;
        for( int p : points_to_keep ){
            std::cout << p << ", ";
        }
        std::cout << std::endl << std::endl;

        int last = 0, c = 1, next = 2;
        while( next < b.size() ){
            if( points_to_keep.count( b[c] ) == 0 ){
                cs.remove( b[c], false );
                Skel::UpdateTopology::pointConnect( &cs, b[last], b[next] );
            }
            else{ last = c; }
            c++;
            next++;
        }
        std::cout << "approximation applied" << std::endl;
        std::cout << "now bone is : " << std::endl;
        for( int p : b ){
            std::cout << p << ", ";
        }
        std::cout << std::endl << std::endl;
    }
    Skel::UpdateTopology::garbage_collector( &cs );

}


void CollapseSpuriousBranches( Skel::CurveSkeleton& cs ){
    for( int i = 0; i < cs.bones.size(); ++i ){
        auto& b = cs.bones[i];
        Skel::SkelPoint &p = cs.points[b.front()];
        Skel::SkelPoint &q = cs.points[b.back()];

        double threshold = p.radius + q.radius;
        if( p.coord.Dist( q.coord ) < threshold ){
            UpdateTopology::branchCollapse( cs, i );
//            return;
        }
    }
    //UpdateTopology::garbage_collector( &cs );
}

void CleanupClustersAtBranchingNodes( Skel::CurveSkeleton& cs ){

    double threshold_scale = 0.85;
    double threshold_scale_for_joints = 0.5;
    for( SkelPoint& p : cs.points ){
        if( !p.isBranchingNode() ){ continue; }
        std::vector<int> bones;
        for( int neighbor : p.neighbors ){
            if( cs.points[neighbor].isBranchingNode() ){ continue; }
            bones.push_back( cs.points[neighbor].boneID );
        }
        for( int bone_id : bones ){
            Skel::Bone& branch = cs.bones[ bone_id ];

            int last, c, next, pace;
            bool go_on= false;
            if( branch.front() == p.id ){
                last = 0; pace = 1;
            }else{
                last = branch.size() - 1;
                pace = -1;
            }
            c    = last + pace;
            next = c + pace;
            go_on = (pace == 1 ? next < branch.size() : next >= 0);
            while( go_on ){
                SkelPoint& lastp    = cs.points[ branch[last] ];
                SkelPoint& currp    = cs.points[ branch[c ] ];
                SkelPoint& nextp    = cs.points[ branch[next] ];

                double threshold = ( lastp.radius + currp.radius );
                threshold *= lastp.isJoint() ? threshold_scale_for_joints : threshold_scale;

                if( lastp.coord.Dist( currp.coord ) < threshold ){
                    cs.remove( branch[c], false );
                    Skel::UpdateTopology::pointConnect( &cs, branch[last], branch[next] );
                }
                else{ last = c; }
                c    += pace;
                next += pace;
                go_on = pace == 1 ? next < branch.size() : next >= 0;
            }
        }
    }
    UpdateTopology::garbage_collector( &cs );

}

}
}


#endif // APPROXIMATION_H
