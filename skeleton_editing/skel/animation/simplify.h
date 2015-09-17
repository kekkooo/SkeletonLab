#pragma once

#include <skel/base.h>
#include <skel/update/topology.h>


namespace Skel
{
namespace Animation
{


        static int MIN_LENGTH = 5;


        // turns the curve skeleton into an animation skeleton, breaking the curves
        // in the points of maximal curvature and deleting all the points lying between
        // two feature points.
        //
        // The point-based rigging information are transfered accordingly: each bone will
        // inherit the surface vertices attached to each skeleton point belonging to it
        //
        static void simplify(CurveSkeleton *cs, int angThresh)
        {

                Timer timer("Skeleton simplification");

                QList<int> names;

                for (CurveSkeleton::BoneIterator it = cs->bones.begin(); it != cs->bones.end(); ++it) names.append(it.key());

                foreach(int boneID, names)
                {

                        // save and remove from the skeleton the current bone
                        //
                        Bone bone = cs->bones[boneID];


                        cs->bones.remove(boneID);


                        int lastSplit = 0;


                        QList<int> riggingInfo;


                        for (int i = 1; i < bone.size() - 1; ++i)
                        {

                                SkelPoint &prev = cs->points[bone[lastSplit]],
                                          &curr = cs->points[bone[i]],
                                          &next = cs->points[bone[i + 1]];


                                riggingInfo += curr.surfacePoints;

                                if (prev.isL()) riggingInfo += prev.surfacePoints;


                                // measure the angle between the line joining the last articulation with the current point,
                                // and the next bone segment
                                //
                                Point u = prev.coord - curr.coord,
                                      v = next.coord - curr.coord;


                                float angle = vcg::Angle(u, v) * (180 / M_PI);


                                // if the path followed so far deviates too much from a straight line,
                                // create a new branch and split the branch in two sub-branches
                                //
                                if ( ABS(angle) < angThresh && (i - lastSplit) >= MIN_LENGTH && bone.size() - i >= MIN_LENGTH )
                                {

                                        Bone newBone;

                                        // handle endpoints
                                        //
                                        Skel::UpdateTopology::pointConnect(cs, curr.id, prev.id);
                                        newBone.append(prev.id);
                                        newBone.append(curr.id);
                                        lastSplit = i;

                                        // inherit rigging
                                        //
                                        newBone.surfacePoints = riggingInfo;
                                        riggingInfo.clear();

                                        // update skeleton data structure
                                        //
                                        int freshID = Skel::UpdateTopology::boneFreshName(cs);
                                        cs->bones.insert(freshID, newBone);
                                }

                                Skel::UpdateTopology::pointDisconnect(cs, curr.id, bone[i-1]);
                        }

                        Skel::UpdateTopology::pointDisconnect(cs, bone.last(), bone[bone.size() - 2]);

                        Bone newBone;

                        // handle endpoints
                        //
                        Skel::UpdateTopology::pointConnect(cs, bone[lastSplit], bone.last());
                        newBone.append(bone[lastSplit]);
                        newBone.append(bone.last());

                        // inherit rigging
                        //
                        newBone.surfacePoints = riggingInfo;
                        if (cs->points[bone.last()].isL()) newBone.surfacePoints += cs->points[bone.last()].surfacePoints;

                        // update skeleton data structure
                        //
                        int freshID = Skel::UpdateTopology::boneFreshName(cs);
                        cs->bones.insert(freshID, newBone);
                }

                timer.stop_and_print();
        }


        static void simplify_for_quadmeshing_project(CurveSkeleton *cs, int angThresh)
        {

                Timer timer("Skeleton simplification");

                foreach(Bone bone, cs->bones)
                {

                        // DIFFERENCE RESPECT TO THE ORIGINAL ONE! it was 0
                        int lastSplit = 1;

                        // DIFFERENCE RESPECT TO THE ORIGINAL ONE! [2..size-2]
                        for (int i = 2; i < bone.size() - 2; ++i)
                        {

                                SkelPoint &prev = cs->points[bone[lastSplit]],
                                          &curr = cs->points[bone[i]],
                                          &next = cs->points[bone[i + 1]];


                                // measure the angle between the line joining the last articulation with the current point,
                                // and the next bone segment
                                //
                                Point u = prev.coord - curr.coord,
                                      v = next.coord - curr.coord;


                                float angle = vcg::Angle(u, v) * (180 / M_PI);


                                // if the path followed so far deviates too much from a straight line,
                                // create a new branch and split the branch in two sub-branches
                                //
                                if ( ABS(angle) < angThresh && (i - lastSplit) >= MIN_LENGTH && bone.size() - i >= MIN_LENGTH )
                                {
                                        Skel::UpdateTopology::pointConnect(cs, curr.id, prev.id);
                                        lastSplit = i;
                                }

                                Skel::UpdateTopology::pointDisconnect(cs, curr.id, bone[i-1]);
                        }

                        Skel::UpdateTopology::pointDisconnect(cs, bone[bone.size() - 2], bone[bone.size() - 3]);

                        // handle endpoints
                        //
                        Skel::UpdateTopology::pointConnect(cs, bone[lastSplit], bone[bone.size() - 2]);
                }

                Skel::UpdateTopology::boneMap(cs);
                Skel::UpdateColor::boneColor(cs);

                timer.stop_and_print();
        }
}
}
