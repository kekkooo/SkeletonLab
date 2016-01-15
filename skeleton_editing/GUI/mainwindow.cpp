#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#ifdef WIN32
	#include "gl/glu.h"
#endif
#ifdef OSX
	#include "OpenGL/glu.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	engine = new Engine();
	ui->setupUi(this);

	this->setFocusPolicy(Qt::NoFocus);

	this->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

	viewer = new Viewer(this);
	viewer->setFocusPolicy(Qt::StrongFocus);

	skelVisualizationWidget = new SkelVisualizationWidget(this);
	skelVisualizationWidget->show();

	samplingWidget = new SamplingWidget(this);
	samplingWidget->hide();

	choosePasteNode = new ChoosePasteNodeWidget(this);

	this->addDockWidget(Qt::BottomDockWidgetArea, choosePasteNode);

	choosePasteNode->setTitleBarWidget(new QWidget());
	choosePasteNode->hide();

	singleBranchingNodeDeletionWidget = new SingleBranchingNodeDeletionWidget(this);

	this->addDockWidget(Qt::BottomDockWidgetArea, singleBranchingNodeDeletionWidget);

	singleBranchingNodeDeletionWidget->setTitleBarWidget(new QWidget());
	singleBranchingNodeDeletionWidget->hide();

	this->setCentralWidget(viewer);
	this->addDockWidget(Qt::LeftDockWidgetArea, skelVisualizationWidget);

	createActions();
	createMenus();

	this->createConnections();

	this->engine->emitInitialUpdate();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::createActions(){

    // FILE MENU
	newSkel = new QAction(tr("&New Skeleton"), this);
	newSkel->setShortcuts(QKeySequence::New);
	newSkel->setStatusTip(tr("Create new skeleton"));
	connect(newSkel, SIGNAL(triggered()), this, SLOT(newSkeleton()));

	openSkel = new QAction(tr("&Open Skeleton"), this);
	openSkel->setShortcuts(QKeySequence::Open);
	openSkel->setStatusTip(tr("Open a skeleton file"));
	connect(openSkel, SIGNAL(triggered()), this, SLOT(openSkeletonFile()));

	saveSkel = new QAction(tr("&Save Skeleton"), this);
	saveSkel->setShortcuts(QKeySequence::SaveAs);
	saveSkel->setStatusTip(tr("Save the skeleton in a file"));
	connect(saveSkel, SIGNAL(triggered()), this, SLOT(saveSkeletonToFile()));

    //EDIT MENU
	undo = new QAction(tr("&Undo"), this);
	undo->setShortcuts(QKeySequence::Undo);
	undo->setStatusTip(tr("Undo last edit"));
	connect(undo, SIGNAL(triggered()), viewer, SLOT(action_undo()));

	redo = new QAction(tr("&Redo"), this);
	redo->setShortcuts(QKeySequence::Redo);
	redo->setStatusTip(tr("Redo last edit"));
	connect(redo, SIGNAL(triggered()), viewer, SLOT(action_redo()));

	articulation = new QAction(tr("&Articulation"), this);
	articulation->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
	articulation->setCheckable(true);
	articulation->setStatusTip(tr("Set a node as articulation"));
	connect(articulation, SIGNAL(triggered(bool)), viewer, SLOT(setNodeArticulationState(bool)));

    buildBVH = new QAction(tr("&Build BVH"), this);
    buildBVH->setStatusTip(tr("Build collision detection hierarchy"));
    connect( buildBVH, SIGNAL(triggered()), engine, SLOT(buildBoundingVolumeHierarchy()));

    approximate = new QAction(tr("&Approximate Skel"), this);
    approximate->setStatusTip(tr("Approximate the skeleton using a modified Donald-Peuker algorithm"));
    connect( approximate, SIGNAL(triggered()), engine, SLOT(approximateSkel()));

    // POINTS MENU
	deleteNode = new QAction(tr("&Delete Node"), this);
	deleteNode->setShortcut(QKeySequence::Delete);
	deleteNode->setStatusTip(tr("Delete selected node"));
	connect(deleteNode, SIGNAL(triggered()), viewer, SLOT(action_deleteNode()));

	copy = new QAction(tr("&Copy"), this);
	copy->setShortcuts(QKeySequence::Copy);
	copy->setStatusTip(tr("Copy a bone"));
	connect(copy, SIGNAL(triggered()), viewer, SLOT(action_copy()));

	paste = new QAction(tr("&Paste"), this);
	paste->setShortcuts(QKeySequence::Paste);
	paste->setStatusTip(tr("Paste a bone previously copied"));
	connect(paste, SIGNAL(triggered()), viewer, SLOT(action_paste()));

	deselectAll = new QAction(tr("&Deselect All"), this);
	deselectAll->setShortcut(Qt::Key_Space);
	deselectAll->setStatusTip(tr("Deselect all nodes"));
	connect(deselectAll, SIGNAL(triggered()), viewer, SLOT(action_deselectAll()));

	deleteConnection = new QAction(tr("&Delete Connection"), this);
	deleteConnection->setShortcut(Qt::Key_C);
	deleteConnection->setStatusTip(tr("Delete connections between the two selected nodes"));
	connect(deleteConnection, SIGNAL(triggered()), viewer, SLOT(action_deleteConnection()));

	addNodeBetween = new QAction(tr("&Add Node Between"), this);
	addNodeBetween->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	addNodeBetween->setStatusTip(tr("Add a new node between the two selected ones"));
	connect(addNodeBetween, SIGNAL(triggered()), viewer, SLOT(action_addNodeBetween()));

	sampling = new QAction(tr("&Resampling"), this);
	sampling->setShortcut(Qt::Key_S);
	sampling->setStatusTip(tr("Resample a bone"));
	connect(sampling, SIGNAL(triggered()), viewer, SLOT(action_sampling()));

    //MESH MENU
	loadMeshAction = new QAction(tr("&Load Mesh"), this);
	loadMeshAction->setStatusTip(tr("Load a polygonal mesh"));
	connect( loadMeshAction, SIGNAL(triggered()), this, SLOT(openMesh()));

	testSkeletonInside = new QAction(tr("&Position skeleton inside mesh"), this);
	testSkeletonInside->setStatusTip(tr("Position the skeleton on the inside of the mesh"));
	connect( testSkeletonInside, SIGNAL(triggered()), engine, SLOT(testSkelInside()));

	centerSkeleton = new QAction(tr("&Center skeleton inside mesh"), this);
	centerSkeleton->setStatusTip(tr("Center the skeleton with respect of the mesh"));
	connect( centerSkeleton, SIGNAL(triggered()), engine, SLOT(centerSkeleton()));

	centerAndRefitSkeleton = new QAction(tr("&Center and refit skel"), this);
	centerAndRefitSkeleton->setStatusTip(tr("Center the skeleton inside the mesh and resize its balls"));
	connect( centerAndRefitSkeleton, SIGNAL(triggered()), engine, SLOT(centerAndResizeSkeleton()));

    centerWithSQEM = new QAction(tr("&Center Skel with SQEM"), this);
    centerWithSQEM->setStatusTip(tr("Center the skeleton inside the mesh and resize its balls using the SQEM"));
    connect( centerWithSQEM, SIGNAL(triggered()), engine, SLOT(centerSkeletonWithSQEM()));

	resetNodesSize = new QAction(tr("&Reset node's size"), this);
	resetNodesSize->setStatusTip(tr("Reset the max balls to a standard non-harmful size"));
	connect( resetNodesSize, SIGNAL(triggered()), engine, SLOT(resetNodeSize()));

	alignSkelWithPCA = new QAction(tr("&Align Skel to Mesh"), this);
	alignSkelWithPCA->setStatusTip(tr("Align the skeleton with the mesh"));
	connect( alignSkelWithPCA, SIGNAL(triggered()), engine, SLOT(alignSkelWithPCA()));


	showHideMesh = new QAction(tr("&Show/Hide Mesh"), this);
	showHideMesh->setStatusTip(tr("Show or Hide Mesh"));
//    connect( loadMesh, SIGNAL(triggered()), viewer, SLOT(loadMesh()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newSkel);
	fileMenu->addAction(openSkel);
	fileMenu->addAction(saveSkel);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(undo);
	editMenu->addAction(redo);
	editMenu->addSeparator();
	editMenu->addAction(copy);
	editMenu->addAction(paste);
	editMenu->addSeparator();
	editMenu->addAction(articulation);
    editMenu->addAction(buildBVH);
    editMenu->addAction(approximate);

	pointsMenu = menuBar()->addMenu(tr("&Points"));
	pointsMenu->addAction(deleteNode);
	pointsMenu->addAction(deselectAll);
	pointsMenu->addAction(deleteConnection);
	pointsMenu->addSeparator();
	pointsMenu->addAction(addNodeBetween);

	boneMenu = menuBar()->addMenu(tr("&Bones"));
	boneMenu->addAction(sampling);

	meshMenu = menuBar()->addMenu(tr("&Mesh"));
	meshMenu->addAction(loadMeshAction);
	meshMenu->addAction(showHideMesh);
	meshMenu->addAction(testSkeletonInside);
	meshMenu->addAction(centerSkeleton);
	meshMenu->addAction(centerAndRefitSkeleton);
    meshMenu->addAction(centerWithSQEM);
	meshMenu->addAction(resetNodesSize);
	meshMenu->addAction(alignSkelWithPCA);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	return QMainWindow::resizeEvent(event);
}

