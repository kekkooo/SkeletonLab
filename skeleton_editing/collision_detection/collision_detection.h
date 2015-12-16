#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "primitives/geometry.h"
#include "skel/base.h"
#include <vector>

namespace CollisionDetection{

class BoundingSphere{
private :
    Primitives::Point3d center = Point3d();
    double              radius = 0.0;
public:
    std::vector<const BoundingSphere*> children;

    BoundingSphere() {}
    BoundingSphere( const Primitives::Point3d& c, double r )
        : center(c), radius(r){}
    bool isColliding( const BoundingSphere& other ) const{
        double dist = this->center.Dist( other.center );
        return dist < this->radius + other.radius;
    }
};

class BranchingBS : public BoundingSphere{
public:
    int ID;
    BranchingBS( int id, const Primitives::Point3d& c, double r )
        : BoundingSphere(c, r), ID(id){}
};

class BoneBS : public BoundingSphere{
public:
    int ID;
    BoneBS( int id, const Primitives::Point3d& c, double r )
        : BoundingSphere(c, r), ID(id){}
};

class BVH
{
private:
    BoundingSphere root;
    const Skel::CurveSkeleton* skel;
public:
    BVH();
    void build();
    bool isColliding( const BVH& other ) const;
};

}

#endif // COLLISION_DETECTION_H
