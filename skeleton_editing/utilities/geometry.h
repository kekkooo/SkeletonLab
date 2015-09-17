#pragma once

#include <mesh/base.h>
#include <common.h>
#include <cmath>


#define PRINT_POINT(P) (std::cout << (P)[0] << "\t" << (P)[1] << "\t" << (P)[2] << std::endl)

#define EPSILON 1e-5


namespace myGeometry
{

        class Plane
        {

                public:         Plane(Mesh::Point point, Mesh::Point normal)
                                {
                                        normal.Normalize();

                                        _a = normal[0];
                                        _b = normal[1];
                                        _c = normal[2];
                                        _d = normal[0] * point[0] +
                                             normal[1] * point[1] +
                                             normal[2] * point[2];
                                }


                                Plane(Mesh::Point p0, Mesh::Point p1, Mesh::Point p2)
                                {
                                        float dx1 = p1[0] - p0[0];
                                        float dx2 = p2[0] - p0[0];
                                        float dy1 = p1[1] - p0[1];
                                        float dy2 = p2[1] - p0[1];
                                        float dz1 = p1[2] - p0[2];
                                        float dz2 = p2[2] - p0[2];

                                        float A = (dy1 * dz2) - (dy2 * dz1);
                                        float B = (dx1 * dz2) - (dx2 * dz1);
                                        float C = (dx1 * dy2) - (dx2 * dy1);

                                        _a =  A;
                                        _b = -B;
                                        _c =  C;
                                        _d = A * p0[0] - B * p0[1] + C * p0[2];

                                        float norm = sqrt(_a*_a + _b*_b + _c*_c);
                                        _a /= norm;
                                        _b /= norm;
                                        _c /= norm;
                                        _d /= norm;
                                }

                                static Plane NotNormalizedPlane(Mesh::Point p0, Mesh::Point p1, Mesh::Point p2)
                                {
                                    float dx1 = p1[0] - p0[0];
                                    float dx2 = p2[0] - p0[0];
                                    float dy1 = p1[1] - p0[1];
                                    float dy2 = p2[1] - p0[1];
                                    float dz1 = p1[2] - p0[2];
                                    float dz2 = p2[2] - p0[2];

                                    float A = (dy1 * dz2) - (dy2 * dz1);
                                    float B = (dx1 * dz2) - (dx2 * dz1);
                                    float C = (dx1 * dy2) - (dx2 * dy1);

                                    Plane alpha = Plane();

                                    alpha._a =  A;
                                    alpha._b = -B;
                                    alpha._c =  C;
                                    alpha._d = A * p0[0] - B * p0[1] + C * p0[2];

                                    return alpha;
                                }


                                Plane() // initialize a null plane
                                {
                                        _a = 0.f;
                                        _b = 0.f;
                                        _c = 0.f;
                                        _d = 0.f;
                                }

                                inline float A() { return _a; }
                                inline float B() { return _b; }
                                inline float C() { return _c; }
                                inline float D() { return _d; }

                                inline Mesh::Point normal() { return Mesh::Point(_a, _b, _c); }

                                inline bool isNull() { return (normal().Norm() == 0); }

                                inline void invertOrientation()
                                {
                                        _a *= -1;
                                        _b *= -1;
                                        _c *= -1;
                                        _d *= -1;
                                }

                                double operator()(Mesh::Point p) { return (p[0] * _a + p[1] * _b + p[2] * _c - _d); }

                private:        double  _a, _b, _c, _d;

        };



        class AAQuad // Axis Aligned Quad
        {

                public:

                        AAQuad() {}

                        AAQuad(Mesh::Point p0, Mesh::Point p1, Mesh::Point p2, Mesh::Point p3)
                        {

                                // Axis alignment check
                                //
                                assert(( fequals( p0[0], p1[0] ) && fequals( p1[0], p2[0] ) && fequals( p2[0], p3[0] )) ||
                                       ( fequals( p0[1], p1[1] ) && fequals( p1[1], p2[1] ) && fequals( p2[1], p3[1] )) ||
                                       ( fequals( p0[2], p1[2] ) && fequals( p1[2], p2[2] ) && fequals( p2[2], p3[2] )));

                                
//                                assert((p0[0] == p1[0] && p1[0] == p2[0] && p2[0] == p3[0]) ||
//                                       (p0[1] == p1[1] && p1[1] == p2[1] && p2[1] == p3[1]) ||
//                                       (p0[2] == p1[2] && p1[2] == p2[2] && p2[2] == p3[2]));
                                       

                                points[0] = p0;
                                points[1] = p1;
                                points[2] = p2;
                                points[3] = p3;

                                axis = -1;
                                for(int i=0; i<3; ++i)
                                {
                                        min[i] = std::min(p0[i], p1[i]);
                                        min[i] = std::min(min[i], p2[i]);

                                        max[i] = std::max(p0[i], p1[i]);
                                        max[i] = std::max(max[i], p2[i]);
                                        
                                        if( fequals(max[i], min[i]) ) { axis = i; }
                                        //std::cout << "abs value : " << fabs(max[i] - min[i]) << std::endl;
                                }
                                assert(axis != -1);

                                supporting_plane = Plane(p0, p1, p2);
                        }


