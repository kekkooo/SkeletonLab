#include "viewer.h"
#include <skel/edit.h>

using namespace qglviewer;

Viewer::Viewer() : QGLViewer()
{
}

Viewer::Viewer(QWidget *parent) : QGLViewer(parent)
{
	this->skeleton_draw_mode = Skel::Paint::BONES           |
							   Skel::Paint::FEATURE_POINTS  |
							   Skel::Paint::DEFAULT_COLOR   |
							   Skel::Paint::PCA             |
							   Skel::Paint::BONE_POINTS;
	this->skel  = NULL;
	this->_mesh = NULL;
	this->scale_factor = 1.0;
}

void Viewer::updateSkeleton(Skel::CurveSkeleton *skeleton)
{
	this->skel = skeleton;
	this->updateSceneBoundingBox();
	this->update();
}

void Viewer::updateMesh(RMesh::mesh *m)
{
	this->_mesh = m;

	// this should be done only in some cases
	Skel::Edit::translate( this->skel, m->bbox.Center() - sceneBBox.Center() );

#ifdef use_cgal
//    std::vector< Primitives::Point3d > cs_points;
//    for( int i = 0; i < skel->points.size(); ++i )
//    {
//        cs_points.push_back( skel->points[i].coord );
//    }
//    skel->axes = RMesh::mesh::buildPCA(cs_points);
//    skel->axes.center = this->_mesh->axes.center;
//    skel->axes.size = this->_mesh->bbox.Diagonal();
#endif


	this->updateSceneBoundingBox();
	this->updateSkeleton(this->skel);
	this->update();

}

void Viewer::drawSkelFPs(bool b)
{
	setSkelDrawMode( b, Skel::Paint::FEATURE_POINTS );
}

void Viewer::drawSkelBonePs(bool b)
{
	setSkelDrawMode( b, Skel::Paint::BONE_POINTS );
}

void Viewer::drawSkelMaxBalls(bool b)
{
	setSkelDrawMode( b, Skel::Paint::MAX_SPHERES_FP );
}

void Viewer::drawSkelMaxBallsBP(bool b)
{
	setSkelDrawMode( b, Skel::Paint::MAX_SPHERES );
}

void Viewer::drawSkelDefaultColors(bool b)
{
	setSkelDrawMode( b, Skel::Paint::DEFAULT_COLOR );
}

void Viewer::drawSkelBoneColors(bool b)
{
	setSkelDrawMode( b, Skel::Paint::BONE_COLOR );
}

void Viewer::editTypeChanged(bool b)
{
	if (true == b)
	{
		setKeyDescription(Qt::Key_Space, "Clear the selection");
		setKeyDescription(Qt::CTRL + Qt::Key_C, "Copy an entire bone");
		setKeyDescription(Qt::CTRL + Qt::Key_V, "Paste what was previously copied, if the copy was valid");
		setKeyDescription(Qt::CTRL + Qt::Key_Y, "Restore previous changes");
		setKeyDescription(Qt::CTRL + Qt::Key_Z, "Undone the last changes");
		setKeyDescription(Qt::CTRL + Qt::Key_N, "Create a new skeleton with only the root node");
		setKeyDescription(Qt::CTRL + Qt::Key_O, "Open a saved skeleton");
		setKeyDescription(Qt::Key_P, "Show on the console informations about the nodes of the skeleton");
		setKeyDescription(Qt::Key_Delete,"Delete all the bone only if the bone end with a leaf but it will not delete the joint. ");
		setKeyDescription(Qt::ShiftModifier + Qt::Key_Delete,"Remove all the nodes in a bone that have two joints. It will not remove the joints itherselfs");
		setKeyDescription(Qt::Key_Backspace,"Make a prouning");
		setKeyDescription(Qt::Key_Space, "Clear the selection");
		setKeyDescription(Qt::Key_O, "");

		setMouseBindingDescription(Qt::ShiftModifier, Qt::LeftButton, "Creates a rectangle that select all nodes contained in his area");
		setMouseBindingDescription(Qt::ControlModifier, Qt::LeftButton, "Rotate the selected nodes basing the rotation on their average position");
		setMouseBindingDescription(Qt::ControlModifier, Qt::RightButton, "Translate the selcted nodes basing the translation on the mouse movement. If you translate the node near another, they will be merged");
		setMouseBindingDescription(Qt::AltModifier, Qt::LeftButton, "Creates a rectangle that deselect all nodes contained in his area");
		setMouseBindingDescription(Qt::ShiftModifier, Qt::RightButton, "");
		setMouseBindingDescription(Qt::AltModifier, Qt::NoButton, "");
		setMouseBindingDescription(Qt::ShiftModifier, Qt::NoButton, "");

		clearSelection();
		currentEditType = BONE_EDIT;

		emit enableSampling();

		emit disableArticulation();
		emit uncheckArticulation();
		emit disableCopy();
		emit disablePaste();
		emit disableDeleteConnection();
	}
	else
	{
		setKeyDescription(Qt::Key_Space, "Clear the selection");
		setKeyDescription(Qt::CTRL + Qt::Key_C, "Copy all the selected nodes");
		setKeyDescription(Qt::CTRL + Qt::Key_V, "Paste what was previously copied, if the copy was valid");
		setKeyDescription(Qt::CTRL + Qt::Key_Y, "Restore previous changes");
		setKeyDescription(Qt::CTRL + Qt::Key_Z, "Undone the last changes");
		setKeyDescription(Qt::CTRL + Qt::Key_N, "Create a new skeleton with only the root node");
		setKeyDescription(Qt::CTRL + Qt::Key_O, "Open a saved skeleton");
		setKeyDescription(Qt::Key_P, "Show on the console informations about the node of the skeleton");
		setKeyDescription(Qt::Key_O, "Show on the console information about the selected nodes");
		setKeyDescription(Qt::Key_Delete,
			"Delete the selection, there are 3 cases: \n\tif a node is a joint, will delete al the nodes that are not connected to another joint and the joint itself;\n\tif is a branch, will delete the branch and will connect the neighbors node;\n\t if is a leaf, will simply delete the leaf. ");
		setKeyDescription(Qt::ShiftModifier + Qt::Key_Delete,"");
		setKeyDescription(Qt::Key_Backspace,"");

		setMouseBindingDescription(Qt::ShiftModifier, Qt::LeftButton, "Creates a rectangle that select all nodes contained in his area");
		setMouseBindingDescription(Qt::ShiftModifier, Qt::RightButton, "You must select at least 1 node and almost 2 nodes: if only one node is selected, you will create a new node connected to the selected one else you will create a node between the 2 nodes selected");
		setMouseBindingDescription(Qt::ControlModifier, Qt::LeftButton, "Rotate the selected nodes basing the rotation on their average position");
		setMouseBindingDescription(Qt::ControlModifier, Qt::RightButton, "Translate the selcted nodes basing the translation on the mouse movement. If you translate the node near another, they will be merged");
		setMouseBindingDescription(Qt::AltModifier, Qt::LeftButton, "Creates a rectangle that deselect all nodes contained in his area");
		setMouseBindingDescription(Qt::AltModifier, Qt::NoButton, "Increase and decrease the radius of the nodes");
		setMouseBindingDescription(Qt::ShiftModifier, Qt::NoButton, "Move a node on the path of the neiborghs that it have. I must have almost 2 neighbors");

		clearSelection();
		currentEditType = NODE_EDIT;

		emit disableSampling();
	}
}

void Viewer::nodeFusionChanged(bool b)
{
	nodeFusionActive = b;
}

void Viewer::meshLimitChanged(bool b)
{
	meshLimit = b;
}

void Viewer::load_new_skeleton()
{
	activeInsertion = false;

	deselection = false;
	operationIsActive = false;
	tempCoordinates = Point3d(0.0,0.0,0.0);
	firstNeighDirection = false;
	secondNeighDirection = false;
	stepsOfCentralNeigh = 0;
	nodeFusionActive = false;
	canMove = true;
	meshLimit = false;

	_mesh = NULL;

	selectionMode_ = NONE;
	deletingSingleBranchingNode = false;
	copyActive = false;

	currentEditType = NODE_EDIT;

	changeNodeFusion(Qt::Unchecked);
	changeEditMode(Qt::Unchecked);
	changeMeshLimit(Qt::Unchecked);

	emit disableCopy();
	emit disablePaste();
	emit disableDeleteConnection();
	emit disableAddBetween();
	emit disableSampling();
	emit disableArticulation();
	emit uncheckArticulation();

	lastPointSelected = -1;

	// The status buffer is cleaned.
	status.clean();

	garbage_collector(skel);

	// The first state is saved in the buffer.
	status.addState((*this->skel), selection_);

	emit disableUndo();
	emit disableRedo();
}

