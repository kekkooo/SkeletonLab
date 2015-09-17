#include "edit.h"

#include "skel/update/topology.h"

namespace Skel
{
namespace Edit
{

        void prune(CurveSkeleton * cs, int length)
        {
                for(int b = 0; b < cs->bones.size(); ++b)
                {
                        bool is_terminal = (cs->points[cs->bones[b].first()].isLeaf() || cs->points[cs->bones[b].last()].isLeaf());

                        if (is_terminal && cs->bones[b].size() == 8) // <= length + 1)
                        {
//                                if (cs->points[cs->bones[b].first()].neighbors.size() == 4 ||
//                                    cs->points[cs->bones[b].last()].neighbors.size() == 4)
                                {
                                        if (cs->points[cs->bones[b].first()].isLeaf()) cs->bones[b].reverse();

                                        Skel::UpdateTopology::pointDisconnect(cs, cs->bones[b][0], cs->bones[b][1]);
                                        if (cs->bones[b].size() > 2)
                                        for(int i = 1; i < cs->bones[b].size(); ++i) Skel::UpdateTopology::pointDisconnectAll(cs, cs->bones[b][i]);

                                }

                        }
                }

                // update global skeleton topology
                //
                Skel::UpdateTopology::boneMap(cs);
        }


        void merge_shortest_inner_branch(CurveSkeleton *cs)
        {
                int shortest_inner_branch = -1;
                int min_length = cs->points.size();

                for(int b = 0; b < cs->bones.size(); ++b)
                {
                        bool is_inner = (cs->points[cs->bones[b].first()].isBranchingNode() && cs->points[cs->bones[b].last()].isBranchingNode());

                        if (is_inner && cs->bones[b].size() < min_length)
                        {
                                shortest_inner_branch = b;
                                min_length = cs->bones[b].size();
                        }
                }

                if (shortest_inner_branch == -1) return;

                int jointA = cs->bones[shortest_inner_branch].first();
                int jointB = cs->bones[shortest_inner_branch].last();

                if (cs->bones[shortest_inner_branch].size() > 2)
                {
                        Skel::UpdateTopology::pointDisconnect(cs, jointA, cs->bones[shortest_inner_branch][1]);
                        Skel::UpdateTopology::pointDisconnect(cs, jointB, cs->bones[shortest_inner_branch][min_length - 2]);
                        for(int i = 1; i < min_length - 2; ++i) Skel::UpdateTopology::pointDisconnectAll(cs, cs->bones[shortest_inner_branch][i]);
                }

                Skel::UpdateTopology::pointMerge(cs, jointA, jointB);
                Skel::UpdateTopology::boneMap(cs);
        }


        void laplacian_smoothing(CurveSkeleton *cs)
        {
                std::vector<Point3d> new_coords;

                for(int i = 0; i < cs->points.size(); ++i)
                {
                        Point3d p = cs->points[i].coord;
                        foreach(int nbr, cs->points[i].neighbors) p += cs->points[nbr].coord;
                        p /= (float)(cs->points[i].neighbors.size() + 1);

                        new_coords.push_back(p);
                }
                for(int i = 0; i < cs->points.size(); ++i) cs->points[i].coord = new_coords[i];
        }

        void translate(CurveSkeleton *cs, Vector3d dir )
        {
            for( int i = 0; i < cs->points.size(); ++i )
            {
                cs->points[i].coord += dir;
            }
        }



//        void apply_geometric_transformation(CurveSkeleton *cs, vcg::Matrix44f t)
//        {
//                for(int i = 0; i < cs->points.size(); ++i)
//                {
//                        cs->points[i].coord = t * cs->points[i].coord;
//                }
//        }
}
}
