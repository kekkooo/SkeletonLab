#pragma once

#include <QVector>

#include <skel/base.h>

#include <mesh/update/color.h>
#include <mesh/select.h>

#include <utilities/geometry.h>

namespace Skel
{
namespace Animation
{

        // A rigging map is a mapping between Skeleton Bones and Mesh Vertices
        //
        typedef QHash< int, QList<int> > RigMap;



        // Here we define a mapping from skeleton points to mesh vertices (rigging).
        // This rigging can be tought as the solution of the skinning problem with
        // binary weights (i.e. each vertex belongs only to a skeleton point). Only
        // branch points and leaves are considered during the process. Rigging inf-
        // ormation are stored in the skeleton points. Each point will contain a list
        // of the vertices attached to it
        //
        // TO DO:
        //              - make sure that each vertex has a mapping with the skeleton
        //                (flooding may be used to expand the mapped areas)
        //
        //              - make sure that each bone maps to a connected area, removing
        //                some mapping errors that actually appear from time to time
        //
        static void pointRigging2(TriMesh* mesh, CurveSkeleton *cs)
        {

                Timer timer("Point Rigging");


                // respectively the ID of the skeleton point associated to the face
                // anche the distance between the face and the associated skel point
                //
                QVector<int>   faceSkelLink(mesh->fn, -1);
                QVector<float> faceSkelDist(mesh->fn, MAXFLOAT);


                //
                // 1) We scan each skeleton branch separately. For each branch we consider one inner point
                //    at a time, and for it we define two cutting planes placed in its adjacent points such
                //    that they define a positive semi-infinite space containing the inner point itself.
                //
                //    Outline:
                //
                //     - slice the mesh
                //     - collect the triangle involved in the cut
                //     - filter the vertices isolating the connected component we are looking for
                //     - resolve ambiguities associating each triangle to the closest skeleton point
                //
                //     IMPORTANT: this mapping is partial: some triangles may not appear in any slice!
                //
                //     IMPORTANT: the cutting planes associated to adjacent skeleton points usually leave
                //     a dead space which is the intersection of the negative half-infinite spaces. To not
                //     lose the triangles placed in this negative space we add all this primitives to the
                //     cut
                //


                for (CurveSkeleton::BoneIterator it = cs->bones.begin(); it != cs->bones.end(); ++it)
                {

                        Bone bone = it.value();


                        // cutting plane used in the previous computation (adjacent point)
                        //
                        myGeometry::Plane oldPlane = myGeometry::Plane();


                        for (int i = 0; i < bone.size() - 1; ++i)
                        {

                                SkelPoint& current = cs->points[bone[i]];


                                QList<int> cc; // connected component. It'll contain the slice


                                if (current.isL())
                                {

                                        // adjacent points
                                        //
                                        SkelPoint& next = cs->points[current.neighbors.first()];


                                        // cutting planes
                                        //
                                        Point normal = current.coord - next.coord;
                                        myGeometry::Plane cutPlane = myGeometry::Plane(current.coord, normal);


                                        // connected component (after slicing)
                                        //
                                        cc = Mesh::Select::connectedComponent(mesh, current.coord, cutPlane);
                                }

                                else

                                //if (current.isB())
                                if (current.isB() || current.isJ())
                                {

                                        // adjacent points
                                        //
                                        SkelPoint& prev = cs->points[current.neighbors.first()];
                                        SkelPoint& next = cs->points[current.neighbors.last()];


                                        // cutting planes
                                        //
                                        Point normal = next.coord - prev.coord;
                                        myGeometry::Plane prevPlane = myGeometry::Plane(prev.coord,  normal);
                                        myGeometry::Plane nextPlane = myGeometry::Plane(next.coord, -normal);


                                        // connected component (after slicing)
                                        //
                                        cc = Mesh::Select::connectedComponent(mesh,
                                                                              current.coord,
                                                                              prevPlane,
                                                                              nextPlane);


                                        // retrive the triangles lying in the negative half space
                                        //
                                        prevPlane.invertOrientation();

                                        cc += Mesh::Select::connectedComponent(mesh,
                                                                               current.coord,
                                                                               oldPlane,
                                                                               prevPlane);


                                        // save the last cutting plane for the next computation
                                        //
                                        nextPlane.invertOrientation();
                                        oldPlane = nextPlane;
                                }

                                else continue; // we consider only leaves and branch points


                                // update the rigging map according with the MINIMA RULE: a face
                                // belongs to a skeleton point if and only if it belongs to its slice
                                // and the distance between the face and the point is minimal respect
                                // to the distances between the same face and the other skeleton points
                                // having it in their slice
                                //
                                foreach(int fIndex, cc)
                                {

                                        Face *f = &mesh->face[fIndex];

                                        float dist = vcg::Distance(f->P(0), current.coord);  // we approximate face-point
                                                                                             // with point-point distance
                                        if (dist < faceSkelDist[fIndex])
                                        {
                                                faceSkelLink[fIndex] = current.id;
                                                faceSkelDist[fIndex] = dist;
                                        }
                                }
                        }
                }


                //
                // 2) Starting from the previous result, create a rigging map that associates
                //    triangles with skeleton bones
                //
                //    IMPORTANT: this mapping is partial too: only the triangles coming from the
                //               previous step will have a correspondence with the skeleton!


                // update the skeleton data structure with rigging information
                //
                assert( mesh->HasPerFaceFlags() );

                vcg::tri::UpdateFlags<TriMesh>::Clear(*mesh);

                for (int fIndex = 0; fIndex < faceSkelLink.size(); ++fIndex)
                {

                        // in this case, fIndex has not been sliced by any plane
                        // therefore the association with the skeleton is missing
                        //
                        if (faceSkelLink[fIndex] == -1) continue;


                        int skelPoint = faceSkelLink[fIndex];

                        Face &f = mesh->face[fIndex];

                        for (int i = 0; i < 3; ++i)
                        {

                                if (!f.V(i)->IsV())
                                {

                                        f.V(i)->SetV();

                                        SkelPoint& p = cs->points[skelPoint];

                                        int vIndex = f.V(i) - &mesh->vert[0];

                                        p.surfacePoints.append(vIndex); // serves for STRAIGHTENING!!!
                                }
                        }
                }

                timer.stop_and_print();
        }


