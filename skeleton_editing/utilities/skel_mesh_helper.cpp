#ifdef use_cgal
#include "skel_mesh_helper.h"
#include <queue>>
#include <CGAL/intersections.h>
#include "SQEM.h"
#include "seqm_helper.h"


RMesh::Utils::skel_mesh_helper::skel_mesh_helper()
{
    _mesh = NULL;
}

void RMesh::Utils::skel_mesh_helper::setMesh( mesh *m )
{
    _mesh = m;
    RMesh::mesh::getCGALTriangles( (*_mesh), trilist );
    AABB_Tree = new CGAL_AABB_Tree( trilist.begin(), trilist.end( ));
}

void Utils::skel_mesh_helper::checkSkeletonInsideMesh(CurveSkeleton &cs)
{

    for( int i = 0; i < cs.points.size(); ++i)
    {
        if( is_inside( (*AABB_Tree), cs.points[i].coord ))
        {
            cs.points[i].setColor( ColorF::Green());
        }
        else
        {
            cs.points[i].setColor( ColorF::Yellow( ));
        }
    }

}

void Utils::skel_mesh_helper::put_back_inside(CurveSkeleton *cs)
{
    for( int i = 0; i < cs->points.size(); ++i)
    {
        if( !is_inside( (*AABB_Tree), cs->points[i].coord ))
        {
            CGAL_Point test = RMesh::build_CGAL_Point( cs->points[i].coord );

            CGAL_AABB_Tree::Point_and_primitive_id point_and_primitive
                    = AABB_Tree->closest_point_and_primitive( test );
            std::list<CGAL_Triangle>::iterator f = point_and_primitive.second;
            CGAL_Triangle t = *f;
            CGAL_Vector nn = t.supporting_plane().orthogonal_vector() * ( -1.0);
            cs->points[i].coord = build_Point3d( point_and_primitive.first + nn );
        }
    }
}


void Utils::skel_mesh_helper::re_center( CurveSkeleton *&cs, bool refit )
{

    cout << "recentering" << endl;
    map<int, Point3d> new_points;
    map<int, double>  new_radii;
    vector<CGAL_Point>skel_points;
    for (int skel_i = 0; skel_i < cs->points.size(); ++skel_i)
    {
        assert( skel_i == cs->points[skel_i].id );
        skel_points.push_back( build_CGAL_Point(cs->points[skel_i].coord ));
    }

    cout << "points saved # there are " << cs->bones.size() << " bones"  << endl;

    foreach (Skel::Bone b, cs->bones)
    {
        cout << "bone" << endl;
        // start always from a joint
        if ( cs->points[b[0]].isLeaf() ) b.reverse();

        CGAL_Point curr, prev, next;

        for(int i = 0; i < b.size(); ++i )
        {            

            SkelPoint& p = cs->points[b[i]];
            curr = skel_points[b[i]];
            CGAL_Vector skel_dir;

            if( !is_inside( (*AABB_Tree), cs->points[b[i]].coord )) { continue; }

            if( p.isJoint() || p.isLeaf()){
                if( p.isLeaf() ) { skel_dir = skel_points[b.size() - 2] - skel_points[b.size() - 1]; }
                if( p.isJoint())
                {
                    prev = skel_points[b[i-1]];
                    next = skel_points[b[i+1]];
                    CGAL_Vector dir1 = curr - prev,
                                dir2 = next - curr;
                    skel_dir = dir1 + dir2;
                }
                Point3d centroid( 0.0, 0.0, 0.0 );
                double mean_squared_radius;
                CGAL_Plane plane( curr, skel_dir.direction().vector() );
                vector<ir_pair> pairs;
                compute_paired_intersections( plane, curr, pairs);
                int valid_pairs = 0;
                for( int pi = 0; pi < pairs.size(); ++pi ){
                    const ir_pair& pair = pairs[pi];
                    if( !pair.first.is_valid || !pair.second.is_valid ) { continue; }

                    Point3d midpoint = ( build_Point3d( pair.first.point ) + build_Point3d(  pair.second.point )) / 2.0;
                    mean_squared_radius += CGAL::squared_distance( pair.first.point, pair.second.point );
                    centroid += midpoint;
                    ++valid_pairs;
                }
                centroid /= (double)valid_pairs;
                mean_squared_radius /= (double)valid_pairs;
                new_points[b[i]] = centroid;
                new_radii[b[i]] = sqrt(mean_squared_radius)/2.0;
            }
        }
    }
    foreach( Skel::SkelPoint p, cs->points ){
        if( !p.isBranchingNode() ) { continue; }
        if( p.isDeleted())         { continue; }
        if( !is_inside( (*AABB_Tree), p.coord )) { continue; }

        vector<Point3d> centroids;
        vector<double>  radii;
        for( int ni = 0; ni < p.neighbors.size(); ++ni){

            CGAL_Point curr = skel_points[p.id];
            CGAL_Vector skel_dir = skel_points[p.neighbors[ni]] - skel_points[p.id];
            Point3d centroid( 0.0, 0.0, 0.0 );
            double mean_squared_radius;
            CGAL_Plane plane( curr, skel_dir.direction().vector() );
            vector<ir_pair> pairs;
            compute_paired_intersections( plane, curr, pairs);
            int valid_pairs = 0;
            for( int pi = 0; pi < pairs.size(); ++pi ){
                const ir_pair& pair = pairs[pi];
                if( !pair.first.is_valid || !pair.second.is_valid ) { continue; }

                Point3d midpoint = ( build_Point3d( pair.first.point ) + build_Point3d(  pair.second.point )) / 2.0;
                mean_squared_radius += CGAL::squared_distance( pair.first.point, pair.second.point );
                centroid += midpoint;
                ++valid_pairs;
            }
            centroid /= (double)valid_pairs;
            centroids.push_back( centroid );
            radii.push_back( sqrt( mean_squared_radius ) / ( 2.0 * (double)valid_pairs ));
        }
        Point3d centroid( 0.0, 0.0, 0.0 );
        double  mean_squared_radius = 0.0;
        for( int ci = 0; ci < centroids.size(); ++ci){
            centroid += centroids[ci];
            mean_squared_radius += radii[ci];
        }
        centroid /= (double)centroids.size();
        new_points[p.id] = centroid;
        new_radii[p.id ] = ( mean_squared_radius / (double)centroids.size() );
    }


    typedef map<int, Point3d>::iterator mapi;
    for( mapi it = new_points.begin(); it != new_points.end(); ++it )
    {
        assert( new_points.count( it->first ) != 0);
        cs->points[it->first].coord  = it->second;
        if( refit ){
            cs->points[it->first].radius = new_radii[it->first];
            assert( new_radii.count( it->first ) != 0);
        }
    }

}