void MainWindow::createConnections()
{
	// FROM MainWindow TO engine
	connect( this, SIGNAL(loadSkeleton(QString)),	engine, SLOT(loadSkeleton(QString)));
	connect( this, SIGNAL(saveSkeleton(QString)),	engine, SLOT(saveSkeleton(QString)));
	connect( this, SIGNAL(createNewSkeleton()),     engine, SLOT(newSkeleton()));
	connect( this, SIGNAL(loadMesh(QString)),       engine, SLOT(loadMesh(QString)));

	// FROM engine TO viewer
	connect( engine, SIGNAL(updateSkeleton(Skel::CurveSkeleton*)), viewer, SLOT(updateSkeleton(Skel::CurveSkeleton*)));
	connect( engine, SIGNAL(updateMesh(RMesh::mesh*)),             viewer, SLOT(updateMesh(RMesh::mesh*)));

	// FROM skelVisualizationWidget to viewer
	connect( skelVisualizationWidget, SIGNAL(featurePointsChanged(bool)),   viewer, SLOT(drawSkelFPs(bool)));
	connect( skelVisualizationWidget, SIGNAL(bonePointsChanged(bool)),      viewer, SLOT(drawSkelBonePs(bool)));
	connect( skelVisualizationWidget, SIGNAL(maxBallsChanged(bool)),        viewer, SLOT(drawSkelMaxBalls(bool)));
	connect( skelVisualizationWidget, SIGNAL(maxBallsBoneChanged(bool)),    viewer, SLOT(drawSkelMaxBallsBP(bool)));
	connect( skelVisualizationWidget, SIGNAL(defaultColor(bool)),           viewer, SLOT(drawSkelDefaultColors(bool)));
	connect( skelVisualizationWidget, SIGNAL(boneColor(bool)),              viewer, SLOT(drawSkelBoneColors(bool)));
	connect( skelVisualizationWidget, SIGNAL(editModeChanged(bool)),        viewer, SLOT(editTypeChanged(bool)));
	connect( skelVisualizationWidget, SIGNAL(nodeFusionChanged(bool)),		viewer, SLOT(nodeFusionChanged(bool)));
	connect( skelVisualizationWidget, SIGNAL(meshLimitChanged(bool)),		viewer, SLOT(meshLimitChanged(bool)));

	// FROM mainwindow to viewer
	connect( this, SIGNAL(loadSkeleton(QString)), viewer, SLOT(load_new_skeleton()));
	connect( this, SIGNAL(createNewSkeleton()),   viewer, SLOT(load_new_skeleton()));

	// Copy connections
	connect( viewer, SIGNAL(startCopy()), choosePasteNode, SLOT(widgetShow()));

	// Sampling connections
	connect( viewer, SIGNAL(initSampling(int, int)), samplingWidget, SLOT(samplingStart(int, int)));
	connect( samplingWidget, SIGNAL(okButtonPressed(int)),		viewer, SLOT(startSampling(int)));
	connect( samplingWidget, SIGNAL(cancelButtonPressed()), viewer, SLOT(cancelSampling()));

	// Enabling and disabling commands connections
	connect( viewer, SIGNAL(enableActions()),			this, SLOT(enableActions()));
	connect( viewer, SIGNAL(disableActions()),			this, SLOT(disableActions()));
	connect( viewer, SIGNAL(enableUndo()),				this, SLOT(enableUndo()));
	connect( viewer, SIGNAL(disableUndo()),				this, SLOT(disableUndo()));
	connect( viewer, SIGNAL(enableRedo()),				this, SLOT(enableRedo()));
	connect( viewer, SIGNAL(disableRedo()),				this, SLOT(disableRedo()));
	connect( viewer, SIGNAL(enableCopy()),				this, SLOT(enableCopy()));
	connect( viewer, SIGNAL(disableCopy()),				this, SLOT(disableCopy()));
	connect( viewer, SIGNAL(enablePaste()),				this, SLOT(enablePaste()));
	connect( viewer, SIGNAL(disablePaste()),			this, SLOT(disablePaste()));
	connect( viewer, SIGNAL(enableDeleteConnection()),	this, SLOT(enableDeleteConnection()));
	connect( viewer, SIGNAL(disableDeleteConnection()),	this, SLOT(disableDeleteConnection()));
	connect( viewer, SIGNAL(enableAddBetween()),		this, SLOT(enableAddBetween()));
	connect( viewer, SIGNAL(disableAddBetween()),		this, SLOT(disableAddBetween()));
	connect( viewer, SIGNAL(enableSampling()),			this, SLOT(enableSampling()));
	connect( viewer, SIGNAL(disableSampling()),			this, SLOT(disableSampling()));
	connect( viewer, SIGNAL(enableArticulation()),		this, SLOT(enableArticulation()));
	connect( viewer, SIGNAL(disableArticulation()),		this, SLOT(disableArticulation()));

	connect( viewer, SIGNAL(checkArticulation()),		this, SLOT(checkArticulation()));
	connect( viewer, SIGNAL(uncheckArticulation()),		this, SLOT(uncheckArticulation()));

	connect( viewer, SIGNAL(deleteBranchingNodeSignal()), singleBranchingNodeDeletionWidget, SLOT(widgetShow()));

	connect( viewer, SIGNAL(changeEditMode(Qt::CheckState)), skelVisualizationWidget, SLOT(changeEditType(Qt::CheckState)));

	connect( viewer, SIGNAL(changeNodeFusion(Qt::CheckState)), skelVisualizationWidget, SLOT(changeNodeFusion(Qt::CheckState)));
	connect( viewer, SIGNAL(changeMeshLimit(Qt::CheckState)), skelVisualizationWidget, SLOT(changeMeshLimit(Qt::CheckState)));

	// Single Branching node deletion widget connection
	connect( singleBranchingNodeDeletionWidget, SIGNAL(transferButtonPressed()),	viewer, SLOT(singleBranchingNodeDeletionWidgetTransferButtonPressed()));
	connect( singleBranchingNodeDeletionWidget, SIGNAL(deleteButtonPressed()),		viewer, SLOT(singleBranchingNodeDeletionWidgetDeleteButtonPressed()));
	connect( singleBranchingNodeDeletionWidget, SIGNAL(cancelButtonPressed()),		viewer, SLOT(singleBranchingNodeDeletionWidgetCancelButtonPressed()));

	connect( choosePasteNode, SIGNAL(selectButtonPressed()),	viewer, SLOT(choosePasteNodeWidgetSelectButtonPressed()));
	connect( choosePasteNode, SIGNAL(helpButtonPressed()),		viewer, SLOT(choosePasteNodeWidgetHelpButtonPressed()));
	connect( choosePasteNode, SIGNAL(cancelButtonPressed()),	viewer, SLOT(choosePasteNodeWidgetCancelButtonPressed()));

	connect( viewer, SIGNAL(hideChoosePasteNodeWidget()), choosePasteNode, SLOT(hide()));
}

