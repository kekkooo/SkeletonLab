/****************************************************************************

 Copyright (C) 2002-2013 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.5.2.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include "manipulatedFrameSetConstraint.h"

void ManipulatedFrameSetConstraint::clearSet()
{
	objects_.clear();
}

void ManipulatedFrameSetConstraint::setSkel(Skel::CurveSkeleton &skel)
{
	this->skel = &skel;
}

void ManipulatedFrameSetConstraint::addObjectToSet(int sphereId)
{
	objects_.append(sphereId);
}

void ManipulatedFrameSetConstraint::constrainTranslation(Vec &translation, Frame *const)
{
	for (int i = 0; i < objects_.size(); i++)
	{
		(*skel).points[objects_[i]].coord.x += translation.x;
		(*skel).points[objects_[i]].coord.y += translation.y;
		(*skel).points[objects_[i]].coord.z += translation.z;
	}
}

void ManipulatedFrameSetConstraint::constrainRotation(Quaternion &rotation, Frame *const frame)
{
	// A little bit of math. Easy to understand, hard to guess (tm).
	// rotation is expressed in the frame local coordinates system. Convert it back to world coordinates.
	const Vec worldAxis = frame->inverseTransformOf(rotation.axis());
	const Vec pos = frame->position();
	const float angle = rotation.angle();

	for (int i = 0; i < objects_.size(); i++)
	{
		Quaternion qObject(rotation.axis(), angle);

		Quaternion qWorld(worldAxis, angle);
		Vec point((*skel).points[objects_[i]].coord.x, (*skel).points[objects_[i]].coord.y, (*skel).points[objects_[i]].coord.z);
		Vec pointRotation = qWorld.rotate(point - pos);

		(*skel).points[objects_[i]].coord.x = pos.x + pointRotation.x;
		(*skel).points[objects_[i]].coord.y = pos.y + pointRotation.y;
		(*skel).points[objects_[i]].coord.z = pos.z + pointRotation.z;

	}

//	for (QList<Object*>::iterator it=objects_.begin(), end=objects_.end(); it != end; ++it)
//	{
//		// Rotation has to be expressed in the object local coordinates system.
//		Quaternion qObject((*it)->frame.transformOf(worldAxis), angle);
//		(*it)->frame.rotate(qObject);
//
//		// Comment these lines only rotate the objects
//		Quaternion qWorld(worldAxis, angle);
//		// Rotation around frame world position (pos)
//		(*it)->frame.setPosition(pos + qWorld.rotate((*it)->frame.position() - pos));
//	}
}
