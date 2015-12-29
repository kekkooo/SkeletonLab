#pragma once

#ifdef use_cgal
	#include <CGAL/AABB_tree.h> // must be inserted before kernel
	#include <CGAL/AABB_traits.h>
	#include <CGAL/Polyhedron_3.h>
	#include <CGAL/Polyhedron_incremental_builder_3.h>
	#include <CGAL/AABB_polyhedron_triangle_primitive.h>
	#include <CGAL/AABB_traits.h>
	#include <CGAL/AABB_triangle_primitive.h>
	#include <CGAL/Simple_cartesian.h>
#endif
#include <assert.h>
#include <QString>
#include <primitives/BBox.h>
#include <primitives/geometry.h>

using namespace std;

namespace RMesh
{
	enum DRAWMODE
	{
		POINTS        = 0x0000000F,
		FLAT          = 0x000000F0,
		SMOOTH        = 0x00000F00,
		WIREFRAME     = 0x0000F000,
        PCA           = 0x000F0000,
		LOOPS         = 0x00F00000,
		MESH          = 0xF0000000
	};

#ifdef use_cgal
	typedef CGAL::Simple_cartesian<double>                                                  K;

	typedef K::Point_3                                                                      CGAL_Point;
	typedef K::Triangle_3                                                                   CGAL_Triangle;
	typedef K::Line_3                                                                       CGAL_Line;
    typedef K::Segment_3                                                                    CGAL_Segment;
	typedef K::Plane_3                                                                      CGAL_Plane;
	typedef K::Vector_3                                                                     CGAL_Vector;
	typedef std::list<CGAL_Triangle>::iterator                                              CGAL_Triangle_Iterator;
#endif

struct MeshTriangle
{
	int p0, p1, p2;
	MeshTriangle( int index0, int index1, int index2 )
	{
		assert( index0 >= 0 );
		assert( index1 >= 0 );
		assert( index2 >= 0 );
		p0 = index0; p1 = index1; p2 = index2;
	}
};

struct MeshPolygon
{
	vector<int> vertices;
	MeshPolygon() { }
	MeshPolygon( int p0, int p1, int p2 )
	{
		vertices.push_back(p0);
		vertices.push_back(p1);
		vertices.push_back(p2);
	}
	MeshPolygon( int p0, int p1, int p2, int p3 )
	{
		vertices.push_back(p0);
		vertices.push_back(p1);
		vertices.push_back(p2);
		vertices.push_back(p3);
	}
	int no_vertices( ) { return vertices.size(); }
};

struct MeshPCA
{
	Primitives::Vector3d up, down, left, right, front, back;
    Primitives::Point3d center;
    double size;
	bool is_valid;
	MeshPCA() { is_valid = false; }
};


class mesh
{
public:
	mesh();

	static  void cube               ( QString filename, mesh &m    );
	static  void load               ( QString filename, mesh &m    );
	static  void buildNormals       ( mesh &m );
			void clear              ();


#ifdef use_cgal
    static  MeshPCA buildPCA( vector<Primitives::Point3d> &points );
	static  void buildPCA           ( mesh &m );

	static  void getCGALTriangles   ( mesh &m, vector< CGAL_Triangle>& cgal_triangles );
	static  void getCGALTriangles   ( mesh &m, list< CGAL_Triangle>& cgal_triangles );
#endif

	// attributes
//    vector< MeshTriangle >          triangles;
	vector< MeshPolygon >           polygons;
	vector< Primitives::Point3d >   vertices;
	vector< Primitives::Vector3d>   v_normals;
	vector< Primitives::Vector3d>   f_normals;
	Primitives::BBoxD               bbox;
	MeshPCA                         axes;
};
#ifdef use_cgal
static inline CGAL_Point build_CGAL_Point( Primitives::Point3d p)    { return CGAL_Point(p[0], p[1], p[2]); }

static inline CGAL_Vector build_CGAL_Vector( Primitives::Vector3d v ) { return CGAL_Vector(v[0], v[1], v[2]); }

static inline Primitives::Point3d build_Point3d( CGAL_Point p )
{
	return Primitives::Point3d( p.x(), p.y(), p.z() );
}


static inline Primitives::Vector3d build_Vector3d ( CGAL_Vector v )
{
	return Primitives::Vector3d( v.x(), v.y(), v.z() );
}

static inline CGAL_Triangle build_CGAL_Triangle( mesh &m, MeshTriangle &t )
{
	return CGAL_Triangle( build_CGAL_Point( m.vertices[t.p0] ),
						  build_CGAL_Point( m.vertices[t.p1] ),
						  build_CGAL_Point( m.vertices[t.p2] ));
}
#endif

}
