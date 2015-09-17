#pragma once

#include <primitives/geometry.h>

using namespace std;
namespace Primitives
{

template <typename T> class BBox{

private:
    Point3<T> min, max;
public:
    BBox() { min.Set( numeric_limits<T>::max(), numeric_limits<T>::max(), numeric_limits<T>::max() ); }

    void Set( const  std::vector< Point3<T> > &points )
    {
        foreach ( const Point3<T> p, points)
        {
            if( p.x < min.x ) min.x = p.x;
            if( p.y < min.y ) min.y = p.y;
            if( p.z < min.z ) min.z = p.z;

            if( p.x > max.x ) max.x = p.x;
            if( p.y > max.y ) max.y = p.y;
            if( p.z > max.z ) max.z = p.z;
        }
    }
    void Set( const  std::vector< const Point3<T>* > &points )
    {
        foreach ( const Point3<T>* p, points)
        {
            if( p->x < min.x ) min.x = p->x;
            if( p->y < min.y ) min.y = p->y;
            if( p->z < min.z ) min.z = p->z;

            if( p->x > max.x ) max.x = p->x;
            if( p->y > max.y ) max.y = p->y;
            if( p->z > max.z ) max.z = p->z;
        }
    }

    void Reset()
    {
        min = Point3<T>();
        max = Point3<T>();
    }

    inline T            Diagonal()  const { return min.Dist(max); }
    inline Point3<T>    Center()    const { return Point3<T>::MidPoint( min, max );  }
    BBox operator +  (const BBox  in) const
    {
        BBox b_new;
        vector< Point3<T> > ps;
        ps.push_back( this->min );
        ps.push_back( this->max );
        ps.push_back( in.min );
        ps.push_back( in.max );
        b_new.Set( ps );
        return b_new;
    }

    Point3<T> Min() const { return this->min; }
    Point3<T> Max() const { return this->min; }


};

typedef BBox<float>     BBoxF;
typedef BBox<double>    BBoxD;


}