void Viewer::action_undo()
{
	if (status.getBufferIndex() > 0)
	{
		*skel = status.revertSkeleton();
		selection_ = status.revertSelection();

		emit enableRedo();

		if (status.getBufferIndex() == 0) {

			emit disableUndo();
		}

		updateGL();
	}
}

void Viewer::action_redo()
{
	if (status.getBufferIndex() < status.getSize() - 1)
	{
		Status restoredStatus = status.restore();

		*skel = restoredStatus.getSkeleton();
		selection_ = restoredStatus.getSelection();

		emit enableUndo();

		if (status.getBufferIndex() == status.getSize() - 1) {

			emit disableRedo();
		}

		updateGL();
	}
}

void Viewer::setNodeArticulationState(bool newState)
{
	skel->points[selection_[0]].setArticulation(newState);
}

void Viewer::action_deselectAll()
{
	if(currentEditType == NODE_EDIT){
		for(int i = 0; i < selection_.size(); i++)
		{
			skel->points[(selection_[i])].deselect();
		}

		if (selection_.size() != 0)
		{
			clearSelection();
			updateGL();
			status.addState((*this->skel), selection_);

			emit enableUndo();
			emit disableRedo();
		}

		emit disableCopy();
		emit disablePaste();
		emit disableDeleteConnection();
		emit disableAddBetween();
		emit disableArticulation();
		emit uncheckArticulation();
	}
	else
	{
		for(int i = 0; i < skel->points.size(); i++)
		{
			skel->points[i].deselect();
		}

		clearSelection();
		updateGL();
		status.addState((*this->skel), selection_);

		emit enableUndo();
		emit disableRedo();
		emit disableSampling();
	}
}

void Viewer::action_deleteNode()
{
	if(currentEditType == NODE_EDIT)
	{
		QSet<int> nodesToDelete;

		foreach (int node, selection_)
		{
			nodesToDelete.insert(node);
		}

		if (nodesToDelete.size() == 1)
		{
			foreach (int node, nodesToDelete)
			{
				if (skel->points[node].isLeaf())
				{
					skel->deleteLeaf(skel->points[node]);
				}
				else if (skel->points[node].isJoint())
				{
					skel->deleteJoint(skel->points[node]);
				}
				else
				{
					deletingSingleBranchingNode = true;

					branchingNodeToDelete = skel->points[node];

					clearSelection();

					emit deleteBranchingNodeSignal();
				}
			}

			selection_.clear();

			garbage_collector(skel);
			status.addState((*this->skel), selection_);
			emit enableUndo();
			emit disableRedo();
		}
		else if (nodesToDelete.size() > 1)
		{
			int deleteExitCode = skel->deleteNodes(nodesToDelete);

			if (deleteExitCode == Skel::CurveSkeleton::ERROR)
			{
				QMessageBox::information(this, "Errore", "Errore imprevisto.");
			}
			else if (deleteExitCode == Skel::CurveSkeleton::TOO_FEW_BN)
			{
				QMessageBox::information(this, "Errore", "Non puoi cancellare il joint.");
			}
			else if (deleteExitCode == Skel::CurveSkeleton::TOO_MUCH_BN)
			{
				QMessageBox::information(this, "Errore", "Non puoi cancellare piu' di un joint.");
			}
			else if (deleteExitCode == Skel::CurveSkeleton::OK)
			{
				selection_.clear();

				garbage_collector(skel);
				status.addState((*this->skel), selection_);
				emit enableUndo();
				emit disableRedo();
			}
			else
			{
				QMessageBox::information(this, "Errore", "Errore imprevisto.");
			}
		}

		updateGL();
	}
	else
	{
		if(selection_.size() > 0)
		{
			int j = 0;
			QList<int> bone = skel->bones.value(selection_.first());
			if(skel->points[bone.first()].isLeaf()||skel->points[bone.last()].isLeaf())
			{
				for(int i = 0; i < skel->points.size(); i++)
				{
					if(skel->points[i].isSelected())
					{
						if(skel->points[i].isJoint())
						{
							removeIdFromSelection(skel->points[i].id);
							skel->deleteJoint(skel->points[i]);
							j--;
						}
						else if(skel->points[i].isLeaf())
						{
							removeIdFromSelection(skel->points[i].id);
							skel->deleteLeaf(skel->points[i]);
							j--;
						}
						garbage_collector(skel);
						i = j;
						j ++;

					}
				}
			}
			else
			{
				QMessageBox::information(this, "Errore", "Non puoi cancellare un bone con più di un joint.");
			}

		}
		action_deselectAll();

	}
}

void Viewer::action_deleteConnection()
{
	skel->points[selection_[0]].removeNeighbor(selection_[1]);
	skel->points[selection_[1]].removeNeighbor(selection_[0]);

	if (!skel->allPointsConnected())
	{
		skel->points[selection_[0]].addNeighbor(selection_[1]);
		skel->points[selection_[1]].addNeighbor(selection_[0]);
	}

	updateGL();
}

void Viewer::action_addNodeBetween()
{
	SkelPoint firstSel = skel->points[(selection_[0])];
	SkelPoint secondSel = skel->points[(selection_[1])];
	Point3d position;
	double rad = 0.2;
	position.x = (firstSel.coord.x + secondSel.coord.x)/2;
	position.y = (firstSel.coord.y + secondSel.coord.y)/2;
	position.z = (firstSel.coord.z + secondSel.coord.z)/2;

	//we will only add a node if this node is not too close
	//to a some other nodes.
	if(checkIfClose(position,rad) == -1)
	{
		QList<int> neighbors;
		int newId = this->skel->getMaxId() + 1;
		this->skel->append(SkelPoint(newId, position, rad, neighbors, false));

		/*if one of the 2 points selected is a joint, we can't retrieve the right
		* bone id from a joint, so we must control from who this must be retrieved.*/
		UpdateTopology::pointDisconnect(skel,firstSel.id,secondSel.id);
		UpdateTopology::pointConnect(skel, firstSel.id,newId);
		UpdateTopology::pointConnect(skel,newId,secondSel.id);

		skel->points[(selection_[0])].deselect();
		skel->points[(selection_[1])].deselect();
//				selection_.clear();

		clearSelection();

		garbage_collector(skel);

		status.addState((*this->skel), selection_);

		emit enableUndo();
		emit disableRedo();

		updateGL();
	}
}

void Viewer::action_sampling()
{
	int articulationsInBones = 0;

	Bone bone = skel->bones[selection_.first()];

	for (int i = 0; i < bone.size(); ++i)
	{
		if (skel->points[bone[i]].isArticulation())
		{
			articulationsInBones++;
		}
	}

	emit initSampling(bone.size(), articulationsInBones + 2);

	emit disableActions();
	emit disableCopy();
	emit disablePaste();
	emit disableDeleteConnection();
	emit disableRedo();
	emit disableUndo();
}

void Viewer::startSampling(int newBoneSize)
{
	Bone selectedBone = skel->bones[selection_.first()];

	int firstNode = selectedBone.first();
	int lastNode = selectedBone.last();

	std::vector<float> radii(selectedBone.size());
	std::vector<float> count(selectedBone.size());

	for (int i = 0; i < selectedBone.size(); i++)
	{
		radii[i] = skel->points[selectedBone[i]].radius;
	}

	int previousBoneSize = selectedBone.size();

	double dist = 0;
	count[0] = 0;

	for (int i = 0; i < previousBoneSize - 1; i++)
	{
		dist += UpdateTopology::distance(skel, selectedBone[i], selectedBone[i+1]);
		count[i + 1] = dist;
	}

	std::vector<float> function = mathalgo::polyfit(count, radii, 2);

	double step = dist / (newBoneSize - 1);

	int currNode = 0;
	double currDist = 0;
	double nextDist = UpdateTopology::distance(skel, selectedBone[currNode], selectedBone[currNode + 1]);
	int lastInsertedNode = firstNode;

	bool lastIsArt = false;

	for (int i = 1; i < newBoneSize - 1; i++)
	{
		while(nextDist <= step * i)
		{
			if (skel->points[selectedBone[currNode + 1]].isArticulation())
			{
				SkelPoint artPoint(skel->points[selectedBone[currNode + 1]]);

				artPoint.id = skel->getMaxId() + 1;

				artPoint.neighbors.clear();
				artPoint.neighbors << lastInsertedNode;

				std::vector<float> tmp(1);

				tmp[0] = step * i;

				std::vector<float> res = mathalgo::polyval(function, tmp);

				artPoint.radius = res[0];

				skel->append(artPoint);

				skel->points[lastInsertedNode].addNeighbor(artPoint.id);

				lastInsertedNode = artPoint.id;

				lastIsArt = true;
			}

			currDist = nextDist;
			currNode++;
			nextDist += UpdateTopology::distance(skel, selectedBone[currNode], selectedBone[currNode + 1]);
		}

		if (lastIsArt)
		{
			lastIsArt = false;

			continue;
		}

		double distInside = (step * i) - currDist;

		// Create the new node in the line that go straight between selectedBone[i]
		// and selectedBone[i+1].

		float p = distInside / UpdateTopology::distance(skel, selectedBone[currNode], selectedBone[currNode + 1]);

		float x = skel->points[selectedBone[currNode]].coord.x + p * (skel->points[selectedBone[currNode + 1]].coord.x - skel->points[selectedBone[currNode]].coord.x);
		float y = skel->points[selectedBone[currNode]].coord.y + p * (skel->points[selectedBone[currNode + 1]].coord.y - skel->points[selectedBone[currNode]].coord.y);
		float z = skel->points[selectedBone[currNode]].coord.z + p * (skel->points[selectedBone[currNode + 1]].coord.z - skel->points[selectedBone[currNode]].coord.z);

		Point3d coord(x, y, z);

		QList<int> neighbors = QList<int>() << lastInsertedNode;

		SkelPoint newPoint(skel->getMaxId() + 1, coord, 0.2, neighbors);

		std::vector<float> tmp(1);

		tmp[0] = step * i;

		std::vector<float> res = mathalgo::polyval(function, tmp);

		newPoint.radius = res[0];

		skel->append(newPoint);

		skel->points[lastInsertedNode].addNeighbor(newPoint.id);

		lastInsertedNode = newPoint.id;
	}

	skel->points[lastInsertedNode].addNeighbor(lastNode);
	skel->points[lastNode].addNeighbor(lastInsertedNode);

	for (int i = 1; i < previousBoneSize - 1; i++)
	{
		skel->remove(selectedBone[i], false);
	}

	garbage_collector(skel);

	status.addState((*this->skel), selection_);

	emit enableActions();
}

