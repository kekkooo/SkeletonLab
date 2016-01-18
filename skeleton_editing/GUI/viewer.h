#pragma once
#ifdef OSX
	#include <QGLViewer/qglviewer.h>
	#include <QGLViewer/vec.h>
	#include <QGLViewer/manipulatedFrame.h>
#endif
#ifdef WIN32
	#include <qglviewer.h>
	#include <vec.h>
	#include <manipulatedFrame.h>
#endif

#include <QTextEdit>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPair>
#include <QListWidgetItem>

#include "skel/update/topology.h"
#include "skel/update/color.h"
#include "skel/base.h"
#include "skel/paint.h"
#include "mesh/mesh.h"
#include "mesh/paint.h"
#include "primitives/BBox.h"
#include "utilities/object.h"
#include "bufferStatus.h"
#include "utilities/manipulatedFrameSetConstraint.h"
#include "utilities/polyfit.h"
#include "utilities/skel_mesh_helper.h"
#include "collision_detection/collision_detection.h"

using namespace UpdateTopology;

class Viewer : public QGLViewer
{
	Q_OBJECT

public:

	Viewer();
	explicit Viewer(QWidget *parent = 0);

protected:

	virtual void init();
	virtual void draw();
	virtual void postSelection(const QPoint&);
	virtual void endSelection(const QPoint&);
	virtual void drawWithNames();

	virtual QString helpString ()const;


	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

	virtual void wheelEvent(QWheelEvent *e);

	virtual void keyPressEvent(QKeyEvent *e);

public slots:

	void updateSkeleton ( Skel::CurveSkeleton*  );
	void updateMesh     ( RMesh::mesh*          );

	void drawSkelFPs			(bool b);
	void drawSkelBonePs			(bool b);
	void drawSkelMaxBalls		(bool b);
	void drawSkelMaxBallsBP		(bool b);
	void drawSkelDefaultColors	(bool b);
	void drawSkelBoneColors		(bool b);

	void editTypeChanged		(bool b);
	void nodeFusionChanged		(bool b);
	void meshLimitChanged		(bool b);

	// This method is called when a new skeleton is loaded. Initializations.
	void load_new_skeleton();

	void setNodeArticulationState(bool newState);

	void action_undo();
	void action_redo();
	void action_copy();
	void action_paste();
	void action_deselectAll();
	void action_deleteNode();
	void action_deleteConnection();
	void action_addNodeBetween();
	void action_sampling();

	void startSampling(int newBoneSize);
	void cancelSampling();

	void singleBranchingNodeDeletionWidgetTransferButtonPressed();
	void singleBranchingNodeDeletionWidgetCancelButtonPressed();
	void singleBranchingNodeDeletionWidgetDeleteButtonPressed();

	void choosePasteNodeWidgetSelectButtonPressed();
	void choosePasteNodeWidgetHelpButtonPressed();
	void choosePasteNodeWidgetCancelButtonPressed();

    void collapseBranch();

signals:

	void transferNeighborsSelected(SkelPoint);

	void startCopy();

	void initSampling(int, int);

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

	void changeEditMode(Qt::CheckState);
	void changeNodeFusion(Qt::CheckState);
	void changeMeshLimit(Qt::CheckState);

	void enableSampling();
	void disableSampling();

	void enableArticulation();
	void disableArticulation();

	void checkArticulation();
	void uncheckArticulation();

	void deleteBranchingNodeSignal();

	void hideChoosePasteNodeWidget();


private:

	Skel::CurveSkeleton	*skel;
	RMesh::mesh         *_mesh;
	Primitives::BBoxD sceneBBox;
	int	skeleton_draw_mode;

	// A flag that is true whether there is an ongoing insertion.
	bool activeInsertion;

	qglviewer::Vec orig, dir, selectedPoint;

	void updateSceneBoundingBox();
	void setDefaultMaterial();
	void setSkelDrawMode(bool set, Skel::Paint::DRAWMODE mode);

	void drawSelectionRectangle() const;
	void addIdToSelection(int id);
	void removeIdFromSelection(int id);
	void startManipulation();

	//removes a joint and all the bones that are not connected to another joint.
	//the operation will be a success only if in the skeleton there are at most
	//2 joints.
	int checkIfClose(Point3d s, float radius);

	void addNewNode();

	void clearSelection();
	void findNearest( double x, double y, double z );

	enum FixedCamera { X_PLUS = 0, X_MINUS = 1, Y_PLUS = 2, Y_MINUS = 3, Z_PLUS = 4, Z_MINUS = 5 };
	void setFixedCamera( FixedCamera c );

	// Current rectangular selection
	QRect rectangle_;

	// Different selection modes
	enum SelectionMode { NONE, ADD, REMOVE };
	SelectionMode selectionMode_;

	QList<Object*> objects_;
	QList<int> selection_;

	int newInsertId, oldPointId;

	int lastPointSelected;
	bool deselection;
	bool nodeFusionActive;
	bool copyActive;

	enum EditType {BONE_EDIT, NODE_EDIT};

	EditType currentEditType;

	bool operationIsActive;

//	bool singleSelection;

	bool deletingSingleBranchingNode;

	SkelPoint branchingNodeToDelete;

	int             TOIid;
	QList<int>      copyList;
	int             pointToMergeForCopy;
	Point3d         tempCoordinates;
	bool            firstNeighDirection,
					secondNeighDirection;
	int             stepsOfCentralNeigh;
	double          scale_factor;

	QList<SkelPoint> normalizePointsToCopy(QList<SkelPoint> pointsToNormalize);

	BufferStatus    status;
	QList<SkelPoint> copyBuffer;
	ManipulatedFrameSetConstraint *copyFrame;

	#ifdef UNIX
	std::list<CGAL_Triangle> trilist;
	RMesh::Utils::CGAL_AABB_Tree* AABB_Tree;
	#endif

	bool canMove;
	bool meshLimit;

	QVector<Point3d> prevPositions;

	void test();
};

