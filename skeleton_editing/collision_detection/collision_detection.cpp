#include "collision_detection.h"
#include <set>
#include <stack>
#include <limits>
#if use_gurobi
#include "bvh_balancing.h"
#endif


namespace CollisionDetection{

#define NODE_ID_OFFSET_FOR_RANGE 9999000

BVH::BVH(const Skel::CurveSkeleton *skeleton) : skel(skeleton) { }

void BVH::Clear(){ has_been_built = false; }

void getBVHSphereData( BVH_MinSphere_ptr sphere, Point3d &center, double& radius ){

    auto it = sphere.get()->center_cartesian_begin();
    center.x = *it;    ++it;
    center.y = *it;    ++it;
    center.z = *it;
    radius = sphere.get()->radius();
}

BoundingSphere_ptr cgal_to_BS( BVH_MinSphere_ptr sphere){
    Point3d center;
    double radius;
    getBVHSphereData( sphere , center, radius );
    BoundingSphere_ptr ptr( new BoundingSphere( center, radius ));
    return ptr;
}

BoneBS_ptr cgal_to_boneBS( BVH_MinSphere_ptr sphere, int BoneID ){
    Point3d center;
    double radius;
    getBVHSphereData( sphere, center, radius );
    BoneBS_ptr ptr( new BoneBS( BoneID, BoundingSphere( center, radius )));
    return ptr;
}

BranchingBS_ptr cgal_to_branchiBS( BVH_MinSphere_ptr sphere, int NodeID ) {
    Point3d center;
    double radius;
    getBVHSphereData( sphere, center, radius );
    BranchingBS_ptr ptr( new BranchingBS( NodeID, BoundingSphere( center, radius )));
    return ptr;
}

BVH_MinSphere_ptr BVH::minSphere(const std::vector<BVH_Sphere>& spheres) const{
    BVH_MinSphere_ptr ms(new BVH_Min_sphere( spheres.begin(), spheres.end() ));
    return ms;
}

BVH_MinSphere_ptr BVH::minBoneSphere( int BoneID ) const {
    std::vector<BVH_Sphere> S;
    Skel::Bone b = skel->bones[BoneID];
    for( size_t nodeID : b ){
        const Skel::SkelPoint& p = skel->points.at( nodeID );
        S.push_back( BVH_Sphere( BVH_Point( p.coord.x, p.coord.y, p.coord.z ), p.radius ));
    }
    BVH_MinSphere_ptr ms = minSphere( S );
    return ms;
}

void BVH::determineBoundingSphere(){
    std::vector<BVH_Sphere> S;
    for(const Skel::SkelPoint& p : skel->points){
        S.push_back( BVH_Sphere( BVH_Point( p.coord.x, p.coord.y, p.coord.z ), p.radius ));
    }
    BVH_MinSphere_ptr ms = minSphere( S );
//    auto ms = minSphere( S );
    root = cgal_to_BS( ms );
}


void BVH::Build(){

    std::map<int, BoneBS_ptr>           bs_bones;
    std::map<int, BranchingBS_ptr>      bs_branchins;
    std::map<int, std::set<int> >       bones_per_Bn;
    std::map<int, int>                  bone_to_Bn;
    std::vector<BVH_Sphere>             spheres(skel->points.size());
    std::vector<BVH_Sphere>             bone_spheres(skel->bones.size());
    // build entire shape bounding sphere
    determineBoundingSphere();
    // build Bones Bounding spheres
    for( int boneID : skel->bones.keys()){
        auto ms = minBoneSphere( boneID );
        Point3d center;
        double radius;
        getBVHSphereData( ms, center, radius );
        bs_bones[boneID] = cgal_to_boneBS(ms, boneID );
        BVH_Sphere s;
        bone_spheres[boneID] = BVH_Sphere( BVH_Point( center.x, center.y, center.z ), radius );
    }
    // assign to each branching node the set of bones assigned to it
    // this can be done naively or with some balancing strategy
    for(const Skel::SkelPoint& p : skel->points ){
        if( !p.isBranchingNode() ) { continue; }
        bones_per_Bn[p.id] = std::set<int>();
        for( int neighbor : p.neighbors ){
            bones_per_Bn[p.id].insert( skel.get()->points[neighbor].boneID );
        }
        // build a CGAL sphere for each node of the skeleton
        assert( spheres.size() > p.id );
        spheres[p.id] =
            BVH_Sphere( BVH_Point( p.coord.x, p.coord.y, p.coord.z ), p.radius );

    }
#if use_gurobi
    BVH_balancing::Solve( bones_per_Bn, skel->bones.size(), bone_to_Bn) ;
    // rebuild the assignments with the computed ones.
    bones_per_Bn.clear();
    for( const auto& bone_and_Bn : bone_to_Bn ){
        if( bones_per_Bn.count( bone_and_Bn.second ) == 0 ){
            bones_per_Bn[bone_and_Bn.second] = std::set<int>();
        }
        bones_per_Bn.at( bone_and_Bn.second ).insert( bone_and_Bn.first );
    }
#endif
    //build Branching Nodes bounding spheres Bn_bones -> BranchingNodeID, bones
    for( const auto& Bn_bones : bones_per_Bn ){
        // this can be optimizied using a vector of pointers or references to BVH_sphere
        // but a new min_sphere function should be defined.

        // calcola la min_bounding_spehre
        // crea la parte di gerarchia.
        std::vector<BVH_Sphere> bone_spheres;
        for( int bone_id : Bn_bones.second ){
            bone_spheres.push_back( bone_spheres[bone_id] );
        }
        // compute the minimal bounding sphere for the bones' bounding sphere,
        // using the trivial assignment or the optimized one
        BVH_MinSphere_ptr bs = minSphere( bone_spheres );
        BranchingBS_ptr ptr = cgal_to_branchiBS( bs, Bn_bones.first );

        // build FP -> Bone hierarchy
        for( int bone_id : Bn_bones.second ){
            ptr.get()->children[bone_id] = bs_bones[bone_id];
        }
    }
    // split bones and build their hierarchy

    typedef std::pair< size_t, size_t > range;              // range.second should be a valid index for the bone it refers to
    typedef std::pair< range, BoundingSphere_ptr > Parent;
    typedef std::stack< Parent > ParentStack;
    int counter = 0;
    for( int boneID : skel->bones.keys()){
        const Skel::Bone& curr_bone = skel->bones[boneID];
        ParentStack stack;
        Parent root = make_pair( make_pair( 0, curr_bone.size()), bs_bones[boneID] );
        stack.push( root );
        do{
            Parent p = stack.top();
            stack.pop();
            range r = p.first;
            assert( r.second > r.first );
            size_t r_size = r.second - r.first + 1;
            std::vector<BVH_Sphere> left, right;
            size_t mid = r.first + (( r_size % 2 == 0 ) ? r_size/2 - 1 : r_size/2 );

            std::cout << "Range is : ( " << r.first << ", " << r.second << " )" << std::endl;

            if( r_size > 2 ){
                // build the sphere vectors for ranges r.first, mid and mid+1, r.second
                for( size_t index = r.first; index <= mid; ++index )          { left.push_back(  spheres.at( curr_bone[index] )); }
                for( size_t index = mid + 1; index  <= r.second; ++index  )   { right.push_back( spheres.at( curr_bone[index] )); }
                std::cout << "split into : ( " << r.first << ", " << mid << " ) and ( " << mid + 1 << ", " << r.second << " )" << std::endl;
            }else{
                left.push_back(  spheres.at( curr_bone[ r.first ]) );
                right.push_back( spheres.at( curr_bone[ r.second ]) );
                std::cout << "just 2 nodes " << std::endl;
                std::cout << "split into : ( " << r.first << " ) and ( " << r.second << " )" << std::endl;
            }
            assert( left.size()  > 0 );
            assert( right.size() > 0 );
            BVH_MinSphere_ptr   left_min   = minSphere( left );
            BVH_MinSphere_ptr   right_min  = minSphere( right );
            BoundingSphere_ptr  left_bs    = cgal_to_BS( left_min  ); //,  left.size()  == 1 ? curr_bone[r.first]  : NODE_ID_OFFSET_FOR_RANGE + counter++ );
            BoundingSphere_ptr  right_bs   = cgal_to_BS( right_min ); //, right.size() == 1 ? curr_bone[r.second] : NODE_ID_OFFSET_FOR_RANGE + counter++ );
            // assign the two spheres to p's children and
            p.second.get()->children[0] = left_bs;
            p.second.get()->children[1] = right_bs;
            // if min sphere's range is > 1 add them to the stack
            if( left.size() > 1 )  { stack.push( make_pair( make_pair( r.first, mid ),      left_bs )); }
            if( right.size() > 1 ) { stack.push( make_pair( make_pair( mid + 1, r.second ), right_bs )); }

        }while( !stack.empty( ));
    }
    has_been_built = true;
}

}
