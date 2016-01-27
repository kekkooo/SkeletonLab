
#include "ellipse_fitting.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <mesh/mesh.h> // I need CGAL definitions from here

void RMesh::Utils::FitEllipseDirect( const RMesh::CGAL_Plane plane, const std::vector<RMesh::CGAL_Point>& points,
                                     RMesh::CGAL_Point& center, double& rmin, double& rmax ){

    // first of all I need to project all the input points on the plane.
    size_t N        = points.size();
    double Ndouble  = (double)N;
    double cx = 0.0, cy=0.0;
    std::vector<Eigen::Vector3d> projected_points;

    // here I build the transformation that rotates the plane to coincide with the XY plane
    RMesh::CGAL_Vector cgal_plane_normal = plane.orthogonal_vector(),
                       cgal_z( 0.0, 0.0, 1.0 );
    double length       = std::sqrt( cgal_plane_normal.squared_length());
    cgal_plane_normal   = CGAL_Vector( cgal_plane_normal.x() / length, cgal_plane_normal.y() / length, cgal_plane_normal.z() / length );
    double cosine       = cgal_plane_normal * cgal_z;
    double angle        = std::acos(cosine);
    bool do_rotate      = std::abs( angle ) > 0.0000001;

    std::cout << angle << " rotate? " << do_rotate << std::endl;

    RMesh::CGAL_Vector axis = CGAL::cross_product( cgal_plane_normal, cgal_z );
    double axis_length = std::sqrt( axis.squared_length() );
    Eigen::Vector3d rot_axis;
    rot_axis << axis.x() / axis_length, axis.y() / axis_length, axis.z() / axis_length;

    Eigen::AngleAxisd rot( angle, rot_axis );
//    Eigen::Transform R( Eigen::AngleAxisd( angle, rot_axis );
    std::for_each( points.begin(), points.end(),[&](const RMesh::CGAL_Point& p)
        { RMesh::CGAL_Point pp = plane.projection(p);
            std::cout << "point projected " << pp << std::endl;
          Eigen::Vector3d eigen_p; eigen_p << pp.x(), pp.y(), pp.z();
          std::cout << "eigen point" << eigen_p << std::endl;
          if( do_rotate ){
            Eigen::Vector3d eigen_pt = rot * eigen_p;
            std::cout << "eigen rotated point" << eigen_pt << std::endl;
            projected_points.push_back( eigen_pt );
          }else{
              projected_points.push_back( eigen_p );
          }
        } );

    assert(projected_points.size() > 0 );
    std::for_each( projected_points.begin(), projected_points.end(),
                   [&cx, &cy ]( Eigen::Vector3d& p ) { cx += p(0); cy += p(1); } );
    double count_double = static_cast<double>( N );
    Eigen::Vector3d centroid;
    centroid << cx / Ndouble, cy / Ndouble,  0.0;

    Eigen::MatrixXd D1(N,3), D2(N,3);
    Eigen::Matrix3d S1, S2, S3, S2T, C1, CQ, T, M, M_2;

//    D1 = [(XY(:,1)-centroid(1)).^2, (XY(:,1)-centroid(1)).*(XY(:,2)-centroid(2)), (XY(:,2)-centroid(2)).^2];
//    D2 = [XY(:,1)-centroid(1), XY(:,2)-centroid(2), ones(size(XY,1),1)];


    // at this point all the data points should lie at the X,Y plane, hence I can just drop the Z coordinate

    size_t row_counter = 0;
    std::for_each( projected_points.begin(), projected_points.end(),
                   [&]( Eigen::Vector3d& p ) {
                        double x = p(0) - centroid(0),  y = p(1) - centroid(1);
                        D1.row(row_counter) << x * x, x * y, y * y;
                        D2.row(row_counter) << x, y, 1.0;
                        ++row_counter;
                    } );

//    S1 = D1'*D1;
//    S2 = D1'*D2;
//    S3 = D2'*D2;
    S1 = D1.transpose() * D1;
    S2 = D1.transpose() * D2;
    S3 = D2.transpose() * D2;

//    T = -inv(S3)*S2';
    T = (- (S3.inverse())) * S2.transpose();

//    M = S1 + S2*T;
//    M = [M(3,:)./2; -M(2,:); M(1,:)./2];
    M =  S1 + S2 * T;
    //M /= 2.0;
    M_2.block( 0, 0, 1, 3 ) = M.block( 2, 0, 1, 3) / 2.0;
    M_2.block( 1, 0, 1, 3 ) = M.block( 1, 0, 1, 3) * (-1.0);
    M_2.block( 2, 0, 1, 3 ) = M.block( 0, 0, 1, 3) /2.0;


    std::cout << "D1" << std::endl << D1 << std::endl;
    std::cout << "D2" << std::endl << D2 << std::endl;
    std::cout << "S1" << std::endl << S1 << std::endl;
    std::cout << "S2" << std::endl << S2 << std::endl;
    std::cout << "S3" << std::endl << S3 << std::endl;
    std::cout << "T" << std::endl << T << std::endl;
    std::cout << "M" << std::endl << M << std::endl;
    std::cout << "M_2" << std::endl << M_2 << std::endl;

//    [evec,eval] = eig(M);
//    cond = 4*evec(1,:).*evec(3,:)-evec(2,:).^2;

    Eigen::EigenSolver<Eigen::Matrix3d> es( M_2, true );
    Eigen::MatrixXcd cevec = es.eigenvectors();
    Eigen::MatrixXd  evec = cevec.real();
    Eigen::Vector3d  eval = es.eigenvalues().real();

    std::cout << "evec" << std::endl << evec << std::endl;
    std::cout << "eval" << std::endl << eval << std::endl;

    bool cond1, cond2, cond3;
    int chosen_column = -1;
    cond1 = ( 4.0 * evec( 0, 0 ) * evec( 2, 0 ) - ( evec( 1, 0 ) * evec( 1, 0 )) > 0.0 );
    cond2 = ( 4.0 * evec( 0, 1 ) * evec( 2, 1 ) - ( evec( 1, 1 ) * evec( 1, 1 )) > 0.0 );
    cond3 = ( 4.0 * evec( 0, 2 ) * evec( 2, 2 ) - ( evec( 1, 2 ) * evec( 1, 2 )) > 0.0 );

    //    A1 = evec(:,find(cond>0));
    //    A = [A1; T*A1];
    chosen_column = cond1 ? 0: ( cond2 ? 1 : ( cond3 ? 2 : -1 ) );
    assert( chosen_column >= 0 && chosen_column < 3 );

    std::cout << "column to use : " << chosen_column << "( " << cond1 << ", " << cond2 << ", " << cond3 << std::endl;

    double a = evec( 0, chosen_column );
    double b = evec( 1, chosen_column );
    double c = evec( 2, chosen_column );


    Eigen::Vector3d A, A1;
    A = evec.col(chosen_column);
//    Eigen::MatrixXd A( 3, 1 ), A1;
//    A << a, b, c;
    A1 = T * A;   

    double d = A1( 0, 0 ) - 2 * a * centroid( 0 )  - b * centroid( 1 );
    double e = A1( 1, 0 ) - 2 * c * centroid( 1 )  - b * centroid( 0 );

    double f = A1( 2, 0 ) +
                a * centroid( 0 ) * centroid( 0 ) +
                c * centroid( 1 ) * centroid( 1 ) +
                b * centroid( 0 ) * centroid( 1 ) -
                A1( 0, 0 ) * centroid( 0 ) -
                A1( 1, 0 ) * centroid( 1 );

    std::cout << "a : " << a << std::endl
              << "b : " << b << std::endl
              << "c : " << c << std::endl
              << "d : " << d << std::endl
              << "e : " << e << std::endl
              << "f : " << f << std::endl;

    Eigen::VectorXd A2(6);
    A2 << a, b, c, d, e, f;
    A2 /= std::sqrt( A.squaredNorm( ));
    a = A2(0); b = A2(1); c = A2(2); d = A2(3); e = A2(4); f = A2(5);

    // computations formulae taken from:
    // http://math.stackexchange.com/questions/616645/determining-the-major-minor-axes-of-an-ellipse-from-general-form

    double ec_x, ec_y;
    double ac4 = 4.0 * a * c;
    double b2  = b * b;
    double a_minus_c = a - c;
    double a_minus_c2 = a_minus_c * a_minus_c;
    double sqrt_b2_plus_a_minus_c2 = std::sqrt( b2 + a_minus_c2 );
    double ae2 = a * e * e,
           bde = b * d * e,
           cd2 = c * d * d;
    double denom = ac4 - b2;
    double denom2 = denom * denom;
    double fdenom64 = 64.0 * f * denom;
    double ae2_bde_cd2_64 = 64.0 * ( ae2 - bde - cd2 );
//    double q = ( fdenom64 - ae2_bde_cd2_64 ) / ( denom2 );
    double q = 1.0;
    double abs_q = std::abs( q );
    double s_under_sqrt = abs_q * sqrt_b2_plus_a_minus_c2;
    double s = 0.25 * std::sqrt( s_under_sqrt );
        rmax = 0.125 * std::sqrt( 2.0 * s_under_sqrt - ( 2.0 * q * ( a + c) ));
        rmin = std::sqrt( rmax * rmax - s * s );

    ec_x = (( b * e ) - ( 2.0 * c * d )) / denom;
    ec_y = (( b * d ) - ( 2.0 * a * e )) / denom;

    std::cout << "denom :" << denom << " q: " << q << std::endl;

    Eigen::Vector3d center_xy, center_on_plane;
    center_xy << ec_x, ec_y, projected_points.front()(2);
    if( do_rotate ){
        rot = rot.inverse();
        center_xy = rot * center_xy;
    }
    center = CGAL_Point( center_xy(0), center_xy(1), center_xy(2) );

    std::cout << "rmin : " << rmin << std::endl << "rmax " << rmax;


    // calculate coordinates of the centroid and the redii of the two axis


}
