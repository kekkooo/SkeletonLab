#pragma once

#include <skel/base.h>
#include <skel/import.h>
#include <skel/export.h>

namespace Skel
{
	namespace Importer
	{
		CurveSkeleton* load_TVCG_2012(QString filename);

		CurveSkeleton* load_DeySun_2006(QString filename);

		CurveSkeleton* load_Tagliasacchi_2012(QString filename);
	}
}