        // here we define a mapping from skeleton points to mesh vertices.
        // This rigging can be thought as the solution of the skinning problem
        // with binary weights (i.e. each vertex belongs only to a skeleton point)
        // Only branch points and leaves are considered during the process.
        // Rigging information is stored into the skeleton points. Each points
        // will contain a list of the vertices attached to it.
        //
        // TO DO:
        //              - make sure that each vertex has a mapping with the skeleton
        //                (flooding may be used to expand the mapped areas)
        //
        //              - make sure that each bone maps to a connected area, removing
        //                some mapping errors that actually appear from time to time
        //
        // THE VERSION BELOW (COMMENTED) SEEMS TO NOT WORK!
//        static void pointRigging(TriMesh* mesh, CurveSkeleton *cs)
//        {

//                Timer timer("Point Rigging");


//                // links boneIDs with the set of vertices that map in it
//                //
//                //RigMap riggingMap;


//                // respectively the ID of the skeleton point associated to the face
//                // anche the distance between the face and the associated skel point
//                //
//                QVector<int>   faceSkelLink(mesh->fn, -1);
//                QVector<float> faceSkelDist(mesh->fn, MAXFLOAT);


//                //
//                // 1) For each skeleton point:
//                //
//                //     - slice the mesh
//                //     - collect the triangle involved in the cut
//                //     - filter the vertices isolating the connected component you are looking for
//                //     - resolve ambiguities associating each triangle to the closest skeleton point
//                //
//                //     IMPORTANT: this mapping is partial: some triangles may not appear in any slice!


//                foreach(SkelPoint p, cs->points)
//                {

//                        QList<int> cc; // connected component


//                        // slice the mesh in case of a leaf (one cutting plane)
//                        //
//                        if (p.isL()) // LEAF
//                        {

//                                Point next = cs->points[p.neighbors.first()].coord;

//                                myGeometry::Plane pA((next + p.coord) * 0.5, p.coord - next);

//                                cc = Mesh::Select::connectedComponent(mesh, p.coord, pA);
//                        }

//                        else

//                        // slice the mesh in case of a branch point (two cutting planes)
//                        //
//                        if (p.isB()) // BRANCH
//                        {

//                                Point prev = cs->points[p.neighbors.first()].coord,
//                                      next = cs->points[p.neighbors.last()].coord;

//                                myGeometry::Plane cutPrev((prev + p.coord) * 0.5, next - prev);
//                                myGeometry::Plane cutNext((next + p.coord) * 0.5, prev - next);

//                                cc = Mesh::Select::connectedComponent(mesh, p.coord, cutPrev, cutNext);
//                        }

//                        else continue; // we consider only leaves and branch points


//                        // update the rigging map according with the MINIMA RULE: a face
//                        // belongs to a skeleton point if and only if it belongs to its slice
//                        // and the distance between the face and the point is minimal respect
//                        // to the distances between the same face and the other skeleton points
//                        // having it in their slice
//                        //
//                        foreach(int fIndex, cc)
//                        {

//                                Face *f = &mesh->face[fIndex];

//                                float dist = vcg::Distance(f->P(0), p.coord);  // we approximate face-point
//                                                                               // with point-point distance
//                                if (dist < faceSkelDist[fIndex])
//                                {
//                                        faceSkelLink[fIndex] = p.id;
//                                        faceSkelDist[fIndex] = dist;
//                                }
//                        }
//                }


//                //
//                // 2) Starting from the previous result, create a rigging map that associates
//                //    triangles with skeleton bones
//                //
//                //    IMPORTANT: this mapping is partial too: only the triangles coming from the
//                //               previous step will have a correspondence with the skeleton!


//                // update the skeleton data structure with rigging information
//                //
//                assert( mesh->HasPerFaceFlags() );

//                vcg::tri::UpdateFlags<TriMesh>::Clear(*mesh);

//                for (int fIndex = 0; fIndex < faceSkelLink.size(); ++fIndex)
//                {

//                        // in this case, fIndex has not been sliced by any plane
//                        // therefore the association with the skeleton is missing
//                        //
//                        if (faceSkelLink[fIndex] == -1) continue;


//                        int skelPoint = faceSkelLink[fIndex];

//                        Face &f = mesh->face[fIndex];

//                        for (int i = 0; i < 3; ++i)
//                        {

//                                if (!f.V(i)->IsV())
//                                {

//                                        f.V(i)->SetV();

//                                        SkelPoint& p = cs->points[skelPoint];

//                                        int vIndex = f.V(i) - &mesh->vert[0];

//                                        p.surfacePoints.append(vIndex); // serves for STRAIGHTENING!!!

//                                        // update the bone-vertex association to the rigging map
//                                        //
////                                        if (!riggingMap.contains(p.boneID)) riggingMap.insert(p.boneID, QList<int>());

////                                        riggingMap[p.boneID].append(vIndex);
//                                }
//                        }
//                }


//                //
//                // 3) For each bone-to-surface mapping keep only the greatest connected component,
//                //    unlabelling the other charts
//                //

////                assert( mesh->HasFFTopology() );

////                vcg::tri::UpdateFlags<TriMesh>::Clear(*mesh);


////                for (RigMap::Iterator it = riggingMap.begin(); it != riggingMap.end(); ++it)
////                {

////                        //int boneID = it.key();


////                        // save the list of vertices into a queue
////                        //
////                        QQueue<int> queue;
////                        QList<int> list = it.value();
////                        foreach(int vIndex, list) queue.enqueue(vIndex);


////                        while (!queue.isEmpty())
////                        {

////                                ;
////                        }
////                }


//                timer.stop_and_print();
//        }

