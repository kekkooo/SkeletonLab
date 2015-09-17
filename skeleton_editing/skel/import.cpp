#include "skel/import.h"

#include <skel/update/color.h>
#include <skel/update/topology.h>
#include <assert.h>


namespace Skel
{
	namespace Importer
	{


		CurveSkeleton* load_TVCG_2012(QString filename)
		{

			CurveSkeleton *cs = new CurveSkeleton(false);

			if (filename.isEmpty()) return cs;

			//
			// load all the points and save them into a temporary list
			//

			QList<SkelPoint> list;

			FILE *f = fopen(filename.toStdString().c_str(),"r");

			if (f)
			{

				fscanf(f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n");

				int count;
				fscanf(f,"%d\n",&count);


				for (int i = 0; i < count; ++i)
				{

					int        id;        // unique ID
                    Point3d    coord;     // XYZ coordinates
					float      radius;    // maximal ball radius
					int        valence;   // number of adjcent points
					QList<int> neighbors; // IDS of the adjcant points


                    fscanf(f, "%d %lf %lf %lf %f %d ", &id, &coord[0], &coord[1], &coord[2], &radius, &valence);
//                    std::cout << id << ") " << coord[0] << ", "<< coord[1] << ", "<< coord[2] << endl;


					for (int j = 0; j < valence; ++j)
					{
						int neigh;

						fscanf(f, "%d ", &neigh);

						neighbors.append(neigh);
					}


					if (valence) list.append(SkelPoint(id, coord, radius, neighbors));
				}

				fclose(f);

				//
				// rename the points in order to create a perfect
				// correspondence between point IDs and List indices
				//

				QHash<int,int> map;

				for (int i = 0; i < list.size(); ++i) map.insert(list[i].id, i);

				for (int i = 0; i < list.size(); ++i)
				{

					list[i].id = map.value(list[i].id);

					for (int j = 0; j < list[i].neighbors.size(); ++j)
					{
						list[i].neighbors[j] = map.value(list[i].neighbors[j]);
					}
				}


				//
				// fill the skeleton with updated IDs
				//

				foreach (SkelPoint p, list) cs->append(p);
			}

			// update topology and colorization
			//
			UpdateTopology::boneMap(cs);
			UpdateColor::pointColor(cs);
			UpdateColor::boneColor(cs);

			cout << "Skeleton loaded\n\t"
			<< cs->points.size()   << " points\n\t"
			<< cs->bones.size()    << " bones.\n"   << endl;

			return cs;
		}


		CurveSkeleton* load_DeySun_2006(QString filename)
		{

			CurveSkeleton *cs = new CurveSkeleton();

			if (filename.isEmpty()) return cs;

			FILE *f = fopen(filename.toStdString().c_str(),"r");

			if (f)
			{

				// #points #edges
				//
				int nPoints, nEdges;
				fscanf(f,"%d %d\n",&nPoints, &nEdges);


				// load points
				//
				for (int i = 0; i < nPoints; ++i)
				{

                    Point3d coord;

					fscanf(f, "%f %f %f", &coord[0], &coord[1], &coord[2]);

					cs->append(SkelPoint(i, coord));
					cs->points.back().radius = 0.1;
				}


				// load connectivity
				//
				for (int i = 0; i < nEdges; ++i)
				{

					int pA, pB;

					fscanf(f, "%d %d", &pA, &pB);

					cs->points[pA].neighbors.append(pB);
					cs->points[pB].neighbors.append(pA);


					// some stuff we don't need at all
					//
					float dummyF;
					int dummyD1, dummyD2;
					fscanf(f, "%f", &dummyF);
					fscanf(f, "%f", &dummyF);
					fscanf(f, "%f", &dummyF);
					fscanf(f, "%d", &dummyD1);
					for(int j = 0; j < dummyD1; ++j) fscanf(f, "%d", &dummyD2);
				}

				fclose(f);

				// update topology and colorization
				//
				UpdateTopology::boneMap(cs);
				UpdateColor::pointColor(cs);
				UpdateColor::boneColor(cs);
			}

			return cs;
		}


		CurveSkeleton* load_Tagliasacchi_2012(QString filename)
		{
			CurveSkeleton *cs = new CurveSkeleton();

			if (filename.isEmpty()) return cs;

			FILE *f = fopen(filename.toStdString().c_str(),"r");

			if (f)
			{
				int dummy, nPoints, nEdges;
				fscanf(f,"# D:%d NV:%d NE:%d\n",&dummy, &nPoints, &nEdges);

                Point3d centroid(0.0, 0.0, 0.0);
				for (int i = 0; i < nPoints; ++i)
				{
                    Point3d coord;
					fscanf(f, "v %f %f %f\n", &coord[0], &coord[1], &coord[2]);
					centroid += coord;
					cs->append(SkelPoint(i, coord));
					cs->points.back().radius = 0.1;
				}
				centroid /= (float)cs->points.size();
                for(int vid=0; vid<(int)cs->points.size(); ++vid) cs->points[vid].coord -= centroid;

				for (int i = 0; i < nEdges; ++i)
				{
					int pA, pB;
					fscanf(f, "e %d %d\n", &pA, &pB);
					pA -= 1;
					pB -= 1;
					cs->points[pA].neighbors.append(pB);
					cs->points[pB].neighbors.append(pA);
				}

				fclose(f);
				UpdateTopology::boneMap(cs);
				UpdateColor::pointColor(cs);
				UpdateColor::boneColor(cs);
			}

			return cs;
		}
	}
}
