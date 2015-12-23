#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include <vector>
#include <memory>

#include <CGAL/Cartesian_d.h>
#include <CGAL/Min_sphere_of_spheres_d.h>

#include "primitives/geometry.h"
#include "skel/base.h"

namespace CollisionDetection{

typedef CGAL::Cartesian_d<double>                               BVH_K;
typedef CGAL::Min_sphere_of_spheres_d_traits_d<BVH_K,double,3>  BVH_Traits;
typedef CGAL::Min_sphere_of_spheres_d<BVH_Traits>               BVH_Min_sphere;
typedef BVH_K::Point_d                                          BVH_Point;
typedef BVH_Traits::Sphere                                      BVH_Sphere;

class BoundingSphere{
protected :
    Primitives::Point3d center = Point3d();
    double              radius = 0.0;
public:
    std::map<uint, std::shared_ptr< const BoundingSphere>> children;

    BoundingSphere() {}
    BoundingSphere( const Primitives::Point3d& c, double r )
        : center(c), radius(r){}
    bool isColliding( const BoundingSphere& other ) const{
        double dist = this->center.Dist( other.center );
        return ( dist < this->radius + other.radius );
    }
};

class BranchingBS : public BoundingSphere{
public:
    int ID;
    BranchingBS( int id, const Primitives::Point3d& c, double r )
        : BoundingSphere(c, r), ID(id){}
    BranchingBS( int id, BoundingSphere b )
        : BoundingSphere(b), ID(id){}
};

class BoneBS : public BoundingSphere{
public:
    int ID;
    BoneBS( int id, const Primitives::Point3d& c, double r )
        : BoundingSphere(c, r), ID(id){}
    BoneBS( int id, BoundingSphere b )
        : BoundingSphere(b), ID(id){}
};

typedef std::shared_ptr<BoundingSphere> BoundingSphere_ptr;
typedef std::shared_ptr<BranchingBS>    BranchingBS_ptr;
typedef std::shared_ptr<BoneBS>         BoneBS_ptr;
typedef std::shared_ptr<BVH_Min_sphere> BVH_MinSphere_ptr;

class BVH
{
private:
    BoundingSphere_ptr root;
    std::shared_ptr<const Skel::CurveSkeleton> skel;
    bool has_been_built = false;

    void determineBoundingSphere();
    BVH_MinSphere_ptr minSphere( const std::vector<BVH_Sphere>& spheres ) const;
    BVH_MinSphere_ptr minBoneSphere( int BoneID ) const;
public:
    BVH( const Skel::CurveSkeleton* skeleton );
    void Build();
    void Clear();
    bool IsColliding( const BVH& other ) const;
};

}

#endif // COLLISION_DETECTION_H