void Viewer::cancelSampling()
{
	emit enableActions();
}

void Viewer::singleBranchingNodeDeletionWidgetTransferButtonPressed()
{
	if (!selection_.isEmpty())
	{
		deletingSingleBranchingNode = false;

		SkelPoint nodeToTransfer = skel->points[skel->points.indexOf(selection_.first())];

		int deleteExitCode = skel->deleteBNWithTransferNeighbors(branchingNodeToDelete, nodeToTransfer);

		if (deleteExitCode == Skel::CurveSkeleton::ERROR)
		{
			qDebug() << "ERROR\n";
		}
		else
		{
			qDebug() << "OK\n";
		}

		garbage_collector(skel);
		status.addState((*this->skel), selection_);
		emit enableUndo();
		emit disableRedo();

		updateGL();
	}
}

void Viewer::singleBranchingNodeDeletionWidgetCancelButtonPressed()
{
	deletingSingleBranchingNode = false;
}

void Viewer::singleBranchingNodeDeletionWidgetDeleteButtonPressed()
{
	deletingSingleBranchingNode = false;

	skel->deleteBNWithoutTransfert(branchingNodeToDelete);

	garbage_collector(skel);
	status.addState((*this->skel), selection_);
	emit enableUndo();
	emit disableRedo();

	updateGL();
}

void Viewer::choosePasteNodeWidgetSelectButtonPressed()
{
	if (selection_.size() == 1)
	{
		pointToMergeForCopy = selection_.first();

		for (int i = 0; i < copyList.size(); i++)
		{
			skel->points[copyList[i]].setSFC(false);
		}

		clearSelection();

		selection_.append(copyList);

		for (int i = 0; i < copyList.size(); i++)
		{
			skel->points[copyList[i]].select();
		}

		//	copyList.removeAll(pointToMergeForCopy);

		updateGL();

		operationIsActive = false;

		emit enableActions();
		emit enablePaste();
		emit hideChoosePasteNodeWidget();
	}
	else
	{
		QMessageBox::information(this, "Errore", "Nessun nodo valido selezionato.");
	}
}

void Viewer::choosePasteNodeWidgetHelpButtonPressed()
{
	QMessageBox::information(this, "Info", "Il paste node e' il nodo che sara' fuso con il nodo su cui si decidera' di incollare la selezione.");
}

void Viewer::choosePasteNodeWidgetCancelButtonPressed()
{
	for (int i = 0; i < copyList.size(); i++)
	{
		skel->points[copyList[i]].setSFC(false);
	}

	clearSelection();

	selection_.append(copyList);

	for (int i = 0; i < copyList.size(); i++)
	{
		skel->points[copyList[i]].select();
	}

	//	copyList.removeAll(pointToMergeForCopy);

	updateGL();

	operationIsActive = false;

	emit enableActions();
}

void Viewer::updateSceneBoundingBox()
{
	this->sceneBBox.Reset();

	std::vector< const Point3d* > skelpoints;
	foreach( Skel::SkelPoint p, this->skel->points )
	{
		skelpoints.push_back( &(p.coord));
	}
	this->sceneBBox.Set(skelpoints);

	if( _mesh != NULL )
	{
		cout << "mesh is not null";
		if( _mesh->polygons.size() > 0 ){
			cout << "number of triangles is more than 0";

			this->sceneBBox = this->sceneBBox + _mesh->bbox;
		}
	}

	float base_radius = this->sceneBBox.Diagonal();
	base_radius = base_radius > 0.0001 ? base_radius : 1.0;

	this->setSceneRadius( base_radius * 1.1);

	Point3d center = this->sceneBBox.Center();

	this->setSceneCenter(Vec(center.x, center.y, center.z));

	this->camera()->showEntireScene();
}

void Viewer::setDefaultMaterial()
{
	GLfloat mat_a[] = {0.1, 0.1, 0.1, 1.0};
	GLfloat mat_d[] = {0.4, 0.4, 0.4, 0.9};
	GLfloat mat_s[] = {0.1, 0.1, 0.1, 0.1};
	GLfloat shine[] = {50.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

void Viewer::setSkelDrawMode(bool set, Paint::DRAWMODE mode)
{
	if (set)
	{
		skeleton_draw_mode |=  mode;
	}
	else
	{
		skeleton_draw_mode &= ~mode;
	}

	this->update();
}

void Viewer::init()
{
    // A flag indicating whether there is an ongoing insertion.
    activeInsertion = false;

    deselection = false;
    operationIsActive = false;
    tempCoordinates = Point3d(0.0,0.0,0.0);
    firstNeighDirection = false;
    secondNeighDirection = false;
    stepsOfCentralNeigh = 0;
    nodeFusionActive = false;

	setKeyDescription(Qt::Key_Space, "Clear the selection");
	setKeyDescription(Qt::CTRL + Qt::Key_C, "In Node Mode, copy all the selected nodes and in Bone Mode copy a entire bone");
	setKeyDescription(Qt::CTRL + Qt::Key_V, "Paste what was previously copied, if the copy was valid");
	setKeyDescription(Qt::CTRL + Qt::Key_Y, "Restore previous changes");
	setKeyDescription(Qt::CTRL + Qt::Key_Z, "Undone the last changes");
	setKeyDescription(Qt::CTRL + Qt::Key_N, "Create a new skeleton with only the root node");
	setKeyDescription(Qt::CTRL + Qt::Key_O, "Open a saved skeleton");
	setKeyDescription(Qt::Key_P, "Show on the console informations about the node of the skeleton");
	setKeyDescription(Qt::Key_O, "In Node Mode, show on the console information about the selected nodes");
	setKeyDescription(Qt::Key_Delete, "Delete the selection, in Node Mode there are 3 cases: \n\tif a node is a joint, will delete al the nodes that are not connected to another joint and the joint itself;\n\tif is a branch, will delete the branch and will connect the neighbors node;\n\t if is a leaf, will simply delete the leaf. In Bone Mode delete all the bone only if the bone end with a leaf but it will not delete the joint. ");
	setKeyDescription(Qt::ShiftModifier + Qt::Key_Delete, "Only in Bone Mode: Remove all the nodes in a bone that have two joints. It will not remove the joints itherselfs");
	setKeyDescription(Qt::Key_Backspace, "Only in Bone Mode: Make a prouning");
	setKeyDescription(Qt::Key_B, "Change edit mode (Bone mode - Node mode)");
	setKeyDescription(Qt::CTRL + Qt::Key_T, "Add a node between the two selected ones");
	setKeyDescription(Qt::Key_D, "When two connected nodes are selected and there is a cycle, with this command you can break the connection between the two selected nodes");
	setKeyDescription(Qt::Key_S, "Resample the selected bone");

	setMouseBindingDescription(Qt::ShiftModifier, Qt::LeftButton, "Creates a rectangle that select all nodes contained in his area");
	setMouseBindingDescription(Qt::ShiftModifier, Qt::RightButton, "Only in Node Mode: You must select at least 1 node and almost 2 nodes: if only one node is selected, you will create a new node connected to the selected one else you will create a node between the 2 nodes selected");
	setMouseBindingDescription(Qt::ControlModifier, Qt::LeftButton, "Rotate the selected nodes basing the rotation on their average position");
	setMouseBindingDescription(Qt::ControlModifier, Qt::RightButton, "Translate the selcted nodes basing the translation on the mouse movement. If you translate the node near another, they will be merged");
	setMouseBindingDescription(Qt::AltModifier, Qt::LeftButton, "Creates a rectangle that deselect all nodes contained in his area");
	setMouseBindingDescription(Qt::AltModifier, Qt::NoButton, "Only in Node Mode: Increase and decrease the radius of the nodes");
	setMouseBindingDescription(Qt::ShiftModifier, Qt::NoButton, "Only in Node Mode: Move a node on the path of the neiborghs that it have. I must have almost 2 neighbors");

	// A ManipulatedFrameSetConstraint will apply displacements to the selection
	setManipulatedFrame(new ManipulatedFrame());
	manipulatedFrame()->setConstraint(new ManipulatedFrameSetConstraint());

    qglClearColor( QColor( 250, 250, 250 ));

	glDisable( GL_DITHER );
	glEnable(GL_NORMALIZE);	// Forces glNormal() vectors normalization

	// Used to display semi-transparent selection rectangle
	glBlendFunc(GL_ONE, GL_ONE);

	GLfloat pos1[] = { 1.0,  1.0, -1.0, 0.0};
	GLfloat pos2[] = {-1.1,  1.1, -1.0, 0.0};
	GLfloat pos3[] = { 1.0,  1.0,  1.1,  0.0};
	GLfloat col1[] = { 0.7,  0.7,  0.8,  1.0};
	GLfloat col2[] = { 0.8,  0.7,  0.7,  1.0};
	GLfloat col3[] = { 1.0,  1.0,  1.0,  1.0};

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_POSITION, pos1);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,  col1);
	glLightfv(GL_LIGHT0,GL_SPECULAR, col1);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1,GL_POSITION, pos2);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,  col2);
	glLightfv(GL_LIGHT1,GL_SPECULAR, col2);

	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2,GL_POSITION, pos3);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,  col3);
	glLightfv(GL_LIGHT2,GL_SPECULAR, col3);

	setDefaultMaterial();

	load_new_skeleton();
}

