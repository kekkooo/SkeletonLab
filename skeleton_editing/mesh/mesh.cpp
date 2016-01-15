#include "mesh.h"
#include "mesh/import.h"

#ifdef use_cgal
    #include <CGAL/linear_least_squares_fitting_3.h>
    #include <CGAL/linear_least_squares_fitting_points_3.h>
#endif


RMesh::mesh::mesh()
{
}

void RMesh::mesh::buildNormals(mesh &m)
{
    for( int i = 0; i < m.polygons.size(); i++ )
    {
        MeshPolygon &t = m.polygons[i];
        assert( t.no_vertices() >= 3 );
        if( t.no_vertices() == 3 )
        {
            Primitives::Vector3d n = ( m.vertices[t.vertices[1]] - m.vertices[t.vertices[0]]).Cross
                                        ( m.vertices[t.vertices[2]] - m.vertices[t.vertices[0]] );
            m.f_normals.push_back( n );
        }
        else
        {
            Primitives::Vector3d n1 = ( m.vertices[t.vertices[1]] - m.vertices[t.vertices[0]]).Cross
                                        ( m.vertices[t.vertices[2]] - m.vertices[t.vertices[0]] );

            Primitives::Vector3d n2 = ( m.vertices[t.vertices[2]] - m.vertices[t.vertices[0]]).Cross
                                        ( m.vertices[t.vertices[3]] - m.vertices[t.vertices[0]] );

            m.f_normals.push_back( ( n1 + n2 )/2.0 );
        }
    }
}

void RMesh::mesh::buildColors(mesh &m){
    m.f_colors = std::vector<Primitives::ColorF>();
    Primitives::ColorF c( 0.5, 0.5, 0.5 );
    //Primitives::ColorF c( 1.0, 0.0, 0.0 );
    for( int i = 0; i < m.polygons.size(); i++ ){
        m.f_colors.push_back( c );
    }
}

void RMesh::mesh::clear()
{
    this->vertices.clear();
    this->polygons.clear();
    this->f_normals.clear();
    this->f_colors.clear();
    this->bbox = Primitives::BBoxD();
}

void RMesh::mesh::cube(QString filename, mesh &m)
{
    assert( &m != NULL );

//    float plus  = 0.5;
//    float minus = -plus;
    float plus  = 2.5;
    float minus = 1.0;

    m.vertices.reserve(8);
    m.polygons.reserve(12);

    // initialize points
    m.vertices.push_back( Primitives::Point3d( minus, plus, plus   ));
    m.vertices.push_back( Primitives::Point3d( minus, plus, minus  ));
    m.vertices.push_back( Primitives::Point3d( plus, plus, minus   ));
    m.vertices.push_back( Primitives::Point3d( plus, plus, plus    ));
    m.vertices.push_back( Primitives::Point3d( minus, minus, plus  ));
    m.vertices.push_back( Primitives::Point3d( minus, minus, minus ));
    m.vertices.push_back( Primitives::Point3d( plus, minus, minus  ));
    m.vertices.push_back( Primitives::Point3d( plus, minus, plus   ));


    m.polygons.push_back( MeshPolygon( 2, 1, 3 ));
    m.polygons.push_back( MeshPolygon( 3, 1, 0 ));
    m.polygons.push_back( MeshPolygon( 1, 5, 0 ));
    m.polygons.push_back( MeshPolygon( 0, 5, 4 ));
    m.polygons.push_back( MeshPolygon( 1, 2, 5 ));
    m.polygons.push_back( MeshPolygon( 5, 2, 6 ));
    m.polygons.push_back( MeshPolygon( 3, 0, 7 ));
    m.polygons.push_back( MeshPolygon( 7, 0, 4 ));
    m.polygons.push_back( MeshPolygon( 7, 2, 3 ));
    m.polygons.push_back( MeshPolygon( 2, 7, 6 ));
    m.polygons.push_back( MeshPolygon( 7, 5, 6 ));
    m.polygons.push_back( MeshPolygon( 5, 7, 4 ));

    buildNormals(m);

    vector < Primitives::Point3d > points_for_bbox;
    for (int i = 0; i < m.vertices.size(); ++i)
    {
        points_for_bbox.push_back( Primitives::Point3d( m.vertices[i][0], m.vertices[i][1], m.vertices[i][2] ));
    }
    m.bbox = Primitives::BBoxD();
    m.bbox.Set(points_for_bbox);
    std::cout << "bbox center : " << m.bbox.Center()[0] << ", " << m.bbox.Center()[1] << ", " << m.bbox.Center()[1]  << endl;
}

void RMesh::mesh::load(QString filename, RMesh::mesh &m)
{
#ifdef use_boost
//    cube( QString::number(0), m );

    QString off = QString::fromStdString("off");
    QString obj = QString::fromStdString("obj");
    QString ext = filename.right(3);


    if ( ext.toLower().compare( obj ) == 0 )
    {
        cout << "loading obj : " << filename.toStdString() << endl;
        RMesh::Importer::from_obj(m, filename);
    }
    else if ( ext.toLower().compare( off ) == 0 )
    {
        cout << "loading off : " << filename.toStdString() << endl;
        RMesh::Importer::from_off(m, filename);
    }
    else cout << endl << ext.toStdString() << " is not a supported file format";

    buildPCA( m );
    buildColors( m );
    m.bbox.Set( m.vertices );
    m.axes.center = m.bbox.Center();

#endif
}

