#include "rotationgizmo.h"
#include "math.h"
#include "vector.h"
#include "glu.h"

rotationGizmo::rotationGizmo()
{
    origin          = vcg::Point3f( 0.0, 0.0, 0.0 );
    radius          = 0.5;
    visibility      = false;
    selected_axis   = 0;
    colors      [0]= buildColor(0);
    colors      [1]= buildColor(1);
    colors      [2]= buildColor(2);
    frame       [0]= vcg::Point3f( 1.0, 0.0, 0.0 );
    frame       [1]= vcg::Point3f( 0.0, 1.0, 0.0 );
    frame       [2]= vcg::Point3f( 0.0, 0.0, 1.0 );

    XYZ         [0]= vcg::Point3f( 1.0, 0.0, 0.0 );
    XYZ         [1]= vcg::Point3f( 0.0, 1.0, 0.0 );
    XYZ         [2]= vcg::Point3f( 0.0, 0.0, 1.0 );

    frame       [0]= XYZ[0];
    frame       [1]= XYZ[1];
    frame       [2]= XYZ[2];

    cube_id        = -1;
}

void rotationGizmo::setPosition(float x, float y, float z)
{
    this->origin = vcg::Point3f( x, y, z );
}

void rotationGizmo::setRadius(float r)
{
    this->radius = r;
}

void rotationGizmo::draw( bool select)
{
    if ( !this->visibility ) { return; }

    this->DrawCircle( origin.X(), origin.Y(), origin.Z(), radius * 0.98, 100, 0, select );
    this->DrawCircle( origin.X(), origin.Y(), origin.Z(), radius       , 100, 1, select );
    this->DrawCircle( origin.X(), origin.Y(), origin.Z(), radius * 1.01, 100, 2, select );
}

void rotationGizmo::DrawCircle( float cx, float cy, float cz,
                                float r, int num_segments, int axis, bool select )
{

    float theta = 2 * 3.1415926 / float(num_segments);
    float tangetial_factor = tanf(theta);   //calculate the tangential factor

    float radial_factor = cosf(theta);      //calculate the radial factor

    float x = r;//we start at angle = 0

    float y = 0;

    vcg::Point3f vertices[num_segments];

    for(int ii = 0; ii < num_segments; ii++)
    {
        float or_x = x,
              or_y = y,
              or_z = 0.0;
        float nx, ny, nz;

        if( axis == 0 )
        {
            nx = or_x; ny = or_y; nz = or_z;
        }

        if( axis == 1 )
        {
            nx = or_x;
            ny = -or_z;
            nz = or_y;
        }

        if( axis == 2 )
        {
            nz = -or_x;
            ny = or_y;
            nx = or_z;
        }

        vcg::Point3f p( nx + cx, ny + cy, nz + cz);
        //p.x = nx + cx; p.y = ny + cy; p.z = nz + cz;

        vertices[ii] = p;

        //calculate the tangential vector
        //remember, the radial vector is (x, y)
        //to get the tangential vector we flip those coordinates and negate one of them

        float tx = -y;
        float ty = x;

        //add the tangential vector

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        //correct using the radial factor

        x *= radial_factor;
        y *= radial_factor;
    }

    //rotate according to current frame.
    for(int ii = 0; ii < num_segments; ii++)
    {
        vcg::Point3f vcgp = vertices[ii];
        vcgp -= origin;
        vcg::Point3f tp = _rot * vcgp;
        tp +=origin;
        vertices[ii] = tp;
    }


    if( this->selected_axis == axis )
        glLineWidth( 8.0 );
    else{
        glLineWidth( 4.0 );
    }

    if (select )
    {
        glLoadName(0);
        glPushName(axis);
    }
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glColor4f( colors[axis].r, colors[axis].g, colors[axis].b, 1.0 );
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++) glVertex3f( vertices[ii].X(), vertices[ii].Y(), vertices[ii].Z() );
    glEnd();
    glLineWidth(1.0);

    if(select)
    {
        //  glPopName();
    }
}

color rotationGizmo::buildColor(int axis)
{
    assert( axis >= 0 && axis < 3 );
    if( axis == 0 )
    { color c;    c.r = 1.0;    c.g = 0.0;    c.b = 0.0;    return c; }

    if( axis == 1 )
    { color c;    c.r = 0.0;    c.g = 1.0;    c.b = 0.0;    return c; }

    if( axis == 2 )
    { color c;    c.r = 0.0;    c.g = 0.0;    c.b = 1.0;    return c; }
}



void rotationGizmo::pickAxis( float x, float y, float width, float height )
{
    std::cout << "PICKING AXIS " << std::endl;
    int BUFSIZE = 512;
    GLuint selectBuf[BUFSIZE];
    GLint hits;
    GLint viewport[4];

    glGetIntegerv (GL_VIEWPORT, viewport);

    glSelectBuffer (BUFSIZE, selectBuf);
    (void) glRenderMode (GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    /*  create 5x5 pixel picking region near cursor location      */
    gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y),
                  25.0, 25.0, viewport);

    //gluOrtho2D (0.0, 3.0, 0.0, 3.0);
    glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
    this->draw(true);

    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glFlush ();

    hits = glRenderMode (GL_RENDER);
    processHits (hits, selectBuf);

    //    glutPostRedisplay();

}

void rotationGizmo::setAxis(int axis_id, float x, float y, float z)
{
    this->frame[axis_id] = vcg::Point3f( x, y, z );

    vcg::Point3f axis, tmp;

    double angle;
    vcg::Matrix44f tx, ty;

    axis  = XYZ[0] ^ frame[0];
    angle = vcg::Angle(XYZ[0], frame[0]);
    tx.SetRotateRad( angle, axis);

    tmp   = tx  * XYZ[1];
    axis  = tmp ^ frame[1];
    angle = vcg::Angle( tmp, frame[1] );
    ty.SetRotateRad( angle, axis);

    _rot = ty * tx;
}

void rotationGizmo::rotateAlongSelected( int angle )
{
    vcg::Matrix44f  extra_rot;

    vcg::Point3f    axis = _rot * XYZ[ selected_axis ];
    //float angle = (float)extra_rot_deg;
    extra_rot.SetRotateDeg( (float)angle, axis);
    _rot = extra_rot * _rot;
}

void rotationGizmo::processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint ii, jj, names, *ptr;

   printf ("hits = %d\n", hits);
   ptr = (GLuint *) buffer;
   for (i = 0; i < hits; i++) { /*  for each hit  */
      names = *ptr;
      printf (" number of names for this hit = %d\n", names);
         ptr++;
      printf("  z1 is %g;", (float) *ptr/0x7fffffff); ptr++;
      printf(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;
      printf ("   names are ");
      for (j = 0; j < names; j++) { /*  for each name */
         printf ("%d ", *ptr);
         if (j == 0)  /*  set row and column  */
            ii = *ptr;
         else if (j == 1)
            jj = *ptr;
         ptr++;
      }
      printf ("\n");
      //board[ii][jj] = (board[ii][jj] + 1) % 3;
   }
}


