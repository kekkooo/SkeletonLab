#pragma once

#include <skel/base.h>

namespace Skel
{
	namespace UpdateColor
	{

		static void highlightSelectedPoint(CurveSkeleton *cs, int vid)
		{
//			int nBones = cs->bones.size();

//			if( nBones > 1)
//			{
				foreach(SkelPoint p, cs->points)
				{
					if (p.id == vid)
					{
						cs->points[p.id].color = ColorF::Blue();
					}
					else
					{
						cs->points[p.id].color = ColorF::White();
					}
				}
//			}
		}

		// colorize the skeleton points with the standard coding, that is:
		//
		//      WHITE  bones
		//      GREEN  leaves
		//      RED    joints
		//
		static void pointColor(CurveSkeleton *cs)
		{
			foreach(SkelPoint p, cs->points)
			{
				if (p.isLeaf())
				{
					cs->points[p.id].color = ColorF::Green();
				}
				else
				{
					if (p.isBranchingNode())
					{
						cs->points[p.id].color = ColorF::Red();
					}
					else
					{
						if (p.isArticulation())
						{
							cs->points[p.id].color = ColorF::Yellow();
						}
					}
				}
				//else
				//cs->points[p.id].color = Color4f::White;
			}
		}

		// colorize each skeleton point according with the skeleton bone it belongs to
		static void boneColor(CurveSkeleton *cs)
		{
			int nBones = cs->bones.size();

            if ( nBones > 1 )
            {
                foreach(SkelPoint p, cs->points)
                {
                    if (p.isJoint() || p.isLeaf())
                    {

                        cs->points[p.id].color = ColorF::Scatter(nBones, p.boneID, 1.f, 1.f);

						if((cs->points[p.id].color.asArray()[0] > 0.6) && (cs->points[p.id].color.asArray()[1] > 0,5) && (0.2 > cs->points[p.id].color.asArray()[2]))
						{
							cs->points[p.id].color = cs->points[p.id].color = ColorF::Scatter(nBones, p.boneID, 0.5, 0.5);
						}

					}
				}
			}
		}
	}
}
