#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <engine.h>
#include <GUI/skelvisualizationwidget.h>
#include <GUI/viewer.h>
#include <GUI/samplingwidget.h>
#include <GUI/singlebranchingnodedeletionwidget.h>
#include <GUI/choosepastenodewidget.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow						* ui;
	Engine								* engine;
	SkelVisualizationWidget				* skelVisualizationWidget;
	Viewer								* viewer;
	SamplingWidget						* samplingWidget;
	SingleBranchingNodeDeletionWidget	* singleBranchingNodeDeletionWidget;
	ChoosePasteNodeWidget				* choosePasteNode;

	void createActions();
	void createMenus();

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *pointsMenu;
	QMenu *boneMenu;
	QMenu *meshMenu;
//	QMenu *help;

    // FILE MENU
	QAction *newSkel;
	QAction *openSkel;
	QAction *saveSkel;

    // EDIT MENU
	QAction *copy;
	QAction *paste;
	QAction *undo;
	QAction *redo;
	QAction *articulation;
    QAction *buildBVH;
    QAction *approximate;
//    QAction *freezeNonFeaturePoints;

    // MESH MENU
	QAction *loadMeshAction;
	QAction *showHideMesh;
	QAction *testSkeletonInside;
	QAction *centerSkeleton;
	QAction *centerAndRefitSkeleton;
    QAction *centerWithSQEM;
    QAction *resetNodesSize;
    QAction *alignSkelWithPCA;    

    // POINTS MENU
	QAction *deleteNode;
	QAction *deselectAll;
	QAction *deleteConnection;
	QAction *addNodeBetween;

    // BONES MENU
	QAction *sampling;

	void createConnections();
	void resizeEvent(QResizeEvent *);

signals:
	void loadMesh(QString);
	void loadSkeleton(QString);
	void saveSkeleton(QString);
	void createNewSkeleton();

public slots:
	void openMesh();

	void openSkeletonFile();
	void saveSkeletonToFile();
	void newSkeleton();

	void enableActions();
	void disableActions();

	void enableUndo();
	void disableUndo();

	void enableRedo();
	void disableRedo();

	void enableCopy();
	void disableCopy();

	void enablePaste();
	void disablePaste();

	void enableDeleteConnection();
	void disableDeleteConnection();

	void enableAddBetween();
	void disableAddBetween();

	void enableSampling();
	void disableSampling();

	void enableArticulation();
	void disableArticulation();

	void checkArticulation();
	void uncheckArticulation();
};

#endif // MAINWINDOW_H
