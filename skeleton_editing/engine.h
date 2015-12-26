#pragma once
#include <QObject>
#include <QString>
#include <skel/base.h>
#include <mesh/mesh.h>
#include <utilities/skel_mesh_helper.h>
#include <collision_detection/collision_detection.h>

class Engine : public QObject
{
    Q_OBJECT

public:
    Engine();
    void emitInitialUpdate();

public
    slots:
        void saveSkeleton   ( QString filename );
        void loadSkeleton   ( QString filename );
        void newSkeleton    (                  );
        void executeSQM     (                  );        
        void loadMesh       ( QString filename );
        void showHideMesh   (                  );
        void testSkelInside (                  );
        void centerSkeleton (                  );
        void centerAndResizeSkeleton
                            (                  );
        void resetNodeSize  (                  );
        void alignSkelWithPCA ();

        void buildBoundingVolumeHierarchy();

		
signals:
        void updateSkeleton ( Skel::CurveSkeleton * );
        void updateMesh     ( RMesh::mesh*          );

private:
    Skel::CurveSkeleton         * skel;
    RMesh::mesh                 * _mesh;
    CollisionDetection::BVH     * bvh = NULL;
	#ifdef use_cgal
    Utils::skel_mesh_helper     * _skelMeshHelper;
	#endif
    bool                showMesh;
};
