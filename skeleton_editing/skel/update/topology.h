#pragma once

#include <skel/base.h>
#include <skel/update/color.h>
#include <QQueue>
#include <string.h>

namespace Skel
{
	namespace UpdateTopology
	{
        enum BranchCollapseMode{ START = 0, END = 1, MIDPOINT = 2 };



		static void check_topology( CurveSkeleton *cs )
		{
			// punti : id -> indice
			for( int i = 0; i < cs->points.size(); ++i )
			{
				// point id and index are aligned
				assert( i == cs->points[i].id);
				// only the root could have 0 neighbots
				assert( cs->points[i].neighbors.size() > 0 || cs->points[i].isRoot() );
				// no point could have himself as neighbor
				assert( cs->points[i].neighbors.indexOf(i) == -1 );
				// no point should have a non existent bone id
				assert( cs->points[i].boneID >= 0 && cs->points[i].boneID < cs->bones.size());

			}
			// bones : first and last are joint or leafs
			// bones : each point should have the correct bone id
			for( int i = 0; i < cs->bones.size(); ++i )
			{
				cout << "### ANALYZING BONE " << i << " with size " << cs->bones[i].size() << " ###" << endl;
				assert( cs->bones[i].size() != 0);
				if( cs->bones[i].size() > 1)
				{
					assert( cs->points[ cs->bones[i].first() ].isBranchingNode()
							|| cs->points[ cs->bones[i].first() ].isLeaf() );
					assert( cs->points[ cs->bones[i].last() ].isBranchingNode()
							|| cs->points[ cs->bones[i].last() ].isLeaf() );
				}
				else
				{
					assert( cs->points[ cs->bones[i].first() ].isRoot() );
				}
				for( int pi = 0; pi < cs->bones[i].size(); ++pi )
				{
					if( cs->points[ cs->bones[i][pi] ].isBranchingNode() ) continue;
					cout << "point " << pi << " has bone_id : " << cs->points[ cs->bones[i][pi] ].boneID << endl;
					assert( cs->points[ cs->bones[i][pi] ].boneID == i );
				}
			}

		}

        static void boneMap(CurveSkeleton *cs)
        {
            qDebug() <<"doing bone remap!" << endl << "skel contains #bones : " << cs->bones.size();

            // collect all the junction points. The skeleton inspection will start from them
            QQueue<int> queue;

            foreach(SkelPoint p, cs->points)
            {
                //if (p.isFeaturePoint())
                if(p.isBranchingNode() )
                {
                    queue.append(p.id);
                }

                cs->points[cs->points.indexOf(p.id)].boneID = -1;
            }

            qDebug() <<" number of start points : " <<  queue.size();
            cs->bones.clear();

            if( cs->points.size() == 1 )
            {
                assert( cs->points.front().isRoot() );
                assert( cs->points.front().id == 0 );
                assert( cs->points.front().id == cs->points.indexOf(cs->points.front()));
                Bone bone;
                bone.append(0);
                cs->points.front().boneID = 0;
                // append the new branch into the branch map
                cs->bones.insert(0, bone);
            }

            else{
                while ( !queue.isEmpty() )
                {
                    // remove and return the head of the queue
                    int fp = queue.dequeue();

                    // iterate around all the incident branches, to label
                    // them (if they have not been labelled yet)
                    for (int i = 0; i < cs->points[cs->points.indexOf(fp)].neighbors.size(); i++)
                    {
                        // load the first branch point of the i-th branch
                        int current = cs->points[cs->points.indexOf(fp)].neighbors[i];

                        // if it has already been visited (ID != -1), the whole
                        // branch has already been visited. So, discard it
                        if ( cs->points[cs->points.indexOf(current)].boneID != -1 )
                        {
                            continue;
                        }

                        // if it is a feature point itself, the branch
                        // have 0 length. So, discard it
                        //			if ( cs->points[current].isF() )
                        //			{
                        //				continue;
                        //			}

                        // otherwise, follow the branch and label it
                        int prev = fp;

                        // list of the elements belonging to the current branch
                        Bone bone;
                        bone.append(fp);

                        // unique ID of the current branch
                        int freshID = cs->bones.size();
                        cs->points[cs->points.indexOf(fp)].boneID = freshID;

                        // follow the skeleton until you reach a new feature point
                        while ( cs->points[cs->points.indexOf(current)].isJoint() )
                        {
                            // assign to the current point the current branchID
                            cs->points[cs->points.indexOf(current)].boneID = freshID;

                            // add to the current branch elements list
                            bone.append(current);

                            // move to the adjacent point
                            int next = cs->points[cs->points.indexOf(current)].next(prev);
                            prev = current;
                            current = next;
                        }

                        cs->points[cs->points.indexOf(current)].boneID = freshID;
                        bone.append(current);

                        // append the new branch into the branch map
                        cout << "adding bone " << freshID << " with num of points = " << bone.size() << endl;
                        cs->bones.insert(freshID, bone);
                    }
                }
            }
            //            check_topology(cs);
            Skel::UpdateColor::boneColor(cs);
        }