        static void pointRigging(TriMesh* mesh, CurveSkeleton *cs)
        {

                Timer timer("Point Rigging");

                // respectively the ID of the skeleton point associated to the face
                // anche the distance between the face and the associated skel point
                //
                QVector<int>   faceSkelLink(mesh->fn, -1);
                QVector<float> faceSkelDist(mesh->fn, MAXFLOAT);


                foreach(SkelPoint p, cs->points)
                {

                        QList<int> cc; // connected component


                        // slice the mesh in case of a leaf (one cutting plane)
                        //
                        if (p.isL()) // LEAF
                        {

                                Point A = cs->points[p.neighbors.first()].coord;

                                myGeometry::Plane pA(A, p.coord - A);

                                cc = Mesh::Select::connectedComponent(mesh, p.coord, pA);
                        }

                        else

                        // slice the mesh in case of a branch point (two cutting planes)
                        //
                        if (p.isB()) // BRANCH
                        {

                                Point A = cs->points[p.neighbors.first()].coord,
                                      B = cs->points[p.neighbors.last()].coord;

                                myGeometry::Plane pA(A, B - A);
                                myGeometry::Plane pB(B, A - B);

                                cc = Mesh::Select::connectedComponent(mesh, p.coord, pA, pB);
                        }

                        else continue; // we consider only leaves and branch points


                        // update the rigging map according with the MINIMA RULE: a face
                        // belongs to a skeleton point if and only if it belongs to its slice
                        // and the distance between the face and the point is minimal respect
                        // to the distances between the same face and the other skeleton points
                        // having it in their slice
                        //
                        foreach(int fIndex, cc)
                        {

                                Face *f = &mesh->face[fIndex];

                                float dist = vcg::Distance(f->P(0), p.coord);  // we approximate face-point
                                                                               // with point-point distance
                                if (dist < faceSkelDist[fIndex])
                                {

                                        faceSkelLink[fIndex] = p.id;
                                        faceSkelDist[fIndex] = dist;
                                }
                        }
                }


                // update the skeleton data structure with rigging information
                //
                assert( mesh->HasPerFaceFlags() );

                vcg::tri::UpdateFlags<TriMesh>::Clear(*mesh);

                for (int fIndex = 0; fIndex < faceSkelLink.size(); ++fIndex)
                {

                        int skelPoint = faceSkelLink[fIndex];

                        if (skelPoint < 0) continue;

                        Face &f = mesh->face[fIndex];

                        cs->points[skelPoint].surfaceFaces.append(fIndex);

                        if (!f.V(0)->IsV())
                        {
                                int vIndex = f.V(0) - &mesh->vert[0];
                                cs->points[skelPoint].surfacePoints.append(vIndex);
                                f.V(0)->SetV();
                        }

                        if (!f.V(1)->IsV())
                        {
                                int vIndex = f.V(1) - &mesh->vert[0];
                                cs->points[skelPoint].surfacePoints.append(vIndex);
                                f.V(1)->SetV();
                        }

                        if (!f.V(2)->IsV())
                        {
                                int vIndex = f.V(2) - &mesh->vert[0];
                                cs->points[skelPoint].surfacePoints.append(vIndex);
                                f.V(2)->SetV();
                        }
                }

                //Mesh::UpdateColor::copySkelPointColor(mesh, cs);

                timer.stop_and_print();
        }




