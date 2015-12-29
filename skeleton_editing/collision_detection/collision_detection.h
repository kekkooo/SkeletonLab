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
    std::string text;
    std::map<uint, std::shared_ptr< const BoundingSphere>> children;

    BoundingSphere() {}
    BoundingSphere( const Primitives::Point3d& c, double r )
        : center(c), radius(r){}
    bool isColliding( const BoundingSphere& other ) const{
        double dist = this->center.Dist( other.center );
        return ( dist < this->radius + other.radius );
    }

    virtual inline void printType() const{
        std::cout << "BS : ( ";
    }

    virtual inline void print() const{
        std::cout << text << " ";
        printType();
        std::cout << center.x << ", " << center.y << ", " << center.z << " ) : r = " << radius
                  << " has #" << children.size() << " chs " << endl;
    }

    inline void printPretty( std::string indent, bool last )const{

        std::cout << indent;
        if( last ){
            std::cout << "\\- ";
            indent += "   ";
        }
        else{
            std::cout << "|- ";
            indent += "|  ";
        }
       print();

       for( auto it = children.begin(); it != children.end(); ++it ){
           auto tmp = it;
           (*it).second.get()->printPretty( indent, tmp == children.end() );
       }
    }

};

class BranchingBS : public BoundingSphere{
public:
    int ID;
    BranchingBS( int id, const Primitives::Point3d& c, double r )
        : BoundingSphere(c, r), ID(id){}
    BranchingBS( int id, BoundingSphere b )
        : BoundingSphere(b), ID(id){}

    inline void printType() const{
        std::cout << "BnBs : ( ";
    }
    inline void print() const {
        std::cout << ID << ") ";
        BoundingSphere::print();
    }
};

class BoneBS : public BoundingSphere{
public:
    int ID;
    BoneBS( int id, const Primitives::Point3d& c, double r )
        : BoundingSphere(c, r), ID(id){}
    BoneBS( int id, BoundingSphere b )
        : BoundingSphere(b), ID(id){}

    inline void printType() const{
        std::cout << "Bone : ( ";
    }
    inline void print() const{
        std::cout << ID << ") ";
        BoundingSphere::print();
    }
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
