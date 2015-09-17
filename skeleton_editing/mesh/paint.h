#pragma once

#include "mesh.h"
#ifdef unix
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#ifdef win32
#include <gl.h>
#include <glu.h>
#endif


namespace RMesh
{

static void setDefaultMaterial()
{
		GLfloat mat_a[] = {0.1, 0.1, 0.1, 1.0};
		GLfloat mat_d[] = {0.1, 0.8, 0.1, 1.0};
		GLfloat mat_s[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat shine[] = {120.0};
//        GLfloat mat_a[] = {0.1, 0.1, 0.1, 1.0};
//        GLfloat mat_d[] = {0.4, 0.4, 0.4, 0.9};
//        GLfloat mat_s[] = {0.1, 0.1, 0.1, 0.1};
//        GLfloat shine[] = {50.0};

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

static void paint( mesh &m, int mode)
{
	if (mode & RMesh::POINTS)
	{
		std::cout << "drawing points" << endl;
		glBegin(GL_POINTS);
		foreach( Primitives::Point3d v, m.vertices) glVertex3f( v[0], v[1], v[2] );
		glEnd();
	}

	if (mode & RMesh::FLAT )
	{
//        std::cout << "drawing flat" << "# n triangles  " << m.triangles.size() << endl;
//        std::cout << "drawing flat" << "# n vertices  " << m.vertices.size() << endl;
		for(int f_id = 0; f_id < m.polygons.size(); ++f_id)
		{
			RMesh::MeshPolygon& t = m.polygons[f_id];
			if(t.no_vertices() == 3 )
			{
				glBegin(GL_TRIANGLES);

			}
			else if(t.no_vertices() == 4 )
			{
				glBegin(GL_QUADS);

			}
			else
			{
				glBegin(GL_POLYGON);
			}

			for( int pi = 0; pi < m.polygons[f_id].vertices.size(); ++pi )
			{
				Primitives::Point3d &p = m.vertices[t.vertices[pi]];
				glVertex3f( p[0], p[1], p[2] );
			}

			glEnd();
		}
	}

	if (mode & RMesh::WIREFRAME)
	{
		for(int f_id = 0; f_id < m.polygons.size(); ++f_id)
		{
			RMesh::MeshPolygon& t = m.polygons[f_id];
			if(t.no_vertices() == 3 )
			{
				glBegin(GL_TRIANGLES);

			}
			else if(t.no_vertices() == 4 )
			{
				glBegin(GL_QUADS);

			}
			else
			{
				glBegin(GL_POLYGON);
			}

			for( int pi = 0; pi < m.polygons[f_id].vertices.size(); ++pi )
			{
				Primitives::Point3d &p = m.vertices[t.vertices[pi]];
				glVertex3f( p[0], p[1], p[2] );
			}

			glEnd();
		}
	}
}

static void draw( RMesh::mesh &m, int mode)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//if ( &m == NULL ) return;

//    if( mode & RMesh::PCA && m.axes.is_valid )
//    {
//        Primitives::Point3d center = m.bbox.Center();
//        Primitives::Point3d ax      = center + m.axes.right * m.bbox.Diagonal();
//        Primitives::Point3d ay      = center + m.axes.up * m.bbox.Diagonal();
//        Primitives::Point3d az      = center + m.axes.front * m.bbox.Diagonal();
//        glLineWidth( 4.0);
//        glColor3f(1.0, 0.0, 0.0);
//        glBegin(GL_LINES);
//        glColor3f(0.0, 0.0, 1.0);
//        glVertex3f( center.x, center.y, center.z );
//        glVertex3f( ax.x, ax.y, ax.z );
//        glColor3f(0.0, 1.0, 0.0);
//        glVertex3f( center.x, center.y, center.z );
//        glVertex3f( ay.x, ay.y, ay.z );
//        glColor3f(1.0, 0.0, 0.0);
//        glVertex3f( center.x, center.y, center.z );
//        glVertex3f( az.x, az.y, az.z );
//        glEnd();
//        glLineWidth( 1.0);
//    }

	setDefaultMaterial();

	if (mode & WIREFRAME)
	{
		if (mode & POINTS)
		{
				glDisable(GL_LIGHTING);
				glShadeModel(GL_FLAT);
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
				glColor4f(0.2f, 0.2f, 0.2f, 0.2f);
                glLineWidth(1.0);
				glDepthRange(0.0, 1.0);
				paint(m, mode);
				glLineWidth(1.0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		else

		if (mode & FLAT)
		{
				glEnable(GL_LIGHTING);
				glShadeModel(GL_FLAT);
				glDepthRange(0.01, 1.0);
				glColor4f(0.5, 0.5, 0.5, 0.5);
//				paint(m, mode);

				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                glColor4f(0.0, 0.0, 0.0, 0.4);
//                                        glColor3f(0.5, 0.5, 0.5);
				glDepthRange(0.0, 1.0);
                paint(m, mode);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		else

		if (mode & SMOOTH)
		{
				glEnable(GL_LIGHTING);
				glShadeModel(GL_SMOOTH);
				glDepthRange(0.01, 1.0);
				paint(m, mode);

				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glColor3f(0.2, 0.2, 0.2);
				glDepthRange(0.0, 1.0);
				glDepthFunc(GL_LEQUAL);
                paint(m, mode);
				glDepthFunc(GL_LESS);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	glDisable(GL_BLEND);
}

}