#ifdef use_cgal
void RMesh::mesh::getCGALTriangles(RMesh::mesh &m, vector<RMesh::CGAL_Triangle> &cgal_triangles)
{
    cout << "this feature needs to be properly tested";
//    assert(false);

    cgal_triangles.clear();
    cgal_triangles.reserve(m.polygons.size());

    for( int i = 0; i < m.polygons.size(); ++i)
    {
        if( m.polygons[i].no_vertices() == 3 )
        {
            MeshTriangle t( m.polygons[i].vertices[0], m.polygons[i].vertices[1], m.polygons[i].vertices[2]);
            cgal_triangles.push_back( RMesh::build_CGAL_Triangle( m, t ));
        }
        else if( m.polygons[i].no_vertices() == 4 )
        {
            MeshTriangle t1( m.polygons[i].vertices[0], m.polygons[i].vertices[1], m.polygons[i].vertices[2]);
            MeshTriangle t2( m.polygons[i].vertices[0], m.polygons[i].vertices[2], m.polygons[i].vertices[3]);
            cgal_triangles.push_back( RMesh::build_CGAL_Triangle( m, t1 ));
            cgal_triangles.push_back( RMesh::build_CGAL_Triangle( m, t2 ));
        }
        else { cout << "general polygons not supported"; assert(false); }
    }
}

void RMesh::mesh::getCGALTriangles ( mesh &m, list< CGAL_Triangle>& cgal_triangles )
{
    cgal_triangles.clear();
//    cgal_triangles.reserve(m.polygons.size());

    for( int i = 0; i < m.polygons.size(); ++i)
    {
        if( m.polygons[i].no_vertices() == 3 )
        {
            MeshTriangle t( m.polygons[i].vertices[0], m.polygons[i].vertices[1], m.polygons[i].vertices[2]);
            cgal_triangles.push_back( RMesh::build_CGAL_Triangle( m, t ));
        }
        else if( m.polygons[i].no_vertices() == 4 )
        {
            MeshTriangle t1( m.polygons[i].vertices[0], m.polygons[i].vertices[1], m.polygons[i].vertices[2]);
            MeshTriangle t2( m.polygons[i].vertices[0], m.polygons[i].vertices[2], m.polygons[i].vertices[3]);
            cgal_triangles.push_back( RMesh::build_CGAL_Triangle( m, t1 ));
            cgal_triangles.push_back( RMesh::build_CGAL_Triangle( m, t2 ));
        }
        else { cout << "general polygons not supported"; assert(false); }
    }
}

void RMesh::mesh::buildPCA(RMesh::mesh &m)
{
    vector<CGAL_Triangle> triangles;
    getCGALTriangles(m, triangles);
    CGAL_Line line;
    CGAL_Plane plane;

    // fit plane to whole triangles
    linear_least_squares_fitting_3( triangles.begin(), triangles.end(), plane,CGAL::Dimension_tag<2>());

    // fit line to triangle vertices
    linear_least_squares_fitting_3( triangles.begin(), triangles.end(), line, CGAL::Dimension_tag<0>());

    m.axes.up       = build_Vector3d( line.direction().vector() );
    m.axes.down     = m.axes.up * (-1.0);
    m.axes.front    = build_Vector3d( plane.orthogonal_direction().vector() );
    m.axes.back     = m.axes.front * (-1.0);
    m.axes.right    = m.axes.up.Cross( m.axes.front );
    m.axes.left     = m.axes.right * (-1.0);
    m.axes.is_valid = true;
}

RMesh::MeshPCA RMesh::mesh::buildPCA( vector<Primitives::Point3d> &points )
{
    vector<CGAL_Point> cgal_points;
    CGAL_Line line;
    CGAL_Plane plane;
    MeshPCA pca;
    Primitives::BBoxD bbox;

    bbox.Set(points);
    for( int i = 0; i < points.size(); ++i )
    {
        cgal_points.push_back( build_CGAL_Point( points[i] ));
    }
    linear_least_squares_fitting_3( cgal_points.begin(), cgal_points.end(), plane,CGAL::Dimension_tag<0>());

    // fit line to triangle vertices
    linear_least_squares_fitting_3( cgal_points.begin(), cgal_points.end(), line, CGAL::Dimension_tag<0>());

    pca.up       = build_Vector3d( line.direction().vector() );
    pca.down     = pca.up * (-1.0);
    pca.front    = build_Vector3d( plane.orthogonal_direction().vector() );
    pca.back     = pca.front * (-1.0);
    pca.right    = pca.up.Cross( pca.front );
    pca.left     = pca.right * (-1.0);
    pca.center   = bbox.Center();
    pca.is_valid = true;

    return pca;
}

#endif