bool Utils::skel_mesh_helper::is_inside( CGAL_Point query ){
    CGAL_AABB_Tree::Point_and_primitive_id point_and_primitive
            = this->AABB_Tree->closest_point_and_primitive( query );

    std::list<CGAL_Triangle>::iterator f = point_and_primitive.second;
    return (*f).supporting_plane().has_on_negative_side( query );
}

bool Utils::skel_mesh_helper::is_inside(Utils::CGAL_AABB_Tree &aabb, Point3d query )
{
    CGAL_Point test = RMesh::build_CGAL_Point( query );

    CGAL_AABB_Tree::Point_and_primitive_id point_and_primitive
            = aabb.closest_point_and_primitive( test );

    std::list<CGAL_Triangle>::iterator f = point_and_primitive.second;
    K::Plane_3 p = (*f).supporting_plane();
    return p.has_on_negative_side( test );
}

RMesh::Utils::IntersectionResult Utils::skel_mesh_helper::mesh_ray_intersection( CGAL_Point p, CGAL_Point q )
{
    CGAL_Ray query( p, q );

    std::list<Ray_intersection> intersections;
    AABB_Tree->all_intersections( query,  std::back_inserter( intersections ) );

    float min_dist      = MAXFLOAT;
    CGAL_Point source   = query.source();
    IntersectionResult result;

    for(std::list<Ray_intersection>::iterator it = intersections.begin(); it != intersections.end(); ++it)
    {
        Ray_intersection obj = *it;

        if ( CGAL_Point *curr = boost::get<CGAL_Point>( &( obj->first )))
        {
            CGAL_Point current( curr->x(), curr->y(), curr->z() );
            float dist = CGAL::squared_distance( current, source );
            std::list<CGAL_Triangle>::iterator tit = obj->second;
            CGAL_Triangle t = *tit;

//            if( t.supporting_plane().has_on( source )
//             || t.supporting_plane().has_on_negative_side( source ))
//            {
                if ( dist < min_dist )
                {
                    min_dist        = dist;
                    result.point    = current;
                    result.face     = t;
                    result.is_valid = true;
                }
//            }
        }
    }
    return result;
}