QString Viewer::helpString () const
{
	QString text("<h2>SKELETON EDITING</h2><\br>");
	text += "<h3>This program is meant for creating skeletal structures.<\br>";
	text += "You can edit your structures in various moodes, such as creating a new<\br>";
	text += "node, moving a node, rotating a node ecc. .<\br>";
	text += "You can use 2 utilization moodes:<\br><\t>-Node edit: In this moode, you can manipulets single nodes;<\br>";
	text += "<\br><\t>-Bone mode: for manipulating bones.<\br>";
	text += "If you want to know what buttons you need to press for do some actions,<\br>";
	text += "please visit the tabs on this help, it will change in base of the mode.<\br></h3>";
	return text;
}
void Viewer::draw()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//	drawLight(GL_LIGHT0);
//	drawLight(GL_LIGHT1);
//	drawLight(GL_LIGHT2);

	// Draw the intersection line

	  // Draw (approximated) intersection point on selected object
	  if (selectedName() >= 0)
		{
		  glColor3f(0.9f, 0.2f, 0.1f);
		  glBegin(GL_POINTS);
		  glVertex3fv(selectedPoint);
		  glEnd();
		}

	if( NULL != this->_mesh )
	{
		RMesh::draw( *this->_mesh, RMesh::FLAT | RMesh::WIREFRAME | RMesh::PCA );
	}
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if( NULL != this->skel )
	{
		// Print the skeleton without names for the selection.
		Skel::Paint::paintGL( this->skel, this->skeleton_draw_mode, 0.02 * this->sceneRadius() * this->scale_factor );
	}

	// Draws manipulatedFrame (the set's rotation center)
	if (manipulatedFrame()->isManipulated())
	{
		glPushMatrix();
		glMultMatrixd(manipulatedFrame()->matrix());
		//		drawAxis(0.5);
		glPopMatrix();
	}

	if (selectionMode_ != NONE)
	{
		drawSelectionRectangle();
	}
}

void Viewer::postSelection(const QPoint &point)
{
	// Compute orig and dir, used to draw a representation of the intersecting line
	camera()->convertClickToLine(point, orig, dir);

	// Find the selectedPoint coordinates, using camera()->pointUnderPixel().
	bool found;
	selectedPoint = camera()->pointUnderPixel(point, found);

	selectionMode_ = NONE;
}

void Viewer::endSelection(const QPoint&)
{
	// Flush GL buffers
	glFlush();

	// Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
	GLint nbHits = glRenderMode(GL_RENDER);

	if(currentEditType == NODE_EDIT)
	{
		if (nbHits > 0)
		{
			GLuint zMin = (selectBuffer())[1];
			setSelectedName((selectBuffer())[3]);

			// Interpret results : each object created 4 values in the selectBuffer().
			// (selectBuffer())[4*i+3] is the id pushed on the stack.
			for (int i=0; i<nbHits; i++)
			{
				int index;

				switch (selectionMode_)
				{
					case ADD:

						if ((selectBuffer())[4*i+1] < zMin)
						{
							zMin = (selectBuffer())[4*i+1];
							setSelectedName((selectBuffer())[4*i+3]);
						}

//						if (false == singleSelection)
//						{
							addIdToSelection((selectBuffer())[4*i+3]);
							index = ((selectBuffer())[4*i+3]);

							this->skel->points[index].select();
//						}

						break;

					case REMOVE:

						if ((selectBuffer())[4*i+1] < zMin)
						{
							zMin = (selectBuffer())[4*i+1];
							setSelectedName((selectBuffer())[4*i+3]);
						}

						removeIdFromSelection((selectBuffer())[4*i+3]);
						index = ((selectBuffer())[4*i+3]);

						this->skel->points[index].deselect();

						break;

					default: break;
				}
			}
		}
		else
		{
			setSelectedName(-1);
		}
	}
	else
	{
		if (nbHits > 0)
		{
			// Interpret results : each object created 4 values in the selectBuffer().
			// (selectBuffer())[4*i+3] is the id pushed on the stack.
			for (int i=0; i<nbHits; i++)
			{
				int index;
				QList<int> nodeList ;
				switch(selectionMode_)
				{
					case ADD:

						addIdToSelection((selectBuffer())[4*i+3]);
						index = ((selectBuffer())[4*i+3]);
						nodeList = skel->bones.value(index);

						for(int i = 0; i < nodeList.size(); i++)
						{
							skel->points[nodeList[i]].select();
						}

						break;

					case REMOVE:

						removeIdFromSelection((selectBuffer())[4*i+3]);
						index = ((selectBuffer())[4*i+3]);
						nodeList = skel->bones.value(index);

						for(int i = 0; i < nodeList.size(); i++)
						{
							skel->points[nodeList[i]].deselect();
						}

						break;

					default: break;
				}
			}
		}
	}
}

void Viewer::drawWithNames()
{
	if(!this->skel->points.isEmpty())
	{
		if( NULL != this->skel )
		{
			if(currentEditType == NODE_EDIT)
			{
				// Only nodes are printed with a name for the selection.
				Skel::Paint::paintGLWithNames( this->skel, this->skeleton_draw_mode, 0.02 * this->sceneRadius() );
			}
			else
			{
				Skel::Paint::paintGLWithNamesInBoneMode( this->skel, this->skeleton_draw_mode, 0.02 * this->sceneRadius() );
			}
		}
	}
}