void MainWindow::openMesh()
{
	QString filename = QFileDialog::getOpenFileName(NULL,
													"Open mesh",
													".",
													"mesh files (*.obj *.off );;");
	//3D Meshes (*.off *.ply *.obj);; OBJ(*.obj);; OFF(*.off);; PLY(*.ply)"
	if (!filename.isEmpty()) emit loadMesh(filename);
}

void MainWindow::openSkeletonFile()
{
	QString filename = QFileDialog::getOpenFileName(NULL,
													"Open skel (TVCG)",
													".",
                                                    "LIVESU2012 (*.skel );; TAGLIASACCHI 2012(*.cg);; DEY_SUN 2006 (*.cskel);;"
                                                    );
	if (!filename.isEmpty()) emit loadSkeleton(filename);


}

void MainWindow::saveSkeletonToFile()
{
	QString filename = QFileDialog::getSaveFileName(NULL,
													"Save Skeleton",
													".",
													"Skeleton (*.skel)");
	if (!filename.isEmpty()) emit saveSkeleton(filename);
}

void MainWindow::newSkeleton()
{
	int ret = QMessageBox::warning(this, tr("Skeleton Editing"),
								   tr("The document has been modified.\n"
									  "Do you want to save your changes?"),
								   QMessageBox::Save | QMessageBox::Discard
								   | QMessageBox::Cancel,
								   QMessageBox::Save);

	if( ret == QMessageBox::Save )
	{
		saveSkeletonToFile();
	}
	if( ret != QMessageBox::Cancel)
	emit MainWindow::createNewSkeleton();

}

