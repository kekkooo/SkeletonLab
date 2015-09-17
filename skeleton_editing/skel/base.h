#pragma once

#include <skel/skelpoint/base.h>
#include <skel/bone/base.h>
//#include <skel/update/topology.h>

#include <stdint.h>
#include <QHash>
#include <QList>
#include <QDebug>
#ifdef use_cgal
	#include <mesh/mesh.h>
#endif


namespace Skel
{

	class CurveSkeleton
	{

		public:

			typedef std::list<SkelPoint>::iterator PointIterator;
			typedef QHash<int, Bone>::iterator BoneIterator;

			CurveSkeleton(bool createRoot = true)
			{
				if(createRoot)
				{
					addRootNode();
				}
			}

			void addRootNode()
			{
				SkelPoint p(0, Point3d(0.0, 0.0, 0.0), 0.2 );
				p.boneID    = 0;
				p.color     = ColorF::Red();
				p.root      = true;

				this->append( p );

				Skel::Bone b;
				b.push_back(0);
				//			this->bones.insert(0, b );
				this->bones[0] = b;
			}

			inline void append(SkelPoint p)
			{
				points.append(p);
			}

			void remove(int id, bool hard_remove = true)
			{
				if(!hard_remove)
					assert( points.indexOf(id) == id );

				for (int i = 0; i < points[points.indexOf(id)].neighbors.size(); i++)
				{
					points[points.indexOf(points[points.indexOf(id)].neighbors[i])].removeNeighbor(id);
				}

				if( hard_remove )
					points.removeAt(points.indexOf(id));
				else
					points[id].neighbors.clear();
			}

			inline void clear()
			{
				points.clear(); bones.clear();
			}

			void outgoing_vectors(int id, std::vector<Vector3d> & vectors)
			{
				vectors.clear();
				Point3d begin =  points[id].coord;
				for(int nbr = 0; nbr < points[id].neighbors.size(); ++nbr)
				{
					Point3d  end = points[points[id].neighbors[nbr]].coord;
					Vector3d u = end - begin;
					u.Normalize();
					vectors.push_back(u);
				}
			}

			void outgoing_vectors(int id, QHash<int,Vector3d> & map)
			{
				map.clear();
				Point3d begin =  points[id].coord;
				foreach(int nbr, points[id].neighbors)
				{
					Point3d end = points[nbr].coord;
					Vector3d u = end - begin;
					u.Normalize();
					map[nbr] = u;
				}
			}

			int getMaxId()
			{
				return points.last().id;
			}

			bool allPointsConnected()
			{
				QList<SkelPoint> Q(points);
				Q.removeFirst();

				QVector<int> dist(points.size());

				for (int i = 0; i < points.size(); i++)
				{
					dist[i] = INT32_MAX;
				}

				dist[0] = 0;

				for (int i = 0; i < points[0].neighbors.size(); i++)
				{
					dist[points[0].neighbors[i]] = 1;
				}

				QList<int> removed = QList<int>() << 0;

				while (!Q.isEmpty())
				{
					int min = Q[0].id;
					SkelPoint point;
					int index;

					for (int i = 0; i < Q.size(); i++)
					{
						if (dist[Q[i].id] <= dist[min])
						{
							min = Q[i].id;
							point = Q[i];
							index = i;
						}
					}

					if (dist[min] == INT32_MAX)
					{
						break;
					}

					removed.append(point.id);
					Q.removeAt(index);

					for (int i = 0; i < point.neighbors.size(); i++)
					{
						if (!removed.contains(point.neighbors[i]))
						{
							int alt = dist[point.id] + 1;

							if (alt < dist[point.neighbors[i]])
							{
								dist[point.neighbors[i]] = alt;
							}
						}
					}
				}

				bool allConnected = true;

				for (int i = 0; i < points.size(); i++)
				{
					if (dist[points[i].id] == INT32_MAX)
					{
						allConnected = false;
					}
				}

				return allConnected;
			}

			//removes a point just like if it is a leaf from an assigned skeleton.
			//(now, is used to remove separeted point too).
			void deleteLeaf(SkelPoint leafNode)
			{
				this->remove(leafNode.id);
			}

			//removes a branch node from a assigned skeleton
			void deleteJoint(SkelPoint jointNode)
			{
				int idFirstNeighbor = jointNode.neighbors[0];
				int idSecondNeighbor = jointNode.neighbors[1];

				this->points[this->points.indexOf(idFirstNeighbor)].neighbors.append(idSecondNeighbor);
				this->points[this->points.indexOf(idSecondNeighbor)].neighbors.append(idFirstNeighbor);

				this->remove(jointNode.id);
			}

