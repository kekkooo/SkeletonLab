#pragma once

#include <skel/base.h>

namespace Skel
{
	namespace Edit
	{
		enum MERGE_POLYCY{ CENTER, FIRST, LAST, ANY };

		// previously implemented edit features
		void prune(CurveSkeleton *cs, int length);

		void merge_shortest_inner_branch(CurveSkeleton *cs);

		void laplacian_smoothing(CurveSkeleton *cs);

		//void apply_geometric_transformation(CurveSkeleton *cs, vcg::Matrix44f t);

		// edit features to be implemented
		void merge_points( CurveSkeleton *cs, int p0, int p1, MERGE_POLYCY policy );

		void cut_branch( CurveSkeleton *cs, int bone_id );

        void translate (CurveSkeleton *cs, Vector3d dir);
	}
}
