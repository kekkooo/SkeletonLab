#define use_boost

#include "engine.h"
#include <skel/export.h>
#include <skel/import.h>
#include <skel/edit.h>



Engine::Engine()
{
    this->skel              = NULL;
    this->skel              = new Skel::CurveSkeleton(true);
    qDebug() << "skel contains #bones : " << skel->bones.size();
    this->_mesh             = NULL;
	#ifdef use_cgal
    this->_skelMeshHelper   = NULL;
	#endif
    this->showMesh          = true;
}

void Engine::emitInitialUpdate()
{
	emit updateSkeleton(skel);
}

void Engine::loadSkeleton(QString filename)
{
    QStringList parts = filename.split( QChar( '.' ));
    int skel_type = -1;

    if ( parts.last( ).toLower() == "skel" )    { skel_type = 0; } // livesu2012
    if ( parts.last( ).toLower() == "cg" )      { skel_type = 1; } // tagliasacchi2012
    if ( parts.last( ).toLower() == "cskel" )   { skel_type = 2; } // dey&sun

    switch ( skel_type ) {
    case 0:
        this->skel = Skel::Importer::load_TVCG_2012( filename );
        break;
    case 1:
        this->skel = Skel::Importer::load_Tagliasacchi_2012( filename );
        break;
    case 2:
        this->skel = Skel::Importer::load_DeySun_2006( filename );
        break;
    default:
        throw(std::domain_error("File Format not Supported"));
        break;
    }
    //this->skel = Skel::Importer::load_TVCG_2012(filename);
	emit updateSkeleton(skel);
}

void Engine::saveSkeleton(QString filename)
{
	Skel::Exporter::SaveToFile(this->skel, filename);
}

void Engine::newSkeleton()
{
	this->skel->clear();
	this->skel->addRootNode();
	emit updateSkeleton(skel);
}

void Engine::executeSQM()
{
    assert(0);
}

void Engine::loadMesh(QString filename)
{
#ifdef use_cgal
    if( _mesh == NULL) this->_mesh = new RMesh::mesh();
    this->_mesh->clear();
    RMesh::mesh::load( filename, *_mesh );
    if( _skelMeshHelper == NULL ) { _skelMeshHelper = new Utils::skel_mesh_helper(); }
    _skelMeshHelper->setMesh( _mesh );
    emit updateMesh(_mesh);
#endif
}

void Engine::showHideMesh()
{

}

void Engine::testSkelInside()
{
#ifdef use_cgal
    if( _skelMeshHelper == NULL ) return;
//    _skelMeshHelper->checkSkeletonInsideMesh( *skel );
    _skelMeshHelper->put_back_inside(skel);
//    _skelMeshHelper->re_center( *skel );
    emit updateSkeleton(skel);
#endif
}

void Engine::centerSkeleton()
{
#ifdef use_cgal
    if( _skelMeshHelper == NULL ) return;
    _skelMeshHelper->re_center( skel, false );
    emit updateSkeleton(skel);
#endif

}

void Engine::centerAndResizeSkeleton()
{
#ifdef use_cgal
    if( _skelMeshHelper == NULL ) return;
    _skelMeshHelper->put_back_inside(skel);
    _skelMeshHelper->re_center( skel, true );
    emit updateSkeleton(skel);
#endif

}

void Engine::resetNodeSize()
{
    if( this->skel && this->_mesh )
    {
        double size = this->_mesh->bbox.Diagonal() / 250;
        for( int i = 0; i < skel->points.size(); ++i )
        {
            skel->points[i].radius = size;
        }
        emit updateSkeleton(skel);
    }


}

void Engine::alignSkelWithPCA()
{
//    assert(0);
#ifdef use_cgal
    vector< Primitives::Point3d > points;
    foreach( SkelPoint p, skel->points ) points.push_back(p.coord);
    skel->axes = RMesh::mesh::buildPCA( points );
    cout << skel->axes.center.x << ", " << skel->axes.center.y << ", " << skel->axes.center.z << endl;
    Skel::Edit::translate( this->skel, this->_mesh->axes.center - skel->axes.center );
    emit updateSkeleton( skel );
#endif

}

void Engine::buildBoundingVolumeHierarchy(){
    if( NULL == this->bvh ){
        assert( NULL != this->skel );
        bvh = new CollisionDetection::BVH( this->skel );
    }
    else bvh->Clear();;
    bvh->Build();
}