void Viewer::mousePressEvent(QMouseEvent* e)
{
	if (currentEditType == NODE_EDIT)
	{
		if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier && false == operationIsActive && false == deletingSingleBranchingNode)
		{
			// Start selection. Mode is ADD with Shift key.
			rectangle_ = QRect(e->pos(), e->pos());

			selectionMode_ = ADD;
		}
		else if (e->button() == Qt::LeftButton && e->modifiers() == Qt::AltModifier && false == operationIsActive && false == deletingSingleBranchingNode)
		{
			// Start selection. Mode is TOGGLE with Alt key.
			rectangle_ = QRect(e->pos(), e->pos());

			selectionMode_ = REMOVE;
		}
		else if (e->modifiers() == (Qt::ControlModifier | Qt::AltModifier) || e->modifiers() == Qt::ControlModifier && false == operationIsActive && false == deletingSingleBranchingNode)
		{
			if (meshLimit)
			{
				if (_mesh != NULL)
				{
					#ifdef UNIX
					RMesh::mesh::getCGALTriangles( (*_mesh), trilist );
					AABB_Tree = new RMesh::Utils::CGAL_AABB_Tree( trilist.begin(), trilist.end( ));
					#endif
				}

				prevPositions.reserve(selection_.size());

				for (int i = 0; i < selection_.size(); i++)
				{
					prevPositions[i] = skel->points[selection_[i]].coord;
				}
			}

			// With Control and Alt or just Control, a manipulation will start.
			startManipulation();

			QGLViewer::mousePressEvent(e);
		}
		else if (e->button() == Qt::RightButton && e->modifiers() == Qt::ShiftModifier && false == operationIsActive && false == deletingSingleBranchingNode)
		{
			if (meshLimit)
			{
				if (_mesh != NULL)
				{
					#ifdef UNIX
					RMesh::mesh::getCGALTriangles( (*_mesh), trilist );
					AABB_Tree = new RMesh::Utils::CGAL_AABB_Tree( trilist.begin(), trilist.end( ));
					#endif
				}

				prevPositions.reserve(selection_.size());

				for (int i = 0; i < selection_.size(); i++)
				{
					prevPositions[i] = skel->points[selection_[i]].coord;
				}
			}

			// With right button pressed and Shift a new node will be added with a translation.
			addNewNode();

			QGLViewer::mousePressEvent(e);
		}
		else if ((e->button() == Qt::LeftButton && e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier)) ||
				 ((true == deletingSingleBranchingNode || true == copyActive) && e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier ))
		{

			if (true == deletingSingleBranchingNode)
			{
				clearSelection();
			}
            cout << " shift click " << endl;
			camera()->convertClickToLine( e->pos(), orig, dir );
			bool found;
			selectedPoint = camera()->pointUnderPixel(e->pos(), found );
			findNearest(selectedPoint[0], selectedPoint[1], selectedPoint[2] );
			update();
		}
		else
		{
			QGLViewer::mousePressEvent(e);
		}
	}
	else if (currentEditType == BONE_EDIT)
	{
		if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier && false == operationIsActive)
		{
			// Start selection. Mode is ADD with Shift key.
			rectangle_ = QRect(e->pos(), e->pos());

			selectionMode_ = ADD;
		}
		else if (e->button() == Qt::LeftButton && e->modifiers() == Qt::AltModifier && false == operationIsActive)
		{
			// Start selection. Mode is TOGGLE with Alt key.
			rectangle_ = QRect(e->pos(), e->pos());

			selectionMode_ = REMOVE;
		}
		else
		{
			QGLViewer::mousePressEvent(e);
		}
	}
	else
	{
		QGLViewer::mousePressEvent(e);
	}
}

void Viewer::mouseMoveEvent(QMouseEvent* e)
{

//	qDebug() << "meshLimit" << meshLimit;

	if (selectionMode_ != NONE/* && false == singleSelection*/)
	{
		// Updates rectangle_ coordinates and redraws rectangle
		rectangle_.setBottomRight(e->pos());
		updateGL();
	}
	else if (selectionMode_ == NONE && _mesh != NULL && meshLimit)
	{
		canMove = true;
		#ifdef UNIX
        for( int i = 0; i < selection_.size(); ++i)
        {

            if(!RMesh::Utils::skel_mesh_helper::is_inside((*AABB_Tree), skel->points[selection_[i]].coord))
            {
                canMove = false;
                break;
            }
        }
		#endif

		if(canMove)
		{
			for (int i = 0; i < selection_.size(); i++)
			{
				prevPositions[i] = skel->points[selection_[i]].coord;
			}

			QGLViewer::mouseMoveEvent(e);
		}
	}
	else
	{
		QGLViewer::mouseMoveEvent(e);
	}
}

void Viewer::mouseReleaseEvent(QMouseEvent* e)
{
	if (currentEditType == NODE_EDIT)
	{
		if (canMove == false)
		{
			for (int i = 0; i < selection_.size(); i++)
			{
				skel->points[selection_[i]].coord = prevPositions[i];
			}
		}

		if (selectionMode_ != NONE)
		{
			int oldSelectionSize = selection_.size();

			// Actual selection on the rectangular area.
			// Possibly swap left/right and top/bottom to make rectangle_ valid.
			rectangle_ = rectangle_.normalized();

			// Define selection window dimensions
			setSelectRegionWidth(rectangle_.width());
			setSelectRegionHeight(rectangle_.height());

			// Compute rectangle center and perform selection
			select(rectangle_.center());

			// Update display to show new selected objects
			updateGL();

			if (oldSelectionSize != selection_.size())
			{
				// Add a state in the buffer with the new selection.
				status.addState((*this->skel), selection_);

				emit enableUndo();
				emit disableRedo();
			}

			// Empty the list of object that have been manipulated to solve a problem
			// when the user try to add a node without any selected node.
			((ManipulatedFrameSetConstraint*) (manipulatedFrame()->constraint()))->clearSet();
		}
		else if(activeInsertion == true)
		{
			activeInsertion = false;

			float radius = max(this->skel->points[newInsertId].radius, this->skel->points[oldPointId].radius);

			// Check if the new inserted point is close to the old point.
			if(checkIfClose(this->skel->points[newInsertId].coord, radius) == this->skel->points[oldPointId].id)
			{
				clearSelection();

				skel->remove(newInsertId);

				updateGL();
			}
			else
			{
				garbage_collector(skel);
				status.addState((*this->skel), selection_);
				emit enableUndo();
				emit disableRedo();
			}
		}
		else
		{
			if(selection_.size() > 0 && nodeFusionActive)
			{
				int selected = selection_.first();
				int closePointId = checkIfClose(this->skel->points[selected].coord, this->skel->points[selected].radius);
				QList<int> temp =  skel->points[selected].neighbors;
				if( closePointId != -1)
				{
					for(int i = 0; i < skel->points[selected].neighbors.size(); i++)
					{
						int neighbor_id = skel->points[selected].neighbors[i];
						if( neighbor_id  == closePointId ) continue;

						if( skel->points[closePointId].neighbors.indexOf( neighbor_id ) == -1
						|| !( neighbor_id == closePointId ))
						{
//                            skel->points[closePointId].neighbors.append( neighbor_id );
							skel->points[closePointId].addNeighbor( neighbor_id );
						}
					}

					for(int i = 0; i < temp.size(); i++)
					{
						if(temp[i] != closePointId)
						{
							skel->points[temp[i]].addNeighbor(closePointId);
						}
					}


					skel->remove(selected);
					selection_.removeAll(selected);

					garbage_collector(skel);
					status.addState((*this->skel), selection_);
				}
			}
			QGLViewer::mouseReleaseEvent(e);
		}

		if (selection_.size() == 1)
		{
			if (!skel->points[selection_[0]].isLeaf() && !skel->points[selection_[0]].isBranchingNode())
			{
				emit enableArticulation();
			}
			else
			{
				emit disableArticulation();
			}

			emit enablePaste();
			emit enableCopy();
			emit disableDeleteConnection();
			emit disableAddBetween();

			if (skel->points[selection_[0]].isArticulation())
			{
				emit checkArticulation();
			}
			else
			{
				emit uncheckArticulation();
			}
		}

		else if (selection_.size() == 2 && skel->points[selection_[0]].neighbors.contains(selection_[1]))
		{
			emit enableDeleteConnection();
			emit disableArticulation();
			emit enableAddBetween();
		}
		else if (selection_.size() == 0)
		{
			emit disableArticulation();
			emit uncheckArticulation();
			emit disableCopy();
			emit disableDeleteConnection();
			emit disablePaste();
			emit disableAddBetween();
		}
		else if (selection_.size() > 0)
		{
			emit enableCopy();
			emit disableArticulation();
			emit uncheckArticulation();
			emit disableDeleteConnection();
			emit disablePaste();
			emit disableAddBetween();
		}
	}
	else
	{
		if (selectionMode_ != NONE)
		{
			int oldSelectionSize = selection_.size();

			// Actual selection on the rectangular area.
			// Possibly swap left/right and top/bottom to make rectangle_ valid.
			rectangle_ = rectangle_.normalized();

			// Define selection window dimensions
			setSelectRegionWidth(rectangle_.width());
			setSelectRegionHeight(rectangle_.height());

			// Compute rectangle center and perform selection
			select(rectangle_.center());

			// Update display to show new selected objects
			updateGL();

			if (oldSelectionSize != selection_.size())
			{
				// Add a state in the buffer with the new selection.
				status.addState((*this->skel), selection_);
				emit enableUndo();
				emit disableRedo();
			}

			// Empty the list of object that have been manipulated to solve a problem
			// when the user try to add a node without any selected node.
			((ManipulatedFrameSetConstraint*) (manipulatedFrame()->constraint()))->clearSet();
		}

		if (selection_.size() > 0)
		{
			emit enableSampling();
		}
	}
}


