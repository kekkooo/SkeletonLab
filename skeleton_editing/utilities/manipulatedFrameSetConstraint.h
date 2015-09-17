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

#ifdef WIN32
	#include "constraint.h"
	#include "frame.h"
#endif
#ifdef OSX
	#include "QGLViewer/constraint.h"
	#include "QGLViewer/frame.h"
#endif
#include "utilities/object.h"
#include <skel/base.h>
#include <skel/paint.h>

using namespace qglviewer;

class ManipulatedFrameSetConstraint : public Constraint
{
public:

	void clearSet();
	void addObjectToSet(int sphereId);
	void setSkel(Skel::CurveSkeleton &skel);

	virtual void constrainTranslation(Vec &translation, Frame *const frame);
	virtual void constrainRotation(Quaternion &rotation, Frame *const frame);

private:

	QList<int> objects_;
	Skel::CurveSkeleton *skel;

};
