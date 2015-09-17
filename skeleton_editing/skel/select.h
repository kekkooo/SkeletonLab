#pragma once

#include <skel/base.h>

namespace Skel
{
	namespace Select
	{

		// return the full list of inner points (i.e. no terminal points) of a given bone (<id>)
		QList<int> bone_without_endpoints(CurveSkeleton *cs, int id);

		// returns all the skeleton bones having fp as terminal point. The filter
		// parameter allows the user to specify one boneID he doesn't want to be
		// returned
		QList<int> incident_bones(CurveSkeleton *cs, int fp, int filter = -1);

	}
}