			int deleteBNWithoutTransfert(SkelPoint branchingNode)
			{
				QSet<int> bonesConnectedToBranchingNode;

				// Getting all the bones connected to the branching node to delete.
				foreach (int nodeId, branchingNode.neighbors)
				{
					bonesConnectedToBranchingNode.insert(this->points[this->points.indexOf(nodeId)].boneID);
				}

				int branchingNodesConnected = 0;
				QSet<int> bonesToDelete = bonesConnectedToBranchingNode;

				// Checking if the bones connected to the branching node to delete ends with another branching node.
				foreach (int boneId, bonesConnectedToBranchingNode)
				{
					// Selecting the first and the last points of the bone.
					SkelPoint firstBonePoint = this->points[this->points.indexOf(this->bones[boneId].first())];
					SkelPoint lastBonePoint = this->points[this->points.indexOf(this->bones[boneId].last())];

					if (firstBonePoint.id == lastBonePoint.id /*&& firstBonePoint.id == point.id*/)
					{
						// The bone is a cycle. Do nothing.
					}
					else if (firstBonePoint.id != branchingNode.id && firstBonePoint.isBranchingNode())
					{
						branchingNodesConnected++;
						bonesToDelete.remove(boneId);
					}
					else if (lastBonePoint.id != branchingNode.id && lastBonePoint.isBranchingNode())
					{
						branchingNodesConnected++;
						bonesToDelete.remove(boneId);
					}
				}

				if (branchingNodesConnected > 1)
				{
					return Skel::CurveSkeleton::TOO_MUCH_BN;
				}

				if (branchingNodesConnected < 1)
				{
					return Skel::CurveSkeleton::TOO_FEW_BN;
				}

				foreach (int boneId, bonesToDelete)
				{
					foreach (int nodeId, this->bones[boneId])
					{
						if (this->points[this->points.indexOf(nodeId)].isLeaf() && nodeId != branchingNode.id)
						{
							this->deleteLeaf(this->points[this->points.indexOf(nodeId)]);
						}
						else if (this->points[this->points.indexOf(nodeId)].isJoint() && nodeId != branchingNode.id)
						{
							this->deleteJoint(this->points[this->points.indexOf(nodeId)]);
						}
					}
				}

				if (this->points[this->points.indexOf(branchingNode.id)].isLeaf())
				{
					this->deleteLeaf(this->points[this->points.indexOf(branchingNode.id)]);
				}
				else
				{
					return Skel::CurveSkeleton::ERROR;
				}

				return Skel::CurveSkeleton::OK;
			}

			int deleteBNWithTransferNeighbors(SkelPoint branchingNode, SkelPoint transferNode)
			{
				if (branchingNode.id == transferNode.id)
				{
					return Skel::CurveSkeleton::ERROR;
				}

				for (int i = 0; i < branchingNode.neighbors.size(); ++i)
				{
					if (transferNode.id != branchingNode.neighbors[i])
					{
						this->points[this->points.indexOf(transferNode.id)].neighbors.append(branchingNode.neighbors[i]);
						this->points[this->points.indexOf(branchingNode.neighbors[i])].neighbors.append(transferNode.id);
					}
				}

				this->remove(branchingNode.id);

				return Skel::CurveSkeleton::OK;
			}

			int deleteNodes(QSet<int> nodesToDelete)
			{
				int branchingNodesNumber = 0;

				// Counting the number of branching nodes contained in the set in input.
				foreach (int nodeId, nodesToDelete)
				{
					if (this->points[this->points.indexOf(nodeId)].isBranchingNode())
					{
						branchingNodesNumber++;
					}
				}

				// If there are more than one branching node, quit.
				if (branchingNodesNumber > 1)
				{
					return Skel::CurveSkeleton::TOO_MUCH_BN;
				}

				// Delete all the nodes selected.
				foreach (int nodeId, nodesToDelete)
				{
					if (this->points.indexOf(nodeId) == -1)
					{
						// Do nothing. Avoid problems in case the point was already deleted by deleting
						// a branching node.
					}
					else if (this->points[this->points.indexOf(nodeId)].isLeaf())
					{
						this->deleteLeaf(this->points[this->points.indexOf(nodeId)]);
					}
					else if (this->points[this->points.indexOf(nodeId)].isJoint())
					{
						this->deleteJoint(this->points[this->points.indexOf(nodeId)]);
					}
					else if (this->points[this->points.indexOf(nodeId)].isBranchingNode())
					{
						int exitCode = this->deleteBNWithoutTransfert(this->points[this->points.indexOf(nodeId)]);

						if (exitCode == Skel::CurveSkeleton::TOO_FEW_BN)
						{
							return Skel::CurveSkeleton::TOO_FEW_BN;
						}
						else if (exitCode == Skel::CurveSkeleton::TOO_MUCH_BN)
						{
							return Skel::CurveSkeleton::TOO_MUCH_BN;
						}
					}
				}

				return Skel::CurveSkeleton::OK;
			}

			QList<SkelPoint>  points;
			QHash<int, Bone>  bones;

			enum removeNodeExitCode {OK, TOO_MUCH_BN, TOO_FEW_BN, ERROR};

#ifdef use_cgal
			RMesh::MeshPCA    axes;
#endif
	};
}
