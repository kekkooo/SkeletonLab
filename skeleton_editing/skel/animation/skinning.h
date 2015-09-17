#pragma once

#include <mesh/base.h>

#include <skel/base.h>
#include <skel/select.h>

#include <utilities/geometry.h>


namespace Skel
{
namespace Animation
{


        // compute binary skinning weights starting from riggin informations.
        //
        static void computeBinarySkinningWeights(TriMesh *mesh, CurveSkeleton *cs)
        {

                int size = cs->bones.size(); // number of weights for each vertex

                for(CurveSkeleton::BoneIterator it = cs->bones.begin(); it != cs->bones.end(); ++it)
                {

                        int boneID = it.key();

                        Bone bone = it.value();

                        foreach(int vIndex, bone.surfacePoints)
                        {

                                float weights[size];

                                for(int i = 0; i < size; ++i) weights[i] = (i == boneID) ? 1.f : 0.f;

                                mesh->vert[vIndex].skinningWeights = weights;
                        }
                }
        }



        // deform the mesh surface according to the rigging information
        // and the geometry transformations setted into the bones
        //
        static void doAnimation(TriMesh *mesh, CurveSkeleton *cs)
        {

                foreach (Bone bone, cs->bones)
                {

                        foreach (int vIndex, bone.surfacePoints)
                        {

                                mesh->vert[vIndex].P() = bone.transform * mesh->vert[vIndex].P();
                        }
                }
        }



        // straighten the terminal chains of the skeleton turning them into lines, and
        // also deforms the shape accordingly applying linear blend skinning
        //
        static void straightenSkeletonAndShape(TriMesh *mesh, CurveSkeleton *cs)
        {

                Timer timer("Straightening");

                for (CurveSkeleton::BoneIterator it = cs->bones.begin(); it != cs->bones.end(); ++it)
                {

                        int boneID = it.key();


                        assert(cs->bones[boneID].size() == 2);


                        // skip non terminal bones
                        //
                        if (!cs->points[cs->bones[boneID].first()].isL() && !cs->points[cs->bones[boneID].last()].isL()) continue;


                        // orient the bone in such a way that the first element is a leaf
                        //
                        if (!cs->points[cs->bones[boneID].first()].isL()) cs->bones[boneID].reverse();


                        // it'll contain the oriented path from the terminal bone to the junction
                        //
                        QList<int> chain;

                        while (!cs->points[cs->bones[boneID].last()].isF())
                        {

                                chain.append(boneID);


                                int endpoint = cs->bones[boneID].last();


                                // give the ID of the other bone incident to the terminal point of the current bone
                                //
                                QList<int> incidentBones = Skel::Select::incident_bones(cs, endpoint, boneID);

                                assert(incidentBones.size() == 1);


                                //  move to the next element
                                //
                                boneID = incidentBones.first();


                                // orient the bone in such a way that the first element is a leaf
                                //
                                if (cs->bones[boneID].first() != endpoint) cs->bones[boneID].reverse();

                                assert(cs->bones[boneID].first() == endpoint);
                        }

                        chain.append(boneID);


                        //
                        // 2) straighten the current chain
                        //

//                        if (chain.size() > 1)
//                        {

//                        int bA = chain[0],
//                            bB = chain[1];


//                        Mesh::Point A = cs->points[cs->bones[bA].first()].coord,
//                                    B = cs->points[cs->bones[bA].last()].coord,
//                                    C = cs->points[cs->bones[bB].last()].coord;

//                        cs->bones[bA].transform = myGeometry::alignByRotation(A, B, C);

//                        }
//                        else
//                                chain.clear();

//                        int prevBone = chain.takeLast();

//                        vcg::Matrix44f t; t.SetIdentity();

//                        while (!chain.isEmpty())
//                        {

//                                int currBone = chain.takeLast();

//                                Mesh::Point A = cs->points[cs->bones[currBone].first()].coord,
//                                            B = cs->points[cs->bones[currBone].last()].coord,
//                                            C = cs->points[cs->bones[prevBone].last()].coord;


//                                // gives the transormation necessary to align two subsequent skeleton bones
//                                //
//                                vcg::Matrix44f currTrans = myGeometry::alignByRotation(A, B, C);

//                                t = currTrans * t;

//                                cs->bones[currBone].transform = t;

//                                cs->points[cs->bones[currBone].first()].coord = currTrans * cs->points[cs->bones[currBone].first()].coord;
//                                cs->points[cs->bones[currBone].last()].coord  = currTrans * cs->points[cs->bones[currBone].last()].coord;

//                                prevBone = currBone;
//                        }


                        for (int i = 0; i < chain.size() - 1; ++i)
                        {

                                Mesh::Point A = cs->points[cs->bones[chain[i]].first()].coord,
                                            B = cs->points[cs->bones[chain[i]].last()].coord,
                                            C = cs->points[cs->bones[chain[i+1]].last()].coord;

                                // gives the transormation necessary to align two subsequent skeleton bones
                                //
                                vcg::Matrix44f t = myGeometry::alignByRotation(A, B, C);

                                QList<int> zoi;

                                for (int j = i; j >= 0; j--)
                                {

                                        cs->points[cs->bones[chain[j]].first()].coord = t * cs->points[cs->bones[chain[j]].first()].coord;

                                        zoi += cs->bones[chain[j]].surfacePoints;
                                }


                                vcg::tri::UpdateFlags<TriMesh>::Clear(*mesh);

                                foreach(int vIndex, zoi)
                                {

                                        if (mesh->vert[vIndex].IsV()) continue;

                                        mesh->vert[vIndex].P() = t * mesh->vert[vIndex].P();

                                        mesh->vert[vIndex].SetV();
                                }
                        }
                }

                timer.stop_and_print();
        }
}
}