void MainWindow::enableActions()
{
	newSkel->setEnabled(true);
	openSkel->setEnabled(true);
	saveSkel->setEnabled(true);

	undo->setEnabled(true);
	redo->setEnabled(true);

	deleteNode->setEnabled(true);
	deselectAll->setEnabled(true);
}

void MainWindow::disableActions()
{
	newSkel->setEnabled(false);
	openSkel->setEnabled(false);
	saveSkel->setEnabled(false);

	undo->setEnabled(false);
	redo->setEnabled(false);

	deleteNode->setEnabled(false);
	deselectAll->setEnabled(false);
}

void MainWindow::enableUndo()
{
	undo->setEnabled(true);
}

void MainWindow::disableUndo()
{
	undo->setEnabled(false);
}

void MainWindow::enableRedo()
{
	redo->setEnabled(true);
}

void MainWindow::disableRedo()
{
	redo->setEnabled(false);
}

void MainWindow::enableCopy()
{
	copy->setEnabled(true);
}

void MainWindow::disableCopy()
{
	copy->setEnabled(false);
}

void MainWindow::enablePaste()
{
	paste->setEnabled(true);
}

void MainWindow::disablePaste()
{
	paste->setEnabled(false);
}

void MainWindow::enableDeleteConnection()
{
	deleteConnection->setEnabled(true);
}

void MainWindow::disableDeleteConnection()
{
	deleteConnection->setEnabled(false);
}

void MainWindow::enableAddBetween()
{
	addNodeBetween->setEnabled(true);
}

void MainWindow::disableAddBetween()
{
	addNodeBetween->setEnabled(false);
}

void MainWindow::enableSampling()
{
	sampling->setEnabled(true);
}

void MainWindow::disableSampling()
{
	sampling->setEnabled(false);
}

void MainWindow::enableArticulation()
{
	articulation->setEnabled(true);
}

void MainWindow::disableArticulation()
{
	articulation->setEnabled(false);
}

void MainWindow::checkArticulation()
{
	articulation->setChecked(true);
}

void MainWindow::uncheckArticulation()
{
	articulation->setChecked(false);
}