void Viewer::wheelEvent(QWheelEvent *e)
{
	if (currentEditType == NODE_EDIT)
	{
		if(selection_.size() == 1 && e->modifiers() == Qt::ShiftModifier)
		{
			Vector3d point;
			float maxRad;
			Vector3d distance;

			if(skel->points[selection_.first()].isLeaf())
			{
				if(tempCoordinates == Point3d(0.0,0.0,0.0))
				{
					tempCoordinates = skel->points[selection_.first()].coord;
				}

				if (e->delta() > 0)
				{
					maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors.first()].radius);

					point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors.first()].coord);
					distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors.first()].coord);;

					if(!(abs(distance.x) < maxRad && abs(distance.y) < maxRad && abs(distance.z) < maxRad))
					{
						skel->points[selection_.first()].coord.x -= point.x*0.02;
						skel->points[selection_.first()].coord.y -= point.y*0.02;
						skel->points[selection_.first()].coord.z -= point.z*0.02;
						status.addState((*this->skel), selection_);
					}


				}
				else if (e->delta() < 0)
				{
					//with float coordinates maybe the condition must be changed
					if(!(tempCoordinates == skel->points[selection_.first()].coord))
					{
						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors.first()].coord);
						skel->points[selection_.first()].coord.x += point.x*0.02;
						skel->points[selection_.first()].coord.y += point.y*0.02;
						skel->points[selection_.first()].coord.z += point.z*0.02;

						status.addState((*this->skel), selection_);
					}

				}

			} else if(skel->points[selection_.first()].isJoint())
			{
				//this condition must be changed.
				if(tempCoordinates == Point3d(0.0,0.0,0.0))
				{
					tempCoordinates = skel->points[selection_.first()].coord;
				}

				//if the node is in the initial position, i can move it
				//in the direction of both neighbors
				if(tempCoordinates == skel->points[selection_.first()].coord)
				{
					stepsOfCentralNeigh = 1;
					if (e->delta() > 0)
					{
						maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors.first()].radius);
						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors.first()].coord);
						distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors.first()].coord);;

						if(!(abs(distance.x) < maxRad && abs(distance.y) < maxRad && abs(distance.z) < maxRad))
						{
							skel->points[selection_.first()].coord.x -= point.x*0.02;
							skel->points[selection_.first()].coord.y -= point.y*0.02;
							skel->points[selection_.first()].coord.z -= point.z*0.02;
							status.addState((*this->skel), selection_);
						}
						firstNeighDirection = true;
					}
					else if (e->delta() < 0)
					{
						maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors[1]].radius);

						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors[1]].coord);
						distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors[1]].coord);;

						if(!(abs(distance.x) < maxRad && abs(distance.y) < maxRad && abs(distance.z) < maxRad))
						{
							skel->points[selection_.first()].coord.x -= point.x*0.02;
							skel->points[selection_.first()].coord.y -= point.y*0.02;
							skel->points[selection_.first()].coord.z -= point.z*0.02;
							status.addState((*this->skel), selection_);
						}
						secondNeighDirection = true;
					}
				}
				else if(firstNeighDirection)
				{
					if (e->delta() > 0)
					{
						maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors.first()].radius);
						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors.first()].coord);
						distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors.first()].coord);;

						if(!(abs(distance.x) < maxRad && abs(distance.y) < maxRad && abs(distance.z) < maxRad))
						{
							skel->points[selection_.first()].coord.x -= point.x*0.02;
							skel->points[selection_.first()].coord.y -= point.y*0.02;
							skel->points[selection_.first()].coord.z -= point.z*0.02;
							stepsOfCentralNeigh ++;
							status.addState((*this->skel), selection_);
						}

					}
					else if (e->delta() < 0)
					{
						maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors.first()].radius);
						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors.first()].coord);
						distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors.first()].coord);
						skel->points[selection_.first()].coord.x += point.x*0.02;
						skel->points[selection_.first()].coord.y += point.y*0.02;
						skel->points[selection_.first()].coord.z += point.z*0.02;
						stepsOfCentralNeigh --;
						if((skel->points[selection_.first()].coord == tempCoordinates)||stepsOfCentralNeigh == 0)
						{
							skel->points[selection_.first()].coord = tempCoordinates;

							firstNeighDirection = false;
						}
						status.addState((*this->skel), selection_);
					}
				}
				else if(secondNeighDirection)
				{
					if (e->delta() < 0)
					{
						maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors[1]].radius);
						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors[1]].coord);
						distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors[1]].coord);;

						if(!(abs(distance.x) < maxRad && abs(distance.y) < maxRad && abs(distance.z) < maxRad))
						{
							skel->points[selection_.first()].coord.x -= point.x*0.02;
							skel->points[selection_.first()].coord.y -= point.y*0.02;
							skel->points[selection_.first()].coord.z -= point.z*0.02;
							stepsOfCentralNeigh ++;
						}
						status.addState((*this->skel), selection_);

					}
					else if (e->delta() > 0)
					{
						maxRad  = max(skel->points[selection_.first()].radius,skel->points[skel->points[selection_.first()].neighbors[1]].radius);
						point = (tempCoordinates - skel->points[skel->points[selection_.first()].neighbors[1]].coord);
						distance  = (skel->points[selection_.first()].coord - skel->points[skel->points[selection_.first()].neighbors[1]].coord);;
						skel->points[selection_.first()].coord.x += point.x*0.02;
						skel->points[selection_.first()].coord.y += point.y*0.02;
						skel->points[selection_.first()].coord.z += point.z*0.02;
						stepsOfCentralNeigh --;

						if(skel->points[selection_.first()].coord == tempCoordinates || stepsOfCentralNeigh == 0)
						{
							skel->points[selection_.first()].coord = tempCoordinates;
							secondNeighDirection = false;

						}
						status.addState((*this->skel), selection_);
					}
				}
			}

			updateGL();

		}
		else if (selection_.size() == 1 && e->modifiers() == Qt::AltModifier)
		{
			if (e->delta() > 0)
			{
				skel->points[(selection_[0])].radius += 0.02 * scale_factor;
			}
			else if (e->delta() < 0 && skel->points[(selection_[0])].radius > 0.2 * scale_factor)
			{
				skel->points[(selection_[0])].radius -= 0.02 * scale_factor;
			}

			updateGL();
		}
		else
		{
			QGLViewer::wheelEvent(e);
		}
	}
	else
	{
		QGLViewer::wheelEvent(e);
	}

}

