#pragma once
#include <iostream>
#include <math.h>
#ifdef WIN32
    #define _USE_MATH_DEFINES
#endif

#include <algorithm>
//#include <gl.h>
#include <cmath>
#include <vector>

#ifdef WIN32
//maybe some macros of another header file is interfering
//with the code below so we need to say to the compiler to ignore
//these macros
#undef min
#undef max
#endif



using namespace std;

namespace Primitives
{
/**
 * @file geometry.h
 * @author Marco Livesu (marco.livesu@gmail.com)
 * @brief The Vector class implements a simple vector
 * embedded in R3
 */

template <typename T> class Vector3
{
    public:

        T x, y, z;

        Vector3  (T _x=0.0, T _y=0.0, T _z=0.0) { Set(_x,_y,_z);    }
        void Set(T _x=0.0, T _y=0.0, T _z=0.0) { x=_x; y=_y; z=_z; }

        T & operator[](int id)       { return *(&x+id); }
        T & operator[](int id) const { return *(&x+id); }

        Vector3 operator +  (const Vector3  in) const   { return Vector3(x+in.x, y+in.y, z+in.z );  }
        Vector3 operator -  (const Vector3  in) const   { return Vector3(x-in.x, y-in.y, z-in.z );  }
        Vector3 operator *  (const T        in) const   { return Vector3(x*in, y*in, z*in );        }
        Vector3 operator /  (const T        in) const   { return Vector3(x/in, y/in, z/in );        }
        Vector3 operator += (const Vector3  in)         { x+=in.x; y+=in.y; z+=in.z; return *this;  }
        Vector3 operator -= (const Vector3  in)         { x-=in.x; y-=in.y; z-=in.z; return *this;  }
        Vector3 operator *= (const T        in)         { x*=in; y*=in; z*=in; return *this;        }
        Vector3 operator /= (const T        in)         { x/=in; y/=in; z/=in; return *this;        }

        T    Length()               const   { return sqrt( x*x + y*y + z*z );                           }
        T    LengthSquared()        const   { return x*x + y*y + z*z;                                   }
        T    Normalize()                    { float len = Length(); x/=len; y/=len; z/=len; return len; }
        Vector3 Normalized()        const   { T len = this->Length(); return Vector3( this->x/len, this->y/len, this->z/len); }
        T    Dot(const Vector3 in)  const   { return x*in.x+y*in.y+z*in.z;                              }

        Vector3 Cross(const Vector3 in)
                                    const   { return Vector3( y*in.z-z*in.y, z*in.x-x*in.z, x*in.y-y*in.x ); }

        static double Angle ( const Vector3 &left, const Vector3 &right )
                                            { T cos = left.Normalized().Dot(right.Normalized());
                                              return acos((double)cos); }
};


/**
 * @file point_vec.h
 * @author Marco Livesu (marco.livesu@gmail.com)
 * @brief The Point3 class implements a simple point
 * embedded in R3
 */

template <typename T> class Point3
{
    public:

        T x, y, z;

        Point3  (T _x=0.0, T _y=0.0, T _z=0.0) { Set(_x,_y,_z);    }
        void Set(T _x=0.0, T _y=0.0, T _z=0.0) { x=_x; y=_y; z=_z; }

        T & operator[](int id)       { return *(&x+id); }
        T & operator[](int id) const { return *(&x+id); }

        Point3  operator+  (const Point3 in) { return Point3(x+in.x, y+in.y, z+in.z );   }
        Point3  operator+  (const Vector3<T> in){ return Point3(x+in.x, y+in.y, z+in.z );   }
        Vector3<T> operator-  (const Point3 in) { return Vector3<T>(x-in.x, y-in.y, z-in.z ); }
        Point3  operator*  (const T      in) { return Point3(x*in, y*in, z*in);          }
        Point3  operator/  (const T      in) { return Point3(x/in, y/in, z/in);          }
        Point3  operator+= (const Point3 in) { x+=in.x; y+=in.y; z+=in.z; return *this;  }
        Point3  operator+= (const Vector3<T> in){ x+=in.x; y+=in.y; z+=in.z; return *this;  }
        Point3  operator-= (const Point3 in) { x-=in.x; y-=in.y; z-=in.z; return *this;   }
        Point3  operator*= (const T      in) { x*=in; y*=in; z*=in; return *this;         }
        Point3  operator/= (const T      in) { x/=in; y/=in; z/=in; return *this;         }
		bool  operator== (const Point3 in)   { return x == in.x && y == in.y && z == in.z;}//same position

        T       Dist (const Point3 in) const { return sqrt ( pow((x-in.x),2) + pow((y-in.y),2) + pow((z-in.z),2) );   }
        Point3  Min(const Point3 in)   const { return Point3( std::min(x,in.x), std::min(y,in.y), std::min(z,in.z) ); }
        Point3  Max(const Point3 in)   const { return Point3( std::max(x,in.x), std::max(y,in.y), std::max(z,in.z) ); }
        static Point3  MidPoint( const Point3 left, const Point3 right )
                                             { return Point3( (left.x + right.x)/2.0,
                                                              (left.y + right.y)/2.0,
                                                              (left.z + right.z)/2.0 );}

        void print() { std::cout << x << "  " << y << "  " << z << endl; }
};

typedef Point3<float>   Point3f;
typedef Point3<double>  Point3d;
typedef Vector3<float>  Vector3f;
typedef Vector3<double> Vector3d;

}
