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

#ifndef OBJECT_H_
#define OBJECT_H_

#ifdef WIN32
	#include "frame.h"
#endif
#ifdef OSX
	#include "QGLViewer/frame.h"
#endif
#include <qgl.h>

using namespace qglviewer;

class Object
{
public:

	void draw() const;
	Frame frame;
};

#endif // OBJECT_H_