		// add the adjacency relation between the point A and B
		static void pointConnect(CurveSkeleton *cs, int A, int B)
		{
			assert( A >= 0 /*&& A < cs->points.size()*/ );
			assert( B >= 0 /*&& B < cs->points.size()*/ );

			if (A != B)
			{
				cs->points[cs->points.indexOf(A)].neighbors.append(B);
				cs->points[cs->points.indexOf(B)].neighbors.append(A);
			}
		}

		// broke the adjacency relation between the point A and B
		static inline void pointDisconnect(CurveSkeleton *cs, int A, int B)
		{
			assert( A >= 0 && A < cs->points.size() );
			assert( B >= 0 && B < cs->points.size() );

			assert( cs->points[A].neighbors.contains(B) );
			assert( cs->points[B].neighbors.contains(A) );

			cs->points[A].removeNeighbor(B);
			cs->points[B].removeNeighbor(A);
		}

		// broke all the adjacency relations for point A
		static void pointDisconnectAll(CurveSkeleton *cs, int A)
		{
			assert(A >= 0 && A < cs->points.size());

			foreach(int neigh, cs->points[A].neighbors) cs->points[neigh].removeNeighbor(A);

			cs->points[A].neighbors.clear();

			assert(!cs->points[A].isConnected());
		}

		static void pointMerge(CurveSkeleton *cs, int A, int B)
		{
			assert( A >= 0 && A < cs->points.size() );
			assert( B >= 0 && B < cs->points.size() );

			cs->points[A].coord = (cs->points[A].coord + cs->points[B].coord) * 0.5;
			foreach(int nbr, cs->points[B].neighbors)
			{
				if (nbr != A) pointConnect(cs, A, nbr);
			}
			pointDisconnectAll(cs, B);
		}

		// returns the first boneID available for the given skeleton. Candidate IDs
		// are swept in ascendent order (starting from 0)
		static inline int boneFreshName(CurveSkeleton *cs)
		{
			for (int candidateID = 0; ; ++candidateID)
			{
				if (!cs->bones.contains(candidateID))
				{
					return candidateID;
				}
			}
		}