void Viewer::keyPressEvent(QKeyEvent *e)
{
	static int camera = 0;

	if( e->key() == Qt::Key_Plus && e->modifiers() & Qt::ControlModifier && e->modifiers() & Qt::ShiftModifier )
	{
		this->scale_factor *= 1.2;
		update();
		return;
	}
	if( e->key() == Qt::Key_Minus && e->modifiers() & Qt::ControlModifier && e->modifiers() & Qt::ShiftModifier )
	{
		this->scale_factor *= 0.8;
		update();
		return;
	}
    if( e->key() == Qt::Key_Escape ){
//        QMessageBox::information(this, "Errore", "Esci in un altro modo");
        return;
    }

	if( e->key() == Qt::Key_1 ) { setFixedCamera( static_cast<FixedCamera>(camera = ( camera + 1 ) % 6) ); }
	//if you press del button or backspace, you will start the
	//deletation process
	if(currentEditType == NODE_EDIT){
		if(e->key() == Qt::Key_P)
		{
			test();
		}
		else if(e->key() == Qt::Key_O)
		{
			for (int i = 0; i < selection_.size(); ++i) {

				qDebug() << "Point id: " << skel->points[selection_[i]].id;

				qDebug() << "Bone id: " << skel->points[selection_[i]].boneID;

				qDebug() << "coordinates: (" << skel->points[selection_[i]].coord.x <<","<<skel->points[selection_[i]].coord.y<<","<<skel->points[selection_[i]].coord.z<<");";

				qDebug() << "Componenti colori: r= "<<skel->points[i].color.asArray()[0]<<" g = "<<skel->points[i].color.asArray()[1]<<" b = "<<skel->points[i].color.asArray()[2]<<"\n";

				qDebug() << "Neighbors:";

				for (int j = 0; j < skel->points[selection_[i]].neighbors.size(); ++j) {

					qDebug() << "| " << skel->points[selection_[i]].neighbors[j] << " ";
				}

				qDebug() << "_____________________________________\n";
			}
		}
		else if (e->key() == Qt::Key_B)
		{
			emit changeEditMode(Qt::Checked);
		}
		else
		{
			QGLViewer::keyPressEvent(e);
		}
	}
	else
	{
		if(e->key() == Qt::Key_P)
		{
			test();
		}
		else if(e->key() == Qt::Key_Backspace)
		{
			if(selection_.size() == 1)
			{

				QList<int> bone = skel->bones.value(selection_.first());
				if(bone.size() != 0){
					if(skel->points[bone.first()].isSelected() && skel->points[bone.first()].boneID != -1 && skel->points[bone.last()].boneID != -1)
					{
						if(skel->points[bone.first()].isLeaf())
						{
							cout<<"stai selezionando il boneID: "<< selection_.first() <<" ma cancellando il punto con boneId" << skel->points[bone.first()].boneID <<".\n1 E' selezionato? "<<skel->points[bone.first()].isSelected()<<"\n";
							removeIdFromSelection(skel->points[bone.first()].id);
							skel->deleteLeaf(skel->points[bone.first()]);

						}
						else if(skel->points[bone.last()].isLeaf())
						{
							cout<<"stai selezionando il boneID: "<< selection_.first() <<" ma cancellando il punto con boneId" << skel->points[bone.last()].boneID <<".\n2 E' selezionato? "<<skel->points[bone.first()].isSelected()<<"\n";
							removeIdFromSelection(skel->points[bone.first()].id);
							skel->deleteLeaf(skel->points[bone.last()]);
						}
						else
						{
							QMessageBox::information(this, "Errore", "Non puoi effettuare il prouning su un bone senza foglie.");
						}
						garbage_collector(skel);
						updateGL();
						status.addState((*this->skel), selection_);
					}
					else
					{
						for(int i = 0; skel->points.size() > i; i++)
						{
							if(skel->points[i].isSelected() && skel->points[i].boneID != -1)
							{
								selection_[0] = skel->points[i].boneID;
								bone = skel->bones.value(selection_.first());

								if(skel->points[bone.first()].isLeaf())
								{
									cout<<"\n Stai selezionando il boneID: "<< selection_.first() <<" ma cancellando il punto con boneId" << skel->points[bone.first()].boneID <<".\n3 E' selezionato? "<<skel->points[bone.first()].isSelected()<<"\n\n";
									removeIdFromSelection(skel->points[bone.first()].id);
									skel->deleteLeaf(skel->points[bone.first()]);
								}
								else if(skel->points[bone.last()].isLeaf())
								{
									cout<<"stai selezionando il boneID: "<< selection_.first() <<" ma cancellando il punto con boneId" << skel->points[bone.last()].boneID <<".\n4 E' selezionato? "<<skel->points[bone.first()].isSelected()<<"\n";
									removeIdFromSelection(skel->points[bone.first()].id);
									skel->deleteLeaf(skel->points[bone.last()]);
								}
								else
								{
									QMessageBox::information(this, "Errore", "Non puoi effettuare il prouning su un bone senza foglie.");
								}
								garbage_collector(skel);
								updateGL();
								status.addState((*this->skel), selection_);
								break;
							}
						}
					}
				}
			}
		}
		else if(e->modifiers() == Qt::ShiftModifier && e->key() == Qt::Key_Delete)
		{
			if(selection_.size() > 0)
			{
				int j = 0;
				QList<int> bone = skel->bones.value(selection_.first());
				if(skel->points[bone.first()].isBranchingNode() && skel->points[bone.last()].isBranchingNode())
				{
					for(int i = 0; i < skel->points.size(); i++)
					{
						if(skel->points[i].isSelected())
						{
							if(skel->points[i].isJoint())
							{
								removeIdFromSelection(skel->points[i].id);
								skel->deleteJoint(skel->points[i]);
								j--;
							}
							else if(skel->points[i].isLeaf())
							{
								removeIdFromSelection(skel->points[i].id);
								skel->deleteLeaf(skel->points[i]);
								j--;
							}
							garbage_collector(skel);
							i = j;
							j ++;

						}
					}
					action_deselectAll();
				}
				else
				{
					QMessageBox::information(this, "Errore", "Non puoi applicare questa funzione a un bone che inizia o termina con una foglia!");
				}
			}
		}
		else if (e->key() == Qt::Key_B)
		{
			emit changeEditMode(Qt::Unchecked);
		}
		else
		{
			QGLViewer::keyPressEvent(e);

		}
	}
}

void Viewer::test()
{
	for (int i = 0; i < skel->points.size(); ++i) {

		qDebug() << "Point id" << " (i =" << i << "): " << skel->points[i].id;

		qDebug() << "Bone id: " << skel->points[i].boneID;

		qDebug() << "coordinates: (" << skel->points[i].coord.x <<","<<skel->points[i].coord.y<<","<<skel->points[i].coord.z<<","<<");";

		qDebug() << "Neighbors:";

		for (int j = 0; j < skel->points[i].neighbors.size(); ++j) {

			qDebug() << "| " << skel->points[i].neighbors[j] << " ";
		}
		qDebug() << "E' selezionato? "<<skel->points[i].isSelected(); ;
		qDebug() << "_____________________________________\n";
	}
}