        // here we define a mapping from skeleton bones to mesh vertices.
        // This rigging can be tought as the solution of the skinning problem
        // with binary weights (i.e. each vertex belongs only to a skeleton bone)
        //
        // TO DO:
        //              - make sure that each vertex has a mapping with the skeleton
        //                (flooding may be used to expand the mapped areas)
        //
        //              - make sure that each bone maps to a connected area, removing
        //                some mapping errors that actually appear from time to time
        //
        static void boneRigging(TriMesh* mesh, CurveSkeleton *cs)
        {

                Timer timer("Bone Rigging");


                // respectively the ID of the skeleton bone associated to the face
                // anche the distance between the face and the associated skel bone
                //
                QVector<int>   faceSkelLink(mesh->fn, -1);
                QVector<float> faceSkelDist(mesh->fn, MAXFLOAT);


                for (CurveSkeleton::BoneIterator it = cs->bones.begin(); it != cs->bones.end(); ++it)
                {

                        Bone &bone = it.value();
                        int boneID = it.key();


                        QList<int> cc;

                        Point A = cs->points[bone.first()].coord,
                              B = cs->points[bone.last()].coord;

                        myGeometry::Plane pA(A, B - A);
                        myGeometry::Plane pB(B, A - B);

                        cc = Mesh::Select::connectedComponent(mesh, A, pA, pB);


                        // update the rigging map according with the MINIMA RULE: a face
                        // belongs to a skeleton point if and only if it belongs to its slice
                        // and the distance between the face and the point is minimal respect
                        // to the distances between the same face and the other skeleton points
                        // having it in their slice
                        //
                        foreach(int fIndex, cc)
                        {

                                Face *f = &mesh->face[fIndex];

                                float dist = myGeometry::pointSegmentDistance(A, B, f->P(0));

                                if (dist < faceSkelDist[fIndex])
                                {

                                        faceSkelLink[fIndex] = boneID;
                                        faceSkelDist[fIndex] = dist;

                                        f->C() = vcg::Color4b::Scatter(cs->bones.size(), boneID, 1.f, 1.f);
                                }
                        }
                }

                timer.stop_and_print();
        }
}
}