                        bool contains(Mesh::Point query)
                        {
                                if (fabs(supporting_plane(query)) > EPSILON)
                                {
                                        std::cout << supporting_plane.A() << "\t" << supporting_plane.B() << "\t" << supporting_plane.C() << "\t" << supporting_plane.D() << std::endl;
                                        std::cout << supporting_plane(query) << std::endl;
                                        std::cout << supporting_plane(points[0]) << std::endl;
                                        std::cout << supporting_plane(points[1]) << std::endl;
                                        std::cout << supporting_plane(points[2]) << std::endl;
                                        std::cout << supporting_plane(points[3]) << std::endl;
                                        assert(0);
                                }

                                for(int i=0; i<3; ++i)
                                {
                                        if (i == axis) continue;
                                        if (query[i] < min[i] || query[i] >= max[i]) return false;
                                }

                                return true;
                        }


                        //  x = Ax + l * t
                        //  y = Ay + m * t
                        //  z = Az + n * t
                        //
                        bool line_intersects(Mesh::Point A, Mesh::Point dir, Mesh::Point &intersection)
                        {

                                if (dir.dot(supporting_plane.normal()) == 0) return false;

                                float t = (min[axis] - A[axis]) / dir[axis];

                                if (t < 0.f) return false;

                                intersection = A + (dir * t);

                                return contains(intersection);
                        }


                        Plane supporting_plane;
                        Mesh::Point points[4];  // quad's point list

                private:

                        Mesh::Point min, max;   // quad's extrema
                        int axis;               // axis to which the quad is aligned
        };



        // returns the norm of a vector
        static float normCalc( float v1, float v2, float v3 ){

            return sqrt( v1 * v2 + v2 * v2 + v3 * v3 );
        }


        // returns the distance between the line AB and the given point
        //
        static float pointLineDistance(Mesh::Point A, Mesh::Point B, Mesh::Point p)
        {

                Mesh::Point u = (B - A).Normalize(),
                            v = p - A;

                return (u ^ v).Norm();
        }        




        // returns the distance between the segment AB and the given point
        //
        //  plane(A,->)
        //       |             |          ||    case 1) plane(A,->)[p] +
        //  -/+  |     +/+     |  +/-     ||            plane(B,<-)[p] -
        //       |             |          ||
        //     A +-------------+ B        ||    case 2) plane(A,->)[p] -
        //       |             |          ||            plane(B,<-)[p] +
        //  -/+  |     +/+     |  +/-     ||
        //       |             |          ||    case 3) plane(A,->)[p] +
        //              plane(B,<-)       ||            plane(B,<-)[p] +
        //
        static float pointSegmentDistance(Mesh::Point A, Mesh::Point B, Mesh::Point p)
        {

                // case -/+
                //
                Plane pA(A, B - A);

                if (pA(p) < 0) return vcg::Distance(A, p);


                // case +/-
                //
                Plane pB(B, A - B);

                if (pB(p) < 0) return vcg::Distance(B, p);


                // case +/+
                //
                return pointLineDistance(A, B, p);
        }



        // given the segment AB and a third point C, it returns the geometry transformation that makes C rotate
        // around the axis (AB)^(BC) in such a way that it lies along the line passing through A and B
        //
        static vcg::Matrix44f alignByRotation(Mesh::Point A, Mesh::Point B, Mesh::Point C)
        {

                Mesh::Point u = A - B,
                            v = B - C,

                            axis = (u ^ v).Normalize();

                // get the rotation amount needed to align the vectors
                //
                float angle = vcg::Angle(u, v);

                // compose the rotation matrix
                //
                vcg::Matrix44f t1, t2, rot;
                t1.SetTranslate(-B);
                t2.SetTranslate( B);
                rot.SetRotateRad(angle, axis);

                return t2 * rot * t1;
        }


        static Mesh::Point principalDirection(std::vector<Mesh::Point> points)
        {

                // compute the covariance matrix
                // (the mesh i sassumed to be centered in the origin)
                //
                vcg::Matrix33f covmat, A;
                covmat.SetZero();

                for (int i=0; i<(int)points.size(); ++i)
                {
                        Mesh::Point p = points[i];
                        std::cout << "\t intersection " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
                        A.OuterProduct(points[i], points[i]);
                        covmat += A;
                }


                // eigenvalues and eigenvectors computation and sorting (from the biggest to the lowest)
                //
                vcg::Matrix33f eigenvectors;    // eigenvectors matrix (column)
                Mesh::Point eigenvalues;        // eigenvalues
                int iterations;                 // Jacobi's iterations

                vcg::Jacobi<vcg::Matrix33f,Mesh::Point>(covmat, eigenvalues, eigenvectors, iterations);
                vcg::SortEigenvaluesAndEigenvectors<vcg::Matrix33f,Mesh::Point>(eigenvalues, eigenvectors);

                return Mesh::Point(eigenvectors[0][0], eigenvectors[1][0], eigenvectors[2][0]);
        }
}