void Utils::skel_mesh_helper::compute_paired_intersections( CGAL_Plane plane, CGAL_Point curr,
    vector<Utils::ir_pair> &pairs){

    std::vector< CGAL_Vector > ray_dirs;
    ray_dirs.push_back( plane.base1().direction().vector( ));
    ray_dirs.push_back( ray_dirs[0] * ( -1.0 ));
    ray_dirs.push_back( plane.base2().direction().vector( ));
    ray_dirs.push_back( ray_dirs[2] * ( -1.0 ));

    ray_dirs.push_back( ray_dirs[0] + ray_dirs[2] );
    ray_dirs.push_back( ray_dirs[4] * ( -1.0 ));
    ray_dirs.push_back( ray_dirs[0] + ray_dirs[3] );
    ray_dirs.push_back( ray_dirs[6] * ( -1.0 ));

    ray_dirs.push_back( ray_dirs[0] + ray_dirs[4] );
    ray_dirs.push_back( ray_dirs[8] * ( -1.0 ));
    ray_dirs.push_back( ray_dirs[4] + ray_dirs[2] );
    ray_dirs.push_back( ray_dirs[10] * ( -1.0 ));

    ray_dirs.push_back( ray_dirs[1] + ray_dirs[7] );
    ray_dirs.push_back( ray_dirs[12] * ( -1.0 ));
    ray_dirs.push_back( ray_dirs[2] + ray_dirs[7] );
    ray_dirs.push_back( ray_dirs[14] * ( -1.0 ));

    double bbox_diag = _mesh->bbox.Diagonal();
    vector<Utils::IntersectionResult> vir;
    for( int ri = 0; ri < ray_dirs.size(); ++ri )
    {
        CGAL_Vector         dir     = ray_dirs[ri];
                            dir     = dir.direction().vector() * bbox_diag;
        CGAL_Point          dest    = curr + dir;
        IntersectionResult  ir      = mesh_ray_intersection( curr, dest );
        if( ir.is_valid )
        {
            if( ir.face.supporting_plane().has_on( curr )
             || ir.face.supporting_plane().has_on_negative_side( curr ))
            {
                vir.push_back(ir);
            }
        }
    }
    for( int ri = 0; ri < vir.size() - 1; ri += 2 )
    {
        pairs.push_back(make_pair( vir[ri], vir[ri+1]));
    }
}

void Utils::skel_mesh_helper::compute_intersections(CGAL_Plane plane,
                                                    CGAL_Point curr, vector<Utils::IntersectionResult> &vir)
{
    std::vector< CGAL_Vector > ray_dirs;
    ray_dirs.push_back( plane.base1().direction().vector( ));
    ray_dirs.push_back( plane.base2().direction().vector( ));
    ray_dirs.push_back( plane.base1().direction().vector() * ( -1.0 ));
    ray_dirs.push_back( plane.base2().direction().vector() * ( -1.0 ));
    ray_dirs.push_back( ray_dirs[0] + ray_dirs[1] );
    ray_dirs.push_back( ray_dirs[1] + ray_dirs[2] );
    ray_dirs.push_back( ray_dirs[2] + ray_dirs[3] );
    ray_dirs.push_back( ray_dirs[3] + ray_dirs[0] );
    ray_dirs.push_back( ray_dirs[0] + ray_dirs[4] );
    ray_dirs.push_back( ray_dirs[1] + ray_dirs[4] );
    ray_dirs.push_back( ray_dirs[1] + ray_dirs[5] );
    ray_dirs.push_back( ray_dirs[5] + ray_dirs[2] );
    ray_dirs.push_back( ray_dirs[2] + ray_dirs[6] );
    ray_dirs.push_back( ray_dirs[6] + ray_dirs[3] );
    ray_dirs.push_back( ray_dirs[3] + ray_dirs[7] );
    ray_dirs.push_back( ray_dirs[7] + ray_dirs[0] );

    double bbox_diag = _mesh->bbox.Diagonal();
    for( int ri = 0; ri < ray_dirs.size(); ++ri )
    {
        CGAL_Vector         dir     = ray_dirs[ri];
                            dir     = dir.direction().vector() * bbox_diag;
        CGAL_Point          dest    = curr + dir;
        IntersectionResult  ir      = mesh_ray_intersection( curr, dest );
        if( ir.is_valid )
        {
            if( ir.face.supporting_plane().has_on( curr )
             || ir.face.supporting_plane().has_on_negative_side( curr ))
            {
                vir.push_back(ir);
            }
        }
    }
}