void Viewer::drawSelectionRectangle() const
{
	startScreenCoordinatesSystem();
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glColor4f(0.0, 0.0, 0.3f, 0.3f);
	glBegin(GL_QUADS);
	glVertex2i(rectangle_.left(),  rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(),  rectangle_.bottom());
	glEnd();

	glLineWidth(2.0);
	glColor4f(0.4f, 0.4f, 0.5f, 0.5f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(rectangle_.left(),  rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(),  rectangle_.bottom());
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	stopScreenCoordinatesSystem();
    glLineWidth(1.0);
}

void Viewer::addIdToSelection(int id)
{
	if (!selection_.contains(id))
	{
		selection_.push_back(id);
	}
}

void Viewer::removeIdFromSelection(int id)
{
	//qDebug() << "selection_:" << selection_;
	selection_.removeAll(id);
	//qDebug() << "selection_:" << selection_;
	tempCoordinates = Point3d(0.0, 0.0, 0.0);
	stepsOfCentralNeigh = 0;
}

void Viewer::startManipulation()
{
	if(currentEditType == NODE_EDIT)
	{
		Vec averagePosition;
		ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
		mfsc->clearSet();
		mfsc->setSkel(*skel);

		// Add all selected points to the list of points that will be manipulated.
		for (int i = 0; i < selection_.size(); i++)
		{
			mfsc->addObjectToSet(selection_[i]);
		}

		// The average position of the selected points is calculated.
		for (int i = 0; i < selection_.size(); i++)
		{
			averagePosition.x += this->skel->points[selection_[i]].coord.x;
			averagePosition.y += this->skel->points[selection_[i]].coord.y;
			averagePosition.z += this->skel->points[selection_[i]].coord.z;
		}

		// Set the average position ad the centre of the manipulation.
		if (selection_.size() > 0)
		{
			manipulatedFrame()->setPosition(averagePosition / selection_.size());
		}
	}
	else
	{
		Vec averagePosition;
		ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
		QList<int> boneNodes = skel->bones.value(selection_.first());
		mfsc->clearSet();
		mfsc->setSkel(*skel);

		// Add all selected points to the list of points that will be manipulated.
		for (int i = 0; i < boneNodes.size(); i++)
		{
			mfsc->addObjectToSet(boneNodes[i]);
		}

		// The average position of the selected points is calculated.
		for (int i = 0; i < boneNodes.size(); i++)
		{
			averagePosition.x += this->skel->points[boneNodes[i]].coord.x;
			averagePosition.y += this->skel->points[boneNodes[i]].coord.y;
			averagePosition.z += this->skel->points[boneNodes[i]].coord.z;
		}

		// Set the average position ad the centre of the manipulation.
		if (boneNodes.size() > 0)
		{
			manipulatedFrame()->setPosition(averagePosition / selection_.size());
		}
	}
}

int Viewer::checkIfClose(Point3d s, float radius)
{
	// closePoints is a list of points near the point s. It includes the id and the distance.
	QList< QPair<int, float> > closePoints;

	// Get the points closer then radius.
	for (int i = 0; i < this->skel->points.size(); i++)
	{
		if(s == skel->points[i].coord)
		{
			continue;
		}
		if ((s.x < this->skel->points[i].coord.x + radius/3 && s.x > this->skel->points[i].coord.x - radius/3) &&
			(s.y < this->skel->points[i].coord.y + radius/3 && s.y > this->skel->points[i].coord.y - radius/3) &&
			(s.z < this->skel->points[i].coord.z + radius/3 && s.z > this->skel->points[i].coord.z - radius/3))
		{
			float distance = abs(s.x - this->skel->points[i].coord.x) +
							 abs(s.y - this->skel->points[i].coord.y) +
							 abs(s.z - this->skel->points[i].coord.z);

			closePoints.push_back(QPair<int,float>(this->skel->points[i].id, distance));
		}
	}

	// Order the list.
	if (closePoints.size() > 0)
	{
		for (int i = 0; i < closePoints.size(); i++)
		{
			for (int j = 0; j < closePoints.size(); j++)
			{
				if (closePoints[i].second > closePoints[j].second)
				{
					QPair<int, float> temp = closePoints[i];
					closePoints[i] = closePoints[j];
					closePoints[j] = temp;
				}
			}
		}

		return closePoints[0].first;
	}

	// If there are not points closer then radius, then the method returns -1.
	return -1;
}

void Viewer::addNewNode()
{
	if (selection_.size() == 1)
	{
		activeInsertion = true;

		// Change translation keybind.
		setMouseBinding(Qt::ShiftModifier, Qt::RightButton, FRAME, TRANSLATE);

		// The id for the new node is calculated increasing the max id by 1.
		newInsertId = this->skel->getMaxId() + 1;
		oldPointId = selection_[0];

		// The new point id is inserted as neighbor of the old point.
		this->skel->points[oldPointId].addNeighbor(newInsertId);

		// The starting position of the new node is the same of the old node.
		Point3d position = this->skel->points[oldPointId].coord;

		float dimension = this->skel->points[oldPointId].radius;

		// Add the old node in the neighbors list of the new node.
		QList<int> neighbors;
		neighbors.push_back(oldPointId);

		// Add the new point to the skeleton.
		this->skel->append(SkelPoint(newInsertId, position, dimension, neighbors, false));

		// Deselect the old point and select the new one.
		this->skel->points[newInsertId].select();
		this->skel->points[oldPointId].deselect();

		// Set the new point inserted as selected.
		selection_[0] = newInsertId;

		// Start the translation of the new point.
		startManipulation();

		// Restore the translation keybind.
		setMouseBinding(Qt::AltModifier | Qt::ControlModifier, Qt::LeftButton, FRAME, TRANSLATE);
	}
	else
	{
		QMessageBox::information(this, "Errore", "Per aggiungere un nuovo nodo devi aver selezionato un solo nodo, o due vicini.");
	}
}

void Viewer::clearSelection()
{
	if(currentEditType == NODE_EDIT)
	{
		for (int i = 0; i < selection_.size(); i++)
		{
			skel->points[skel->points.indexOf(selection_[i])].deselect();
		}

		selection_.clear();
		tempCoordinates = Point3d(0.0, 0.0, 0.0);
		stepsOfCentralNeigh = 0;
	}
	else
	{
		for (int i = 0; i < skel->points.size(); i++)
		{
			skel->points[i].deselect();
		}

		selection_.clear();
	}

	emit disableArticulation();
	emit disableCopy();
	emit disableDeleteConnection();
	emit disablePaste();
	emit disableSampling();
	emit uncheckArticulation();
}

void Viewer::setFixedCamera(Viewer::FixedCamera c)
{
	if ( this->_mesh == NULL || !this->_mesh->axes.is_valid ) return;
	Primitives::Point3d new_cam_pos = this->_mesh->bbox.Center();
	double scaling = this->_mesh->bbox.Diagonal() * 1.2;

	switch (c) {
	case X_PLUS:
		new_cam_pos += this->_mesh->axes.right  * scaling;
		break;
	case X_MINUS:
		new_cam_pos += this->_mesh->axes.left   * scaling;
		break;
	case Y_PLUS:
		new_cam_pos += this->_mesh->axes.up     * scaling;
		break;
	case Y_MINUS:
		new_cam_pos += this->_mesh->axes.down   * scaling;
		break;
	case Z_PLUS:
		new_cam_pos += this->_mesh->axes.front  * scaling;
		break;
	case Z_MINUS:
		new_cam_pos += this->_mesh->axes.back   * scaling;
		break;
	default:
		break;
	}
	this->camera()->setPosition( Vec( new_cam_pos.x, new_cam_pos.y, new_cam_pos.z ));
	this->camera()->lookAt( Vec( this->_mesh->bbox.Center().x,
								 this->_mesh->bbox.Center().y, this->_mesh->bbox.Center().z ));
	update();
}

QList<SkelPoint> Viewer::normalizePointsToCopy(QList<SkelPoint> pointsToNormalize)
{
	QList<SkelPoint> list;

	float xDist = (skel->points[selection_[0]].coord.x - skel->points[pointToMergeForCopy].coord.x);
	float yDist = (skel->points[selection_[0]].coord.y - skel->points[pointToMergeForCopy].coord.y);
	float zDist = (skel->points[selection_[0]].coord.z - skel->points[pointToMergeForCopy].coord.z);

	for (int i = 0; i < pointsToNormalize.size(); i++)
	{
		Point3d newCoord(pointsToNormalize[i].coord.x + xDist,
						 pointsToNormalize[i].coord.y + yDist,
						 pointsToNormalize[i].coord.z + zDist);

		for (int j = pointsToNormalize[i].neighbors.size() - 1; j >= 0; j--)
		{
			if (!copyList.contains(pointsToNormalize[i].neighbors[j]))
			{
				pointsToNormalize[i].removeNeighbor(pointsToNormalize[i].neighbors[j]);
			}
		}

		list.append(SkelPoint(pointsToNormalize[i].id, newCoord, pointsToNormalize[i].radius, pointsToNormalize[i].neighbors));
	}

	QHash<int, int> map;

	int maxId = skel->getMaxId();

	for (int i = 0; i < list.size(); i++)
	{
		map.insert(list[i].id, i + maxId + 1);
	}

	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].id == pointToMergeForCopy)
		{
			pointToMergeForCopy = map.value(list[i].id);
		}

		list[i].id = map.value(list[i].id);

		for (int j = 0; j < list[i].neighbors.size(); j++)
		{
			list[i].neighbors[j] = map.value(list[i].neighbors[j]);
		}
	}

	return list;
}

void Viewer::action_copy()
{
	//	if( selection_.size() == 1)
	//	{
	//		//find the bone of the selected node
	//		Skel::Bone boneToCopy = skel->bones.find(skel->points[selection_.first()].boneID).value();

	//		//avoid to insert bones that were already being copied.
	//		for( int i = 0; i < copyBuffer.size(); i++)
	//		{
	//			if(skel->points[boneToCopy.first()].boneID == copyBuffer[i].boneID)
	//			{
	//				return;
	//			}
	//		}

	//		for( int i = 0; i < boneToCopy.size(); i++)
	//		{
	//			cout<<"Inserisco: " << skel->points[boneToCopy[i]].id <<"\n";
	//			copyBuffer.append(skel->points[boneToCopy[i]]);
	//		}
	//	}

	copyList.clear();

	operationIsActive = true;
	copyActive = true;

	emit disableActions();

	copyList.append(selection_);

	for (int i = 0; i < selection_.size(); i++)
	{
		skel->points[selection_[i]].setSFC(true);
	}

	clearSelection();

//	emit startCopy(&copyList);
	emit startCopy();
}

void Viewer::findNearest( double x, double y, double z )
{
    cout << "tryng to single select" << endl;
	Primitives::Vector3d click_dir( dir[0], dir[1], dir[2] );
	Primitives::Point3d x1(x, y, z);
	Primitives::Point3d x2 = x1 + click_dir;

	int candidate_id = -1;
	float min_dist = numeric_limits<float>::max();

	for( int pid = 0; pid < skel->points.size(); ++pid )
	{
		SkelPoint skelp = skel->points[pid];
		Primitives::Vector3d crossed = (skelp.coord - x1).Cross(skelp.coord - x2 );
		double dist = crossed.Length()/sceneRadius();
		if( dist < min_dist && dist )
		{
			candidate_id = skelp.id;
			min_dist     = dist;
		}
	}
	cout << "il candidato è " << candidate_id << endl;
	if(candidate_id != -1 || (candidate_id != -1 && true == copyActive && copyList.indexOf(candidate_id) != -1))
	{
		status.addState((*this->skel), selection_);
		selection_.append(candidate_id);
		skel->points[candidate_id].select();
	}

	if (true == copyActive && selection_.size() > 1)
	{
		skel->points[selection_.first()].deselect();
		selection_.removeFirst();
	}

	if (true == deletingSingleBranchingNode && !skel->points[skel->points.indexOf(selection_.first())].neighbors.contains(branchingNodeToDelete.id))
	{
		clearSelection();

		QMessageBox::information(this, "Errore", "Puoi selezionare soltanto un vicino del branching node da cancellare.");
	}
}



void Viewer::action_paste()
{
	if (selection_.size() == 1)
	{
		QList<SkelPoint> pointsToCopy;

		for (int i = 0; i < copyList.size(); i++)
		{
			pointsToCopy.append(skel->points[copyList[i]]);
		}

		QList<SkelPoint> points = normalizePointsToCopy(pointsToCopy);

		for (int i = 0; i < points.size(); i++)
		{
			skel->append(points[i]);
		}

		skel->points[pointToMergeForCopy].neighbors.append(skel->points[selection_[0]].neighbors);

		for (int i = 0; i < skel->points[selection_[0]].neighbors.size(); i++)
		{
			skel->points[skel->points[selection_[0]].neighbors[i]].addNeighbor(pointToMergeForCopy);
		}

		action_deleteNode();

		copyList.clear();
		clearSelection();
		garbage_collector(skel);
		status.addState((*this->skel), selection_);

		emit disablePaste();
	}
}
