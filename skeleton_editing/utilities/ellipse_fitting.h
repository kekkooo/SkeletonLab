#ifndef ELLIPSE_FITTING_H
#define ELLIPSE_FITTING_H

#include <mesh/mesh.h> // I need CGAL definitions from here

namespace RMesh{
namespace Utils{


void FitEllipseDirect(const RMesh::CGAL_Plane plane, const std::vector<RMesh::CGAL_Point>& points,
                       RMesh::CGAL_Point& center, double& rmin, double& rmax );


}
}

#endif // ELLIPSE_FITTING_H
