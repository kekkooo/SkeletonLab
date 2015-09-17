#pragma once
#ifdef OSX
	#include <OpenGL/glu.h>
#endif
#ifdef WIN32
#include <gl/GLU.h>
#endif
using namespace Skel;
namespace MyGLutils
{        

        static void cylinder(Point3d A, Point3d B, float beginSize, float endSize, ColorF color)
        {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);

//                vcg::Color4f c = vcg::Color4f::Construct(color);
//                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color.asArray());

                glColor4fv(color.asArray());
                Vector3d dir    = B - A;
                Vector3d Z      = Vector3d(0,0,1);
                Vector3d normal = dir.Cross(Z);
                double  angle      = Vector3d::Angle(dir, Z) * 180 / M_PI; // degrees

                glPushMatrix();
                glTranslated(A[0], A[1], A[2]);
                glRotatef(-angle, normal[0], normal[1], normal[2]);
                gluCylinder(gluNewQuadric(), beginSize, endSize, A.Dist(B), 10, 5);
                glPopMatrix();
        }


        static void sphere(Point3d center, float radius, ColorF color)
        {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);

//                vcg::Color4f c = vcg::Color4f::Construct(color);
//                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color.asArray());
                glColor4fv(color.asArray());

                glPushMatrix();
                glTranslated( center[0], center[1], center[2] );
                GLUquadric *sphere = gluNewQuadric();
                gluQuadricOrientation(sphere,GLU_OUTSIDE);
                gluSphere(sphere, radius, 30, 30);
                glPopMatrix();
        }


}