void Utils::skel_mesh_helper::PlaneMeshIntersectionForSQEM( const CGAL_Plane& plane, const CGAL_Point& origin_point,
                                                            std::vector<SQEM_centering_data>& results ){
    std::list<Plane_Intersection> intersections;
    AABB_Tree->all_intersections( plane, std::back_inserter( intersections ));

    bool query_point_inside = is_inside( origin_point );

    for(std::list<Plane_Intersection>::iterator it = intersections.begin(); it != intersections.end(); ++it)
    {
        Plane_Intersection intersection = *it;
        std::list<CGAL_Triangle>::iterator tri_it = intersection->second;

        CGAL_Triangle t = *tri_it;
        CGAL_Plane sup_plane = t.supporting_plane();

        // if point is inside I should avoid computing wrong faces
        if( query_point_inside && sup_plane.has_on_positive_side( origin_point ) ){ continue; }

        SQEM_centering_data data;

        if ( CGAL_Segment *intersected_segment = boost::get<CGAL_Segment>( &( intersection->first ))){
            data.plane = sup_plane;
            data.distance = CGAL::squared_distance( origin_point, *intersected_segment );
            data.segment = *intersected_segment;
        }
        else{
            std::cout << "intersection is not a segment, please consider to improve your code with point intersections" << std::endl;
            assert(false);
        }

        // color the intesected triangles
        auto pos =std::distance( trilist.begin(), tri_it );
        this->_mesh->f_colors[pos] = Primitives::ColorF::Red();
        data.intersected_triangle_index = pos;

        results.push_back( data );
    }
}


void Utils::skel_mesh_helper::StatisticalAnalisys( std::vector<SQEM_centering_data>& resulting_data)
{
    std::sort( resulting_data.begin(), resulting_data.end(), compare_SQEM_centering_data );
    // statistical reasonings
    double n_minus_one_double = static_cast<double>( resulting_data.size() - 1 );
    double n_double = static_cast<double>( resulting_data.size() );
    double mean     = 0.0, median   = 0.0, sd = 0.0, mad = 0.0;
    std::vector<double> mad_i;

    std::for_each( resulting_data.begin(), resulting_data.end(),
                   [&](SQEM_centering_data &d ){ mean += d.distance ; });

    mean /= n_double;

    // median computation
    median = resulting_data[ ( resulting_data.size() / 2 ) + 1].distance ;
    if( resulting_data.size() % 2 == 0 ){
        median += resulting_data[resulting_data.size() / 2].distance ;
        median /= 2.0;
    }

//    std::cout << "mean distance : " << mean << " and median distance " << median << std::endl;
    // compute standard deviation
    std::for_each( resulting_data.begin(), resulting_data.end(),
                   [&](SQEM_centering_data &d )
                    {
                        sd += (( d.distance - mean ) * ( d.distance - mean )) / n_minus_one_double;
                        mad_i.push_back( std::fabs( d.distance - median ));
                        assert( mad_i.back() >= 0 );
                    });

    sd = std::sqrt( sd );

    // compute mad
    std::sort( mad_i.begin(), mad_i.end() );
    mad = mad_i[( mad_i.size() / 2 ) + 1 ];
    if( resulting_data.size() % 2 == 0 ){
        mad += mad_i[mad_i.size() / 2];
        mad /= 2.0;
    }

//    std::cout << "standard deviation : " << sd << " and mad " << mad << std::endl;

    for( SQEM_centering_data &SQEM_data : resulting_data ){
        SQEM_data.z_score = ( SQEM_data.distance - mean ) / sd;
        SQEM_data.z_score_mod = ( 0.6745 * ( SQEM_data.distance - mean )) / mad;

//        std::cout << SQEM_data.distance << " # z score : " << SQEM_data.z_score << " and modified z score " << SQEM_data.z_score_mod << std::endl;

//        if( SQEM_data.z_score_mod > 3.5 ){
//            //SQEM_data.is_outlier = true;
//        }

        if( SQEM_data.distance > median ){ SQEM_data.is_outlier = true; }
    }
}

