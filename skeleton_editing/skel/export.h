#pragma once

#include <skel/base.h>
//#include <skel/update/topology.h>
//#include <skel/chain.h>
#include <math.h>
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif

#include <algorithm>
//#include <gl.h>
#include <cmath>

namespace Skel
{
	namespace Exporter
	{

//		static Chain bone_to_chain(CurveSkeleton *skel, int bone_id)
//		{
//			assert(bone_id >= 0 && bone_id < skel->bones.size());

//			Chain res;
//			for(int i=0; i<skel->bones[bone_id].size(); ++i)
//			{
//				int vid = skel->bones[bone_id][i];
//				res.add_node(ChainNode(skel->points[vid].coord, skel->points[vid].radius));
//			}

//			return res;
//		}


//		static void substitute_bone_with_chain(CurveSkeleton *skel, int bone_id, Chain chain)
//		{
//			assert(bone_id >= 0 && bone_id < skel->bones.size());


//			Bone &b(skel->bones[bone_id]);
//			for(int i=1; i < b.size() - 1; ++i)
//			{
//				Skel::UpdateTopology::pointDisconnectAll(skel, b[i]);
//			}

//			if (chain.nodes.size() == 0)
//			{
//				skel->points[b.first()].neighbors.push_back(b.last());
//				skel->points[b.last()].neighbors.push_back(b.first());
//			}
//			else
//			{
//				int prev = b.first();
//				for(int i=1; i < chain.nodes.size() - 1; ++i)
//				{
//					SkelPoint tmp;
//					tmp.id     = skel->points.size();
//					tmp.coord  = chain[i].coord;
//					tmp.radius = chain[i].radius;
//					tmp.neighbors.push_back(prev);
//					skel->points[prev].neighbors.push_back(tmp.id);
//					prev = tmp.id;

//					skel->points.push_back(tmp);
//				}
//				skel->points[prev].neighbors.push_back(b.last());
//				skel->points[b.last()].neighbors.push_back(prev);
//			}

//			Skel::UpdateTopology::boneMap(skel);
//			return;
//		}

		static void SaveToFile(CurveSkeleton *skel,  QString filename)
		{

			if (filename.isEmpty())
			{
				return;
			}

			// load all the points and save them into a temporary list
			FILE *f = fopen(filename.toStdString().c_str(),"w");

			if (f)
			{
				int num_of_points = 0;

				foreach( SkelPoint skelpoint, skel->points )
				{
                    if( skelpoint.isBranchingNode() || skelpoint.isLeaf() )
					{
						num_of_points++;
					}
				}

				foreach( Bone b, skel->bones )
				{
					num_of_points+=b.size() - 2;
				}

				//fprintf(f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n%d\n",skel->points.size());
				fprintf( f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n%d\n", num_of_points );

				foreach(SkelPoint skelpoint, skel->points)
				{
					int valence = skelpoint.neighbors.size();

					if(valence > 0)
					{
						fprintf(f, "%d %f %f %f %f %d ", skelpoint.id, skelpoint.coord[0], skelpoint.coord[1], skelpoint.coord[2], skelpoint.radius, valence);

						for (int j = 0; j < valence; ++j)
						{
							fprintf(f, "%d ", skelpoint.neighbors[j]);
						}

						fprintf(f, "\n");
					}
				}

				fclose(f);
			}

			cout << "Skeleton saved\n\t"
				 << skel->points.size()   << " points\n\t"
				 << skel->bones.size()    << " bones.\n"   << endl;
		}

		static void ExportToPOVRay(CurveSkeleton *skel,  QString filename)
		{

		}

		static void angle_statistics(CurveSkeleton *skel)
		{
			std::cout << "\n" << std::endl;

			foreach(SkelPoint point, skel->points)
			{
				// consider only joints...
				//
				if (!point.isBranchingNode())
				{
					continue;
				}

                Point3d O = point.coord;

				std::cout << "Joint #" << point.id << "\t(valence " << point.neighbors.size() << ")" << std::endl;

				for(int i = 0;     i < point.neighbors.size() - 1; ++i)
				{
					for(int j = i + 1; j < point.neighbors.size(); ++j    )
					{
						assert (i != j);

                        Point3d A = skel->points[point.neighbors[i]].coord;
                        Point3d B = skel->points[point.neighbors[j]].coord;

                        Vector3d u = A - O; u.Normalize();
                        Vector3d v = B - O; v.Normalize();

						double angle = acos( u.Dot(v)) * (180.f / M_PI);


						std::cout << "\t"   << skel->points[point.neighbors[i]].id
								  << " vs " << skel->points[point.neighbors[j]].id
								  << "\t\t" << angle <<
									 std::endl;
					}
				}

				std::cout << "\n" << std::endl;
			}
		}
	}
}

