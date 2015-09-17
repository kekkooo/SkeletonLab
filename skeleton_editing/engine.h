#pragma once
#include <QObject>
#include <QString>
#include <skel/base.h>
#include <mesh/mesh.h>
#include <utilities/skel_mesh_helper.h>

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

		
signals:
        void updateSkeleton ( Skel::CurveSkeleton * );
        void updateMesh     ( RMesh::mesh*          );

private:
    Skel::CurveSkeleton         * skel;
    RMesh::mesh                 * _mesh;
	#ifdef use_cgal
    Utils::skel_mesh_helper     * _skelMeshHelper;
	#endif
    bool                showMesh;
};