		static void garbage_collector(CurveSkeleton *skel)
		{
			QList<SkelPoint> list;
			bool have_found_root = false;

			for (int i = 0; i < skel->points.size(); ++i)
			{
//				if (skel->points[i].neighbors.size() > 0)
//				{
				if( skel->points[i].isDeleted() && !skel->points[i].isRoot() ) continue;

				list.append(SkelPoint( skel->points[i].id,
									   skel->points[i].coord,
									   skel->points[i].radius,
									   skel->points[i].neighbors ));



				if (skel->points[i].isTransferOnIt())
				{
					list.last().setTOI(true);
				}

//				if (skel->points[i].isSelected())
//				{
//					list.last().select();
//				}

				if (skel->points[i].isArticulation())
				{
					list.last().setArticulation(true);
				}

				if( skel->points[i].isRoot())
				{
					list.last().root = true;
					have_found_root = true;
				}
//				}
			}
			if(!have_found_root)
			{
				for (int i = 0; i < list.size() && !have_found_root; ++i)
				{
					if( list[i].isBranchingNode())
					{
						list[i].root = true;
						have_found_root = true;
					}
				}
			}
			if(!have_found_root)
			{
				list[0].root = true;
			}

			// rename the points in order to create a perfect
			// correspondence between point IDs and List indices
			QHash<int,int> map;

			for (int i = 0; i < list.size(); ++i)
			{
				map.insert( list[i].id, i );
			}

			for (int i = 0; i < list.size(); ++i)
			{
				list[i].id = map.value(list[i].id);
				for (int j = 0; j < list[i].neighbors.size(); ++j)
				{
					list[i].neighbors[j] = map.value(list[i].neighbors[j]);
				}
			}

			// fill the skeleton with updated IDs
			skel->points.clear();
			skel->bones.clear();

			foreach (SkelPoint p, list)
			{
				skel->append(p);
			}

			// update topology and colorization
			boneMap(skel);
//			UpdateColor::pointColor(skel);
//			UpdateColor::boneColor(skel);
		}

		//in a determinated CurveSkeleton, return true only when B is
		//connected whit A else it will return false.
		static bool isConnected(CurveSkeleton *cs, int A, int B)
		{
			int prevID=-1;

			return false;
		}

		static double distance(CurveSkeleton *cs, int A, int B)
		{
			assert( A >= 0 && A < cs->points.size() );
			assert( B >= 0 && B < cs->points.size() );

			return sqrt(pow(cs->points[A].coord.x - cs->points[B].coord.x, 2) +
						pow(cs->points[A].coord.y - cs->points[B].coord.y, 2) +
						pow(cs->points[A].coord.z - cs->points[B].coord.z, 2));

//			return cs->points[A].coord.Dist(cs->points[B].coord);
		}

        static void branchCollapse( CurveSkeleton& cs, int b, BranchCollapseMode mode = MIDPOINT ){
            assert( b >= 0 && b < cs.bones.size() );

            Skel::Bone& branch = cs.bones[b];
            // case of a Bn-Ln or Ln-Bn branch

            if( cs.points[ branch.front() ].isLeaf() && cs.points[branch.back()].isLeaf() ){
                std::cout << "not deleting the only branch";
                return;
            }

            if( cs.points[ branch.front() ].isLeaf() || cs.points[branch.back()].isLeaf() ){
                if( cs.points[branch.front()].isLeaf() ){ branch.reverse(); }
                for( int i = 1; i < branch.size(); ++i ){ cs.remove( branch[i] ); }
            }
            else{
                // case of a Bn-Bn branch
                Skel::SkelPoint& new_branching = cs.points[branch.front()];
                switch(mode){
                        case START :{
                        new_branching.coord = cs.points[ branch.front() ].coord;
                        new_branching.radius = cs.points[ branch.front() ].radius;
                        break;
                    }
                    case END :{
                        new_branching.coord = cs.points[ branch.back() ].coord;
                        new_branching.radius = cs.points[ branch.back() ].radius;
                        break;
                    }
                    case MIDPOINT: {
                        new_branching.coord = Primitives::Point3d::MidPoint( cs.points[ branch.front() ].coord,
                                                                             cs.points[ branch.back() ].coord );
                        new_branching.radius = std::max( cs.points[ branch.front() ].radius,
                                                         cs.points[ branch.back() ].radius );
                        break;
                    }
                }

                for( int i = 1; i < branch.size() - 1; ++i ){ cs.remove( branch[i], false ); }

                for( int neighbor : cs.points[ branch.back()].neighbors ){
                    UpdateTopology::pointConnect( &cs, new_branching.id, neighbor );
                }
                cs.remove( branch.back() );
            }

            UpdateTopology::garbage_collector( &cs );
        }
	}
}
