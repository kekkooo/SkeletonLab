#define use_boost

#include "engine.h"
#include <skel/export.h>
#include <skel/import.h>
#include <skel/edit.h>
#include <skel/update/approximation.h>



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
    if( _skelMeshHelper == NULL ) return;
    _skelMeshHelper->ResetMaximalBalls( *skel );

    emit updateSkeleton(skel);
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

void Engine::approximateSkel(){
// TODO it should open a widget allowing to select a "fidelity" or a target reduction of the number of nodes
    double min_radius = std::numeric_limits<double>::max();
    double multiplier = 0.25;
//    for( const Skel::SkelPoint& p : skel->points ) { if( min_radius > p.radius ){ min_radius = p.radius; }}

//    double threshold = ( min_radius / 2.0 ) * multiplier;
    double threshold = multiplier;    

    Skel::Update::approximate( *this->skel, threshold );
    emit updateSkeleton( skel );
}


void Engine::centerSkeletonWithSQEM(){
    //find the selected node
    int selected_id = -1;
    for( const auto& p : skel->points ){
        if( p.isSelected()){ selected_id = p.id; break; }
    }

    if( _skelMeshHelper == NULL ) return;
    RMesh::mesh::buildColors( *_mesh );
    _skelMeshHelper->centeringWithSQEM( *skel, true, selected_id );
    emit updateSkeleton( skel );
}

void Engine::cleanupClusters(){
    Skel::Update::CleanupClustersAtBranchingNodes( *skel );
    emit updateSkeleton( skel );
}

void Engine::collapseSpurious(){
    Skel::Update::CollapseSpuriousBranches( *skel );
    emit updateSkeleton( skel );
}
