#pragma once

#include <primitives/geometry.h>
#include <primitives/color.h>

//#include <list>
#include <QList>
#include <vector>
#include <assert.h>
#ifdef OSX
	#include <QGLViewer/qglviewer.h>
#endif
#ifdef WIN32
	#include <qglviewer.h>
#endif

#include <QDebug>

using namespace Primitives;

namespace Skel
{

	class SkelPoint
	{

	public:

        SkelPoint (unsigned int	_id        = -1,
                   Point3d		_coord   = Point3d(0,0,0),
                   float		_radius    = 0.f,
                   QList<int>	_neighbors = QList<int>(),
                   bool			_root      = false)
		{
			id			= _id;
			coord		= _coord;
			radius		= _radius;
			real_radius	= _radius;
            neighbors	= _neighbors;
			boneID		= -1;
			color		= ColorF::White();
			_isV		= false;
			_isA		= false;
			hidden		= false;
			_isSel		= false;
			_isTOI		= false;
			_isSFC		= false;
			root		= _root;
		}


		// Flag handling
		inline bool isArticulation()	{  return _isA;												} // ARTICULATION
		inline bool isVisited()			{  return _isV;												} // VISITED
		inline bool isDeleted()			{  return (neighbors.size() == 0);							} // DELETED
		inline bool isConnected()		{  return (neighbors.size()  > 0 || root);					} // CONNECTED
		inline bool isHidden()			{  return hidden;											} // HIDDEN
		inline bool isLeaf()			{  return (neighbors.size() == 1);							} // LEAF
		inline bool isJoint()			{  return (neighbors.size() == 2) && !isArticulation();		} // JOINT
		inline bool isBranchingNode()	{  return (neighbors.size() > 2);							} // BRANCHING NODE
		inline bool isFeaturePoint()	{  return (isLeaf() || isBranchingNode() || isArticulation());		} // FEATURE POINT
		inline bool isRoot()			{  return root;												} // ROOT node
		inline bool isSelected()		{  return _isSel;											} // SELECTED
		inline bool isTransferOnIt()	{  return _isTOI;											} // TRANSFER ON IT
		inline bool isSelectedForCopy() {  return _isSFC;											} // SELECTED FOR COPY

		// Neighbors handling
		inline void addNeighbor(int neighbor)
		{
            if ( neighbors.contains( neighbor ) || neighbor == this->id ) return;
			neighbors.append(neighbor);
		}

		inline void removeNeighbor(int neighbor)
		{
			assert(isConnected());
			assert(neighbors.contains(neighbor));

			int pos = neighbors.indexOf(neighbor);
			assert(pos != -1);

			neighbors.removeAt(pos);
		}

		inline int valence()
		{
			return neighbors.size();
		}

		// Navigation
		inline int next(int prev = -1)
		{
			assert( isConnected() );
			assert( !isBranchingNode() );

			if ( prev == -1) assert( isLeaf() );

			if ( isJoint() )
			{
				if (neighbors.first() == prev) return neighbors.last();

				return neighbors.first();
			}

			assert(isLeaf());

			return neighbors.last();
		}

		inline void setHidden(bool b)
		{
			hidden = b;
		}

		//c is a color that will be used for change
		//the color of the sphere
        inline void setColor(ColorF c)
		{
			color = c;
		}

		//set true the flag that indicate that a sphere is selected
		inline void select()
		{
			_isSel = true;
		}

		//set false the flag that indicate that a sphere is selected
		inline void deselect()
		{
			_isSel=false;
		}

		inline void setTOI(bool b)
		{
			_isTOI = b;
		}

		inline void setSFC(bool b)
		{
			_isSFC = b;
		}

		inline void setArticulation(bool b)
		{
			_isA = b;
		}

		bool operator==(const SkelPoint &point)
		{
			return this->id == point.id;
		}

		int          id;               // unique ID
        Point3d      coord;            // XYZ coordinates
		float        radius;           // radius of the maximal ball centered in the point
		float        real_radius;      // real distance from mesh boundaries
		bool         _isV;             // VISITED      flag
		bool         _isA;             // ARTICULATION flag
		int          boneID;           // unique ID of the skeleton branch the point belongs to
        QList<int>   neighbors;        // IDs of the adjacent skeleton points
		QList<int>   surfacePoints;    // IDs of the surface points attached to the point
		QList<int>   surfaceFaces;     // IDs of the surface faces attached to the point
		ColorF       color;            // point color
		bool         hidden;           // the point exists, but it should not be taken into account for anything
		bool         root;             // point is the elected root of the skeletal graph
		bool		 _isSel;           // SELECTED FLAG
		bool		 _isTOI;		   // TRANSFER ON IT FLAG
		bool		 _isSFC;		   // SELECTED FOR COPY FLAG
	};

}