void Utils::skel_mesh_helper::ChooseClosestComponent( std::vector<SQEM_centering_data>& resulting_data ){
    std::sort( resulting_data.begin(), resulting_data.end(), compare_SQEM_centering_data );
    size_t closest_tri = resulting_data.front().intersected_triangle_index;
    std::queue<size_t> tris_to_check;
    std::set<size_t> intersected_tris;
    std::set<size_t> good_ones;

    for( const auto& item : resulting_data ){ intersected_tris.insert( item.intersected_triangle_index ); }

    assert( intersected_tris.count( closest_tri ) > 0 );
    tris_to_check.push( closest_tri );

    while( !tris_to_check.empty( )){
        size_t curr = tris_to_check.front();
        tris_to_check.pop();
        assert( intersected_tris.count( curr ) > 0 );
        good_ones.insert( curr );

        for( size_t neighbor : _mesh->polygons[curr].neighbors ){
            if(( intersected_tris.count( neighbor ) > 0 ) &&  ( good_ones.count( neighbor ) == 0 )) {
                tris_to_check.push( neighbor );
            }
        }
    }

    for( auto& item : resulting_data ){
        if( good_ones.count( item.intersected_triangle_index ) == 0 ){ item.is_outlier = true; }
    }

}

void Utils::skel_mesh_helper::centeringWithSQEM( CurveSkeleton &cs, bool refit, int nodeID){

//    assert( nodeID < cs.points.size() );
    ColorF intersected_face_color( 1.0, 0.0, 0.0 );

    // seleziona un punto e questo verrà ricentrato
//    const SkelPoint& p = cs.points.at( nodeID );

    std::vector<Primitives::Point3d> new_coords(cs.points.size());
    std::vector<double> new_radii(cs.points.size());

    for( const SkelPoint& p : cs.points ){

        if( p.isBranchingNode() ){ continue; }
//        if( ( nodeID != -1 ) && p.id != nodeID ) { continue; }

        std::vector<SQEM_centering_data> resulting_data;
        CGAL_Vector dir;
        CGAL_Point coord        = build_CGAL_Point( p.coord );

        if( p.isJoint() ){
            const SkelPoint& prev   = cs.points.at( p.neighbors[0] );
            const SkelPoint& next   = cs.points.at( p.neighbors[1] );
            CGAL_Point cgal_prev    = build_CGAL_Point( prev.coord );
            CGAL_Point cgal_next    = build_CGAL_Point( next.coord );
                       dir          =  ( cgal_next - coord ) + ( coord - cgal_prev );
        }
        else{
            assert( p.isLeaf() );
            CGAL_Point cgal_prev    = build_CGAL_Point( cs.points.at( p.neighbors[0] ).coord );
                       dir          = ( coord - cgal_prev );
        }
        CGAL_Plane plane( coord, dir );

        PlaneMeshIntersectionForSQEM( plane, coord, resulting_data );
        ChooseClosestComponent( resulting_data );
        // set as outlier each bad data
        StatisticalAnalisys( resulting_data );

        CGAL_Point farthest = coord;
        double max_dist = std::numeric_limits<double>::min();
        size_t sqem_count = 0;
        bool first = true;
        SQEM sqem;


        for( const SQEM_centering_data& d : resulting_data ){

//            std::cout << d.intersected_triangle_index << std::endl;
            if( d.is_outlier ) {
                _mesh->f_colors[d.intersected_triangle_index] = Primitives::ColorF::LightGrey();
//                _mesh->f_colors[d.intersected_triangle_index] = Primitives::ColorF::Blue();
                continue;
            }

            CGAL_Point point_on_plane   = CGAL::midpoint( d.segment.source(), d.segment.target() );
            //assert( d.plane.has_on( point_on_plane ));
            CGAL_Vector plane_dir       = d.plane.orthogonal_vector();
            double length               = sqrt( plane_dir.squared_length());

            CGAL_Vector plane_normal( plane_dir.x()/length, plane_dir.y()/length, plane_dir.z()/length );

//            std::cout << "point on plane: " << point_on_plane << std::endl
//                      << "normal        : " << plane_normal.x() << ", " << plane_normal.y() << ", " << plane_normal.z() << std::endl;

            if( first ){
                first = false;
                sqem.setFromPlan ( SEQM_helper::SEQM_Point( point_on_plane.x(), point_on_plane.y(), point_on_plane.z()),
                                   SEQM_helper::SEQM_Point( plane_normal.x(), plane_normal.y(), plane_normal.z() ));
                ++sqem_count;
            }else{
                sqem += SQEM( SEQM_helper::SEQM_Point( point_on_plane.x(), point_on_plane.y(), point_on_plane.z()),
                              SEQM_helper::SEQM_Point( plane_normal.x(), plane_normal.y(), plane_normal.z() ));
                ++sqem_count;
            }

            double dist = CGAL::squared_distance( coord, point_on_plane );
            if( dist > max_dist ){
                max_dist = dist;
                farthest = point_on_plane;
            }
        }

        assert( sqem_count > 1 );

        // need to find two intersections on the mesh in order to bound the sphere
        // one is the farthest point, the other one can be found using a mesh ray intersction
        CGAL_Ray query_ray( farthest, coord );

        std::list<Ray_intersection> ray_intersections;
        AABB_Tree->all_intersections( query_ray,  std::back_inserter( ray_intersections ) );

        float min_dist      = MAXFLOAT;
        CGAL_Point opposite = farthest;

        for(std::list<Ray_intersection>::iterator it = ray_intersections.begin(); it != ray_intersections.end(); ++it){
            Ray_intersection obj = *it;

            if ( CGAL_Point *curr = boost::get<CGAL_Point>( &( obj->first )))
            {
                CGAL_Point current( curr->x(), curr->y(), curr->z() );
                float dist = CGAL::squared_distance( current, coord );
                std::list<CGAL_Triangle>::iterator tit = obj->second;
                CGAL_Triangle t = *tit;
                if( t.supporting_plane().has_on_positive_side( current )) { continue; }

                if ( dist < min_dist ){
                    min_dist        = dist;
                    opposite    = current;
                }
            }
        }

        SEQM_helper::SEQM_Point sphereCenter;
        float sphereRadius;
    //    sqem.minimize (sphereCenter, sphereRadius,
    //                   SEQM_helper::SEQM_Point ( -100000.0, -100000.0, -100000.0 ),
    //                   SEQM_helper::SEQM_Point ( 100000.0, 100000.0, 100000.0 ));

        sqem.minimize (sphereCenter, sphereRadius,
                       SEQM_helper::SEQM_Point ( farthest.x(), farthest.y(), farthest.z()),
                       SEQM_helper::SEQM_Point ( opposite.x(), opposite.y(), opposite.z()));

        size_t min_reference = 10;
        CGAL_Point new_coord( sphereCenter[0], sphereCenter[1], sphereCenter[2] );
        if( sqem_count >= min_reference && is_inside( new_coord ) ){
            new_coords[p.id ] = Primitives::Point3d( sphereCenter[0], sphereCenter[1], sphereCenter[2] );
            new_radii[p.id] = sphereRadius;
        }
        else{
            std::cout << "not considering the computed sphere ";
            if( sqem_count < min_reference ){ std::cout << "due to too few sqem data" << std::endl; } else { std::cout << "new point is outside " << std::endl; }
            new_coords[p.id ] = p.coord;
            new_radii[p.id] = p.radius;
        }

        cout << "count : "<< sqem_count
             << "    Result: optimal sphere centered at [" << sphereCenter[0] << ", " << sphereCenter[1] << ", " << sphereCenter[3] << "], with radius " << sphereRadius << "." << endl;

    }

    for( Skel::SkelPoint& p : cs.points ){
        if( p.isBranchingNode() ){ continue; }

        p.coord     = new_coords[p.id];
        p.radius    = new_radii[p.id];

//        p.coord = (new_coords[p.id] + p.coord ) / 2.0;
//        p.radius = ( new_radii[p.id] + p.radius ) / 2.0;
    }

}


void Utils::skel_mesh_helper::ResetMaximalBalls( CurveSkeleton &cs ){
    for( Skel::SkelPoint& p :  cs.points ){
        CGAL_Point test    = build_CGAL_Point( p.coord );

        CGAL_AABB_Tree::Point_and_primitive_id point_and_primitive
                = AABB_Tree->closest_point_and_primitive( test );

        p.radius = std::sqrt( CGAL::squared_distance( point_and_primitive.first, test ) );
    }
}

#endif
