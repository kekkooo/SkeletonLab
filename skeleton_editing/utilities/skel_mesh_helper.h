#pragma once
#ifdef use_cgal
#include <mesh/mesh.h>
#include <skel/base.h>


using namespace RMesh;
using namespace Skel;
using namespace Primitives;

namespace RMesh{
	namespace Utils{

		typedef RMesh::K::Ray_3                                                                  CGAL_Ray;
		typedef RMesh::K::Segment_3                                                              CGAL_Segment;                      
		typedef CGAL::AABB_triangle_primitive<K,CGAL_Triangle_Iterator>                          CGAL_Triangle_Primitive;
		typedef CGAL::AABB_traits<K, CGAL_Triangle_Primitive>                                    AABB_triangle_traits;
		typedef CGAL::AABB_tree<AABB_triangle_traits>                                            CGAL_AABB_Tree;

		typedef boost::optional<CGAL_AABB_Tree::Intersection_and_primitive_id<CGAL_Ray>::Type>   Ray_intersection;
		typedef boost::optional<CGAL_AABB_Tree::Intersection_and_primitive_id<CGAL_Line>::Type>  Line_intersection;
		typedef boost::optional<CGAL_AABB_Tree::Intersection_and_primitive_id<CGAL_Plane>::Type> Plane_Intersection;

		struct IntersectionResult
		{
			CGAL_Triangle   face;
			CGAL_Point      point;
			bool            is_valid;
			IntersectionResult() { is_valid = false; }
		};

        struct SQEM_centering_data{
            CGAL_Plane plane;
            CGAL_Segment segment;
            size_t intersected_triangle_index;
            double distance;
            double z_score;
            double z_score_mod;
            bool is_outlier = false;
        };

        inline bool compare_SQEM_centering_data( SQEM_centering_data l, SQEM_centering_data r ){ return l.distance < r.distance; }

        typedef std::pair<IntersectionResult, IntersectionResult> ir_pair;

		class skel_mesh_helper
		{
		public:
			skel_mesh_helper                ( );
			void setMesh                    ( mesh *m );
			void checkSkeletonInsideMesh    ( CurveSkeleton& cs );
			void put_back_inside            ( CurveSkeleton *cs );
            void re_center                  ( CurveSkeleton *&cs , bool refit);
            void centeringWithSQEM          ( CurveSkeleton &cs, bool refit, int nodeID = -1 );

			// static functions
			static bool is_inside( CGAL_AABB_Tree &aabb, Point3d query );
			mesh*                       _mesh;
		private:

			std::list<CGAL_Triangle>    trilist;
			CGAL_AABB_Tree*             AABB_Tree;
			IntersectionResult          mesh_ray_intersection ( CGAL_Point p, CGAL_Point q );
			void                        compute_intersections ( CGAL_Plane plane, CGAL_Point curr,
																vector<IntersectionResult>& vir );
            void                        compute_paired_intersections( CGAL_Plane plane, CGAL_Point curr,
                                                                      vector<Utils::ir_pair> &pairs );
            void                        PlaneMeshIntersectionForSQEM(const CGAL_Plane& plane, const CGAL_Point &origin_point,
                                                                      std::vector<SQEM_centering_data>& results );
		};


	}}
#endif // use_cgal
